#include <ESP32Servo.h>

//indexed angles that the servo should move to:
#define SERVO_PIN       9
#define RESET_ANGLE     78
#define RELEASE_ANGLE_1 90
#define RELEASE_ANGLE_2 68

void dropper_setup();
void dropper_response(int cmd);
