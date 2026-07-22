#pragma once

#include <Arduino.h>

namespace calibration {
    constexpr uint8_t SENSOR_COUNT = 4;

    // Arbitrary marker distinguishing "a real calibration was saved here"
    // from blank/erased EEPROM, which AVR chips read back as 0xFF per
    // byte from the factory (or after a full chip erase). Without this
    // check, a fresh/unflashed board would boot with garbage dry/wet
    // values instead of falling back to sane firmware defaults.
    constexpr uint8_t MAGIC = 0xC7;

    constexpr int EEPROM_ADDRESS = 0;

    struct Data {
        uint8_t magic;
        uint16_t dryAdc[SENSOR_COUNT];
        uint16_t wetAdc[SENSOR_COUNT];
    };
}

// Thin EEPROM-backed wrapper around per-sensor dry/wet calibration
// points. Persists across power loss/reboot so calibration only needs
// to be done once per physical sensor, not once per firmware flash.
class CalibrationStore {
public:
    // Loads from EEPROM. Falls back to firmware-default ADC_DRY/ADC_WET
    // (see config/sensor.h) for all 4 sensors if the stored data's magic
    // byte doesn't match -- i.e. a fresh board, or EEPROM that was never
    // written by this calibration system.
    void begin();

    // Resets all 4 sensors back to firmware defaults and saves.
    void resetToDefaults();

    // sensorIndex is 0-3. Out-of-range indices are ignored (no-op).
    void setDry(uint8_t sensorIndex, uint16_t rawAdc);
    void setWet(uint8_t sensorIndex, uint16_t rawAdc);

    const calibration::Data& data() const { return data_; }

private:
    calibration::Data data_{};
    void save();
};
