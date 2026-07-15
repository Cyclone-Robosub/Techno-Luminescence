#include <ESP32Servo.h>
#include "dropper.h"

static Servo dropperServo;
static unsigned long dropperStartTime;
static bool dropperActive;
static int cmd = 0;

void dropper_setup() {
  dropperServo.attach(SERVO_PIN);
  dropperServo.write(RESET_ANGLE);
}

void dropper_response(int msg_cmd) {
  if (msg_cmd != 1 && msg_cmd != 2) return; // exits if invalid message
  dropperStartTime = millis();
  dropperActive = true;
  cmd = msg_cmd;
}

void dropper_update() {
  if (!dropperActive) return;
  unsigned long dropperActiveTime = millis() - dropperStartTime;
  int release_angle = (cmd == 1) ? RELEASE_ANGLE_1 : RELEASE_ANGLE_2;

  if (dropperActiveTime > DROPPER_DURATION_MS) {
    dropperServo.write(RESET_ANGLE);
    dropperActive = false;
    return;
  }

  // alternate every 1000 ms
  dropperServo.write((dropperActiveTime % 2000 < 1000) ? release_angle : RESET_ANGLE);
}
