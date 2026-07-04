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
        int soil1Percent = soil1.readPercent();
        int soil2Percent = soil2.readPercent();
        uart.sendSoil(soil1Percent, soil2Percent);
    }
}