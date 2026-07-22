#pragma once

#include <Arduino.h>

namespace node_id {
    // Arbitrary marker distinguishing "an id was actually saved here"
    // from blank/erased EEPROM (0xFF per byte). Different value from
    // calibration::MAGIC on purpose -- they're unrelated fields and
    // shouldn't be able to accidentally validate each other.
    constexpr uint8_t MAGIC = 0x5A;

    // Placed well clear of CalibrationStore's data (address 0, 17 bytes:
    // 1 magic + 4x2 dry + 4x2 wet), so the two stores can never overlap
    // even if calibration::Data grows a bit later.
    constexpr int EEPROM_ADDRESS = 32;

    struct Data {
        uint8_t magic;
        uint8_t nodeId;
    };
}

// Persists this unit's RS485 node id across reboots/power loss, so the
// same compiled firmware image can be flashed to every physical
// SensorPod and then given a unique id afterwards over serial, instead
// of needing a separate compile per unit.
class NodeIdStore {
public:
    // Loads from EEPROM. If nothing valid was ever saved (fresh board,
    // or EEPROM this firmware has never written), seeds it with
    // defaultNodeId and saves that as the starting point.
    void begin(uint8_t defaultNodeId);

    uint8_t getNodeId() const { return data_.nodeId; }

    // Rejects values outside 1-254 (0 and 255 are reserved, not used by
    // this system today, but kept free in case a future broadcast/
    // discovery scheme needs a sentinel value).
    void setNodeId(uint8_t newNodeId);

private:
    node_id::Data data_{};
    void save();
};
