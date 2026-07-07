#include "rs485.h"
#include "config/config.h"
#include "protocol/protocol.h"

namespace {
    bool isRequestForThisNode(const char *line) {
        if (line[0] != protocol::REQUEST[0] || line[1] == '\0') {
            return false;
        }
        uint8_t requestedId = 0;
        for (const char *p = &line[1]; *p != '\0'; ++p) {
            if (*p < '0' || *p > '9') return false;
            uint8_t digit = static_cast<uint8_t>(*p - '0');
            if (requestedId > (255 - digit) / 10) return false; 
            requestedId = requestedId * 10 + digit;
        }
        return requestedId == config::NODE_ID;
    }
}

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

bool Rs485::requestReceived() {
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
            return isRequestForThisNode(line);
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
        return isRequestForThisNode(line);
    }

    return false;
}

void Rs485::sendPacket(uint8_t soil1, uint8_t soil2) {
    setTransmitMode();

    Serial.print(protocol::PREFIX);
    Serial.print(config::NODE_ID);
    Serial.write(':');
    Serial.print(soil1);
    Serial.write(',');
    Serial.print(soil2);
    Serial.write('\r');
    Serial.write('\n');

    Serial.flush();
    delayMicroseconds(config::RS485_TURNAROUND_US);

    setReceiveMode();
}