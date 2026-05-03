# DOSEMATIC – Automatic Pill Dispenser
A commissioned embedded hardware product that automatically dispenses medication at scheduled times. Built with ESP32 and delivered as a finished product in a custom enclosure.
## Overview
DOSEMATIC is a standalone automatic pill dispenser designed for patients who need to take medication at fixed times. The motor rotates to the correct compartment at the scheduled time, while an IR sensor verifies the dose is present and prevents double dispensing.
## Features
- Automatic motor rotation at scheduled medication times
- IR sensor detects dose presence and prevents double dispensing
- OLED display shows current time and next dose schedule
- Buzzer alert when it is time to take medication
- Delivered as a finished product in a custom enclosure
- Commissioned and built for a real client
## Hardware
| Component | Purpose |
|-----------|---------|
| ESP32 | Main microcontroller |
| DC Motor | Rotates pill compartment |
| ULN2003 | Motor driver |
| TCRT5000 | IR sensor to detect dose presence |
| SSD1306 | OLED display |
| Buzzer | Alarm at medication time |
## Tech Stack
| Item | Detail |
|------|--------|
| Framework | ESP-IDF |
| Language | C |
| IDE | VS Code + ESP-IDF Extension |
## Getting Started
### Prerequisites
- ESP-IDF v5.x installed
- VS Code with the ESP-IDF extension
- ESP32 development board
### Setup
1. Clone the repository
```bash
git clone https://github.com/majidkhan-dev/dosemetic-esp-code
cd dosemetic-esp-code
Configure your medication schedule in main/config.h
#define DOSE_HOUR_1    8    // 8:00 AM
#define DOSE_HOUR_2    14   // 2:00 PM
#define DOSE_HOUR_3    20   // 8:00 PM
Build and flash
idf.py build flash monitor
How It Works
ESP32 tracks the current time using its internal timer
At each scheduled dose time, the buzzer alerts the patient
The ULN2003 drives the DC motor to rotate the pill tray to the correct compartment
The TCRT5000 IR sensor checks whether a pill is present in the compartment
If no pill is detected, the system alerts without rotating again — preventing double dispensing
The SSD1306 OLED displays the current time and the next scheduled dose
License
MIT — feel free to use, modify, and distribute.
