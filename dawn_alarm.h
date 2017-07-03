#pragma once

// #define DEBUG_DAWN
#undef DEBUG_DAWN

#ifdef DEBUG_DAWN
#define DEBUG_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif
