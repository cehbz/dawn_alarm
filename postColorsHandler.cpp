#include "postColorsHandler.h"
#include <ArduinoJson.h>
#include "singleFramer.h"

void PostColorsHandler::Handle(const char* path) {
  static CRGB leds[hwLeds::NUM_LEDS];
  static SingleFramer singleFramer(leds);
  const size_t bufferSize = JSON_ARRAY_SIZE(hwLeds::NUM_LEDS) + hwLeds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*hwLeds::NUM_LEDS;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonArray& root = jsonBuffer.parseArray(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }

  for (int i = 0; i < hwLeds::NUM_LEDS; i++) {
    const JsonObject& r = root[i];
    const CRGB c(r["r"], r["g"], r["b"]);
    leds[i] = c;
    Serial.printf(
                  "@%lu: leds[%2d] %u [%02x], %u [%02x], %u [%02x]\n",
                  millis(), i, leds[i].R, leds[i].R, leds[i].G, leds[i].G, leds[i].B, leds[i].B);
  }

  hwLeds::setAnimator(singleFramer);
  client.send200();
};
