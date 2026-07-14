#include <ESP32Servo.h>

//indexed angles that the servo should move to:
const int SERVO_PIN       = 12;
const int RESET_ANGLE     = 78;
const int RELEASE_ANGLE_1 = 90;
const int RELEASE_ANGLE_2 = 66;

const int DROPPER_DURATION_MS = 2000;

void dropper_setup();
void dropper_response(int cmd);
void dropper_update();
