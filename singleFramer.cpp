#include "singleFramer.h"
#include "leds.h"

void SingleFramer::render(CRGB16*) {
  leds::setColors(leds);
};
