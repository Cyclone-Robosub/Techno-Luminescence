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

fl::u32 rand_color(){
  int colors_size = 6;
  fl::u32 colors[] = {
    CRGB::Red, 
    CRGB::DarkOrange, 
    CRGB::Green, 
    CRGB::HotPink, 
    CRGB::Blue, 
    CRGB::Purple
  };
  int i = rand() % (colors_size + 1); 
  return colors[i];
}

void dim_leds(int fade_rate){
  for (auto &i:leds) {
    i.fadeLightBy( fade_rate);
  }
  FastLED.show();
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

void shifting_animation(
  int cycle_rate
) {
  for (int i = 0; i < 12; i++){
    light_at_pos(i);
    delay(cycle_rate);
  }
}

void pulse_animation (
  fl::u32 color,
  int cycle_rate
) {
  volatile float intensity;
  volatile float cycle_index = 0;
  
  while (cycle_index < 2 * PI) {
    intensity = (-1* abs(sin(cycle_index)) + 1) * MAX_INTENSITY;
    cycle_index += PI / 64; 

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color; 
      FastLED.setBrightness(intensity);
    }
    FastLED.show();
    delay(cycle_rate / 4);
  }
}

void cross_animation(
  fl::u32 primary_color,
  fl::u32 secondary_color,
  int cycle_rate
){
  for(int i = 0; i < NUM_LEDS / 2; i++){
    leds[i] = primary_color;
    leds[NUM_LEDS - i] = secondary_color;
    FastLED.show();
    delay(cycle_rate);
    leds[i] = CRGB::Black;
    leds[NUM_LEDS - i] = CRGB::Black;
    FastLED.show();
  }

  int middle = NUM_LEDS / 2;

  for(int i = 0; i < NUM_LEDS / 2; i++){
    leds[middle - i] = secondary_color;
    leds[middle + i] = primary_color;
    FastLED.show();
    delay(cycle_rate);
    leds[middle - i] = CRGB::Black;
    leds[middle + i] = CRGB::Black;
    FastLED.show();
  }
}

void trailing_animation(
  fl::u32 primary_color,
  int cycle_rate
){
  int outer_margin = 3;
  int inner_margin = 1;
  for(int i = outer_margin; i < (NUM_LEDS / 2) - inner_margin; i++){
    FastLED.setBrightness(MAX_INTENSITY * sin(i * PI / 20));
    trail_in(i);
    if(i == 0) delay(cycle_rate);
    else {
      int in_delay = (cycle_rate/2.0) + (cycle_rate / (i * i * i * 2.0));
      delay(in_delay);
    }
    FastLED.show();
  }

  int middle = NUM_LEDS / 2;

  for(int i = inner_margin; i < (NUM_LEDS / 2) - outer_margin; i++){
    trail_out(i);
    if(i == 0) delay(cycle_rate);
    else{
      int out_delay = ((cycle_rate) + (cycle_rate / i));
      delay(0.75 * out_delay);
    }
    FastLED.setBrightness(MAX_INTENSITY * cos(i * PI / 20));
    FastLED.show();
  }
}

void twinkle_animation (
  int spacing,
  int cycle_rate
) {
  int index = 0;  
  int tracker = 0;
  int offset = 0; 

  for (int i = 0; i < NUM_LEDS; i++) {
    tracker = i * spacing;
    offset = floor(tracker / NUM_LEDS);
    index = tracker % NUM_LEDS + offset; 

    leds[index] = rand_color();

    FastLED.show();
    delay(cycle_rate);
    dim_leds(40);
  } 
}

void error_animation() {
  for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Red;
  }
  delay(1000);
  for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Black;
  }
  delay(1000);
}
