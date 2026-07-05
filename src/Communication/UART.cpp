#include "UART.h"

void UART::begin(long baud, uint8_t dirPin)
{
    this->dirPin = dirPin;
    pinMode(dirPin, OUTPUT);
    setReceiveMode();
    Serial.begin(baud);
}

void UART::setTransmitMode()
{
    if (dirPin != 255)
    {
        digitalWrite(dirPin, HIGH);
    }
}

void UART::setReceiveMode()
{
    if (dirPin != 255)
    {
        digitalWrite(dirPin, LOW);
    }
}

bool UART::requestReceived()
{
    static char line[32];
    static uint8_t index = 0;

    while (Serial.available() > 0)
    {
        const char incoming = static_cast<char>(Serial.read());
        if (incoming == '\r')
        {
            continue;
        }

        if (incoming == '\n')
        {
            line[index] = '\0';
            index = 0;
            if (strcmp(line, "R") == 0 || strcmp(line, "REQ") == 0)
            {
                return true;
            }
            return false;
        }

        if (index < sizeof(line) - 1)
        {
            line[index++] = incoming;
        }
        else
        {
            index = 0;
        }
    }

    return false;
}

void UART::sendPacket(uint8_t soil1, uint8_t soil2)
{
    setTransmitMode();
    delayMicroseconds(10);
    Serial.print("SensorPod:");
    Serial.print(soil1);
    Serial.print(',');
    Serial.println(soil2);
    Serial.flush();
    delayMicroseconds(10);
    setReceiveMode();
}