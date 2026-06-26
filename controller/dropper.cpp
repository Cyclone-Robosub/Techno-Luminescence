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

void dropper_serial_reading() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Read input from serial monitor
        command.trim(); // Remove any whitespace

        if (command == "1") {
            myServo.write(releaseAngle1);
            Serial.println("Servo moved to state 1");
        } 
        else if (command == "2") {
            myServo.write(releaseAngle2);
            Serial.println("Servo moved to state 2");
        } 
        else if (command == "r") {
            myServo.write(restAngle);
            Serial.println("Servo moved to rest state");
        } 
        else {
            Serial.println("Invalid command. Use 1, 2, or r.");
        }
    }
}
