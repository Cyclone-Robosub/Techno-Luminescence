#pragma once

#include <FastLED.h>
const int NUM_LEDS = 15;

const int CYCLE_RATE = 100;
const int DEFAULT_COLOR = CRGB::White;
const int MAX_INTENSITY = 20;

const int LED_PIN = 12;

void animations_setup();

// ---- ANIMATION HELPERS ----

void light_at_pos(int lit_position = 0, fl::u32 color = DEFAULT_COLOR);

fl::u32 rand_color();

void dim_leds(int fade_rate = 100);

void trail_in(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

void trail_out(int pos = 0, fl::u32 primary_color = DEFAULT_COLOR);

// ---- MAIN ANIMATIONS ----

void animate_shifting(int cycle_rate = CYCLE_RATE * 1.5);

void animate_pulse (fl::u32 color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE);

void animate_cross(fl::u32 primary_color = DEFAULT_COLOR, fl::u32 secondary_color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE / 2);

void animate_trailing(fl::u32 primary_color = DEFAULT_COLOR, int cycle_rate = CYCLE_RATE);

void animate_twinkle (int spacing = 4, int cycle_rate = CYCLE_RATE / 4);

void animate_seeking();

void animate_error();
