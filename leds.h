#ifndef LEDS_H
#define LEDS_H

#include "crgb16.h"

// #define LEDS_DEBUG
#undef LEDS_DEBUG

#ifdef LEDS_DEBUG
#define LEDS_DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define LEDS_DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define LEDS_DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define LEDS_DEBUG_PRINTF(...)
#define LEDS_DEBUG_PRINT(...)
#define LEDS_DEBUG_PRINTLN(...)
#endif

namespace leds {
  CRGB16 getColor();
  void setColor(const CRGB16& color);
  void setup();
  void loop();
  void start(int duration);
  void stop();
  void clear();
}

#endif
