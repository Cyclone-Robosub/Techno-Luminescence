start agent. we're supposed use distrobox but thats not my issue rn quyen u got that down right??  
```bash
docker run -it --rm -v /dev:/dev --privileged --net=host   microros/micro-ros-agent:jazzy serial --dev /dev/ttyUSB0 -b 921600
```

open like 8 more terminals to view ros2 stuff if u want
```bash
docker exec -it {container} bash
```

view heartbeat debug
```bash
ros2 topic echo /heartbeat_status
```

mock heartbeats
```bash
ros2 topic pub -r 2 /mission_manager_heartbeat std_msgs/msg/Empty "{}"
ros2 topic pub -r 2 /echo_heartbeat std_msgs/msg/Empty "{}"
ros2 topic pub -r 2 /mux_heartbeat std_msgs/msg/Empty "{}"
ros2 topic pub -r 2 /ctrl_heartbeat std_msgs/msg/Empty "{}"
ros2 topic pub -r 2 /cli_heartbeat std_msgs/msg/Empty "{}"
ros2 topic pub -r 2 /thrust_interface_heartbeat std_msgs/msg/Empty "{}"
```

mock mission_manager to control lights
```bash
ros2 topic pub --once /mission_manager std_msgs/msg/String "data: 'DistanceTrick'"
```

mock manipulator command
```bash
ros2 topic pub --once /manipulator_cmd std_msgs/msg/UInt8 "data: 1"
```

watch go signal topic
```bash
ros2 topic echo /go_signal
```
