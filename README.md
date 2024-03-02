# Autonomous Arduino Car Project

This project is about building an autonomous car using Arduino as the core controller, integrating various sensors for environmental awareness and navigation, and leveraging an ESP module for remote control capabilities over the web. The goal is to create a vehicle capable of autonomous movement, obstacle avoidance, and direction control, with an added feature of remote operation via a web interface.

## Features

- **Autonomous Navigation**: Utilizes LIDAR (Light Detection and Ranging) for obstacle detection and avoidance, enabling the car to navigate through its environment intelligently.
- **Directional Control**: Incorporates a compass module for accurate directional orientation, aiding in the precise movement and pathfinding of the car.
- **Remote Control**: Features an ESP module for Wi-Fi connectivity, allowing the car to be controlled remotely through a web interface. This adds a manual override and remote operation capability to the autonomous vehicle.
- **Customizable Web Interface**: A simple yet effective web interface to control the car remotely, offering commands like forward, backward, left, and right.

## Components

- Arduino Board (Uno/Nano/Mega)
- LIDAR Sensor for obstacle detection
- Compass Module for directional control
- ESP8266 or ESP32 Module for Wi-Fi connectivity
- Motors and Motor Driver Module
- Battery and Power Management Module
- Chassis and Wheels
- Miscellaneous (Wires, Breadboard, etc.)

## Circuit Diagram

> Include a circuit diagram here. You can use tools like Fritzing to design and export the diagram.

## Software Requirements

- Arduino IDE for programming the Arduino and ESP modules.
- Any modern web browser to access the car's web interface.

## Setup and Installation

1. **Assemble the Hardware**: Follow the circuit diagram to connect the sensors, ESP module, motor driver, and other components to the Arduino board.

2. **Programming the Arduino**:
    - Open the Arduino IDE.
    - Load the provided sketch (`arduino_car.ino`) into the IDE.
    - Connect the Arduino to your computer via USB.
    - Select the correct board and port from the Tools menu.
    - Upload the sketch to your Arduino.

3. **Setting Up the ESP Module**:
    - Ensure the ESP module is correctly wired to the Arduino.
    - Load the ESP sketch (`esp_car_code.ino`) into the Arduino IDE.
    - Follow the instructions for setting up the ESP environment if not already done.
    - Upload the sketch to the ESP module.

4. **Connecting to the Web Interface**:
    - Power up the car and ensure it's connected to your Wi-Fi network.
    - Open a web browser and enter the ESP module's IP address.
    - You should now see the web interface for controlling the car.

## Usage

- Use the web interface to control the car remotely. The interface includes buttons for moving the car in different directions.
- Switch to autonomous mode to let the car navigate on its own. The LIDAR sensor and compass module will enable it to avoid obstacles and maintain its path.


Enjoy building and experimenting with your autonomous Arduino car!
