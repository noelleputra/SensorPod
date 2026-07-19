#pragma once

#include <Arduino.h>
#include "drivers/soil_sensor.h"
#include "communication/rs485.h"

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

    State state = State::IDLE;
    bool requestPending = false;
    unsigned long stateStartTime = 0;
};
