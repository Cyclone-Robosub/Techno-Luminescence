#include <Servo.h>

Servo myServo;
const int servoPin = 9;

//indexed angles that the servo should move to:
const int restAngle = 78;
const int releaseAngle1 = 90;
const int releaseAngle2 = 68;

void dropper_setup();

void dropper_response(int cmd);