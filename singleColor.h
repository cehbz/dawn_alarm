#ifndef SINGLECOLOR_H
#define SINGLECOLOR_H

#include "leds.h"

namespace singleColor {
  class Monochromer : public leds::Animator {
  private:
    // const prevents copy construction and assignment.
    CRGB16 color;
  public:
    Monochromer(const CRGB16& c) : color{c} {};
    void render(CRGB16*);
  };
};

#endif
