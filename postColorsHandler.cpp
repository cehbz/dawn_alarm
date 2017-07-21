#include "postColorsHandler.h"
#include <ArduinoJson.h>
#include "singleFramer.h"

void PostColorsHandler::Handle(const char* path) {
  static CRGB leds[leds::NUM_LEDS];
  static SingleFramer singleFramer(leds);
  const size_t bufferSize = JSON_ARRAY_SIZE(leds::NUM_LEDS) + leds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*leds::NUM_LEDS;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonArray& root = jsonBuffer.parseArray(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }

  for (int i = 0; i < leds::NUM_LEDS; i++) {
    const JsonObject& r = root[i];
    const CRGB c(r["r"], r["g"], r["b"]);
    leds[i] = c;
    Serial.printf(
                  "@%lu: leds[%2d] %u [%02x], %u [%02x], %u [%02x]\n",
                  millis(), i, leds[i].R, leds[i].R, leds[i].G, leds[i].G, leds[i].B, leds[i].B);
  }

  leds::setAnimator(singleFramer);
  client.send200();
};
