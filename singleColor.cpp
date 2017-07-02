#include "singleColor.h"
#include "leds.h"

namespace singleColor {
  void Monochromer::render(CRGB16*) {leds::setColor(color);};
};
