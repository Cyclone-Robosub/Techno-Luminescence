#include <Arduino.h>
#include "go_switch.h"
#include "microros.h"

volatile bool go_switch_triggered = false;

void go_switch_isr() {
  go_switch_triggered = true;
}

void go_switch_setup() {
  pinMode(GO_SWITCH_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(GO_SWITCH_PIN), go_switch_isr, RISING);
}

void go_switch_update() {
  if (!go_switch_triggered) return;
  go_switch_triggered = false;
  publish_go_signal(true);
}
