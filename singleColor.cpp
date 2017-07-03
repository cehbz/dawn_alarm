#include "singleColor.h"
#include "leds.h"

namespace singleColor {
  void Monochromer::render() {leds::setColor(color);};
};
