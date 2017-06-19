#ifndef LEDS_H
#define LEDS_H

#include "crgb16.h"

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
