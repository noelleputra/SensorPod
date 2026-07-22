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

    // Uses THIS sensor's own dry/wet points (set via setCalibration()),
    // not a single shared constant -- this is the actual fix for
    // component-tolerance drift between the 4 physical sensors.
    int percent = map(raw, dryAdc, wetAdc, 0, 100);
    percent = constrain(percent, 0, 100);

    cachedPercent = static_cast<uint8_t>(percent);
    return cachedPercent;
}

uint8_t soilSensor::getPercent() const
{
    return cachedPercent;
}

void soilSensor::setCalibration(uint16_t newDryAdc, uint16_t newWetAdc)
{
    dryAdc = newDryAdc;
    wetAdc = newWetAdc;
}

uint16_t soilSensor::readRawBlocking() const
{
    uint32_t total = 0;
    for (uint8_t i = 0; i < sensor::CALIBRATION_RAW_SAMPLES; ++i)
    {
        total += analogRead(analogPin);
        if (i + 1 < sensor::CALIBRATION_RAW_SAMPLES)
        {
            delay(sensor::CALIBRATION_RAW_SAMPLE_DELAY_MS);
        }
    }
    return static_cast<uint16_t>(total / sensor::CALIBRATION_RAW_SAMPLES);
}
