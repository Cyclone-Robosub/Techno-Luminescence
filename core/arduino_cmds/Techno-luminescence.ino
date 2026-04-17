#include <FastLED.h>
#include <stdlib.h>
#define NUM_LEDS 16
#define LED_PIN 2
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

void light_chunk_at_interval(
  bool even = true,
  int chunk = 3,
  fl::u32 color = DEFAULT_COLOR
) {

  for(int i = 0; i < NUM_LEDS; i++){
    if(i != 0 && i % chunk == 0) even = !even;
    if(even) leds[i] = DEFAULT_COLOR;
    else leds[i] = CRGB::Black; 
  }
  FastLED.show();
}

void shifting_animation(
  int cycle_rate = CYCLE_RATE * 4
) {
  for (int i = 0; i < 6; i++){
    if(i % 2 == 0) light_chunk_at_interval();
    else light_chunk_at_interval(false);
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
    leds[index] = rand_color();
    //leds[index] = DEFAULT_COLOR;
    FastLED.show();
    delay(cycle_rate);
    dim_leds(40);
  } 
}

void heartbeat_animation ( // UNUSED
  // char preset = "slow", 
  // fl::u32 color = CRGB::Red, 
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE * 8
) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color; 
  }
  FastLED.show(); 
  delay(cycle_rate);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; 
  }
  FastLED.show(); 
  delay(cycle_rate);
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

void progress_bar ( // SO FAR UNUSED
  float progress,
  char direction = "left", 
  bool inverted = false, 
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE
) {
  fl::u32 primary_color;
  fl::u32 secondary_color;

  if (inverted) {
    primary_color = CRGB::Black;
    secondary_color = color; 
  } else {
    primary_color = color;
    secondary_color = CRGB::Black; 
  }

  int led_threshold = floor(NUM_LEDS * progress);

  for (int i = 0; i < led_threshold; i++) {
    leds[i] = primary_color; 
  }

  for (int i = led_threshold; i < NUM_LEDS; i++) {
    leds[i] = secondary_color; 
  }
  FastLED.show(); 
  delay(10);
}


void progress_bar_animation (
  char direction = "center", 
  bool inverted = false, 
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE/16
) {
  for (int i = 0; i < 100; i++) {
    progress_bar(i * 0.01);
    delay(cycle_rate);
  }
  for (int i = 0; i < 100; i++) {
    progress_bar(i * 0.01, "left", true);
    delay(cycle_rate);
  }
}


void loop () {
  // CMD 1: Drive to World Waypoint (Driving)
  shifting_animation();
  shifting_animation();
  delay(300); 

  // CMD 3: Idle / Standby
  pulse_animation(true);
  pulse_animation(true);
  delay(300); 

  // CMD 2: Drive to World Waypoint (Seeking)
  shifting_animation();
  projectile_animation(BACKWARD, CRGB::DeepPink); // DeepPink
  shifting_animation();
  projectile_animation(BACKWARD, CRGB::DeepPink); // DeepPink
  delay(300); 

  // CMD 4: Track Object Waypoint (Tracking)
  bouncing_animation(CRGB::Gold); // DarkOrange
  bouncing_animation(CRGB::Gold); // DarkOrange
  delay(300);

  // CMD 5: Trick
  twinkle_animation();
  twinkle_animation();
  twinkle_animation();
  twinkle_animation();
  delay(300);


  /**break sequence idea 1
    * if read from serial detects new command.. break for loop
    * send new command??
  **/

  /**break sequence idea 2
    * thread 1: keeps track of commands
    * thread 2: executes commands
  **/
 }
