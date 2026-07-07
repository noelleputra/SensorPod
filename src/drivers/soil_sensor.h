#pragma once

#include <Arduino.h>
#include "config/sensor.h"

// Non-blocking soil moisture sensor.
// Power pin is shared and managed externally (by SensorService) since
// two sensors on the same board can share one power rail. This class
// only owns its analog pin and its own sample buffer/percent cache.
class soilSensor
{
public:
    explicit soilSensor(uint8_t analogPin);

    void begin();

    // Call once when a new read cycle starts (after power has been
    // applied and the warmup delay has already elapsed).
    void beginAsync();

    // Call every loop() tick while sampling is in progress.
    // Internally paced by config::sensor::SENSOR_SAMPLE_DELAY_MS,
    // so calling it more often than that is harmless (it's a no-op
    // until enough time has passed).
    void takeSample();

    // True once SENSOR_SAMPLE samples have been collected.
    bool isDone() const;

    // Call once after isDone() is true. Computes, caches, and returns
    // the moisture percentage (0-100).
    uint8_t computePercent();

    // Last computed percentage (cached, doesn't trigger a new read).
    uint8_t getPercent() const;

private:
    const uint8_t analogPin;

    uint16_t samples[sensor::SENSOR_SAMPLE] = {0};
    uint8_t sampleCount = 0;
    unsigned long lastSampleTime = 0;

    uint8_t cachedPercent = 0;
};
