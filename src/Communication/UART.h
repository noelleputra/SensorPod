#pragma once

#include <Arduino.h>
#include "Config.h"

class UART
{
public:
    void begin(
        long baud,
        uint8_t dirPin = Config::NO_DIRECTION_PIN
    );
    bool requestReceived();
    void sendPacket(
        uint8_t soil1,
        uint8_t soil2
    );

private:
    void setTransmitMode();
    void setReceiveMode();
    uint8_t dirPin = 0xFF;
};