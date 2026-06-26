#include <Servo.h>

Servo myServo;
const int servoPin = 9;

//indexed angles that the servo should move to:
const int restAngle = 78;
const int releaseAngle1 = 90;
const int releaseAngle2 = 68;

void dropper_setup() {
    Serial.begin(9600); // Start serial communication
    myServo.attach(servoPin); // Attach the servo to pin 9
    myServo.write(restAngle); // Set initial position to restAngle
    Serial.println("Servo initialized at restAngle");
}