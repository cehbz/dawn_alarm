#ifndef LEDS_H
#define LEDS_H

#include "crgb16.h"

// #define DEBUG_LEDS
#undef DEBUG_LEDS

#ifdef DEBUG_LEDS
#define DEBUG_LEDS_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_LEDS_PRINT(...)
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
