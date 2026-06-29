#include <Servo.h>
#include "dropper.h"

void dropper_setup() {
    Serial.begin(9600); // Start serial communication
    myServo.attach(servoPin); // Attach the servo to pin 9
    myServo.write(restAngle); // Set initial position to restAngle
    Serial.println("Servo initialized at restAngle");
}
