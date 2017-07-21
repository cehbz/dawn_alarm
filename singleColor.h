#pragma once

#include "leds.h"

class Monochromer : public Animator {
private:
  // const prevents copy construction and assignment.
  CRGB color;
public:
  Monochromer(const CRGB& c) : color{c} {};
  void render() { leds::setColor(color); };
  void print() { Serial.printf("Monochromer(%02x%02x%02x)", color.R, color.G, color.B); }
};
