#include "optionsHandler.h"

void OptionsHandler::Handle(const char* path) {
  client.send200();
  client.println("Access-Control-Allow-Methods: POST, GET, OPTIONS");
  client.println("Access-Control-Allow-Headers: content-type");
};
