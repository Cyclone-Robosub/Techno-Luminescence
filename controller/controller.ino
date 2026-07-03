#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/u_int8.h>

#include "light_animations.h"
#include "dropper.h"
#include "go_switch.h"

rcl_publisher_t lights_publisher;

rcl_subscription_t mission_subscriber;
rcl_subscription_t echo_heartbeat_subscriber; //"echo_heartbeat"
rcl_subscription_t mux_heartbeat_subscriber; //"mux_heartbeat"
rcl_subscription_t ctrl_heartbeat_subscriber; //"ctrl_heartbeat"
rcl_subscription_t cli_heartbeat_subscriber; //"cli_heartbeat"
rcl_subscription_t thust_int_heartbeat_subscriber; //"thrust_interface_heartbeat"
rcl_subscription_t manipulator_subscriber; //"manipulator_cmd"

std_msgs__msg__String mission_msg;
std_msgs__msg__String heartbeat_msg;
std_msgs__msg__UInt8 manipulator_msg;

const bool echo_heartbeat_msg;
const bool mux_heartbeat_msg;
const bool ctrl_heartbeat_msg;
const bool cli_heartbeat_msg;
const bool thrust_int_heartbeat_msg;

#define ERROR true
#define NO_ERROR false

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t light_controller_node;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// ---- PUBLISHING COMMANDS ----

void publish_driving(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    shifting_animation();
}

void publish_seeking(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    shifting_animation();
    cross_animation();
    cross_animation();
}

void publish_idle(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    pulse_animation();
}

void publish_tracking(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    trailing_animation();
}

void publish_trick(){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &mission_msg, NULL));
    twinkle_animation();
}

void publish_heartbeat(bool error){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &heartbeat_msg, NULL));
    if(error) error_animation();
}

void publish_mission_command(){
  const char* cmd = strcpy(mission_msg.data.data, cmd);
  if (strcmp(cmd, "DriveToWorldWaypoint") == 0) {
    publish_driving();
  }
  else if (strcmp(cmd, "DriveToWorldWaypointSeeking") == 0) {
    publish_seeking();
  }
  else if (strcmp(cmd, "Idle") == 0) {
    publish_idle();
  }
  else if (strcmp(cmd, "TrackObjectWaypoint") == 0) {
    publish_tracking();
  }
  else if (strcmp(cmd, "DistanceTrick") == 0 || strcmp(cmd, "DurationTrick") == 0) {
    publish_trick();
  }
  else return;
}

// ---- SERIAL HANDLING ----

void setup_serial(){
    int rxPin = 32; // GPIO4 works on esp32
    int txPin = 33; // GPIO5 works on esp32
    int baud = 115200; 

    Serial1.begin(baud, SERIAL_8N1, rxPin, txPin); //SERIAL_8N1 -> 8 bits, no parity, 1 stop bit
}

// ---- MICROROS ----

/* Callbacks*/

void mission_callback(const void *msgin) {
  const char* text = (const char *)msgin;
  strcpy(mission_msg.data.data, text);
  mission_msg.data.size = strlen(text);
}

void write_to_heartbeat(const char* text) {
  strcpy(heartbeat_msg.data.data, text);
  heartbeat_msg.data.size = strlen(text);
}

bool all_heartbeats_valid() {
  return echo_heartbeat_msg 
        && mux_heartbeat_msg 
        && ctrl_heartbeat_msg 
        && cli_heartbeat_msg 
        && thrust_int_heartbeat_msg;
}

void heartbeat_callback(const char* error_msg, const char* resolve_msg, const bool prev_msg, const bool* curr_msg) {
  if (!curr_msg) {
    write_to_heartbeat(error_msg);
    publish_heartbeat(ERROR);
  }
  else{
    if (!prev_msg && curr_msg) { // heartbeat comes back on
      write_to_heartbeat(resolve_msg);
      publish_heartbeat(ERROR);
    }

    if (all_heartbeats_valid()) { 
      write_to_heartbeat("");
      publish_heartbeat(NO_ERROR);
    }
  }
}

void echo_callback(const void *msgin) {
  const char* error_msg = "Error: Echo Down\n";
  const char* resolve_msg = "Resolved: Echo Up\n";
  const bool prev_msg = echo_heartbeat_msg;
  echo_heartbeat_msg = (const bool *)msgin;

  heartbeat_callback(error_msg, resolve_msg, prev_msg, echo_heartbeat_msg);
}

void mux_callback(const void *msgin) {
  const char* error_msg = "Error: Mux Down\n";
  const char* resolve_msg = "Resolved: Mux Up\n";
  const bool prev_msg = mux_heartbeat_msg;
  mux_heartbeat_msg = (const bool *)msgin;

  heartbeat_callback(error_msg, resolve_msg, prev_msg, mux_heartbeat_msg);
}

