#include "calibration_store.h"

#include <EEPROM.h>
#include "config/sensor.h"

void CalibrationStore::begin()
{
    EEPROM.get(calibration::EEPROM_ADDRESS, data_);

    if (data_.magic != calibration::MAGIC)
    {
        // Fresh board / never calibrated -- start from firmware defaults
        // rather than trusting whatever garbage was in EEPROM.
        resetToDefaults();
    }
}

void CalibrationStore::resetToDefaults()
{
    data_.magic = calibration::MAGIC;
    for (uint8_t i = 0; i < calibration::SENSOR_COUNT; ++i)
    {
        data_.dryAdc[i] = sensor::ADC_DRY;
        data_.wetAdc[i] = sensor::ADC_WET;
    }
    save();
}

void CalibrationStore::setDry(uint8_t sensorIndex, uint16_t rawAdc)
{
    if (sensorIndex >= calibration::SENSOR_COUNT) return;
    data_.dryAdc[sensorIndex] = rawAdc;
    save();
}

void CalibrationStore::setWet(uint8_t sensorIndex, uint16_t rawAdc)
{
    if (sensorIndex >= calibration::SENSOR_COUNT) return;
    data_.wetAdc[sensorIndex] = rawAdc;
    save();
}

void CalibrationStore::save()
{
    // EEPROM.put() only actually writes bytes that changed (the
    // underlying update() call compares before writing), so repeated
    // calibration saves during a session don't needlessly wear the
    // EEPROM (rated ~100k write cycles per cell on AVR).
    EEPROM.put(calibration::EEPROM_ADDRESS, data_);
}
