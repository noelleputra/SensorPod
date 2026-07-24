# 🎛️ SensorPod

> A super-modular IoT sensor node built around the ATmega328, designed for Smart Irrigation / Smart Farming with RS485 communication.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ATmega328-green.svg)
![Framework](https://img.shields.io/badge/framework-Arduino%20-orange.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)

## 📖 Overview

**SensorPod** is an affordable, modular Smart Irrigation system optimized for farming operations, particularly in Indonesia. It features 4 capacitive soil moisture sensors connected via RS485 communication protocol, enabling multi-node deployments in distributed IoT networks.

## ⚙️ Hardware Specifications

| Component | Specification |
|-----------|---------------|
| **Microcontroller** | ATmega328 Pro Mini (3.3V, 8MHz) |
| **Soil Moisture Sensors** | 4x Capacitive Soil Moisture Sensor V2 |
| **Communication** | RS485 Protocol (MAX3485) |
| **Power Supply** | 3-12V input with 3V regulator |
| **UART Baud Rate** | 9600 baud |
| **Buffer Size** | 32 bytes |

## 🚀 Core Features

- ✅ **4 Soil Moisture Sensors** - Per-sensor calibration to compensate for component tolerance drift
- ✅ **RS485 Multi-Node Communication** - Connect up to 254 nodes on a single bus
- ✅ **Configurable Node IDs** - Persistent storage via EEPROM
- ✅ **Per-Sensor Calibration** - Individual dry/wet point adjustment for each sensor
- ✅ **Low Power Design** - Sensor power rails toggle on/off for efficiency
- ✅ **Non-blocking Async Sampling** - 10-sample averaging with outlier rejection
- ✅ **Watchdog Timer** - 8-second reset protection
- ✅ **EEPROM Persistence** - Calibration data and node ID stored permanently

## 📁 Project Structure


## 🔌 Pin Configuration

| Pin | Function | Port |
|-----|----------|------|
| A0 | Soil Sensor 1 (Analog Input) | - |
| A1 | Soil Sensor 2 (Analog Input) | - |
| A2 | Soil Sensor 3 (Analog Input) | - |
| A3 | Soil Sensor 4 (Analog Input) | - |
| 2 | Soil Power Rail 1-2 (Digital Out) | - |
| 3 | Soil Power Rail 3-4 (Digital Out) | - |
| 5 | RS485 Enable (Digital Out) | - |

## 📡 Communication Protocol

### Command Format
All commands follow the pattern: `<PREFIX><NODE_ID>[:<PARAMETERS>]`

### Available Commands

#### Read Request
- **Format:** `R<NODE_ID>\r\n`
- **Example:** `R2` (Request reading from node 2)
- **Response:** `N2:45,52,38,41` (Node 2, sensors 1-4 moisture percentages)

#### Calibration Commands
- **Enter Calibration:** `K<NODE_ID>\r\n` (Hold both power rails on)
- **Set Dry Point:** `K<NODE_ID>:DRY:<SENSOR_INDEX>\r\n` (Store raw ADC value)
- **Set Wet Point:** `K<NODE_ID>:WET:<SENSOR_INDEX>\r\n` (Store raw ADC value)
- **Set Node ID:** `K<NODE_ID>:SETID:<NEW_ID>\r\n` (Persist new node ID)
- **Calibration Info:** `K<NODE_ID>:INFO\r\n` (List calibration status)
- **Exit Calibration:** Auto-exit after 5 minutes of inactivity

### Packet Format
```cpp
struct SensorPacket {
    uint8_t nodeId;       // 1-254
    uint8_t soil1;        // Moisture % (0-100)
    uint8_t soil2;        // Moisture % (0-100)
    uint8_t soil3;        // Moisture % (0-100)
    uint8_t soil4;        // Moisture % (0-100)
};

# Activate calibration mode on node 2
K2

# Place sensor 1 in dry air and capture raw value
K2:DRY:1
# Board will output raw ADC reading; note it down

# Place sensor 1 in water and capture raw value
K2:WET:1
# Board will output raw ADC reading

# Repeat for sensors 2, 3, 4 as needed
