#include <Arduino.h>
#include "application/node.h"
#include <avr/wdt.h>

Node node;
void setup() {
  wdt_enable(WDTO_8S);
  node.begin();
}

void loop() {
  node.loop();
  wdt_reset();
}