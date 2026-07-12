#include <cstring>

#include "microros.h"
#include "dropper.h"

#include <rmw_microros/rmw_microros.h>

class HeartbeatChannel {
    public:
      rcl_subscription_t subscriber;
      const char* topic;
      std_msgs__msg__Empty msg;
      unsigned long last_timestamp;
      bool timed_out;

    HeartbeatChannel(const char* topic)
      : subscriber{}, msg{}, topic(topic),
        last_timestamp(millis()), timed_out(false)
    {}

    void callback(const void *msgin){
        last_timestamp = millis();
    }
};

static HeartbeatChannel heartbeat_channels[6] = {
  HeartbeatChannel("mission_manager_heartbeat"),
  HeartbeatChannel("echo_heartbeat"),
  HeartbeatChannel("mux_heartbeat"),
  HeartbeatChannel("ctrl_heartbeat"),
  HeartbeatChannel("cli_heartbeat"),
  HeartbeatChannel("thrust_interface_heartbeat"),
};

const unsigned long HEARTBEAT_TIMEOUT_MS = 1000;

rcl_subscription_t mission_subscriber;
rcl_subscription_t manipulator_subscriber;
rcl_publisher_t heartbeat_status_publisher;
rcl_publisher_t go_signal_publisher;

rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_node_t esp32_node;

std_msgs__msg__String mission_msg;
std_msgs__msg__UInt8 manipulator_msg;
std_msgs__msg__String heartbeat_msg;
std_msgs__msg__Bool go_signal_msg;

static char mission_buffer[64];
static char heartbeat_buffer[64];

// override arduino_transport_open() to increase baud rate
extern "C" bool arduino_transport_open(struct uxrCustomTransport * transport) {
  Serial.begin(921600);
  return true;
}

// --- CALLBACKS ---

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

// --- SUBSCRIBERS ---

void create_subscribers() {
  // heartbeat subscribers
  for(auto& heartbeat : heartbeat_channels) {
      RCCHECK(rclc_subscription_init_default(
        &heartbeat.subscriber,
        &esp32_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty),
        heartbeat.topic));
  }
  
  // mission subscriber
  RCCHECK(rclc_subscription_init_default(
    &mission_subscriber,
    &esp32_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "mission_manager")); // TBD: idk what the topic name is that we're subscribing to, this is the node

  // dropper subscriber
  RCCHECK(rclc_subscription_init_default(
    &manipulator_subscriber,
    &esp32_node,
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

enum class HeartbeatStatus { Healthy, Error, Resolved };

void publish_heartbeat_status (HeartbeatStatus hb_status, const char* topic) {
  switch (hb_status) {
    case HeartbeatStatus::Healthy:
      heartbeat_msg.data.data[0] = '\0';
      heartbeat_msg.data.size = 0;
      break;
    case HeartbeatStatus::Error:
      heartbeat_msg.data.size = snprintf(heartbeat_msg.data.data, 
        heartbeat_msg.data.capacity, 
        "ERROR: %s", topic);
      break;
    case HeartbeatStatus::Resolved:
      heartbeat_msg.data.size = snprintf(heartbeat_msg.data.data, 
        heartbeat_msg.data.capacity, 
        "RESOLVED: %s", topic);
      break;
  }

  RCSOFTCHECK(rcl_publish(&heartbeat_status_publisher, &heartbeat_msg, NULL));
}

int handle_heartbeat_timeout() {
    unsigned long current_time = millis();
    bool all_heartbeats_valid = true;

    for (auto& heartbeat : heartbeat_channels) {
      unsigned long timeSinceHeartbeat = current_time - heartbeat.last_timestamp;
      if (timeSinceHeartbeat > HEARTBEAT_TIMEOUT_MS) {
        heartbeat.timed_out = true;
        all_heartbeats_valid = false;
        publish_heartbeat_status(HeartbeatStatus::Error, heartbeat.topic);
      } else if (heartbeat.timed_out) {
        // no heartbeat resolved
        heartbeat.timed_out = false;
        publish_heartbeat_status(HeartbeatStatus::Resolved, heartbeat.topic);
      }
    }

    if (!all_heartbeats_valid) return 1;
    
    publish_heartbeat_status(HeartbeatStatus::Healthy, nullptr);
    return 0;  
}

void string_messages_setup() { // memory allocation for string messages
  mission_msg.data.data     = mission_buffer;  // pointer to the buffer
  mission_msg.data.capacity = sizeof(mission_buffer);
  mission_msg.data.size     = 0;               // current length, starts empty

  heartbeat_msg.data.data     = heartbeat_buffer;
  heartbeat_msg.data.capacity = sizeof(heartbeat_buffer);
  heartbeat_msg.data.size     = 0;
}

void microros_setup() {
  allocator = rcl_get_default_allocator();
  string_messages_setup();

  // wait until microros agent 
  while (rmw_uros_ping_agent(100, 1) != RMW_RET_OK) {
    delay(25);
  }

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&esp32_node, "esp32_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &heartbeat_status_publisher,
    &esp32_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "heartbeat_status"));

  // create go switch publisher
  RCCHECK(rclc_publisher_init_default(
    &go_signal_publisher,
    &esp32_node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "go_signal"));

  create_subscribers();

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 8, &allocator));
  add_subscriptions_to_executor();
}

void microros_spin() {
  static unsigned long last_agent_ping = millis();
  unsigned long now = millis();

  // poll agent connection. restart ESP on fail.
  if (now - last_agent_ping > 500) {
    last_agent_ping = now;
    if (rmw_uros_ping_agent(100, 1) != RMW_RET_OK) {
      esp_system_abort("Agent connection lost."); 
    }
  }

  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(0)));
}

const char* get_mission_msg() {
  return mission_msg.data.data;
}

void publish_go_signal(bool triggered) {
  go_signal_msg.data = triggered;
  RCSOFTCHECK(rcl_publish(&go_signal_publisher, &go_signal_msg, NULL));
}
