#include "interpolate.h"
#include "leds.h"

namespace interpolate {

  void Interpolater::render() {
    if (useStart) {
      useStart = false;
      leds::setColor(start);
      return;
    }
    useStart = true;
    leds::setColor(end);
  };
};
