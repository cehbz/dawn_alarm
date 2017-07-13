#include "dawn_alarm.h"
#include "ntp.h"
#include "leds.h"
#include "server.h"
#include "alarmer.h"

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("\nbooted\n");
  ntp::setup();
  hwLeds::setup();
  HTTPServer::setup();
  alarmer::setup();
}

void loop() {
  ntp::loop();
  hwLeds::loop();
  HTTPServer::loop();
  alarmer::loop();
}
