#include "rs485.h"
#include <string.h>
#include "config/config.h"
#include "protocol/protocol.h"

void Rs485::begin(long baud, uint8_t enPin) {
    this->enPin = enPin;
    pinMode(enPin, OUTPUT);
    setReceiveMode();
    Serial.begin(baud);
    delay(100);
}

void Rs485::setTransmitMode() {
    digitalWrite(enPin, HIGH);
    delayMicroseconds(config::RS485_TURNAROUND_US);
}

void Rs485::setReceiveMode() {
    digitalWrite(enPin, LOW);
    delayMicroseconds(config::RS485_TURNAROUND_US);
}

bool Rs485::receiveLine(char* buf, size_t bufSize) {
    static char line[config::UART_BUFFER_SIZE];
    static size_t index = 0;
    static unsigned long lastByteTime = 0;
    static bool overflowed = false;

    while (Serial.available()) {
        char incoming = Serial.read();
        if (incoming == '\r') continue;

        if (incoming == '\n') {
            const bool wasOverflowed = overflowed;
            overflowed = false;

            if (index == 0 && !wasOverflowed) {
                lastByteTime = 0;
                continue;
            }

            line[index] = '\0';
            index = 0;
            lastByteTime = 0;

            // Line exceeded buffer capacity: it's malformed/truncated,
            // discard it instead of parsing a partial, misleading string.
            if (wasOverflowed) {
                return false;
            }
            strncpy(buf, line, bufSize - 1);
            buf[bufSize - 1] = '\0';
            return true;
        }

        if (!overflowed) {
            if (index < (sizeof(line) - 1)) {
                line[index++] = incoming;
            } else {
                // Buffer full: mark overflow and keep consuming bytes
                // until the next '\n', instead of resetting mid-token
                // (which would let the tail of this line be misread as
                // the start of the next command).
                overflowed = true;
            }
        }

        lastByteTime = millis();
    }

    if (index > 0 && (millis() - lastByteTime) > 20) {
        line[index] = '\0';
        index = 0;
        const bool wasOverflowed = overflowed;
        overflowed = false;
        lastByteTime = 0;
        if (wasOverflowed) {
            return false;
        }
        strncpy(buf, line, bufSize - 1);
        buf[bufSize - 1] = '\0';
        return true;
    }

    return false;
}

void Rs485::sendLine(const char* line) {
    setTransmitMode();

    Serial.print(line);
    Serial.write('\r');
    Serial.write('\n');

    Serial.flush();
    delayMicroseconds(config::RS485_TURNAROUND_US);

    setReceiveMode();
}

void Rs485::sendPacket(const protocol::SensorPacket &packet) {
    char line[32];
    snprintf(line, sizeof(line), "%s%u:%u,%u,%u,%u",
             protocol::PREFIX, packet.nodeId,
             packet.soil1, packet.soil2, packet.soil3, packet.soil4);
    sendLine(line);
}
