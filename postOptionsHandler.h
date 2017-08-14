#pragma once

#include "server.h"

class PostOptionsHandler : public RequestHandler {
public:
  PostOptionsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
