#ifndef DAWN_ALARM_H
#define DAWN_ALARM_H

#define NDEBUG
// #undef NDEBUG

#ifndef NDEBUG
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) 0
#define DEBUG_PRINT(...) 0
#define DEBUG_PRINTLN(...) 0
#endif

#endif
