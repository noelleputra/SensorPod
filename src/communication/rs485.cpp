#include "rs485.h"
#include "config.h"

namespace {
    bool isRequestForThisNode(const char *line) {
        if (line[0] != config::protocol::REQUEST[0] || line[1] == '\0') {
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

void Rs485::begin(long baud, uint8_t dirPin) {
    this->dirPin = dirPin;
    if (dirPin != config::NO_DIRECTION_PIN) {
        pinMode(dirPin, OUTPUT);
        setReceiveMode();
    }
    Serial.begin(baud);
}

void Rs485::setTransmitMode() {
    if (dirPin != config::NO_DIRECTION_PIN) {
        digitalWrite(dirPin, HIGH);
    }
}

void Rs485::setReceiveMode() {
    if (dirPin != config::NO_DIRECTION_PIN) {
        digitalWrite(dirPin, LOW);
    }
}

bool Rs485::requestReceived() {
    static char line[config::UART_BUFFER_SIZE];
    static size_t index = 0;

    while (Serial.available()) {
        char incoming = Serial.read();
        if (incoming == '\r') continue;
        if (incoming == '\n') {
            if (index == 0) continue;
            line[index] = '\0';
            index = 0;
            return isRequestForThisNode(line);
        }
        if (index < (sizeof(line) - 1)) {
            line[index++] = incoming;
        } else {
            index = 0;
        }
    }
    return false;
}

void Rs485::sendPacket(uint8_t soil1, uint8_t soil2) {
    setTransmitMode();
    delayMicroseconds(config::RS485_TURNAROUND_US);

    Serial.print(config::protocol::PREFIX);
    Serial.print(config::NODE_ID);
    Serial.write(':');
    Serial.print(soil1);
    Serial.write(',');
    Serial.println(soil2);

    Serial.flush();
    delayMicroseconds(config::RS485_TURNAROUND_US);

    setReceiveMode();
}