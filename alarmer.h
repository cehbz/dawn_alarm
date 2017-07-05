#pragma once

// #define DEBUG_ALARMER

#ifdef DEBUG_ALARMER
#define DEBUG_ALARMER_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_ALARMER_PRINT(...)
#endif

namespace alarmer {
  void setup();
  void loop();
}
