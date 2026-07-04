#include "SoilSensor.h"

#include "../../include/Config.h"

SoilSensor::SoilSensor(
    uint8_t analogPin,
    uint8_t powerPin)

    : analogPin(analogPin),
      powerPin(powerPin)
{
}

void SoilSensor::begin()
{
    pinMode(analogPin, INPUT);
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, LOW);
}

int SoilSensor::readRaw()
{
    digitalWrite(powerPin, HIGH);
    delay(Config::SENSOR_WARMUP_MS);
    long total = 0;
    for (uint8_t i = 0; i < Config::SENSOR_SAMPLE; i++)
    {
        total += analogRead(analogPin);
        delay(5);
    }
    digitalWrite(powerPin, LOW);
    return total / Config::SENSOR_SAMPLE;
}

uint8_t SoilSensor::readPercent()
{
    const int raw = readRaw();
    int percent = map(
        raw,
        Config::ADC_DRY,
        Config::ADC_WET,
        0,
        100);
    percent = constrain(percent, 0, 100);
    return static_cast<uint8_t>(percent);
}