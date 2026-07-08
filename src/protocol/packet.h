#pragma once

#include <Arduino.h>

// Defines the *data* being sent, independent of how it gets serialized
// onto the wire. Rs485 (the transport) only needs to know about this
// struct, not about soil sensors or any other hardware detail.
namespace protocol
{
    struct SensorPacket
    {
        uint8_t nodeId;
        uint8_t soil1;
        uint8_t soil2;
    };
}
