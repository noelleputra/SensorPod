#include "node_id_store.h"

#include <EEPROM.h>

void NodeIdStore::begin(uint8_t defaultNodeId)
{
    EEPROM.get(node_id::EEPROM_ADDRESS, data_);

    if (data_.magic != node_id::MAGIC)
    {
        // Fresh board, or EEPROM never written by this system -- seed
        // with the compile-time default and save it, so subsequent
        // boots load this same value until setNodeId() is called.
        data_.magic = node_id::MAGIC;
        data_.nodeId = defaultNodeId;
        save();
    }
}

void NodeIdStore::setNodeId(uint8_t newNodeId)
{
    if (newNodeId < 1 || newNodeId > 254) return; // reject reserved/invalid values
    data_.nodeId = newNodeId;
    save();
}

void NodeIdStore::save()
{
    EEPROM.put(node_id::EEPROM_ADDRESS, data_);
}
