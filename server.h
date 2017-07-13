#pragma once

class RequestHandler {
public:
  virtual void Handle(const char* path) = 0;
};

class HTTPServer {
public:
  static void setup();
  static void loop();
  static void addRoute(const char *method, const char* path, const RequestHandler& r);
};
