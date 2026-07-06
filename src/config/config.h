#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 2;
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint8_t UART_BUFFER_SIZE = 32;
    constexpr uint8_t NO_DIRECTION_PIN = 0xFF;
    constexpr uint8_t RS485_TURNAROUND_US = 200;
}