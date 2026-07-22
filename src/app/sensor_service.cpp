#include "sensor_service.h"
#include <string.h>
#include <stdlib.h>
#include "config/config.h"
#include "config/pin.h"
#include "config/sensor.h"
#include "protocol/protocol.h"

SensorService::SensorService()
    : soil1(pin::SOIL1),
      soil2(pin::SOIL2),
      soil3(pin::SOIL3),
      soil4(pin::SOIL4)
{
}

void SensorService::begin()
{
    soil1.begin();
    soil2.begin();
    soil3.begin();
    soil4.begin();

    pinMode(pin::SOIL_POWER_1_2, OUTPUT);
    digitalWrite(pin::SOIL_POWER_1_2, LOW);
    pinMode(pin::SOIL_POWER_3_4, OUTPUT);
    digitalWrite(pin::SOIL_POWER_3_4, LOW);

    rs485.begin(config::UART_BAUD, pin::RS485_EN_PIN);

    // Load this unit's persistent node id (seeds EEPROM with
    // DEFAULT_NODE_ID on a factory-fresh board -- see node_id_store.h).
    nodeIdStore.begin(config::DEFAULT_NODE_ID);

    // Load any previously-saved per-sensor calibration (falls back to
    // firmware defaults internally if none was ever saved) and apply it
    // to each sensor so computePercent() uses the right dry/wet points
    // from the very first read after boot.
    calibrationStore.begin();
    const calibration::Data& cal = calibrationStore.data();
    soil1.setCalibration(cal.dryAdc[0], cal.wetAdc[0]);
    soil2.setCalibration(cal.dryAdc[1], cal.wetAdc[1]);
    soil3.setCalibration(cal.dryAdc[2], cal.wetAdc[2]);
    soil4.setCalibration(cal.dryAdc[3], cal.wetAdc[3]);
}

soilSensor* SensorService::sensorByIndex(uint8_t sensorIndex)
{
    switch (sensorIndex)
    {
        case 1: return &soil1;
        case 2: return &soil2;
        case 3: return &soil3;
        case 4: return &soil4;
        default: return nullptr;
    }
}

bool SensorService::isSensorCalibrated(uint8_t sensorIndex) const
{
    // "Calibrated" here just means "differs from the firmware-default
    // dry/wet points" -- a cheap, honest proxy: a sensor that has never
    // had K<id>:DRY/WET run on it still sits exactly on
    // sensor::ADC_DRY/ADC_WET, so this tells the user at a glance which
    // of the 4 sensors on this board still need attention.
    if (sensorIndex < 1 || sensorIndex > calibration::SENSOR_COUNT) return false;
    const calibration::Data& cal = calibrationStore.data();
    const uint8_t i = sensorIndex - 1;
    return cal.dryAdc[i] != sensor::ADC_DRY || cal.wetAdc[i] != sensor::ADC_WET;
}

void SensorService::enterCalibrationMode()
{
    // Both rails on and held on for the whole session -- unlike the
    // normal read cycle, which powers up right before sampling and back
    // down right after. Calibration needs the sensor continuously live
    // so the user can watch readings settle in real time while moving
    // the probe between dry air and water.
    digitalWrite(pin::SOIL_POWER_1_2, HIGH);
    digitalWrite(pin::SOIL_POWER_3_4, HIGH);
    calibrating = true;
    DEBUG_PRINTLN(F("CAL: entered calibration mode, power on"));
}

void SensorService::exitCalibrationMode()
{
    digitalWrite(pin::SOIL_POWER_1_2, LOW);
    digitalWrite(pin::SOIL_POWER_3_4, LOW);
    calibrating = false;
    DEBUG_PRINTLN(F("CAL: exited calibration mode, power off"));
}

