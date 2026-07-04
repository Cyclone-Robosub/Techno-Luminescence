#include <chrono>
#include <cstring>

#include "microros.h"
#include "dropper.h"

class HeartbeatChannel {
    public:
      rcl_subscription_t subscriber;
      const char* topic;
      std_msgs__msg__Empty msg;
      std::chrono::time_point<std::chrono::steady_clock> recent_heartbeat;
      bool no_heartbeat;

    HeartbeatChannel(const char* topic)
      : subscriber{}, msg{}, topic(topic),
        recent_heartbeat(std::chrono::steady_clock::now()), no_heartbeat(false)
    {}

    void callback(const void *msgin){
        recent_heartbeat = std::chrono::steady_clock::now();
    }
};

HeartbeatChannel heartbeat_channels[6] = {
  HeartbeatChannel("mission_manager_heartbeat"),
  HeartbeatChannel("echo_heartbeat"),
  HeartbeatChannel("mux_heartbeat"),
  HeartbeatChannel("ctrl_heartbeat"),
  HeartbeatChannel("cli_heartbeat"),
  HeartbeatChannel("thrust_interface_heartbeat"),
};

int heartbeat_checks() {
    auto current_time = std::chrono::steady_clock::now();
    bool all_heartbeats_valid = true;
    char status_buffer[64];
    for(auto& heartbeat : heartbeat_channels) {
        if (current_time - heartbeat.recent_heartbeat > std::chrono::seconds(1)) {
          // no heartbeat error
          heartbeat.no_heartbeat = true;
          all_heartbeats_valid = false;
          strcpy(status_buffer, "Error: ");
          strcat(status_buffer, heartbeat.topic);
          write_to_heartbeat(status_buffer);
          publish_heartbeat(ERROR);
        } else if (heartbeat.no_heartbeat) {
          // no heartbeat resolved 
          heartbeat.no_heartbeat = false;
          strcpy(status_buffer, "Resolved: ");
          strcat(status_buffer, heartbeat.topic);
          write_to_heartbeat(status_buffer);
          publish_heartbeat(ERROR);
        }
    }

    if(all_heartbeats_valid) {
      write_to_heartbeat("");
      publish_heartbeat(NO_ERROR);
      return 0;
    }
    return 1;
}

rcl_subscription_t mission_subscriber;
rcl_subscription_t manipulator_subscriber;
rcl_publisher_t lights_publisher;

std_msgs__msg__String mission_msg;
std_msgs__msg__UInt8 manipulator_msg;
std_msgs__msg__String heartbeat_msg;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t light_controller_node;

void publish_heartbeat(bool error){
    RCSOFTCHECK(rcl_publish(&lights_publisher, &heartbeat_msg, NULL));
    if(error) animate_error();
}

/* Callbacks*/

void heartbeat_context_callback(const void *msgin, void *context) {
  static_cast<HeartbeatChannel*>(context)->callback(msgin);
}

void mission_callback(const void *msgin) {
  const std_msgs__msg__String* msg = (const std_msgs__msg__String*) msgin;
  strcpy(mission_msg.data.data, msg->data.data);
  mission_msg.data.size = strlen(mission_msg.data.data);
}

void manipulator_callback(const void *msgin) {
  manipulator_msg.data = ((const std_msgs__msg__UInt8*) msgin)->data;
  dropper_response(manipulator_msg.data);
}

void write_to_heartbeat(const char* text) {
  strcpy(heartbeat_msg.data.data, text);
  heartbeat_msg.data.size = strlen(text);
}

void string_messages_setup() { // memory allocation for string messages
  static char mission_buffer[64];
  mission_msg.data.data     = mission_buffer;  // pointer to the buffer
  mission_msg.data.capacity = sizeof(mission_buffer);
  mission_msg.data.size     = 0;               // current length, starts empty

  static char heartbeat_buffer[64];
  heartbeat_msg.data.data     = heartbeat_buffer;
  heartbeat_msg.data.capacity = sizeof(heartbeat_buffer);
  heartbeat_msg.data.size     = 0;
}

void create_subscribers() {
  // heartbeat subscribers
  for(auto& heartbeat : heartbeat_channels) {
      RCCHECK(rclc_subscription_init_default(
        &heartbeat.subscriber,
        &light_controller_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty),
        heartbeat.topic));
  }
  
  // mission subscriber
  RCCHECK(rclc_subscription_init_default(
    &mission_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "mission_manager")); // TBD: idk what the topic name is that we're subscribing to, this is the node

  // dropper subscriber
  RCCHECK(rclc_subscription_init_default(
    &manipulator_subscriber,
    &light_controller_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
    "manipulator_cmd"));
}

void add_subscriptions_to_executor() {
  // add heartbeat subscribers
  for (auto& heartbeat : heartbeat_channels) {
    RCCHECK(rclc_executor_add_subscription_with_context(
      &executor,
      &heartbeat.subscriber,
      &heartbeat.msg,
      &heartbeat_context_callback,
      &heartbeat,
      ON_NEW_DATA));
  }

  // add mission subscriber
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &mission_subscriber,
    &mission_msg,
    &mission_callback,
    ON_NEW_DATA));

  // add manipulator subscriber
  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &manipulator_subscriber,
    &manipulator_msg,
    &manipulator_callback,
    ON_NEW_DATA));
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
  RCCHECK(rclc_executor_init(&executor, &support.context, 8, &allocator));

  add_subscriptions_to_executor();
}
