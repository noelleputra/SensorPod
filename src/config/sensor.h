#pragma once

#include <Arduino.h>

namespace sensor {
    constexpr uint8_t SENSOR_SAMPLE = 10;
    constexpr uint8_t SENSOR_DISCARD_LOW = 1;
    constexpr uint8_t SENSOR_DISCARD_HIGH = 1;

    constexpr uint16_t ADC_DRY = 590;
    constexpr uint16_t ADC_WET = 280;

    constexpr uint16_t SENSOR_WARMUP_MS = 50;
    constexpr uint8_t SENSOR_SAMPLE_DELAY_MS = 5;
}