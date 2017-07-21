#include "getColorHandler.h"
#include <ArduinoJson.h>
#include "leds.h"

void GetColorHandler::Handle(const char* path) {
  const CRGB color = leds::getColor();
  const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.createObject();
  root["r"]=color.R;
  root["g"]=color.G;
  root["b"]=color.B;
  client.sendJSON(root);
}
