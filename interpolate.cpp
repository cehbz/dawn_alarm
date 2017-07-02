#include "interpolate.h"
#include "leds.h"

namespace interpolate {

  void Interpolater::render(CRGB16* leds) {
    if (useStart) {
      useStart = false;
      leds::setColor(start);
      return;
    }
    useStart = true;
    leds::setColor(end);
  };
};
