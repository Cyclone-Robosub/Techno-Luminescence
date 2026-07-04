#include "microros.h"
#include "light_animations.h"
#include "dropper.h"
#include "go_switch.h"

// ---- PUBLISHING COMMANDS ----

void publish_driving(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    shifting_animation();
}

void publish_seeking(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    shifting_animation();
    cross_animation();
    cross_animation();
}

void publish_idle(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    pulse_animation();
}

void publish_tracking(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    trailing_animation();
}

void publish_trick(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    twinkle_animation();
}

void publish_mission_command(){
  const char* cmd = strcpy(mission_msg.data.data, cmd);
  if (strcmp(cmd, "DriveToWorldWaypoint") == 0) {
    publish_driving();
  }
  else if (strcmp(cmd, "DriveToWorldWaypointSeeking") == 0) {
    publish_seeking();
  }
  else if (strcmp(cmd, "Idle") == 0) {
    publish_idle();
  }
  else if (strcmp(cmd, "TrackObjectWaypoint") == 0) {
    publish_tracking();
  }
  else if (strcmp(cmd, "DistanceTrick") == 0 || strcmp(cmd, "DurationTrick") == 0) {
    publish_trick();
  }
  else return;
}

// ---- SERIAL HANDLING ----

void setup_serial(){
    int rxPin = 32; // GPIO4 works on esp32
    int txPin = 33; // GPIO5 works on esp32
    int baud = 115200;

    Serial1.begin(baud, SERIAL_8N1, rxPin, txPin); //SERIAL_8N1 -> 8 bits, no parity, 1 stop bit
}

// ---- ARDUINO ----

void setup() {
  set_microros_transports();

  animations_setup();

  delay(2000);

  dropper_setup();
  microros_setup();
  go_switch_setup();
  setup_serial();
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  while(1){
    if(all_heartbeats_valid()) { 
      publish_mission_command();
    }

    go_switch_handling();
  }
}
