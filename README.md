## Running Microros

```colcon build```

```source install/local_setup.bash ```

```ros2 run micro_ros_setup build_agent.sh```

```source ~/uros_ws/install/local_setup.bash```

```ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0```

> Side notes:
> * Assuming ROS2 is installed, microros can be installed following the README at https://github.com/micro-ROS/micro_ros_setup
> * While the repo should have the microros agent "created", if not, run the following after ```source install``` command:  ```ros2 run micro_ros_setup create_agent_ws.sh```
> * Refer to https://www.hackster.io/514301/micro-ros-on-esp32-using-arduino-ide-1360ca for troubleshooting microros on the esp32, remember to use the jazzy distro






