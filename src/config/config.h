#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 2;
    
    constexpr uint32_t UART_BAUD = 9600; // UART Baud
    constexpr uint8_t UART_BUFFER_SIZE = 32;

    constexpr uint8_t NO_DIRECTION_PIN = 0xFF;
    constexpr uint8_t RS485_TURNAROUND_US = 200; // 1 bit time at 9600 baud, safe for RS485 turnaround
}