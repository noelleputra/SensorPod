#include "sensor_service.h"
#include "config/config.h"
#include "config/pin.h"

SensorService::SensorService()
    : soil1(pin::SOIL1, pin::SOIL_POWER),
      soil2(pin::SOIL2, pin::SOIL_POWER)
{
}

void SensorService::begin() {
    soil1.begin();
    soil2.begin();
    rs485.begin(config::UART_BAUD, pin::RS485Dir);
}

void SensorService::loop() {
    if (rs485.requestReceived())
    {
        soil1Percent = soil1.readPercent();
        soil2Percent = soil2.readPercent();
        DEBUG_PRINT(F("Request Received. S1: "));
        DEBUG_PRINT(soil1Percent);
        DEBUG_PRINT(F(" | S2: "));
        DEBUG_PRINTLN(soil2Percent);
        rs485.sendPacket(soil1Percent, soil2Percent);
    }
}

