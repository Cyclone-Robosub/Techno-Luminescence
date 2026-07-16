#pragma once

#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/empty.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/bool.h>

#include "light_animations.h"

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){animate_leds(Animation::Error);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void microros_setup();
void microros_spin();
int handle_heartbeat_timeout();
const char* get_mission_msg();
void publish_go_signal(bool triggered);
