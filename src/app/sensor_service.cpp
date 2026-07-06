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

        // Debug output src/config/config.h
        DEBUG_PRINT(F("DEBUG: "));
        DEBUG_PRINT(soil1Percent);
        DEBUG_PRINT(F(","));
        DEBUG_PRINTLN(soil2Percent);

        rs485.sendPacket(soil1Percent, soil2Percent);
    }
}

