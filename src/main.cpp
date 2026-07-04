#include <Arduino.h>
#include "Application/Node.h"

Node node;
void setup() {
  node.begin();
}

void loop() {
  node.loop();
}