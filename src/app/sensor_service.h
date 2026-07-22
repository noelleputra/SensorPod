#pragma once

#include <Arduino.h>
#include "drivers/soil_sensor.h"
#include "communication/rs485.h"
#include "calibration/calibration_store.h"
#include "calibration/node_id_store.h"

class SensorService
{
public:
    SensorService();

    void begin();
    void loop();

private:
    enum class State
    {
        IDLE,     // waiting for a request
        WARMUP,   // power just applied, waiting for sensors to settle
        SAMPLING  // collecting samples (non-blocking)
    };

    soilSensor soil1;
    soilSensor soil2;
    soilSensor soil3;
    soilSensor soil4;
    Rs485 rs485;
    CalibrationStore calibrationStore;
    NodeIdStore nodeIdStore;

    State state = State::IDLE;
    bool requestPending = false;
    unsigned long stateStartTime = 0;

    // Calibration mode is a separate, exclusive mode: while active it
    // holds both power rails on continuously and bypasses the normal
    // IDLE/WARMUP/SAMPLING cycle entirely, so the two can't fight over
    // the same power pins. See handleCalibrationCommand().
    bool calibrating = false;
    unsigned long lastCalActivityMs = 0;

    // sensorIndex is 1-4 (matches the "K2:DRY:1" wire format, and the
    // node's own soil1..soil4 pin labels). Returns nullptr if out of
    // range.
    soilSensor* sensorByIndex(uint8_t sensorIndex);

    // True if this sensor's saved dry/wet points differ from the
    // firmware defaults -- used by the INFO command to flag sensors
    // that still haven't been calibrated.
    bool isSensorCalibrated(uint8_t sensorIndex) const;

    void enterCalibrationMode();
    void exitCalibrationMode();
    void handleCalibrationCommand(const char* rest);
};
