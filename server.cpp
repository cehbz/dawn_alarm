#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <TimeAlarms.h>

#include "server.h"
#include "leds.h"
#include "singleColor.h"
#include "singleFramer.h"
#include "interpolate.h"
#include "alarmer.h"

WiFiServer server(80);

WiFiClient client;
uint32_t clientConnectMillis = 0;
static const uint32_t clientConnectTimeoutMillis = 10;
static const uint32_t clientAvailableTimeoutMillis = 1000;

void sendCommon(const char* s) {
  client.print("HTTP/1.1 ");
  client.println(s);
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");  // the connection will be closed after completion of the response
}

void send200() {
  sendCommon("200 OK");
}

void send404() {
  sendCommon("404 Not Found");
}

void send400() {
  sendCommon("400 Bad Request");
}

void send500() {
  sendCommon("500 Internal Server Error");
}

class GetRootHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    send200();
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    CRGB color = hwLeds::getColor();
    client.printf("Current color is %u %u %u", color.r, color.g, color.b);
    client.println("</html>");
  };
} getRootHandler;

class GetColorsHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    const CRGB* colors = hwLeds::getColors();
    const size_t bufferSize = JSON_ARRAY_SIZE(hwLeds::NUM_LEDS) + hwLeds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*hwLeds::NUM_LEDS;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonArray& root = jsonBuffer.createArray();
    for (int i = 0; i < hwLeds::NUM_LEDS; i++) {
      JsonObject& led = root.createNestedObject();
      led["r"]=colors[i].r;
      led["g"]=colors[i].g;
      led["b"]=colors[i].b;
    }
    send200();
    client.println("Content-Type: application/json");
    client.println();
    root.printTo(client);
  };
} getColorsHandler;

class GetColorHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    const CRGB color = hwLeds::getColor();
    const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.createObject();
    root["r"]=color.r;
    root["g"]=color.g;
    root["b"]=color.b;
    send200();
    client.println("Content-Type: application/json");
    client.println();
    root.printTo(client);
  }
} getColorHandler;

const char* const dayNames[] = {
  "sunday",
  "monday",
  "tuesday",
  "wednesday",
  "thursday",
  "friday",
  "saturday"
};
class GetAlarmsHandler : public RequestHandler {

  void Handle(WiFiClient& client, const char* path) const {
    const size_t bufferSize = JSON_OBJECT_SIZE(7) + 110;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.createObject();
    bool alarms[dowSaturday];
    typedef char AlarmTimeStr[6];
    AlarmTimeStr alarmTimes[7];
    for ( int d = dowSunday; d <= dowSaturday; d++) {
      timeDayOfWeek_t dow = timeDayOfWeek_t(d);
      alarms[dow] = false;
    }
    for (int i = 0; i < dtNBR_ALARMS; i++) {
      Serial.printf("%d: %d\n", i, Alarm.readType(i));
      if (Alarm.readType(i) != dtWeeklyAlarm) continue;
      auto a = Alarm.read(i);
      Serial.printf("%2lu: %lu %02lu:%02lu", a, (dayOfWeek(a)+2)%7+1, numberOfHours(a), numberOfMinutes(a)); Serial.flush();
      auto dow = timeDayOfWeek_t((dayOfWeek(a)+2)%7+1);
      Serial.printf(" %s\n", dayNames[dow-1]); Serial.flush();
      if (alarms[dow]) {
        Serial.printf("duplicate alarm for %s\n", dayNames[dow-1]);
        send500();
        return;
      }
      alarms[dow] = true;
      sprintf(alarmTimes[dow-1], "%02lu:%02lu", numberOfHours(a), numberOfMinutes(a));
      root[dayNames[dow-1]] = alarmTimes[dow-1];
    }
    for ( int d = dowSunday; d <= dowSaturday; d++) {
      timeDayOfWeek_t dow = timeDayOfWeek_t(d);
      if (!alarms[dow-1]) {
        Serial.printf("missing alarm for %s\n", dayNames[dow-1]);
        send500();
        return;
      }
      Serial.printf("\"%s\": \"%s\"\n", dayNames[dow-1], root[dayNames[dow-1]].as<const char *>());
    }
    send200();
    client.println("Content-Type: application/json");
    client.println();
    root.printTo(client);
  };
} getAlarmsHandler;

class PostColorHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    static singleColor::Monochromer monochromer(CRGB(0,0,0));
    const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    const CRGB c(root["r"], root["g"], root["b"]);
    Serial.printf("@%lu: color %u [%02x], %u [%02x], %u [%02x]\n", millis(), c.r, c.r, c.g, c.g, c.b, c.b);
    monochromer = singleColor::Monochromer(c);
    hwLeds::setAnimator(monochromer);
    send200();
  }
} postColorHandler;

class PostColorsHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    static CRGB leds[hwLeds::NUM_LEDS];
    static SingleFramer singleFramer(leds);
    const size_t bufferSize = JSON_ARRAY_SIZE(hwLeds::NUM_LEDS) + hwLeds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*hwLeds::NUM_LEDS;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonArray& root = jsonBuffer.parseArray(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    for (int i = 0; i < hwLeds::NUM_LEDS; i++) {
      const JsonObject& r = root[i];
      const CRGB c(r["r"], r["g"], r["b"]);
      leds[i] = c;
      Serial.printf("@%lu: leds[%2d] %u [%02x], %u [%02x], %u [%02x]\n", millis(), i, leds[i].r, leds[i].r, leds[i].g, leds[i].g, leds[i].b, leds[i].b);
    }

    hwLeds::setAnimator(singleFramer);
    send200();
  };
} postColorsHandler;


class PostInterpolateHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    static interpolate::Interpolater interpolater(CRGB(0,0,0),CRGB(0,0,0));
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 70;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    CRGB start(root["start"]["r"], root["start"]["g"], root["start"]["b"]);
    CRGB end(root["end"]["r"], root["end"]["g"], root["end"]["b"]);

    Serial.printf("@%lu: start color %u [%02x], %u [%02x], %u [%02x]\n", millis(), start.r, start.r, start.g, start.g, start.b, start.b);
    Serial.printf("@%lu: end color %u [%02x], %u [%02x], %u [%02x]\n", millis(), end.r, end.r, end.g, end.g, end.b, end.b);
    interpolater = interpolate::Interpolater(start, end);
    hwLeds::setAnimator(interpolater);
    send200();
  }
} postInterpolateHandler;

int stoi(const char* s) {
  // returns a two digit integer pointed to by s
  // returns -1 if error
  if (s[0] < '0' || s[0] > '9' || s[1] < '0' || s[1] > '9') return -1;
  return (s[0]-'0')*10+s[1]-'0';
}

class PostAlarmsHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    const size_t bufferSize = JSON_OBJECT_SIZE(7) + 110;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }
    for ( int d = dowSunday; d <= dowSaturday; d++) {
      timeDayOfWeek_t dow = timeDayOfWeek_t(d);
      const char *c = root[dayNames[dow-1]];
      if (c == NULL) {
        Serial.printf("missing alarm for %s\n", dayNames[dow-1]);
        send400();
        return;
      }
      Serial.printf("%s: %s\n", dayNames[dow-1], c);
      if (strlen(c) != 5 || c[2] != ':') {
        Serial.printf("bad alarm time for %s: %s. strlen = %d, c[2] = %c\n", dayNames[dow-1], c, strlen(c), c[2]);
        send400();
        return;
      }
      int h = stoi(c);
      int m = stoi(&c[3]);
      if (h < 0 || h > 23 || m < 0 || m > 59) {
        Serial.printf("bad alarm time for %s: %s. h %d, m %d\n", dayNames[dow-1], c, h, m);
        send400();
        return;
      }
    }
    for ( int d = dowSunday; d <= dowSaturday; d++) {
      timeDayOfWeek_t dow = timeDayOfWeek_t(d);
      const char *c = root[dayNames[dow-1]];
      int h = stoi(c);
      int m = stoi(&c[3]);
      alarmer::SetAlarm(dow, h, m);
    }
    send200();
  }
} postAlarmsHandler;

class OptionsHandler : public RequestHandler {
  void Handle(WiFiClient& client, const char* path) const {
    send200();
    client.println("Access-Control-Allow-Methods: POST, GET, OPTIONS");
    client.println("Access-Control-Allow-Headers: content-type");
  };
} optionsHandler;


typedef struct {
  const char* path;
  const RequestHandler& handler;
} Route;

const Route getRoutes[] = {
  { "/", getRootHandler},
  { "/color", getColorHandler},
  { "/colors", getColorsHandler},
  { "/alarms", getAlarmsHandler},
};

const Route postRoutes[] = {
  { "/color", postColorHandler},
  { "/colors", postColorsHandler},
  { "/interpolate", postInterpolateHandler},
  { "/alarms", postAlarmsHandler},
};

const Route optionsRoutes[] = {
  { "/color", optionsHandler},
  { "/colors", optionsHandler},
  { "/interpolate", optionsHandler},
  { "/alarms", optionsHandler},
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

void HTTPServer::setup() {
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

void HTTPServer::loop() {
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
  Serial.printf("@%lu: got request\n", millis());
  Serial.println(req);
  String s;
  do {
    s = client.readStringUntil('\n');
    s.trim();
    Serial.println(s);
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
  Serial.printf("req \"%s\", request \"%s\", path \"%s\"\n", req.c_str(), request.c_str(), path.c_str());
  for (unsigned int i = 0; i < ARRAY_COUNT(methods); i++) {
    Serial.printf("strcmp(\"%s\",\"%s\")\n", methods[i].method, request.c_str());
    if (strcmp(methods[i].method, request.c_str()) != 0) continue;
    auto method = methods[i];
    for (unsigned int j = 0; j <  method.num_routes; j++) {
      Serial.printf("strcmp(\"%s\",\"%s\")\n", method.routes[j].path, path.c_str());
      if (strcmp(method.routes[j].path, path.c_str()) !=0) continue;
      method.routes[j].handler.Handle(client, path.c_str());
      goto done;
    }
  }
  send404();
 done:
  client.flush();

  Serial.printf("@%lu: finished request\n", millis());
  client.stop();
  }
