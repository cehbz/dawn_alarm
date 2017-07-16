#include "postColorHandler.h"
#include <ArduinoJson.h>
#include "singleColor.h"

void PostColorHandler::Handle(const char* path) {
  static singleColor::Monochromer monochromer(CRGB(0,0,0));
  const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }

  const CRGB c(root["r"], root["g"], root["b"]);
  Serial.printf("@%lu: color %u [%02x], %u [%02x], %u [%02x]\n", millis(), c.r, c.r, c.g, c.g, c.b, c.b);
  monochromer = singleColor::Monochromer(c);
  hwLeds::setAnimator(monochromer);
  client.send200();
}
