#include "postInterpolateHandler.h"
#include <ArduinoJson.h>
#include "interpolate.h"

void PostInterpolateHandler::Handle(const char* path) {
  static interpolate::Interpolater interpolater(CRGB(0,0,0),CRGB(0,0,0));
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 70;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }

  CRGB start(root["start"]["r"], root["start"]["g"], root["start"]["b"]);
  CRGB end(root["end"]["r"], root["end"]["g"], root["end"]["b"]);

  Serial.printf("@%lu: start color %u [%02x], %u [%02x], %u [%02x]\n", millis(), start.r, start.r, start.g, start.g, start.b, start.b);
  Serial.printf("@%lu: end color %u [%02x], %u [%02x], %u [%02x]\n", millis(), end.r, end.r, end.g, end.g, end.b, end.b);
  interpolater = interpolate::Interpolater(start, end);
  hwLeds::setAnimator(interpolater);
  client.send200();
}
