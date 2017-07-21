#include "getColorsHandler.h"
#include <ArduinoJson.h>
#include "leds.h"

void GetColorsHandler::Handle(const char* path) {
  const CRGB* colors = leds::getColors();
  const size_t bufferSize = JSON_ARRAY_SIZE(leds::NUM_LEDS) + leds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*leds::NUM_LEDS;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonArray& root = jsonBuffer.createArray();
  for (int i = 0; i < leds::NUM_LEDS; i++) {
    JsonObject& led = root.createNestedObject();
    led["r"]=colors[i].R;
    led["g"]=colors[i].G;
    led["b"]=colors[i].B;
  }
  client.sendJSON(root);
};