void SensorService::handleCalibrationCommand(const char* rest)
{
    if (!calibrating)
    {
        enterCalibrationMode();
    }
    lastCalActivityMs = millis();

    // Sized for the worst case across all CAL replies -- INFO (node id +
    // firmware date/time + uptime + per-sensor cal status) is now the
    // longest one, with real headroom past its worst case, not sitting
    // right on the edge of truncation.
    char reply[100];

    if (strcmp(rest, "RESET") == 0)
    {
        calibrationStore.resetToDefaults();
        const calibration::Data& cal = calibrationStore.data();
        soil1.setCalibration(cal.dryAdc[0], cal.wetAdc[0]);
        soil2.setCalibration(cal.dryAdc[1], cal.wetAdc[1]);
        soil3.setCalibration(cal.dryAdc[2], cal.wetAdc[2]);
        soil4.setCalibration(cal.dryAdc[3], cal.wetAdc[3]);
        snprintf(reply, sizeof(reply), "%s%u:CAL:RESET:OK", protocol::PREFIX, nodeIdStore.getNodeId());
        rs485.sendLine(reply);
        return;
    }

    if (strcmp(rest, "DUMP") == 0)
    {
        const calibration::Data& cal = calibrationStore.data();
        snprintf(reply, sizeof(reply), "%s%u:CAL:ID=%u;1=%u,%u;2=%u,%u;3=%u,%u;4=%u,%u",
                 protocol::PREFIX, nodeIdStore.getNodeId(),
                 nodeIdStore.getNodeId(),
                 cal.dryAdc[0], cal.wetAdc[0],
                 cal.dryAdc[1], cal.wetAdc[1],
                 cal.dryAdc[2], cal.wetAdc[2],
                 cal.dryAdc[3], cal.wetAdc[3]);
        rs485.sendLine(reply);
        return;
    }

    if (strcmp(rest, "INFO") == 0)
    {
        // General node health/identity summary -- separate from DUMP
        // (raw calibration numbers) so each command has one clear job.
        // Uptime is plain millis(), so it wraps back to 0 after ~49.7
        // days -- a low reading right after you'd expect a high one is
        // itself useful information (the node restarted), wraparound
        // just means don't treat a small value as proof of a FRESH
        // restart on a node that's been running for months uninterrupted.
        snprintf(reply, sizeof(reply), "%s%u:INFO:ID=%u;FW=%s %s;UP=%lu;CAL=1:%s,2:%s,3:%s,4:%s",
                 protocol::PREFIX, nodeIdStore.getNodeId(),
                 nodeIdStore.getNodeId(),
                 __DATE__, __TIME__,
                 millis() / 1000UL,
                 isSensorCalibrated(1) ? "CAL" : "DEF",
                 isSensorCalibrated(2) ? "CAL" : "DEF",
                 isSensorCalibrated(3) ? "CAL" : "DEF",
                 isSensorCalibrated(4) ? "CAL" : "DEF");
        rs485.sendLine(reply);
        return;
    }

    if (strcmp(rest, "EXIT") == 0)
    {
        exitCalibrationMode();
        snprintf(reply, sizeof(reply), "%s%u:CAL:EXIT:OK", protocol::PREFIX, nodeIdStore.getNodeId());
        rs485.sendLine(reply);
        return;
    }

    // Remaining valid forms are "SETID:<n>", "RAW:<n>", "DRY:<n>",
    // "WET:<n>" -- all share the same "<ACTION>:<number>" shape, so parse
    // the action once and branch on it.
    const char* colon = strchr(rest, ':');
    if (!colon)
    {
        return; // malformed command, ignore silently
    }

    char action[8];
    const size_t actionLen = static_cast<size_t>(colon - rest);
    if (actionLen >= sizeof(action))
    {
        return;
    }
    memcpy(action, rest, actionLen);
    action[actionLen] = '\0';

    if (strcmp(action, "SETID") == 0)
    {
        // Reconfigures this unit's persistent node id. Addressed using
        // whatever id this node is CURRENTLY responding as (matched
        // already, before this function ran) -- the reply below reports
        // the NEW id, since that's this node's identity from now on.
        // IMPORTANT: only ever have ONE unconfigured/to-be-renumbered
        // board connected to the bus at a time when doing this -- two
        // boards both still on the same default id would both answer
        // the same SETID command at once and collide.
        const int newId = atoi(colon + 1);
        if (newId < 1 || newId > 254)
        {
            return; // out-of-range id, ignore silently
        }
        nodeIdStore.setNodeId(static_cast<uint8_t>(newId));
        snprintf(reply, sizeof(reply), "%s%u:CAL:SETID:OK", protocol::PREFIX, nodeIdStore.getNodeId());
        rs485.sendLine(reply);
        return;
    }

    const uint8_t sensorIndex = static_cast<uint8_t>(atoi(colon + 1));
    soilSensor* sensor = sensorByIndex(sensorIndex);
    if (!sensor)
    {
        return; // out-of-range sensor index, ignore
    }

    const uint16_t raw = sensor->readRawBlocking();

    if (strcmp(action, "RAW") == 0)
    {
        snprintf(reply, sizeof(reply), "%s%u:CAL:%u:RAW:%u",
                 protocol::PREFIX, nodeIdStore.getNodeId(), sensorIndex, raw);
    }
    else if (strcmp(action, "DRY") == 0)
    {
        calibrationStore.setDry(sensorIndex - 1, raw);
        const calibration::Data& cal = calibrationStore.data();
        sensor->setCalibration(cal.dryAdc[sensorIndex - 1], cal.wetAdc[sensorIndex - 1]);
        snprintf(reply, sizeof(reply), "%s%u:CAL:%u:DRY:%u:OK",
                 protocol::PREFIX, nodeIdStore.getNodeId(), sensorIndex, raw);
    }
    else if (strcmp(action, "WET") == 0)
    {
        calibrationStore.setWet(sensorIndex - 1, raw);
        const calibration::Data& cal = calibrationStore.data();
        sensor->setCalibration(cal.dryAdc[sensorIndex - 1], cal.wetAdc[sensorIndex - 1]);
        snprintf(reply, sizeof(reply), "%s%u:CAL:%u:WET:%u:OK",
                 protocol::PREFIX, nodeIdStore.getNodeId(), sensorIndex, raw);
    }
    else
    {
        return; // unknown action, ignore silently
    }

    rs485.sendLine(reply);
}

