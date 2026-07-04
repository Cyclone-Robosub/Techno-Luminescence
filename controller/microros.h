#pragma once

#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <chrono>

#include <std_msgs/msg/empty.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/u_int8.h>

#include "light_animations.h"

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){animate_error();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void microros_setup();
int heartbeat_checks();
