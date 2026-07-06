#include <Arduino.h>
#include "app/sensor_service.h"
#include <avr/wdt.h>

SensorService sensorService;

void setup() {
  wdt_enable(WDTO_8S);
  sensorService.begin();
}

void loop() {
  sensorService.loop();
  wdt_reset();
}