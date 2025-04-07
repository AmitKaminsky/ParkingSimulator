# Parking System Project

An Arduino-based parking management system that integrates various sensors, lighting controls, and MQTT for seamless parking automation.
Watch the video of the simulator, linnk below :)

## Overview

- **Objective:** Automate parking operations by detecting vehicle presence, controlling lighting and barriers, and communicating status over MQTT.
- **Features:**
  - **IRReceiver:** Receives remote control signals.
  - **LightController & Lights:** Manage parking lot illumination.
  - **UltrasonicSensor:** Detects vehicle presence and distance.
  - **ServoMover:** Controls mechanical movements like barrier operation.
  - **Stick:** Provides additional input functionality.
  - **MQTT_Helper:** Facilitates IoT communication via MQTT.

## Setup

1. Open `ParkingMain.ino` in the Arduino IDE.
2. Ensure all necessary libraries are installed and the hardware components are correctly connected (check inline code comments for details).
3. Upload the code to your Arduino board and configure your MQTT broker settings.

## Demo

Watch the project in action: [YouTube Demo](https://youtu.be/7ftjWcbVuFA?si=QlNHWxdWnDBPtAW4)

## Contributors

- Theodore_Dai_Maman  
- Omer_Dan  
- Amit_Kaminsky
