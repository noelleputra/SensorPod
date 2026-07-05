#include "UART.h"

#include "Config.h"
#include "Protocol.h"

void UART::begin(long baud, uint8_t dirPin)
{
    this->dirPin = dirPin;

    if (dirPin != Config::NO_DIRECTION_PIN)
    {
        pinMode(dirPin, OUTPUT);
        setReceiveMode();
    }

    Serial.begin(baud);
}

void UART::setTransmitMode()
{
    if (dirPin != Config::NO_DIRECTION_PIN)
    {
        digitalWrite(dirPin, HIGH);
    }
}

void UART::setReceiveMode()
{
    if (dirPin != Config::NO_DIRECTION_PIN)
    {
        digitalWrite(dirPin, LOW);
    }
}

bool UART::requestReceived()
{
    static char line[Config::UART_BUFFER_SIZE];
    static uint8_t index = 0;

    while (Serial.available())
    {
        char incoming = Serial.read();

        if (incoming == '\r' || incoming == '\n')
        {
            if (index == 0)
                continue;

            line[index] = '\0';
            index = 0;

            return strcmp(line, Protocol::REQUEST) == 0;
        }

        if (index < Config::UART_BUFFER_SIZE - 1)
        {
            line[index++] = incoming;

            // support single-byte request 'R' without newline / carriage return
            if (index == 1 && line[0] == Protocol::REQUEST[0] && !Serial.available())
            {
                index = 0;
                return true;
            }
        }
        else
        {
            // Overflow protection
            index = 0;
        }
    }

    return false;
}

void UART::sendPacket(uint8_t soil1, uint8_t soil2)
{
    setTransmitMode();

    delayMicroseconds(Config::RS485_TURNAROUND_US);

    Serial.print(Protocol::PREFIX);
    Serial.print(soil1);
    Serial.write(',');
    Serial.println(soil2);

    Serial.flush();

    delayMicroseconds(Config::RS485_TURNAROUND_US);

    setReceiveMode();
}