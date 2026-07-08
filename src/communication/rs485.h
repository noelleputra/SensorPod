#pragma once

#include <Arduino.h>
#include "config/config.h"
#include "protocol/packet.h"

class Rs485 {
public:
    void begin(long baud, uint8_t enPin);
    bool requestReceived();
    void sendPacket(const protocol::SensorPacket &packet);

private:
    void setTransmitMode();
    void setReceiveMode();
    uint8_t enPin;
};
