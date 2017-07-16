#pragma once
#include "server.h"

class GetColorsHandler : public RequestHandler {
public:
  GetColorsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
