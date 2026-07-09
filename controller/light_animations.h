#pragma once

#include <FastLED.h>
const int NUM_LEDS = 15;

const int CYCLE_RATE = 100;
const int MAX_INTENSITY = 20;
const fl::u32 DEFAULT_COLOR = CRGB::White;
const int LED_PIN = 12;

enum class Animation { None, Shifting, Pulse, Cross, Trailing, Twinkle, Error };

void animations_setup();
void animate_leds(Animation animation);

// ---- ANIMATION HELPERS ----

void light_at_pos(int lit_position = 0, fl::u32 color = DEFAULT_COLOR);

fl::u32 rand_color();

void dim_leds(int fade_rate = 100);

void trail_in(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

void trail_out(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);
