#pragma once

#include "server.h"

class PostColorHandler : public RequestHandler {
public:
  PostColorHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
