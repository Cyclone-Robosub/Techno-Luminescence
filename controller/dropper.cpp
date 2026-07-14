#include <ESP32Servo.h>
#include "dropper.h"

static Servo dropperServo;
static unsigned long dropperStartTime;
static bool dropperActive;

void dropper_setup() {
  dropperServo.attach(SERVO_PIN);
  dropperServo.write(RESET_ANGLE);
}

void dropper_response(int cmd) {
  if (cmd != 1 && cmd != 2) return; // exits if invalid message

  dropperServo.write(cmd == 1 ? RELEASE_ANGLE_1 : RELEASE_ANGLE_2);
  dropperStartTime = millis();
  dropperActive = true;
}

void dropper_update() {
  if (!dropperActive) return;
  if (millis() - dropperStartTime < DROPPER_DURATION_MS) return;
  
  dropperServo.write(RESET_ANGLE);
  dropperActive = false;
}
