#include "UART.h"

void UART::begin(long baud)
{
    Serial.begin(baud);
}

bool UART::requestReceived()
{
    String line;

    while (Serial.available() > 0)
    {
        const char incoming = static_cast<char>(Serial.read());
        if (incoming == '\n' || incoming == '\r')
        {
            line.trim();
            return line.equalsIgnoreCase("R") || line.equalsIgnoreCase("REQ");
        }

        line += incoming;
    }

    return false;
}

void UART::sendSoil(int soil1, int soil2)
{
    Serial.print(F("SensorPod:"));
    Serial.print(soil1);
    Serial.print(',');
    Serial.println(soil2);
    Serial.flush();
}