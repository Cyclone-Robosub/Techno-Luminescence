#include <ESP32Servo.h>
#include "dropper.h"

static Servo dropperServo;
static unsigned long dropperStartTime;
static bool dropperActive;

void dropper_setup() {
    dropperServo.attach(SERVO_PIN); // Attach the servo to pin 9
    dropperServo.write(RESET_ANGLE); // Set initial position to restAngle
}

void dropper_response(int cmd) {
    if (cmd != 1 && cmd != 2) return; // exits if invalid message
  
    dropperServo.write(cmd == 1 ? RELEASE_ANGLE_1 : RELEASE_ANGLE_2);
    dropperStartTime = millis();
    dropperActive = false;
}

void dropper_update() {
  if (!dropperActive) return;
  if (millis() - dropperStartTime < DROPPER_DURATION_MS) return;
  
  dropperServo.write(RESET_ANGLE);
  dropperActive = false;
}
