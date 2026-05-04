#include <FastLED.h>
#include <stdlib.h>
#define NUM_LEDS 24
#define LED_PIN 12 //working with esp32
//#define LED_PIN 2 //working with arduino uno
#define CYCLE_RATE 100
#define DEFAULT_COLOR CRGB::White
#define MAX_INTENSITY 20
#define PI 3.14

CRGB leds[NUM_LEDS];
enum direction {
  FORWARD, BACKWARD
};

void setup () {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_INTENSITY);
  delay(2000);
}

void dim_leds (int fade_rate = 100) {
  for (auto &i:leds) {
    i.fadeLightBy( fade_rate );
  }

  FastLED.show();
}

void projectile_animation (
  enum direction direct = FORWARD,
  fl::u32 color = DEFAULT_COLOR,
  int cycle_rate = CYCLE_RATE
) { 

  int dir;
  int start;
  int end;

  if (direct == BACKWARD) {
    dir = -1; 
    start = NUM_LEDS - 1;
    end = 0;
  } else {
    dir = 1;
    start = 0;
    end = NUM_LEDS - 1;
  }

  for (int i = start; i != end + dir; i += dir) {
    leds[i] = color;
    FastLED.show();
    delay(cycle_rate);
    dim_leds();
  }
}

void bouncing_animation (
  fl::u32 color = DEFAULT_COLOR,
  int cycle_rate = CYCLE_RATE
) {
  projectile_animation(FORWARD, color, cycle_rate);
  projectile_animation(BACKWARD, color, cycle_rate);
}

void light_at_pos(
  int lit_position = 0,
  fl::u32 color = DEFAULT_COLOR
) {
  for(int i = 0; i < NUM_LEDS; i++){
    if(((i - lit_position) % 4) == 0) leds[i] = DEFAULT_COLOR;
    else leds[i] = CRGB::Black; 
  }
  FastLED.show();
}

void shifting_animation(
  int cycle_rate = CYCLE_RATE * 1.5
) {
  for (int i = 0; i < 12; i++){
    light_at_pos(i);
    delay(cycle_rate);
  }
}

fl::u32 rand_color(){
  int colors_size = 6;
  fl::u32 colors[] = {CRGB::Red, CRGB::DarkOrange, CRGB::Green, CRGB::HotPink, CRGB::Blue, CRGB::Purple};
  int i = rand() % (colors_size + 1); 
  return colors[i];
}

void twinkle_animation (
  bool default_color = false,
  int spacing = 4, 
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE / 4
) {
  int index = 0;  
  int tracker = 0;
  int offset = 0; 

  for (int i = 0; i < NUM_LEDS; i++) {
    tracker = i * spacing;
    offset = floor(tracker / NUM_LEDS);
    index = tracker % NUM_LEDS + offset; 
    if(!default_color) leds[index] = rand_color();
    else leds[index] = DEFAULT_COLOR;
    FastLED.show();
    delay(cycle_rate);
    dim_leds(40);
  } 
}

void pulse_animation (
  bool inverted = false, 
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE
) {
  
  volatile float intensity;
  volatile float cycle_index = 0;
  
  while (cycle_index < 2 * PI) {
    if (inverted) {
      intensity = (-1* abs(sin(cycle_index)) + 1) * MAX_INTENSITY;
    } else {
      intensity = abs(sin(cycle_index)) * MAX_INTENSITY;
    }
    cycle_index += PI / 64; 

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color; 
      FastLED.setBrightness(intensity);
    }
    FastLED.show();
    delay(cycle_rate/4);
  }
  // for (int i = 0, )
}

void center_in_out_animation(
  fl::u32 primary_color = DEFAULT_COLOR, 
  fl::u32 secondary_color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE / 2
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

void center_in_trailing_out(
  fl::u32 primary_color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE
){
  // delay = cycle_rate * 1 / (i^3)
  for(int i = 0; i <= NUM_LEDS / 2; i++){
    leds[i] = primary_color;
    leds[NUM_LEDS - i] = primary_color;
    //delay(cycle_rate * cos(i * PI / NUM_LEDS));
    delay((cycle_rate / 2) + (cycle_rate * 1 / (i * i * i * 2)));
    FastLED.setBrightness((MAX_INTENSITY * i * i) / (NUM_LEDS * NUM_LEDS / 4));
    FastLED.show();
  }

  delay(cycle_rate * 2);
  int middle = NUM_LEDS / 2;

  for(int i = 0; i < NUM_LEDS / 2; i++){
    leds[middle - i] = CRGB::Black;
    leds[middle + i] = CRGB::Black;
    delay((2 * cycle_rate / 3) - (2 * cycle_rate * i / (NUM_LEDS * 3)));
    //delay((cycle_rate / 2) + (cycle_rate * 1 / (i * i * i * 2)));
    FastLED.setBrightness(MAX_INTENSITY - ((MAX_INTENSITY * i * i) / (NUM_LEDS * NUM_LEDS / 4)));
    FastLED.show();
  }
}

void toggle_animation(
  fl::u32 primary_color = CRGB::DeepPink, 
  int cycle_rate = CYCLE_RATE * 2
){
  for(int i = 0; i < NUM_LEDS - 1; i++){
   if(i % 4 == 0){
    leds[i + 1] = CRGB::Black;
    leds[i] = primary_color;
   }
 }
 FastLED.show();
 delay(cycle_rate);
 
 for(int i = 1; i < NUM_LEDS; i++){
  if(((i - 1) % 4) == 0){
    leds[i - 1] = CRGB::Black;
    leds[i] = primary_color;
  }
 }
 FastLED.show();
 delay(cycle_rate);
}



void loop () {
  
//  // CMD 1: Drive to World Waypoint (Driving)
//  shifting_animation(); 
//  shifting_animation();
//  delay(300); 
//
//  // CMD 3: Idle / Standby
//  pulse_animation(true);
//  pulse_animation(true);
//  delay(300); 

//  // CMD 2: Drive to World Waypoint (Seeking)
//  shifting_animation();
//  center_in_out_animation();
//  center_in_out_animation();
//  shifting_animation();
//  toggle_animation();
//  toggle_animation();

//  delay(300); 

//  // CMD 4: Track Object Waypoint (Tracking)
  center_in_trailing_out();
//  center_in_out_animation();
//  center_in_out_animation();
//  twinkle_animation(true);
//  center_in_out_animation();
//  center_in_out_animation();
//  twinkle_animation(true);
//  delay(300);
//
//  // CMD 5: Trick
//  twinkle_animation();
//  twinkle_animation();
//  twinkle_animation();
//  twinkle_animation();
//  delay(300);


  /**break sequence idea 1
    * if read from serial detects new command.. break for loop
    * send new command??
  **/

  /**break sequence idea 2
    * thread 1: keeps track of commands
    * thread 2: executes commands
  **/
 }
