#ifndef LIGHTS_H
#define LIGHTS_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include "custom_interfaces/msg/missionmanager.h"
//#include techno-luminescence somehow

#ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
#include <rmw_microros/rmw_microros.h>
#endif

rcl_subscription_t subscriber;
rcl_node_t node;
rclc_executor_t executor;

int openFile(char* path);

char readFromSerial(int fd);

void publishToTopic(char cmd);

#endif