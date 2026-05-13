#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <termios.h>

#include <std_msgs/msg/int32.h>

#include <FastLED.h>
#define NUM_LEDS 21

#define CYCLE_RATE 100
#define DEFAULT_COLOR CRGB::White
#define MAX_INTENSITY 20

CRGB leds[NUM_LEDS];
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;
int fd;

#define LED_PIN 12

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


/*
-----Thursday Action Items----

figure out naming conventions

figure out message datatypes from Mission Manager - use ints for now

figure out how to set up mock system

*/

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
  for(int i = 0; i <= NUM_LEDS / 2; i++){
    leds[i] = primary_color;
    leds[NUM_LEDS - i] = primary_color;

    FastLED.setBrightness((MAX_INTENSITY * i * i) / (NUM_LEDS * NUM_LEDS / 4));

    if(i == 0) delay(cycle_rate);
    else delay((cycle_rate / 2.0) + (cycle_rate / (i * i * i * 2.0)));

    FastLED.show();
  }

  delay(cycle_rate * 2);
  int middle = NUM_LEDS / 2;

  for(int i = 0; i < NUM_LEDS / 2; i++){
    leds[middle - i] = CRGB::Black;
    leds[middle + i] = CRGB::Black;

    FastLED.setBrightness(MAX_INTENSITY - ((MAX_INTENSITY * i * i) / (NUM_LEDS * NUM_LEDS / 4)));
    delay((2 * cycle_rate / 3) - (2 * cycle_rate * i / (NUM_LEDS * 3)));

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

int open_file(char* path){
    struct termios tty;
    speed_t baud = 0010002; // This means baud rate of 115200
    int fd;
    
    if ((fd = open(path, O_RDWR | O_NOCTTY | O_SYNC)) == -1) {
        return -1;
    }
    
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        return -1;
    }

    // Get and modify current options:
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    
    // Configure 8N1, no flow control
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits
    tty.c_cflag &= ~PARENB; // no parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CRTSCTS; // no hardware flow control
    tty.c_cflag |= CLOCAL | CREAD; // enable receiver

    tty.c_lflag = 0; // non-canonical mode
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_iflag = 0; // no special handling

    tty.c_cc[VMIN] = 0;  // non-blocking read
    tty.c_cc[VTIME] = 10; // 1 second timeout (VTIME is in deciseconds)
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        close(fd);
        return -1;
    }   
    return fd;
}

char get_command_from_serial(){
    ssize_t n = -1;
    char c = '\0';
    
    while(c < '1' || c > '5'){ // keep reading until a command is recieved
        n = read(fd->get_read_fd(), &c, 1); // read 1 byte from the serial port
        if (n <= 0) return '0'; // return error if cannot read from file
    }

    return c; 
}

void publish_command_from_serial(int cmd){
    switch(cmd){
        case '1':
            publish_driving();
            break;
        case '2':
            publish_seeking();
            break;
        case '3':
            publish_idle();
            break;
        case '4':
            publish_tracking();
            break;
        default:
            publish_trick();
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

  char* path = "example path"; // TO DO: SET TO ACTUAL SERIAL PORT
  fd = open_file(path);

  if(fd == -1) error_loop();

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

  msg.data = 0;
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  char cmd;

  while(1){
    cmd = get_command_from_serial();
    if(cmd != '0') publish_command_from_serial(cmd);
  }
}