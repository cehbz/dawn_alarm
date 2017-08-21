#include "leds.h"
#include <NeoPixelBus.h>
#include "options.h"
#include "crgb16.h"
#include "crgb32.h"

uint16_t gamma16[256]; // initialised in setup
#ifdef fastled
const float GAMMA = 2.5f;
// static const int DATA_PIN = 7; // D7, GPIO13, RXD2, HMOSI
// static const int CLK_PIN = 5; // D5, GPIO14, HSCLK
CRGB buf[leds::NUM_LEDS];
#endif
#ifdef pixie
const float GAMMA = 1.0f; // pixies have 'built in' gamma correction
Adafruit_Pixie strip = Adafruit_Pixie(leds::NUM_LEDS, &Serial1);
CRGB buf[leds::NUM_LEDS];
#endif
#ifdef neopixelbus
const float GAMMA = 2.5f;
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(leds::NUM_LEDS);
#endif
CRGB frame[leds::NUM_LEDS];
bool dirty = false;

class LedsOff : public Animator {
public:
  void render() { leds::setColor(CRGB(0,0,0));};
  void print() { Serial.print("LedsOff()"); }
};
static LedsOff ledsOff;
Animator* animator = &ledsOff;

const CRGB leds::getColor() {
  uint16_t r = 0;
  uint16_t g = 0;
  uint16_t b = 0;
  for (int i = 0; i < leds::NUM_LEDS; i++) {
    r += frame[i].R;
    g += frame[i].G;
    b += frame[i].B;
  }
  r = (r+leds::NUM_LEDS/2)/leds::NUM_LEDS;
  g = (g+leds::NUM_LEDS/2)/leds::NUM_LEDS;
  b = (b+leds::NUM_LEDS/2)/leds::NUM_LEDS;
  return CRGB(r, g, b);
}

const CRGB* leds::getColors() {
  return frame;
}

void leds::setColors(const CRGB* b) {
  for (int i = 0; i<leds::NUM_LEDS; i++) {
    frame[i] = b[i];
  }
  dirty = true;
}

void leds::setColor(const CRGB& color) {
  for (int i = 0; i<leds::NUM_LEDS; i++) {
    frame[i] = color;
  }
  dirty = true;
}

class Error16 {
public: // so that you can print it, should be private
  int16_t r;
  int16_t g;
  int16_t b;

public:
  // default values are zero
  inline Error16() __attribute__((always_inline)) : r(0), g(0), b(0) {}

  // allow construction from signed 16 bit R, G, B
  inline Error16( int16_t ir, int16_t ig, int16_t ib)  __attribute__((always_inline)) : r(ir), g(ig), b(ib) {}

  void correctOne(int16_t& error, uint16_t& component) {
    int c = component + error;
    if (c < 0) {
      error = c;
      component = 0;
      if (options::debug_leds) Serial.printf("<");
      return;
    }
    if (c > 65535) {
      error = c - 65535;
      component = 65535;
      if (options::debug_leds) Serial.printf(">");
      return;
    }
    error = 0;
    component = c;
    if (options::debug_leds) Serial.printf("=");
  }

  CRGB16  correct(CRGB16& c) {
    correctOne(r, c.r);
    correctOne(g, c.g);
    correctOne(b, c.b);
    return c;
  }

  void update(CRGB16& wanted, CRGB& got) {
    CRGB16 got16(got);
    r += wanted.r-got16.r;
    g += wanted.g-got16.g;
    b += wanted.b-got16.b;
  }

  // plus
  Error16& operator+=(const Error16& rhs)
  {
    (*this).r += rhs.r;
    (*this).g += rhs.g;
    (*this).b += rhs.b;
    return *this;
  }

