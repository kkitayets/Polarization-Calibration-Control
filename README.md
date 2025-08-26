
# Polarization Control System

![Arduino](https://img.shields.io/badge/Arduino-IDE%202.3.2-blue)
![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU%20V3-orange)
![License](https://img.shields.io/badge/License-MIT-green)

A web-based control system for polarization calibration using a stepper motor. This project allows precise angular control of a NEMA 17 stepper motor through a WiFi web interface.

## Features

- Web-based control interface with captive portal
- Precise angular control (2°, 5°, 10°, 20°, 40°, 90° increments)
- Both clockwise and counter-clockwise rotation
- Home position reset functionality
- Responsive web design optimized for mobile devices
- Open access point (no password required)

## Hardware Requirements

- NodeMCU ESP8266 V3
- NEMA 17 stepper motor
- A4988 stepper motor driver
- External power supply for motor (8-35V)
- Jumper wires and breadboard

## Pin Connections

| ESP8266 Pin | A4988 Pin | Function |
|-------------|-----------|----------|
| D1 (GPIO5)  | STEP      | Step signal |
| D2 (GPIO4)  | DIR       | Direction control |
| 3.3V        | VDD       | Logic power |
| GND         | GND       | Common ground |

## Installation

1. Install Arduino IDE (version 2.3.2 or compatible)
2. Add ESP8266 board support:
   - Go to File → Preferences
   - Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to Additional Board Manager URLs
   - Install ESP8266 board package from Tools → Board → Boards Manager

3. Install required libraries:
   - ESP8266WiFi
   - ESP8266WebServer
   - DNSServer

4. Connect hardware according to pinout table
5. Upload the sketch to your NodeMCU

## Usage

1. Power on the system
2. Connect to WiFi network "ESP8266-AP" (no password)
3. Open web browser and navigate to any address (will redirect to control page)
4. Use the buttons to control motor rotation:
   - Select angle increment (2°, 5°, 10°, 20°, 40°, 90°)
   - Choose rotation direction (clockwise/counter-clockwise)
   - Reset to home position with "Начальное положение" button

## Technical Specifications

- Steps per revolution: 200 (1.8° per step)
- Step delay: 1000μs (adjustable in code)
- Angular resolution: 1.8° (with microstepping capable of higher resolution)
- Web server port: 80
- DNS server port: 53

## Customization

### Modifying Step Parameters
#define STEPS_PER_REV 200 // Change according to your motor
#define STEP_DELAY 1000UL // Adjust for speed control


### Changing WiFi Settings
const char *ssid = "ESP8266-AP"; // Change AP name
const char *password = ""; // Add password if needed

### Adding New Angles
Edit the HTML in the `getPage()` function to add new rotation buttons.

## Project Structure
Polarization-Control/
├── Polarization.ino # Main Arduino sketch
├── README.md # This file
└── (libraries automatically installed by Arduino IDE)

## Troubleshooting

1. **Motor not moving**
   - Check power supply to motor driver
   - Verify pin connections
   - Ensure common ground between ESP8266 and driver

2. **Web interface not accessible**
   - Verify ESP8266 is in AP mode
   - Check if smartphone/computer is connected to "ESP8266-AP"

3. **Inaccurate positioning**
   - Adjust STEP_DELAY for better precision
   - Consider implementing microstepping

## Safety Notes

- Ensure proper heat dissipation for A4988 driver
- Do not exceed motor voltage rating
- Disconnect power when making wiring changes

## License

This project is licensed under the MIT License.

## Author

Lazarev Daniil  
Created: 2024-06-30  
Last Updated: 2024-06-30

## Acknowledgments

- Arduino community for extensive documentation
- ESP8266 core developers for stable WiFi implementation
- Pololu for A4988 driver design
