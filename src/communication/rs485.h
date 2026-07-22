#pragma once

#include <Arduino.h>
#include "config/config.h"
#include "protocol/protocol.h"

class Rs485 {
public:
    void begin(long baud, uint8_t enPin);

    // Returns true once a complete line has been received (terminated
    // by '\n', '\r' ignored, overflow-safe -- same framing as before).
    // Writes the null-terminated line into buf (truncated to bufSize-1
    // chars if needed). Unlike the old requestReceived(), this does NOT
    // filter by node id or command type -- callers decide what a line
    // means (see protocol::parseNodeCommand), so this same line stream
    // can carry both normal read requests ("R2") and calibration
    // commands ("K2:DRY:1") without Rs485 needing to know about either.
    bool receiveLine(char* buf, size_t bufSize);

    void sendPacket(const protocol::SensorPacket &packet);

    // Sends an arbitrary line (CRLF-terminated), wrapped in the same
    // transmit/receive turnaround as sendPacket(). Used for calibration
    // replies (raw readings, confirmations) that don't fit the
    // fixed SensorPacket shape.
    void sendLine(const char* line);

private:
    void setTransmitMode();
    void setReceiveMode();
    uint8_t enPin;
};
