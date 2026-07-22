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

    // Sets this sensor's own dry/wet ADC endpoints, replacing the
    // firmware-default sensor::ADC_DRY/ADC_WET used by computePercent().
    // This is what makes per-sensor calibration actually take effect.
    void setCalibration(uint16_t dryAdc, uint16_t wetAdc);

    // Blocking averaged raw ADC read, for interactive calibration only
    // (NOT used by the normal async read cycle). Power to this sensor
    // must already be applied and settled -- the caller (calibration
    // mode in SensorService) is responsible for that, since during
    // calibration the power rail is held on continuously rather than
    // being toggled per-read like the normal cycle does.
    uint16_t readRawBlocking() const;

private:
    const uint8_t analogPin;

    uint16_t samples[sensor::SENSOR_SAMPLE] = {0};
    uint8_t sampleCount = 0;
    unsigned long lastSampleTime = 0;

    uint8_t cachedPercent = 0;

    uint16_t dryAdc = sensor::ADC_DRY;
    uint16_t wetAdc = sensor::ADC_WET;
};
