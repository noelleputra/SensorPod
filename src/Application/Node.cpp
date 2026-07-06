#include "node.h"
#include "debug.h"
#include "config.h"

Node::Node()
    : soil1(config::pins::SOIL1, config::pins::SOIL_POWER),
      soil2(config::pins::SOIL2, config::pins::SOIL_POWER)
{
}

void Node::begin()
{
    soil1.begin();
    soil2.begin();
    rs485.begin(config::UART_BAUD, config::pins::RS485Dir);
}

void Node::loop()
{
    if (rs485.requestReceived())
    {
        soil1Value = soil1.readPercent();
        soil2Value = soil2.readPercent();
        DEBUG_PRINT(F("Request Received. S1: "));
        DEBUG_PRINT(soil1Value);
        DEBUG_PRINT(F(" | S2: "));
        DEBUG_PRINTLN(soil2Value);
        rs485.sendPacket(soil1Value, soil2Value);
    }
}

