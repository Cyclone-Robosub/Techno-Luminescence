#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

#include <FastLED.h>
#define NUM_LEDS 15

#define CYCLE_RATE 100
#define DEFAULT_COLOR CRGB::White
#define MAX_INTENSITY 20

CRGB leds[NUM_LEDS];
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
std_msgs__msg__Int32 empty_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define LED_PIN 12

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Red;
    }
    delay(1000);
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Black;
    }
    delay(1000);
  }
}

// ---- ANIMATION HELPERS ----

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

void dim_leds(int fade_rate = 100){
  for (auto &i:leds) {
    i.fadeLightBy( fade_rate);
  }
  FastLED.show();
}

void trail_in(
  int pos = 0,
  fl::u32 primary_color = DEFAULT_COLOR
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
  int pos = 0,
  fl::u32 primary_color = DEFAULT_COLOR
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
  int cycle_rate = CYCLE_RATE * 1.5
) {
  for (int i = 0; i < 12; i++){
    light_at_pos(i);
    delay(cycle_rate);
  }
}

void pulse_animation (
  fl::u32 color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE
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

void trailing_animation(
  fl::u32 primary_color = DEFAULT_COLOR, 
  int cycle_rate = CYCLE_RATE
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
  int spacing = 4, 
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

    FastLED.show();
    delay(cycle_rate);
    dim_leds(40);
  } 
}

// ---- PUBLISHING COMMANDS ----

void publish_driving(){
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    shifting_animation();
}

void publish_seeking(){
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    shifting_animation();
    cross_animation();
    cross_animation();
}

void publish_idle(){
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    pulse_animation();
}

void publish_tracking(){
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    trailing_animation();
}

void publish_trick(){
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    twinkle_animation();
}

// ---- SERIAL HANDLING ----

void setup_serial(){
    int rxPin = 32; // GPIO4 works on esp32
    int txPin = 33; // GPIO5 works on esp32
    int baud = 115200; 

    Serial1.begin(baud, SERIAL_8N1, rxPin, txPin); //SERIAL_8N1 -> 8 bits, no parity, 1 stop bit
}

void get_command_from_serial(char* cmd){
    int i = 0;
    while(Serial1.available() > 0){ // keep reading while data available (terminates when null character met)
        char n = Serial1.read(); // read 1 byte from the serial port
        cmd[i] = n;
        i++;
    }
    cmd[i] = '\0';
}

int convert_command_to_int(char* cmd){
  if(strcmp(cmd, "DriveToWorldWaypoint") == 0) return 1;
  if(strcmp(cmd, "DriveToWorldWaypointSeeking") == 0) return 2;
  if(strcmp(cmd, "Idle") == 0) return 3;
  if(strcmp(cmd, "TrackObjectWaypoint") == 0) return 4;
  if(strcmp(cmd, "DistanceTrick") == 0 || strcmp(cmd, "DurationTrick") == 0) return 5;
  else return 0;
}

//returns total delay
void publish_command_from_serial(){
    switch(msg.data){
      case 1:
        publish_driving();
        break;
      case 2:
        publish_seeking();
        break;
      case 3:
        publish_idle();
        break;
      case 4:
        publish_tracking();
        break;
      case 5:
        publish_trick();
        break;
      default:
        RCSOFTCHECK(rcl_publish(&publisher, &empty_msg, NULL));
        break;
    }
}

// ---- ARDUINO ----

void setup() {
  set_microros_transports();
  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_INTENSITY);
  
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "micro_ros_arduino_node_publisher"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  setup_serial();
  empty_msg.data = 0;
  msg.data = 0;
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  delay(10000);
  const char* test_message = "Idle";
  Serial1.write(test_message);

  int prev_cmd = 0;
  
  while(1){
    char cmd[100];
    get_command_from_serial(cmd);
    prev_cmd = convert_command_to_int(cmd);
    if(prev_cmd != 0) msg.data = prev_cmd;
    publish_command_from_serial();
  }
}
