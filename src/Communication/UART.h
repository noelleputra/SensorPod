#pragma once

#include <Arduino.h>

class UART
{
public:
    void begin(long baud, uint8_t dirPin);
    bool requestReceived();
    void sendPacket(uint8_t soil1, uint8_t soil2);

private:
    uint8_t dirPin = 255;
    void setTransmitMode();
    void setReceiveMode();
};