#pragma once

#include <Arduino.h>

namespace Config
{
    // ========= UART =========
    constexpr uint32_t UART_BAUD = 9600;
    // ========= Sensor =========
    constexpr uint8_t SENSOR_COUNT = 2;
    constexpr uint8_t SENSOR_SAMPLES = 10;
    constexpr uint16_t SENSOR_WARMUP_MS = 50;
    constexpr uint32_t SENSOR_READ_INTERVAL_MS = 1000;
    // ========= Debug =========
    constexpr bool DEBUG_ENABLED = true;
    constexpr uint32_t DEBUG_INTERVAL_MS = 1000;
    // ========= Calibration =========
    constexpr uint16_t SOIL_AIR_RAW = 590;
    constexpr uint16_t SOIL_WATER_RAW = 280;
}