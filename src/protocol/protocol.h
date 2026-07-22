#pragma once

#include <Arduino.h>

namespace protocol {
    constexpr char REQUEST[] = "R";
    constexpr char CALIBRATION[] = "C";
    constexpr char PREFIX[]  = "SP";

    struct SensorPacket {
        uint8_t nodeId;
        uint8_t soil1;
        uint8_t soil2;
        uint8_t soil3;
        uint8_t soil4;
    };

    // Parses a line of the form "<prefix><nodeId>" or
    // "<prefix><nodeId>:<rest>" -- e.g. "R2" or "K2:DRY:1". Returns true
    // and copies whatever follows the ':' into restOut (empty string if
    // there was none) only if the line starts with `prefix` and the
    // nodeId digits exactly match expectedNodeId. Shared by both the
    // plain read request ('R') and calibration ('K') commands so the
    // node-id-matching logic isn't duplicated per command type.
    bool parseNodeCommand(const char* line, char prefix, uint8_t expectedNodeId,
                           char* restOut, size_t restOutSize);
}