void SensorService::loop()
{
    char line[config::UART_BUFFER_SIZE];
    if (rs485.receiveLine(line, sizeof(line)))
    {
        char rest[16];
        if (protocol::parseNodeCommand(line, protocol::REQUEST[0], nodeIdStore.getNodeId(), rest, sizeof(rest)))
        {
            requestPending = true;
        }
        else if (protocol::parseNodeCommand(line, protocol::CALIBRATION[0], nodeIdStore.getNodeId(), rest, sizeof(rest)))
        {
            handleCalibrationCommand(rest);
        }
    }

    const unsigned long now = millis();

    // Calibration mode takes exclusive control of the power rails and
    // completely bypasses the IDLE/WARMUP/SAMPLING cycle below -- the
    // two must never run at the same time or they'd fight over the same
    // pins. Auto-exits after CALIBRATION_TIMEOUT_MS of no calibration
    // commands, so a forgotten session doesn't leave the sensors
    // energized indefinitely.
    if (calibrating)
    {
        if (now - lastCalActivityMs > sensor::CALIBRATION_TIMEOUT_MS)
        {
            exitCalibrationMode();
        }
        return;
    }

    switch (state)
    {
    case State::IDLE:
        if (requestPending)
        {
            // Both power rails must come up together: soil3/soil4 sit on
            // SOIL_POWER_3_4, which previously was only ever set LOW
            // (never powered), so those two sensors always read floating
            // ADC noise instead of real moisture.
            digitalWrite(pin::SOIL_POWER_1_2, HIGH);
            digitalWrite(pin::SOIL_POWER_3_4, HIGH);
            stateStartTime = now;
            state = State::WARMUP;
        }
        break;

    case State::WARMUP:
        if (now - stateStartTime >= sensor::SENSOR_WARMUP_MS)
        {
            soil1.beginAsync();
            soil2.beginAsync();
            soil3.beginAsync();
            soil4.beginAsync();
            state = State::SAMPLING;
        }
        break;

    case State::SAMPLING:
        soil1.takeSample();
        soil2.takeSample();
        soil3.takeSample();
        soil4.takeSample();

        if (soil1.isDone() && soil2.isDone() && soil3.isDone() && soil4.isDone())
        {
            digitalWrite(pin::SOIL_POWER_1_2, LOW);
            digitalWrite(pin::SOIL_POWER_3_4, LOW);

            const uint8_t soil1Percent = soil1.computePercent();
            const uint8_t soil2Percent = soil2.computePercent();
            const uint8_t soil3Percent = soil3.computePercent();
            const uint8_t soil4Percent = soil4.computePercent();

            DEBUG_PRINT(F("DEBUG: "));
            DEBUG_PRINT(soil1Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil2Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil3Percent);
            DEBUG_PRINT(F(","));
            DEBUG_PRINTLN(soil4Percent);
            

            const protocol::SensorPacket packet{nodeIdStore.getNodeId(), soil1Percent, soil2Percent, soil3Percent, soil4Percent};
            rs485.sendPacket(packet);

            requestPending = false;
            state = State::IDLE;
        }
        break;
    }
}
