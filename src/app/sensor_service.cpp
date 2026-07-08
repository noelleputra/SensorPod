#include "sensor_service.h"
#include "config/config.h"
#include "config/pin.h"
#include "config/sensor.h"
#include "protocol/protocol.h"

SensorService::SensorService()
    : soil1(pin::SOIL1),
      soil2(pin::SOIL2)
{
}

void SensorService::begin()
{
    soil1.begin();
    soil2.begin();

    pinMode(pin::SOIL_POWER, OUTPUT);
    digitalWrite(pin::SOIL_POWER, LOW);

    rs485.begin(config::UART_BAUD, pin::RS485_EN_PIN);
}

void SensorService::loop()
{
    // Keep feeding the RS485 line parser every tick regardless of state,
    // so incoming bytes are never missed while a read cycle is running.
    if (rs485.requestReceived())
    {
        requestPending = true;
    }

    const unsigned long now = millis();

    switch (state)
    {
    case State::IDLE:
        if (requestPending)
        {
            digitalWrite(pin::SOIL_POWER, HIGH);
            stateStartTime = now;
            state = State::WARMUP;
        }
        break;

    case State::WARMUP:
        if (now - stateStartTime >= sensor::SENSOR_WARMUP_MS)
        {
            soil1.beginAsync();
            soil2.beginAsync();
            state = State::SAMPLING;
        }
        break;

    case State::SAMPLING:
        soil1.takeSample();
        soil2.takeSample();

        if (soil1.isDone() && soil2.isDone())
        {
            digitalWrite(pin::SOIL_POWER, LOW);

            const uint8_t soil1Percent = soil1.computePercent();
            const uint8_t soil2Percent = soil2.computePercent();

            DEBUG_PRINT(F("DEBUG: "));
            DEBUG_PRINT(soil1Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil2Percent);

            const protocol::SensorPacket packet{config::NODE_ID, soil1Percent, soil2Percent};
            rs485.sendPacket(packet);

            requestPending = false;
            state = State::IDLE;
        }
        break;
    }
}
