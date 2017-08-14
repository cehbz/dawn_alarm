#include "postInterpolateHandler.h"
#include <ArduinoJson.h>
#include "interpolate.h"

void PostInterpolateHandler::Handle(const char* path) {
  static Interpolater interpolater(CRGB(0,0,0),CRGB(0,0,0));
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

  Serial.printf("@%lu: start rgb(%u, %u, %u) #%02x%02x%02x", millis(), start.R, start.G, start.B, start.R, start.G, start.B);
  Serial.printf(", end rgb(%u, %u, %u) #%02x%02x%02x\n", end.R, end.G, end.B, end.R, end.G, end.B);
  interpolater = Interpolater(start, end);
  leds::setAnimator(interpolater);
  client.send200();
}