void ctrl_callback(const void *msgin) {
  const char* error_msg = "Error: Ctrl Down\n";
  const char* resolve_msg = "Resolved: Ctrl Up\n";
  const bool prev_msg = ctrl_heartbeat_msg;
  ctrl_heartbeat_msg = (const bool *)msgin;

  heartbeat_callback(error_msg, resolve_msg, prev_msg, ctrl_heartbeat_msg);
}

void cli_callback(const void *msgin) {
  const char* error_msg = "Error: CLI Down\n";
  const char* resolve_msg = "Resolved: CLI Up\n";
  const bool prev_msg = cli_heartbeat_msg;
  cli_heartbeat_msg = (const bool *)msgin;

  heartbeat_callback(error_msg, resolve_msg, prev_msg, cli_heartbeat_msg);
}

void thrust_interface_callback(const void *msgin) {
  const char* error_msg = "Error: Thrust Interface Down\n";
  const char* resolve_msg = "Resolved: Thrust Interface Up\n";
  const bool prev_msg = thrust_int_heartbeat_msg;
  thrust_int_heartbeat_msg = (const bool *)msgin;

  heartbeat_callback(error_msg, resolve_msg, prev_msg, thrust_int_heartbeat_msg);
}

void manipulator_callback(const void *msgin) {
  manipulator_msg.data = *((const std_msgs__msg__UInt8*) msgin);
  dropper_response(manipulator_msg.data);
}

/* Setups */
void string_messages_setup() { // memory allocation for string messages
  char mission_buffer[64];
  mission_msg.data.data     = mission_buffer;  // pointer to the buffer
  mission_msg.data.capacity = sizeof(mission_buffer);
  mission_msg.data.size     = 0;               // current length, starts empty

  char heartbeat_buffer[64];
  heartbeat_msg.data.data     = heartbeat_buffer;
  heartbeat_msg.data.capacity = sizeof(heatbeat_buffer);
  heartbeat_msg.data.size     = 0;
}

void microros_setup() {
  allocator = rcl_get_default_allocator();
  string_messages_setup();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&light_controller_node, "lights_controller_node", "", &support));

  // create lights_publisher
  RCCHECK(rclc_publisher_init_default(
    &lights_publisher,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "lights_controller_publisher"));

  create_subscribers();

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  add_subscriptions_to_executor();
}

void create_subscribers() {
  // create mission_subscriber
  RCCHECK(rclc_subscription_init_default(
    &mission_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "mission_manager")); // TBD: idk what the topic name is that we're subscribing to, this is the node

  // create heartbeat subscribers
  RCCHECK(rclc_subscription_init_default(
    &echo_heartbeat_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), 
    "echo_heartbeat"));

  RCCHECK(rclc_subscription_init_default(
    &mux_heartbeat_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "mux_heartbeat"));

  RCCHECK(rclc_subscription_init_default(
    &ctrl_heartbeat_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "ctrl_heartbeat"));
  
  RCCHECK(rclc_subscription_init_default(
    &cli_heartbeat_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "cli_heartbeat"));

  RCCHECK(rclc_subscription_init_default(
    &thrust_int_heartbeat_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "thrust_interface_heartbeat"));

  // create dropper subscriber
  RCCHECK(rclc_subscription_init_default(
    &manipulator_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Uint8),
    "manipulator_cmd"));
}

void add_subscriptions_to_executor() {
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &mission_subscriber,
    &mission_msg,
    &mission_callback,
    ON_NEW_DATA));

  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &echo_heartbeat_subscriber,
    &heartbeat_msg,
    &echo_callback,
    ON_NEW_DATA));

  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &mux_heartbeat_subscriber,
    &heartbeat_msg,
    &mux_callback,
    ON_NEW_DATA));
  
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &ctrl_heartbeat_subscriber,
    &heartbeat_msg,
    &ctrl_callback,
    ON_NEW_DATA));
  
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &cli_heartbeat_subscriber,
    &heartbeat_msg,
    &cli_callback,
    ON_NEW_DATA));
  
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &thrust_int_heartbeat_subscriber,
    &heartbeat_msg,
    &thrust_interface_callback,
    ON_NEW_DATA));
  
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &manipulator_subscriber,
    &manipulator_msg,
    &manipulator_callback,
    ON_NEW_DATA));
}

// ---- ARDUINO ----

void setup() {
  set_microros_transports();
  
  animations_setup();
  
  delay(2000);

  dropper_setup();
  microros_setup();
  go_switch_setup()
  setup_serial();
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  
  while(1){
    if(heartbeat_msg.size == NO_ERROR) { // size == 0 == false
      publish_mission_command();
    }

    go_switch_handling();
  }
}
