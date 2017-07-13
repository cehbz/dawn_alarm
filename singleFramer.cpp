#include "singleFramer.h"
#include "leds.h"

void SingleFramer::render() {
  hwLeds::setColors(leds);
};
