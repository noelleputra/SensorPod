#include <Arduino.h>

namespace Config {
constexpr uint8_t SENSOR_COUNT = 2;
constexpr uint8_t SENSOR_SAMPLE = 10;
constexpr uint16_t SENSOR_WARMUP = 50;
constexpr uint16_t SENSOR_INTERVAL = 1000;
constexpr uint16_t DEBUG_INTERVAL = 1000;
constexpr uint16_t UART_BAUD = 9600;
}