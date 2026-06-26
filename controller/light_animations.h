#include <FastLED.h>
#define NUM_LEDS 15

#define CYCLE_RATE 100
#define DEFAULT_COLOR CRGB::White
#define MAX_INTENSITY 20
CRGB leds[NUM_LEDS];

#define LED_PIN 12

void animations_setup();

void error_loop();

// ---- ANIMATION HELPERS ----

void light_at_pos(int lit_position = 0, fl::u32 color = DEFAULT_COLOR);

fl::u32 rand_color();

void dim_leds(int fade_rate = 100);

void trail_in(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

void trail_out(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

// ---- MAIN ANIMATIONS ----

void shifting_animation(int cycle_rate = CYCLE_RATE * 1.5);

void pulse_animation (fl::u32 color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE);

void cross_animation(fl::u32 primary_color = DEFAULT_COLOR, fl::u32 secondary_color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE / 2);

void trailing_animation(fl::u32 primary_color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE);

void twinkle_animation (int spacing = 4, int cycle_rate = CYCLE_RATE / 4); 

