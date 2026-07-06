#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 2; // Change this value for each node.

    constexpr uint32_t UART_BAUD = 9600; // UART Baud
    constexpr uint8_t UART_BUFFER_SIZE = 32;

    constexpr uint8_t NO_DIRECTION_PIN = 0xFF;
    constexpr uint8_t RS485_TURNAROUND_US = 200; // 1 bit time at 9600 baud, safe for RS485 turnaround

    constexpr uint8_t SENSOR_SAMPLE = 10;
    constexpr uint8_t SENSOR_DISCARD_LOW = 1;
    constexpr uint8_t SENSOR_DISCARD_HIGH = 1;

    constexpr uint16_t ADC_DRY = 590;
    constexpr uint16_t ADC_WET = 280;

    constexpr uint16_t SENSOR_WARMUP_MS = 50;
    constexpr uint8_t SENSOR_SAMPLE_DELAY_MS = 5;

    namespace pins {
        constexpr uint8_t SOIL1 = A0;
        constexpr uint8_t SOIL2 = A1;
        constexpr uint8_t SOIL_POWER = 2;
        constexpr uint8_t RS485Dir = 4;
    }

    namespace protocol {
        constexpr char REQUEST[] = "R";
        constexpr char PREFIX[]  = "SensorPod";
    }
}