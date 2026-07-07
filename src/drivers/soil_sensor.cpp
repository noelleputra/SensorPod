#include "soil_sensor.h"

#include "config/config.h"
#include "config/sensor.h"

soilSensor::soilSensor(uint8_t analogPin)
    : analogPin(analogPin)
{
}

void soilSensor::begin()
{
    pinMode(analogPin, INPUT);
    analogReference(DEFAULT);
}

void soilSensor::beginAsync()
{
    sampleCount = 0;
    lastSampleTime = millis();
}

void soilSensor::takeSample()
{
    if (sampleCount >= sensor::SENSOR_SAMPLE)
    {
        return; // already done, nothing to do
    }

    const unsigned long now = millis();

    // First sample fires immediately (lastSampleTime was just set in
    // beginAsync()); subsequent samples wait SENSOR_SAMPLE_DELAY_MS apart.
    if (sampleCount != 0 && (now - lastSampleTime) < sensor::SENSOR_SAMPLE_DELAY_MS)
    {
        return;
    }

    samples[sampleCount++] = static_cast<uint16_t>(analogRead(analogPin));
    lastSampleTime = now;
}

bool soilSensor::isDone() const
{
    return sampleCount >= sensor::SENSOR_SAMPLE;
}

uint8_t soilSensor::computePercent()
{
    // Simple insertion sort (small, fixed-size array).
    for (size_t i = 0; i < static_cast<size_t>(sensor::SENSOR_SAMPLE) - 1u; ++i)
    {
        for (size_t j = i + 1u; j < static_cast<size_t>(sensor::SENSOR_SAMPLE); ++j)
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
    for (size_t i = static_cast<size_t>(sensor::SENSOR_DISCARD_LOW);
         i < static_cast<size_t>(sensor::SENSOR_SAMPLE) - static_cast<size_t>(sensor::SENSOR_DISCARD_HIGH);
         ++i)
    {
        total += samples[i];
    }

    const size_t validCount = static_cast<size_t>(sensor::SENSOR_SAMPLE)
        - static_cast<size_t>(sensor::SENSOR_DISCARD_LOW)
        - static_cast<size_t>(sensor::SENSOR_DISCARD_HIGH);

    const int raw = static_cast<int>(total / validCount);

    int percent = map(raw, sensor::ADC_DRY, sensor::ADC_WET, 0, 100);
    percent = constrain(percent, 0, 100);

    cachedPercent = static_cast<uint8_t>(percent);
    return cachedPercent;
}

uint8_t soilSensor::getPercent() const
{
    return cachedPercent;
}
