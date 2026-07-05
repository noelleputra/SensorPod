#pragma once

#include <Arduino.h>

class UART
{
public:
    void begin(
        long baud,
        uint8_t dirPin = 0xFF
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