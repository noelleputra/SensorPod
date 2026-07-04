#pragma once

#include <Arduino.h>

class UART
{
public:
    void begin(long baud);
    bool requestReceived();
    void sendSoil(int soil1,int soil2);
};