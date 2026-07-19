#pragma once

#include <Arduino.h>

namespace protocol {
    constexpr char REQUEST[] = "R";
    constexpr char PREFIX[]  = "SP";

    struct SensorPacket {
        uint8_t nodeId;
        uint8_t soil1;
        uint8_t soil2;
        uint8_t soil3;
        uint8_t soil4;
    };
}