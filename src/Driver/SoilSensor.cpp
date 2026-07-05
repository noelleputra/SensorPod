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
    analogReference(DEFAULT);
}

int SoilSensor::readRaw()
{
    digitalWrite(powerPin, HIGH);
    delay(Config::SENSOR_WARMUP_MS);

    uint16_t samples[Config::SENSOR_SAMPLE] = {0};
    for (uint8_t i = 0; i < Config::SENSOR_SAMPLE; ++i)
    {
        samples[i] = static_cast<uint16_t>(analogRead(analogPin));
        if (i < Config::SENSOR_SAMPLE - 1)
        {
            delay(Config::SENSOR_SAMPLE_DELAY_MS);
        }
    }

    digitalWrite(powerPin, LOW);

    // Discard the lowest and highest values to reduce noise influence.
    for (uint8_t i = 0; i < Config::SENSOR_SAMPLE - 1; ++i)
    {
        for (uint8_t j = i + 1; j < Config::SENSOR_SAMPLE; ++j)
        {
            if (samples[j] < samples[i])
            {
                uint16_t temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }

    uint32_t total = 0;
    for (uint8_t i = Config::SENSOR_DISCARD_LOW; i < Config::SENSOR_SAMPLE - Config::SENSOR_DISCARD_HIGH; ++i)
    {
        total += samples[i];
    }

    const uint8_t validCount = Config::SENSOR_SAMPLE - Config::SENSOR_DISCARD_LOW - Config::SENSOR_DISCARD_HIGH;
    return static_cast<int>(total / validCount);
}

uint8_t SoilSensor::readPercent()
{
    const int raw = readRaw();
    int percent = map(raw, Config::ADC_DRY, Config::ADC_WET, 0, 100);
    percent = constrain(percent, 0, 100);
    return static_cast<uint8_t>(percent);
}