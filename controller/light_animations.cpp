#include "light_animations.h"

static CRGB leds[NUM_LEDS];

void animations_setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_INTENSITY);
}

// ---- ANIMATION HELPERS ----

void light_at_pos(
  int lit_position,
  fl::u32 color
) {
  for(int i = 0; i < NUM_LEDS; i++){
    if(((i - lit_position) % 4) == 0) leds[i] = DEFAULT_COLOR;
    else leds[i] = CRGB::Black;
  }
  FastLED.show();
}

CHSV rand_color(){
  return CHSV(rand() % 255, 255, 255);
}

void fill_leds(fl::u32 color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void dim_leds(int fade_rate){
  for (auto &i:leds) {
    i.fadeLightBy( fade_rate);
  }
  // FastLED.show();
}

void trail_in(
  int pos,
  fl::u32 primary_color
) {

  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = primary_color;
  }

  for(int i = 0; i <= pos; i++) {
    leds[i] = CRGB::Black;
    leds[NUM_LEDS - i] = CRGB::Black;
  }
}

void trail_out(
  int pos,
  fl::u32 primary_color
) {
  int middle = NUM_LEDS / 2;

  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  for(int i = 0; i <= pos; i++) {
    leds[middle - i] = primary_color;
    leds[middle + i] = primary_color;
  }
}

// ---- MAIN ANIMATIONS ----

static void step_shifting(int &step) {
  light_at_pos(step, DEFAULT_COLOR);
  step = (step + 1) % 12;
}

static void step_pulse(int &step) {
  float cycle_angle = step * (PI / 64.0f);
  float intensity = (-1 * fabsf(sinf(cycle_angle)) + 1) * MAX_INTENSITY;

  for (int i = 0; i < NUM_LEDS; i++) leds[i] = DEFAULT_COLOR;
  FastLED.setBrightness(intensity);
  FastLED.show();

  step = (step + 1) % 128; // cycle from 0 to 2pi
}

static void step_cross(int &step, int &phase) {
  int half = NUM_LEDS / 2;
  int middle = NUM_LEDS / 2;

  if (step == 0) {
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB::Black;
  }

  if (phase == 0) {
    leds[step] = DEFAULT_COLOR;
    leds[NUM_LEDS - step] = DEFAULT_COLOR;
  } else {
    leds[middle - step] = DEFAULT_COLOR;
    leds[middle + step] = DEFAULT_COLOR;
  }
  FastLED.show();

  step++;
  if (step >= half) {
    step = 0;
    phase = (phase + 1) % 2;
  }
}

static void step_trailing(int &step, int &phase) {
  const int outer_margin = 3;
  const int inner_margin = 1;
  const int in_end = (NUM_LEDS / 2) - inner_margin;
  const int out_end = (NUM_LEDS / 2) - outer_margin;

  if (phase == 0) {
    int i = outer_margin + step;
    FastLED.setBrightness(MAX_INTENSITY * sinf(i * PI / 20));
    trail_in(i, DEFAULT_COLOR);
    FastLED.show();

    step++;
    if (outer_margin + step >= in_end) {
      step = 0;
      phase = 1;
    }
  } else {
    int i = inner_margin + step;
    trail_out(i, DEFAULT_COLOR);
    FastLED.setBrightness(MAX_INTENSITY * cosf(i * PI / 20));
    FastLED.show();

    step++;
    if (inner_margin + step >= out_end) {
      step = 0;
      phase = 0;
    }
  }
}

static void step_twinkle(int &step) {
  const int spacing = 4;
  int tracker = step * spacing;
  int offset = tracker / NUM_LEDS;
  int index = tracker % NUM_LEDS + offset;

  dim_leds(30);
  leds[index] = CHSV(rand() % 255, 255, 255);
  FastLED.show();

  step = (step + 1) % NUM_LEDS;
}

static void step_error(int &step) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = (step == 0) ? CRGB::Red : CRGB::Black;
  }
  FastLED.show();
  step = (step + 1) % 2;
}

static void step_rgb(int &step) {
  const uint8_t led_hue_step = 88 / NUM_LEDS; // 256 for full rainbow coverage
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * led_hue_step + step) & 0xFF, 255, 255);
  }
  FastLED.show();

  step = (step + 1) & 0xFF;
}

static void step_comb_jelly(int &step, int &phase) {
  static constexpr uint8_t led_hue_step = 176 / NUM_LEDS;

  dim_leds(60);

  uint8_t hue = (step * led_hue_step + phase) & 0xFF;
  leds[step] = CHSV(hue, 255, 255);

  FastLED.show();

  step = (step + 1) % NUM_LEDS;
  if (step == 0) {
    phase = (phase + 20) & 0xFF;
  }
}

// ---- MAIN ENTRY POINT ----

static unsigned long get_step_time(Animation animation) {
  switch (animation) {
    case Animation::Shifting:  return CYCLE_RATE * 1.5;
    case Animation::Pulse:     return CYCLE_RATE / 4;
    case Animation::Cross:     return CYCLE_RATE / 2;
    case Animation::Trailing:  return CYCLE_RATE;
    case Animation::Twinkle:   return CYCLE_RATE / 4;
    case Animation::Error:     return CYCLE_RATE * 3;
    case Animation::Rgb:       return CYCLE_RATE / 8;
    case Animation::CombJelly: return CYCLE_RATE / 6;
    default:                   return CYCLE_RATE;
  }
}

void animate_leds(Animation animation) {
  static Animation current = Animation::None;
  static unsigned long last_frame_time = 0;
  static unsigned long animation_step_time = 0;
  static int step = 0;
  static int phase = 0;

  if (animation != current) {
    current = animation;
    animation_step_time = get_step_time(animation);
    step = 0;
    phase = 0;
    last_frame_time = 0;
    FastLED.setBrightness(MAX_INTENSITY);
  }

  if (animation == Animation::None) return;

  unsigned long now = millis();
  if (now - last_frame_time < animation_step_time) return;
  last_frame_time = now;

  switch (animation) {
    case Animation::Shifting:  step_trailing(step, phase); break;
    case Animation::Pulse:     step_trailing(step, phase); break;
    case Animation::Cross:     step_trailing(step, phase); break;
    case Animation::Trailing:  step_trailing(step, phase); break;
    case Animation::Twinkle:   step_twinkle(step); break;
    case Animation::Error:     step_error(phase); break;
    case Animation::Rgb:       step_rgb(step); break;
    case Animation::CombJelly: step_comb_jelly(step, phase); break;
    case Animation::None:      break;
  }
}
