# Techno-Luminescence
A Human Machine Interface (HMI) for Underwater Autonomous Vehicles (AUVs) using LED strips for information communication. 

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

## Project Context & Objective
During AUV operations, the vehicle is frequently changing through the following states

- **Driving** - moving to the next programmed way-point
- **Seeking** - looking for an object using onboard vision system
- **Tracking** - with an object identified, move with respect to that object
- **Trick!** - motion programed to be fun
- **Standby** - programed wait time for the next command
- **Error** - robot not functioning as expected

While some states are obviously identifiable from robot motions, others are a bit harder for observers to read. The goal of Techno-Luminescence is to address this gap in machine transparency so anyone watching the AUV can easily understand and interpret its intentions.

## Methods 
By using two LED strips affixed to the sides of the AUV hull, we can use different flashing sequences to represent each of the robot states. When designing the LED sequences, we adhered to the following principals.

1. **Users should not need a key to understand what a sequence represents**. Sequences should be understandable to users who have never interacted with the system before, and they should lean on preexisting symbolism (colors, sounds, patterns) where possible.

2. **Sequences must be visually distinct from each other in shape, speed, and color.** The familiar user should never confuse one sequences for another during normal operations. 

3. **Sequences must not solely rely on color to communicate information.** Color can be used to _emphasize_ a pattern's intent, but it must not be the only differentiating factor. This is especially important given the discoloring effects of underwater environments. 

## Implementation
We will use the FastLED library for controlling LED strips. Using ROS, will we track the state of the AUV, and switch between patterns when the state is updated.

We will use a separate ESP-32 microprocessor so that if the main board goes offline, we can detect and display that.  


## References
- https://racheldebarros.com/how-to-use-fastled-with-arduino-to-program-led-strips/
