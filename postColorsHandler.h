#pragma once

#include "server.h"

class PostColorsHandler : public RequestHandler {
public:
  PostColorsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
