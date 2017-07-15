#pragma once
#include <ESP8266WiFi.h>

class RequestHandler {
public:
  virtual void Handle(WiFiClient& client, const char* path) const = 0;
};

class HTTPServer {
public:
  static void setup();
  static void loop();
  static void addRoute(const char *method, const char* path, const RequestHandler& r);
};
