#include "interpolate.h"
#include "leds.h"

void Interpolater::render() {
  if (useStart) {
    useStart = false;
    leds::setColor(start);
    return;
  }
  useStart = true;
  leds::setColor(end);
};
