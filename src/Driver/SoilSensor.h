#pragma once

#include <Arduino.h>

class SoilSensor
{
public:
    SoilSensor(uint8_t analogPin,
               uint8_t powerPin);

    void begin();
    int readPercent();

private:
    uint8_t analogPin;
    uint8_t powerPin;
};