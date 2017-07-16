#include "getRootHandler.h"
#include "leds.h"

void GetRootHandler::Handle(const char* path) {
  CRGB color = hwLeds::getColor();
  char buf[50];
  sprintf(buf, "Current color is #%02x%02x%02x", color.r, color.g, color.b);
  client.sendHTML(buf);
};
