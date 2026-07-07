#include <ESP32Servo.h>
#include "dropper.h"

static Servo dropperServo;

void dropper_setup() {
    dropperServo.attach(SERVO_PIN); // Attach the servo to pin 9
    dropperServo.write(RESET_ANGLE); // Set initial position to restAngle
}

void dropper_response(int cmd) {
    if (cmd != 1 && cmd != 2) return; // exits if invalid message
  
    if (cmd == 1) dropperServo.write(RELEASE_ANGLE_1);
    else dropperServo.write(RELEASE_ANGLE_2);

    usleep(2000000);  // 2000ms 
    dropperServo.write(RESET_ANGLE); // Go back to reset position
}
