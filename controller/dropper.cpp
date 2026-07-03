#include <Servo.h>
#include "dropper.h"

void dropper_setup() {
    Serial.begin(9600); // Start serial communication
    myServo.attach(servoPin); // Attach the servo to pin 9
    myServo.write(restAngle); // Set initial position to restAngle
}

void dropper_response(int cmd) {
    if (cmd != 1 && cmd != 2) return; // exits if invalid message
  
    if (cmd == 1) myServo.write(releaseAngle1);
    else myServo.write(releaseAngle2);

    usleep(2000000);  // 2000ms
    myServo.write(restAngle); // Go back to reset position
}
