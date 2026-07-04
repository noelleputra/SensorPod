#include <Arduino.h>
#include "../Driver/SoilSensor.h"
#include "../Communication/UART.h"
#include "../../include/Pins.h"

class Node
{
public:
    Node();

    void begin();
    void loop();

private:
    SoilSensor soil1;
    SoilSensor soil2;
    UART uart;

    uint8_t soil1Value = 0;
    uint8_t soil2Value = 0;

    uint32_t lastRead = 0;
};