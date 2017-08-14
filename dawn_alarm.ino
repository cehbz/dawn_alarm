#include "ntp.h"
#include "leds.h"
#include "server.h"
#include "alarmer.h"

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("\nbooted\n");
  ntp::setup();
  leds::setup();
  HTTPserver::setup();
  alarmer::setup();
}

void loop() {
  ntp::loop();
  leds::loop();
  HTTPserver::loop();
  alarmer::loop();
}
