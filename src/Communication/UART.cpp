#include "UART.h"

void UART::begin(long baud)
{
    Serial.begin(baud);
    delay(50);
    Serial.println(F("UART ready"));
}

bool UART::requestReceived()
{
    static String line;

    while (Serial.available() > 0)
    {
        const char incoming = static_cast<char>(Serial.read());
        // debug: show incoming byte as printable and hex
        if (incoming >= ' ' && incoming <= '~')
        {
            Serial.print(F("RX: "));
            Serial.println(incoming);
        }
        else
        {
            Serial.print(F("RX (hex): 0x"));
            if ((uint8_t)incoming < 16) Serial.print('0');
            Serial.println((uint8_t)incoming, HEX);
        }

        if (incoming == '\n' || incoming == '\r')
        {
            line.trim();
            Serial.print(F("Line received: "));
            Serial.println(line);
            const bool matched = line.equalsIgnoreCase("R") || line.equalsIgnoreCase("REQ");
            line.clear();
            return matched;
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