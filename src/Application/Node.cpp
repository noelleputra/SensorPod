#include "Node.h"

#include "../Driver/SoilSensor.h"
#include "../Communication/UART.h"
#include "../../include/Pins.h"

SoilSensor soil1(
    Pins::Soil1,
    Pins::SoilPower1);
    
SoilSensor soil2(
    Pins::Soil2,
    Pins::SoilPower2);

UART uart;

void Node::begin()
{
    soil1.begin();
    soil2.begin();
    uart.begin(9600);
}
void Node::loop()
{
    if (uart.requestReceived())
    {
        uint8_t soil1Value = soil1.readPercent();
        uint8_t soil2Value = soil2.readPercent();

        Serial.print(F("Soil 1 : "));
        Serial.print(soil1Value);
        Serial.println(F("%"));

        Serial.print(F("Soil 2 : "));
        Serial.print(soil2Value);
        Serial.println(F("%"));

        uart.sendSoil(soil1Value, soil2Value);
    }
}