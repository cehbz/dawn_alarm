#include "dawn_alarm.h"
#include "leds.h"
#include <FastLED.h>
#include "crgb16.h"
#include "crgb32.h"

// #define DEBUG_LEDS

#ifdef DEBUG_LEDS
#define DEBUG_LEDS_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_LEDS_PRINT(...)
#endif

namespace leds {
  static const int DATA_PIN = 1; // D1, GPIO5
  CRGB leds[NUM_LEDS];
  CRGB16 buf[NUM_LEDS];

  class LedsOff : public Animator {
  public:
    void render(CRGB16*) { setColor(CRGB16(0,0,0));};
  };
  static LedsOff ledsOff;
  Animator* animator = &ledsOff;

  CRGB16 getColor() { return buf[NUM_LEDS-1]; } // TODO return average

  void setColors(const CRGB16* b) {
    for (int i = 0; i<NUM_LEDS; i++) {
      buf[i] = b[i];
    }
  }

  void setColor(const CRGB16& color) {
    for (int i = 0; i<NUM_LEDS; i++) {
      buf[i] = color;
    }
  }

  class Error16 {
  public: // so that you can print it, should be private
    int16_t r;
    int16_t g;
    int16_t b;

  public:
    // default values are UNINITIALIZED (not allowed)
    // inline Error16() __attribute__((always_inline)) {}

    // allow construction from signed 16 bit R, G, B
    inline Error16( int16_t ir, int16_t ig, int16_t ib)  __attribute__((always_inline)) : r(ir), g(ig), b(ib) {}

    void correctOne(int16_t& error, uint16_t& component) {
      int c = component + error;
      if (c < 0) {
        error = c;
        component = 0;
        DEBUG_LEDS_PRINT("<");
        return;
      }
      if (c > 65535) {
        error = c - 65535;
        component = 65535;
        DEBUG_LEDS_PRINT(">");
        return;
      }
      error = 0;
      component = c;
      DEBUG_LEDS_PRINT("=");
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

  };

  Error16 error(0,0,0);

  void show() {
    for (int i = 0; i<NUM_LEDS; i++) {
      CRGB16 c(buf[i]);
      DEBUG_LEDS_PRINT("start error: %+4d, %+4d, %+4d ", error.r, error.g, error.b);
      c = error.correct(c);
      leds[i] = c.CRGB16to8();
      DEBUG_LEDS_PRINT(", led[%2d] %02x%02x%02x", i, leds[i].r, leds[i].g, leds[i].b);
      error.update(c, leds[i]);
      DEBUG_LEDS_PRINT(", end error: %+4d, %+4d, %+4d", error.r, error.g, error.b);
      DEBUG_LEDS_PRINT("\n");
    }
    FastLED.show();
    FastLED.delay(0);
  }

  // void show() {
  //   for (int i = 0; i<NUM_LEDS; i++) {
  //     leds[i] = buf[i].CRGB16to8();
  //   }
  //   FastLED.show();
  //   FastLED.delay(0);
  // }

  void setAnimator(Animator& a) {
    animator = &a;
  }

  uint32_t fpsEndTime;
  uint32_t frames;

  void setup() {
    fpsEndTime = millis()+1000;
    frames = 0;
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  }

  void loop() {
    animator->render(buf);
    show();
    frames++;
    if (millis()>=fpsEndTime) {
      DEBUG_PRINT("@%lu, %d fps\n", millis(), frames);
      fpsEndTime = millis()+1000;
      frames = 0;
    }
  }
}
