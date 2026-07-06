#include <Arduino.h>
#include "drivers/soil_sensor.h"
#include "communication/rs485.h"
#include "config.h"

class SensorService
{
public:
    SensorService();

    void begin();
    void loop();

private:
    soilSensor soil1;
    soilSensor soil2;
    Rs485 rs485;

    uint8_t soil1Percent = 0;
    uint8_t soil2Percent = 0;
};