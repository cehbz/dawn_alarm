#include "interpolate.h"
#include "leds.h"

namespace interpolate {

  void Interpolater::render() {
    if (useStart) {
      useStart = false;
      hwLeds::setColor(start);
      return;
    }
    useStart = true;
    hwLeds::setColor(end);
  };
};
