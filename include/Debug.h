// Debug.h

#pragma once

#define DEBUG_ENABLED 1 // 1 for enabled, 0 for disabled

#if DEBUG_ENABLED

#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)

#else

#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)

#endif