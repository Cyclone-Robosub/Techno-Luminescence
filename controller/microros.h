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

#define ERROR true
#define NO_ERROR false

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_animation();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

rcl_publisher_t lights_publisher;
rclc_executor_t executor;

extern std_msgs__msg__String mission_msg;

void microros_setup();
