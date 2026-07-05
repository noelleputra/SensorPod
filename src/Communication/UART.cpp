#include "UART.h"

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
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

static bool isRequestForThisNode(const char *line)
{
    if (line[0] != Protocol::REQUEST[0])
        return false;

    // Require R<ID> format; reject plain R broadcast or incomplete commands.
    if (line[1] == '\0')
        return false;

    uint8_t requestedId = 0;
    for (const char *p = &line[1]; *p != '\0'; ++p)
    {
        if (*p < '0' || *p > '9')
            return false;
        requestedId = requestedId * 10 + static_cast<uint8_t>(*p - '0');
    }

    return requestedId == Config::NODE_ID;
}

bool UART::requestReceived()
{
    static char line[Config::UART_BUFFER_SIZE];
    static size_t index = 0;

    while (Serial.available())
    {
        char incoming = Serial.read();

        if (incoming == '\r' || incoming == '\n')
        {
            if (index == 0)
                continue;

            line[index] = '\0';
            index = 0;

            return isRequestForThisNode(line);
        }

        if (index < static_cast<size_t>(Config::UART_BUFFER_SIZE) - 1u)
        {
            line[index++] = incoming;
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
    Serial.print(Config::NODE_ID);
    Serial.write(':');
    Serial.print(soil1);
    Serial.write(',');
    Serial.println(soil2);

    Serial.flush();

    delayMicroseconds(Config::RS485_TURNAROUND_US);

    setReceiveMode();
}