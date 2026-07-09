#include "microros.h"
#include "light_animations.h"
#include "dropper.h"
#include "go_switch.h"

// ---- PUBLISHING COMMANDS ----

struct MissionCommand {
  const char* name;
  Animation animation;
};

MissionCommand mission_commands[] = {
  {"DriveToWorldWaypoint",        Animation::Shifting},
  {"DriveToWorldWaypointSeeking", Animation::Cross},
  {"Idle",                        Animation::Pulse},
  {"TrackObjectWaypoint",         Animation::Trailing},
  {"DistanceTrick",               Animation::Twinkle},
  {"DurationTrick",               Animation::Twinkle},
};

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

void animate_cmd(const char* cmd) {
  for (auto& mission_command : mission_commands) {
    if (strcmp(cmd, mission_command.name) == 0) {
      animate_leds(mission_command.animation);
      return;
    }
  }
}

void loop() {
  microros_spin();
  go_switch_update();
  dropper_update();
  
  if (handle_heartbeat_timeout()) {
    animate_leds(Animation::Error);
  } else {
    animate_cmd(get_mission_msg());
  }
  
}
