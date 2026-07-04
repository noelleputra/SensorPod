#include "UART.h"
#include "Config.h"

void UART::begin(long baud)
{
    Serial.begin(baud);
}
bool UART::requestReceived()
{
    if (!Serial.available())
        return false;
    return Serial.read() == 'R';
}
void UART::sendSoil(int soil1,int soil2)
{
    Serial.print("SOIL:");
    Serial.print(soil1);
    Serial.print(",");
    Serial.println(soil2);
}