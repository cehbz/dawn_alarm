#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace ntp {

const char ssid[] = "Little Palmerston 2.4G";  //  your network SSID (name)
const char pass[] = "littlepalmerstonst";       // your network password
const char NTPServerName[] = "pool.ntp.org";

// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

//static const int timeZone = 1;     // Central European Time
//static const int timeZone = -5;  // Eastern Standard Time (USA)
//static const int timeZone = -4;  // Eastern Daylight Time (USA)
//static const int timeZone = -8;  // Pacific Standard Time (USA)
//static const int timeZone = -7;  // Pacific Daylight Time (USA)
static const int timeZone = 10; // Eastern Standard Time (AU) [Melbourne]

time_t curTime = 0;
uint32_t curTimeLastSetMillis = 0;
uint32_t ntpPacketSentMillis = 0;
uint32_t syncIntervalMillis = 0;

void blink(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  delay(500);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.printf("%02d-%02d-%02dT%02d:%02d:%02d%+02d:00\n",
                year(),
                month(),
                day(),
                hour(),
                minute(),
                second(),
                timeZone);
}

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  ntpPacketSentMillis = millis();
}

time_t getTime() {
  return curTime + (millis()-curTimeLastSetMillis)/1000;
}

void gotNTPResponse() {
  Serial.printf("@%u: Receive NTP Response\n", millis());
  ntpPacketSentMillis = 0;
  blink(3);
  Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
  static const unsigned long secsFrom1900ToUnixEpoch = 2208988800UL;
  unsigned long secsSince1900;
  // convert four bytes starting at location 40 to a long integer
  secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
  secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
  secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
  secsSince1900 |= (unsigned long)packetBuffer[43];
  digitalClockDisplay();
  time_t prevTime = getTime();
  curTime = secsSince1900 - secsFrom1900ToUnixEpoch + timeZone * SECS_PER_HOUR;
  setTime(curTime);
  if (curTime-prevTime != 0) {
    Serial.printf("Time adjusted  %+d secs\n", curTime-prevTime) ;
  }
  curTimeLastSetMillis = millis();
  digitalClockDisplay();
}

time_t getNTPTime()
{
  if (ntpPacketSentMillis == 0 || millis() - ntpPacketSentMillis >= 1500) {
    while (Udp.parsePacket() > 0) ; // discard any previously received packets
    Serial.printf("@%u: Transmit NTP Request\n", millis());
    sendNTPpacket(timeServer);
  }
  return getTime();
}

  void lookupTimeServerByName(const char* name) {
    WiFi.hostByName(name, timeServer);
    Serial.printf("NTP server %s at IP ", NTPServerName);
    Serial.println(timeServer);
  }

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    blink(1);
    Serial.print(".");
  }

  blink(2);
  Serial.printf("@%u: IP: ", millis()); Serial.print(WiFi.localIP());
  Serial.print("RSSI (signal strength):"); Serial.print(WiFi.RSSI()); Serial.println(" dBm");

  time_t ntpSyncStartMillis = 0;
  while (curTimeLastSetMillis == 0) {
    ntpSyncStartMillis = millis();
    lookupTimeServerByName(NTPServerName);
    Udp.begin(localPort);
    syncIntervalMillis = 1000*SECS_PER_HOUR;
    Serial.println("Initial NTP Sync");
    getNTPTime();
    // wait for first sync
    while (curTimeLastSetMillis == 0 && millis()-ntpSyncStartMillis < 10*1000) { // try this server for 10 seconds before trying a different one
      loop();
    }
  }
  setSyncInterval(1*SECS_PER_HOUR);
  setSyncProvider(getTime);
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{
  if (ntpPacketSentMillis == 0) {
    if  (millis() - curTimeLastSetMillis >= syncIntervalMillis) {
      Serial.println("NTP sync update");
      getNTPTime();
      return;
    }
    return;
  }
  if (millis() - ntpPacketSentMillis >= 1500) {
    Serial.println("NTP packet timed out");
    getNTPTime();
    return;
  }
  // ntpPacketSentMillis > 0 - we have an NTP packet in flight
  if (Udp.parsePacket() >= NTP_PACKET_SIZE) {
    gotNTPResponse();
    return;
  }
}

/*-------- NTP code ----------*/
}
