# DOSEMATIC – Automatic Pill Dispenser

A commissioned embedded hardware product that automatically dispenses medication at scheduled times. Built with ESP32 using ESP-IDF and delivered as a finished product in a custom enclosure.

## Overview

DOSEMATIC manages up to 6 pill compartments on a rotating tray. At each scheduled dose time, it checks whether the previous pill was taken using a TCRT5000 IR sensor. If taken, it rotates the stepper motor to the next compartment. If missed, it logs the missed dose and alerts via buzzer. Status is shown in real time on an SSD1306 OLED display.

## Features

- 6 scheduled dose times configurable in firmware
- Stepper motor rotates tray 51° per dose (7 positions including home)
- TCRT5000 IR sensor detects whether the pill compartment is empty
- Buzzer alert when a pill is still present at dose time
- SSD1306 OLED shows next dose countdown and last dose status
- Tracks successful and missed doses
- Auto home rotation at the end if no doses were missed
- Delivered as a finished product in a custom enclosure

## Hardware

| Component | Purpose |
|-----------|---------|
| ESP32 | Main microcontroller |
| 28BYJ-48 Stepper Motor | Rotates pill tray |
| ULN2003 | Stepper motor driver |
| TCRT5000 | IR sensor — detects pill presence |
| SSD1306 (I2C) | OLED status display |
| Buzzer | Audio alert at dose time |

## Wiring

| Signal | ESP32 GPIO |
|--------|------------|
| Stepper IN1 | GPIO 13 |
| Stepper IN2 | GPIO 12 |
| Stepper IN3 | GPIO 14 |
| Stepper IN4 | GPIO 27 |
| TCRT5000 OUT | GPIO 25 |
| Buzzer | GPIO 32 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |

## Tech Stack

| Item | Detail |
|------|--------|
| Framework | ESP-IDF |
| Language | C |
| Display Protocol | I2C (SSD1306) |
| IDE | VS Code + ESP-IDF Extension |

## Getting Started

### Prerequisites
- ESP-IDF v5.x installed
- VS Code with the ESP-IDF extension

### Setup

1. Clone the repository
```bash
git clone https://github.com/majidkhan-dev/dosemetic-esp-code
cd dosemetic-esp-code
```

2. Set your dose schedule in `main/main.c`
```c
// seconds from boot — adjust to real clock times as needed
int rotation_times[6] = {10, 20, 30, 40, 50, 60};
```

3. Build and flash
```bash
idf.py build flash monitor
```

## How It Works

1. On boot the system loads the 6 scheduled dose times
2. OLED displays countdown to the next dose
3. At each scheduled time the IR sensor checks the compartment
4. Pill still present → dose missed, buzzer alerts, missed counter increments
5. Compartment empty → pill was taken, stepper rotates 51° to the next compartment
6. After all 6 doses, if none were missed, the tray performs a 7th home rotation
7. Final OLED screen shows **ALL DOSES DONE** or **CHECK BOX** depending on result

## License

MIT — feel free to use, modify, and distribute.
