#include "UART.h"

void UART::begin(long baud)
{
    Serial.begin(baud);
}

bool UART::requestReceived()
{
    while (Serial.available() > 0)
    {
        const char incoming = static_cast<char>(Serial.read());
        if (incoming == 'R')
        {
            return true;
        }
    }

    return false;
}

void UART::sendSoil(int soil1, int soil2)
{
    Serial.print(F("SensorPod:"));
    Serial.print(soil1);
    Serial.print(',');
    Serial.println(soil2);
}