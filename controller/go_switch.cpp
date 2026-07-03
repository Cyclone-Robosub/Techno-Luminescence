#include <go_switch.h>

#define GO_SWITCH_PIN 27

volatile bool go_switch_triggered = false;

void go_switch_isr() {
  go_switch_triggered = true;
}

void go_switch_setup() {
  pinMode(GO_SWITCH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(GO_SWITCH_PIN), go_switch_isr, RISING);
}

void go_switch_handling() {
  if (!go_switch_triggered) return;
  go_switch_triggered = false;
  // TODO: publish to node
}
