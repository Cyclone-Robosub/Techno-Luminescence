#include "microros.h"
#include "light_animations.h"
#include "dropper.h"
#include "go_switch.h"

// ---- PUBLISHING COMMANDS ----

struct MissionCommand {
  const char* name;
  void (*animate)();
};

MissionCommand mission_commands[] = {
  {"DriveToWorldWaypoint",        animate_shifting},
  {"DriveToWorldWaypointSeeking", animate_seeking},
  {"Idle",                        animate_pulse},
  {"TrackObjectWaypoint",         animate_trailing},
  {"DistanceTrick",               animate_twinkle},
  {"DurationTrick",               animate_twinkle},
};

void publish_mission_command(){
  RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));

  const char* cmd = mission_msg.data.data;
  for (auto& mission_command : mission_commands) {
    if (strcmp(cmd, mission_command.name) == 0) {
      mission_command.animate();
      return;
    }
  }
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
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  while(1){
    if (heartbeat_checks()) continue;
    
    publish_mission_command()
    go_switch_handling();
  }
}
