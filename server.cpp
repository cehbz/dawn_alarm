#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "options.h"

#include "getRootHandler.h"
#include "getColorHandler.h"
#include "getColorsHandler.h"
#include "getAlarmsHandler.h"
#include "getOptionsHandler.h"

#include "postColorHandler.h"
#include "postColorsHandler.h"
#include "postInterpolateHandler.h"
#include "postAlarmsHandler.h"
#include "postOptionsHandler.h"
#include "postTriggerAlarmHandler.h"

#include "optionsHandler.h"

WiFiServer server(80);

WiFiClient client;
uint32_t clientConnectMillis = 0;
static const uint32_t clientConnectTimeoutMillis = 10;
static const uint32_t clientAvailableTimeoutMillis = 1000;

typedef struct {
  const char* path;
  RequestHandler* handler;
} Route;

GetRootHandler getRootHandler(&client);
GetColorHandler getColorHandler(&client);
GetColorsHandler getColorsHandler(&client);
GetAlarmsHandler getAlarmsHandler(&client);
GetOptionsHandler getOptionsHandler(&client);
const Route getRoutes[] = {
  { "/", &getRootHandler},
  { "/color", &getColorHandler},
  { "/colors", &getColorsHandler},
  { "/alarms", &getAlarmsHandler},
  { "/options", &getOptionsHandler},
};

PostColorHandler postColorHandler(&client);
PostColorsHandler postColorsHandler(&client);
PostInterpolateHandler postInterpolateHandler(&client);
PostAlarmsHandler postAlarmsHandler(&client);
PostOptionsHandler postOptionsHandler(&client);
PostTriggerAlarmHandler postTriggerAlarmHandler(&client);
const Route postRoutes[] = {
  { "/color", &postColorHandler},
  { "/colors", &postColorsHandler},
  { "/interpolate", &postInterpolateHandler},
  { "/alarms", &postAlarmsHandler},
  { "/options", &postOptionsHandler},
  { "/triggerAlarm", &postTriggerAlarmHandler},  
};

OptionsHandler optionsHandler(&client);
const Route optionsRoutes[] = {
  { "/color", &optionsHandler},
  { "/colors", &optionsHandler},
  { "/interpolate", &optionsHandler},
  { "/alarms", &optionsHandler},
  { "/options", &optionsHandler},
};

#define ARRAY_COUNT(a) (sizeof(a)/sizeof(a[0]))

typedef struct {
  const char* method;
  const Route* const routes;
  const unsigned int num_routes;
} Method;

const Method methods[] = {
  {"GET", getRoutes, ARRAY_COUNT(getRoutes) },
  {"POST", postRoutes, ARRAY_COUNT(postRoutes) },
  {"OPTIONS", optionsRoutes, ARRAY_COUNT(optionsRoutes) },
};

void registerDynamicDNS() {
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure secureClient;
  const char *host = "dynamicdns.park-your-domain.com";
  const char* fingerprint = "8A BD E4 A3 CE 85 A3 FB 94 71 B2 05 3D C6 78 7A 4D 55 63 8B";
  Serial.print("GET https://"); Serial.print(host);
  if (!secureClient.connect(host, 443)) {
    Serial.println("\nconnection failed");
    return;
  }

  if (!secureClient.verify(fingerprint, host)) {
    Serial.println("\ncertificate doesn't match");
    return;
  }

  const char* url = "/update?host=dawn&domain=ceh.bz&password=90abd539fc424917a3cae3d285d8d791&ip=";
  Serial.print(url); Serial.print(WiFi.localIP());

  secureClient.print("GET "); secureClient.print(url); secureClient.print(WiFi.localIP()); secureClient.print(" HTTP/1.1\r\n");
  secureClient.print("Host: "); secureClient.print(host); secureClient.print("\r\n");
  secureClient.print("User-Agent: DynDNSClientESP8266\r\n");
  secureClient.print("Connection: close\r\n\r\n");
  secureClient.flush();

  Serial.println();
  do {
    Serial.println(secureClient.readStringUntil('\n'));
  } while (secureClient.connected());
  secureClient.stop();
  Serial.println("dawn.ceh.bz registered");
}

void HTTPserver::setup() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("@%lu: PANIC connect WiFi before starting server!\n", millis());
    panic();
  }
  if (!MDNS.begin("dawn")) {
    Serial.printf("@%lu: PANIC Error setting up MDNS responder!\n", millis());
    panic();
  }
  Serial.printf("@%lu: mDNS responder started\n", millis());
  registerDynamicDNS();
  server.begin();
  Serial.printf("@%lu: HTTP server started\n", millis());
  MDNS.addService("http", "tcp", 80);
}

void HTTPserver::loop() {
  // wait for a client
  if (!client) {
    client = server.available();
    if (!client) return;
    clientConnectMillis = millis();
    Serial.printf("@%lu: new client\n", millis());
  }

  // wait for client to connect (establish connection)
  if (!client.connected()) {
    if (millis() - clientConnectMillis > clientConnectTimeoutMillis) {
      client.stop();
      Serial.printf("@%lu: client timed out waiting to connect\n", millis());
    }
    return;
  }

  // wait for client to send data
  if (!client.available()) {
    if (millis() - clientConnectMillis > clientAvailableTimeoutMillis) {
      client.stop();
      Serial.printf("@%lu: client timed out waiting for data\n", millis());
    }
    return;
  }

  String req = client.readStringUntil('\n');
  req.trim();
  if (options::debug_http) Serial.printf("@%lu: got request\n", millis());
  if (options::debug_http )Serial.println(req);
  String s;
  do {
    s = client.readStringUntil('\n');
    s.trim();
    if (options::debug_http) Serial.println(s);
  } while (s != "");

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int path_start = req.indexOf(' ');
  int path_end = req.indexOf(' ', path_start + 1);
  if (path_start == -1) {
    Serial.printf("@%lu: Invalid request\n", millis());
    return;
  }
  if (path_end == -1) {
    Serial.printf("@%lu: Invalid path\n", millis());
    return;
  }
  const String request = req.substring(0, path_start);
  const String path = req.substring(path_start+1, path_end);
  Serial.printf("@%lu: %s %s\n", millis(), request.c_str(), path.c_str());
  for (unsigned int i = 0; i < ARRAY_COUNT(methods); i++) {
    if (strcmp(methods[i].method, request.c_str()) != 0) continue;
    auto method = methods[i];
    for (unsigned int j = 0; j <  method.num_routes; j++) {
      if (strcmp(method.routes[j].path, path.c_str()) !=0) continue;
      method.routes[j].handler->Handle(path.c_str());
      goto done;
    }
  }
  HTTPClient(&client).send404();
 done:
  client.flush();

  Serial.printf("@%lu: finished request\n", millis());
  client.stop();
  }
