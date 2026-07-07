#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 1;
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