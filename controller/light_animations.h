#pragma once

#include <FastLED.h>
const int NUM_LEDS = 22;

const int CYCLE_RATE = 100;
const int MAX_INTENSITY = 100;
const fl::u32 DEFAULT_COLOR = CRGB::Magenta;
const int LED_PIN = 13;

enum class Animation { None, Shifting, Pulse, Cross, Trailing, Twinkle, Error, Rgb, CombJelly };

void animations_setup();
void animate_leds(Animation animation);

// ---- ANIMATION HELPERS ----

void light_at_pos(int lit_position = 0, fl::u32 color = DEFAULT_COLOR);

CHSV rand_color();

void fill_leds(fl::u32 color = DEFAULT_COLOR);

void dim_leds(int fade_rate = 100);

void trail_in(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

void trail_out(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);
