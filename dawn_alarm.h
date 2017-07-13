#pragma once

// see if this helps the flicker
#define FASTLED_INTERRUPT_RETRY_COUNT 1

// #define DEBUG_DAWN

#ifdef DEBUG_DAWN
#define DEBUG_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif
