#include "Node.h"
#include "Debug.h"
#include "Config.h"
#include <avr/wdt.h>

Node::Node()
    : soil1(Pins::Soil1, Pins::SoilPower1),
      soil2(Pins::Soil2, Pins::SoilPower2)
{
}

void Node::begin()
{
    wdt_enable(WDTO_8S);
    soil1.begin();
    soil2.begin();
    uart.begin(Config::UART_BAUD);
    delay(50);
    Serial.println(F("SensorPod Node started"));
}

void Node::loop()
{
    if (uart.requestReceived())
    {
        soil1Value = soil1.readPercent();
        soil2Value = soil2.readPercent();

        DEBUG_PRINT(F("Request diterima. S1: "));
        DEBUG_PRINT(soil1Value);
        DEBUG_PRINT(F(" | S2: "));
        DEBUG_PRINTLN(soil2Value);

        uart.sendSoil(soil1Value, soil2Value);
    }

    wdt_reset();
}

