#include "sensor_service.h"
#include "config/config.h"
#include "config/pin.h"
#include "config/sensor.h"
#include "protocol/protocol.h"

SensorService::SensorService()
    : soil1(pin::SOIL1),
      soil2(pin::SOIL2),
      soil3(pin::SOIL3),
      soil4(pin::SOIL4)
{
}

void SensorService::begin()
{
    soil1.begin();
    soil2.begin();
    soil3.begin();
    soil4.begin();

    pinMode(pin::SOIL_POWER_1_2, OUTPUT);
    digitalWrite(pin::SOIL_POWER_1_2, LOW);
    pinMode(pin::SOIL_POWER_3_4, OUTPUT);
    digitalWrite(pin::SOIL_POWER_3_4, LOW);

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
            // Both power rails must come up together: soil3/soil4 sit on
            // SOIL_POWER_3_4, which previously was only ever set LOW
            // (never powered), so those two sensors always read floating
            // ADC noise instead of real moisture.
            digitalWrite(pin::SOIL_POWER_1_2, HIGH);
            digitalWrite(pin::SOIL_POWER_3_4, HIGH);
            stateStartTime = now;
            state = State::WARMUP;
        }
        break;

    case State::WARMUP:
        if (now - stateStartTime >= sensor::SENSOR_WARMUP_MS)
        {
            soil1.beginAsync();
            soil2.beginAsync();
            soil3.beginAsync();
            soil4.beginAsync();
            state = State::SAMPLING;
        }
        break;

    case State::SAMPLING:
        soil1.takeSample();
        soil2.takeSample();
        soil3.takeSample();
        soil4.takeSample();

        if (soil1.isDone() && soil2.isDone() && soil3.isDone() && soil4.isDone())
        {
            digitalWrite(pin::SOIL_POWER_1_2, LOW);
            digitalWrite(pin::SOIL_POWER_3_4, LOW);

            const uint8_t soil1Percent = soil1.computePercent();
            const uint8_t soil2Percent = soil2.computePercent();
            const uint8_t soil3Percent = soil3.computePercent();
            const uint8_t soil4Percent = soil4.computePercent();

            DEBUG_PRINT(F("DEBUG: "));
            DEBUG_PRINT(soil1Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil2Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil3Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil4Percent);
            

            const protocol::SensorPacket packet{config::NODE_ID, soil1Percent, soil2Percent, soil3Percent, soil4Percent};
            rs485.sendPacket(packet);

            requestPending = false;
            state = State::IDLE;
        }
        break;
    }
}