  friend Error16 operator+(Error16 lhs, const Error16& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  // minus
  Error16& operator-=(const Error16& rhs)
  {
    (*this).r -= rhs.r;
    (*this).g -= rhs.g;
    (*this).b -= rhs.b;
    return *this;
  }

  friend Error16 operator-(Error16 lhs, const Error16& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  // divide by int
  Error16& operator/=(const int& rhs)
  {
    (*this).r /= rhs;
    (*this).g /= rhs;
    (*this).b /= rhs;
    return *this;
  }

  friend Error16 operator/(Error16& lhs, const int& rhs)
  {
    lhs /= rhs;
    return lhs;
  }

};

Error16 errors[leds::NUM_LEDS];
// Error16 error(0,0,0);

CRGB16 gammaCorrect(CRGB& c) {
  return CRGB16(
                gamma16[c.R],
                gamma16[c.G],
                gamma16[c.B]);
};

void show() {
  if (!dirty) {
    return;
  }
#ifdef neopixelbus
  CRGB* buf = (CRGB*)strip.Pixels();
  strip.Dirty();
#endif
  if (options::debug_leds) Serial.printf("@%lu leds show()\n", millis());
  for (int i = 0; i<leds::NUM_LEDS; i++) {
    if (options::debug_leds || options::debug_fade) Serial.printf("#%02x,%02x,%02x", frame[i].R, frame[i].G, frame[i].B);
    CRGB16 c(gammaCorrect(frame[i]));	if (options::debug_leds) Serial.printf("->%04x,%04x,%04x", c.r, c.g, c.b);
    Error16 error = errors[i];				if (options::debug_leds) Serial.printf(", err: %+4d, %+4d, %+4d ", error.r, error.g, error.b);
    c = error.correct(c);
    buf[i] = c.CRGB16to8();				if (options::debug_leds) Serial.printf(", led[%2d] %02x%02x%02x", i, buf[i].R, buf[i].G, buf[i].B);
    error.update(c, buf[i]);					if (options::debug_leds || options::debug_fade) Serial.printf(", err: %+4d, %+4d, %+4d", error.r, error.g, error.b);
    errors[(i+1) % leds::NUM_LEDS] += error/4;
    errors[i] = error/2;;
    errors[(i-1) % leds::NUM_LEDS] += (error-error/2-error/4);
    // errors[i] = error;
    if (options::debug_leds || options::debug_fade) Serial.printf("\n");
  }
  for (int i = 0; i < leds::NUM_LEDS; i++) {
    strip.SetPixelColor(i, buf[i]);
  }
#ifdef neopixelbus
  if (options::debug_leds) Serial.printf("strip.Show()\n");
  strip.Show();
  buf = NULL;
#endif
#ifdef pixie
  strip.show();
#endif
#ifdef fastled
  FastLED.show();
#endif
  dirty = false;
}

// void show() {
//   for (int i = 0; i<leds::NUM_LEDS; i++) {
//     buf[i] = frame[i].CRGB16to8();
//   }
//   FastLED.show();
//   FastLED.delay(0);
// }

void leds::setAnimator(Animator& a) {
  animator = &a;
  if (options::debug_alarmer) Serial.printf("@%d Animator  = ", millis()); a.print(); Serial.println();
}

uint32_t fpsEndTime;
uint32_t frames;

void leds::setup() {
  for (int i = 0; i < 256; i++) {
    gamma16[i] = uint16_t(65535.0f*pow(i/255.0f, GAMMA)+0.5f);
  }
  fpsEndTime = millis()+1000;
  frames = 0;
#ifdef neopixelbus
  strip.Begin();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
#endif
#ifdef pixie
  Serial1.begin(115200);  // <- Alt. if using hardware serial port
  strip.setBrightness(255);  // Adjust as necessary to avoid blinding
#endif
#ifdef fastled
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000); // external power brick
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000); // usb power
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, leds::NUM_LEDS);
  // FastLED.addLeds<APA102, DATA_PIN, CLK_PIN, RGB>(leds, leds::NUM_LEDS);
#endif
}

void leds::loop() {
  animator->render();
  show();
#ifdef fastled
  FastLED.delay(0);
#endif
  frames++;
  if (millis()>=fpsEndTime) {
    if (options::show_fps) Serial.printf("@%lu, %d fps\n", millis(), frames);
    fpsEndTime = millis()+1000;
    frames = 0;
  }
}
