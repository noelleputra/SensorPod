#pragma once

#include <Arduino.h>

namespace sensor {
    constexpr uint8_t SENSOR_SAMPLE = 10;
    constexpr uint8_t SENSOR_DISCARD_LOW = 1;
    constexpr uint8_t SENSOR_DISCARD_HIGH = 1;

    // Firmware-default fallback ADC endpoints, used only until a sensor
    // has an actual saved calibration (see CalibrationStore). Individual
    // sensors will drift from these due to normal component tolerance,
    // which is exactly what per-sensor calibration corrects for.
    constexpr uint16_t ADC_DRY = 590;
    constexpr uint16_t ADC_WET = 280;

    constexpr uint16_t SENSOR_WARMUP_MS = 50;
    constexpr uint8_t SENSOR_SAMPLE_DELAY_MS = 5;

    // How many raw samples readRawBlocking() averages during an
    // interactive calibration read (a few samples is enough since the
    // point here is a stable live readout for a human to watch, not the
    // full noise-rejecting SENSOR_SAMPLE pipeline used for real reads).
    constexpr uint8_t CALIBRATION_RAW_SAMPLES = 8;
    constexpr uint8_t CALIBRATION_RAW_SAMPLE_DELAY_MS = 5;

    // Safety auto-exit: if calibration mode is entered but no further
    // K-command arrives for this long, power to the sensors is cut and
    // calibration mode exits on its own -- so a forgotten/aborted
    // calibration session doesn't leave the probes energized (and, for
    // resistive-type probes, slowly corroding) indefinitely.
    constexpr uint32_t CALIBRATION_TIMEOUT_MS = 5UL * 60UL * 1000UL;
}