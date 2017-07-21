#pragma once

#include "leds.h"

class Interpolater : public Animator {
private:
  CRGB start;
  CRGB end;
  bool useStart;
public:
  Interpolater(const CRGB& s, const CRGB& e) : start{s}, end{e}, useStart{true} {};
  void render();
  void print() {
    Serial.printf("Interpolater(#%02x%02x%02x, %02x%02x%02x)", start.R, start.G, start.B, end.R, end.G, end.B);
  }
};
