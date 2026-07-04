#include "Node.h"
#include "Debug.h"
#include "Config.h"

Node::Node()
    : soil1(Pins::Soil1, Pins::SoilPower1),
      soil2(Pins::Soil2, Pins::SoilPower2)
{
}

void Node::begin()
{
    soil1.begin();
    soil2.begin();
    uart.begin(Config::UART_BAUD);
}

void Node::loop()
{
    if (millis() - lastRead >= Config::SENSOR_INTERVAL_MS)
    {
        lastRead = millis();
        soil1Value = soil1.readPercent();
        soil2Value = soil2.readPercent();

        DEBUG_PRINT(F("S1: "));
        DEBUG_PRINT(soil1Value);
        DEBUG_PRINT(F(" | S2: "));
        DEBUG_PRINTLN(soil2Value);
    }

    if (uart.requestReceived())
    {
        uart.sendSoil(
            soil1Value,
            soil2Value
        );
    }
}

