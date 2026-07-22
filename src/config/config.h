#pragma once

#include <Arduino.h>

namespace config {
    // Only used to seed a factory-fresh board's EEPROM the very first
    // time it boots (see NodeIdStore). Every SensorPod unit is flashed
    // from the SAME firmware image, so this value is intentionally the
    // same for all of them -- it is NOT meant to stay as each unit's
    // permanent, unique node id. After flashing, give each physical unit
    // its real id ONE AT A TIME over serial: `K<currentId>:SETID:<newId>`
    // (a fresh board currently responds as DEFAULT_NODE_ID). From then
    // on the id lives in EEPROM and this constant is irrelevant to that
    // unit until/unless its EEPROM is wiped.
    constexpr uint8_t DEFAULT_NODE_ID = 0;
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint8_t UART_BUFFER_SIZE = 32;
    constexpr uint32_t RS485_TURNAROUND_US = 1000;
}

#define DEBUG_ENABLED 0 // 1 for enabled, 0 for disabled

#if DEBUG_ENABLED
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)

#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)

#endif