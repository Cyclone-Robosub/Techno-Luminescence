# Techno-Luminescence
A Human Machine Interface (HMI) for Underwater Autonomous Vehicles (AUVs) using LED strips for information communication. 

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

3. **Sequences must not solely rely on color to communicate information.** Color can be used to _emphasize_ a pattern's intent, but different sequences must be different only by the basis of color. This is especially important given the discoloring effects of underwater environments. 


## References
- https://racheldebarros.com/how-to-use-fastled-with-arduino-to-program-led-strips/
