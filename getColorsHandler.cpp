#include "getColorsHandler.h"
#include <ArduinoJson.h>
#include "leds.h"

void GetColorsHandler::Handle(const char* path) {
  const CRGB* colors = hwLeds::getColors();
  const size_t bufferSize = JSON_ARRAY_SIZE(hwLeds::NUM_LEDS) + hwLeds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*hwLeds::NUM_LEDS;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonArray& root = jsonBuffer.createArray();
  for (int i = 0; i < hwLeds::NUM_LEDS; i++) {
    JsonObject& led = root.createNestedObject();
    led["r"]=colors[i].R;
    led["g"]=colors[i].G;
    led["b"]=colors[i].B;
  }
  client.sendJSON(root);
};
