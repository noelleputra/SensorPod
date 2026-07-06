#pragma once

#include <Arduino.h>

class soilSensor
{
public:
    soilSensor(uint8_t analogPin, uint8_t powerPin);
    void begin();
    int readRaw();
    uint8_t readPercent();
    
private:
    const uint8_t analogPin;
    const uint8_t powerPin;
};