#pragma once
#include <Arduino.h>

namespace Pins
{
    constexpr uint8_t Soil1 = A0;
    constexpr uint8_t Soil2 = A1;
    constexpr uint8_t SoilPower1 = 2;
    constexpr uint8_t SoilPower2 = 3;
    constexpr uint8_t RS485Dir = 4; // DE/RE pin for MAX3485, set low for receive, high for transmit
}