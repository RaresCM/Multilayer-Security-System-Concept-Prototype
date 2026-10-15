# Multilayer-Security-System-Concept-Prototype

> **Implementation Note:** Drivers for RC552 RFID reader were sourced online in C code. The rest of the code is written by me using ATmega 2560 standard AVR libraries which are also available in my documentation. Direct register manipulation was performed using my own ATmega 2560 memory register map, [linked here](https://github.com/RaresCM/ATMega2560-Custom-Documentation).

## Overview
My custom alarm system built on a ATmega 2560 development board platform in bare metal C. It uses a RC552 RFID reader, sound sensor, HC-SR501 PIR module, membrane keypad button matrix and a active buzzer.

<img width="826" height="1415" alt="securitysystem" src="https://github.com/user-attachments/assets/6ff5ff27-29dd-47fa-a683-1c08eb4036da" />

## Key Features
This project was built to apply concepts from my EEE course as well as module integration and features:
* My own keypad scan logic along with debounce and pressed key detection
* Interrupts to catch any sensor triggers no matter what
* PIR sensor with around 3 meters of detection range
* Sound sensor to detect sounds and air pressure changes
* Keypad and RFID reader to disable the alarm using a password or card/fob
* Active buzzer to sound the alarm if any triggers are detected

## Issue and possible improvement

Difficult to tune the sensors perfectly due to them being cheap so while they work 90 percent of the time some outside triggers are physically not detected by the hardware. Lacks advanced protection against attacks on the RFID reader.

## Repository Structure
- /main.c - The main code for the clock to be uploaded to the chip
- /platformio.ini - PlatformIO settings to be used with VS code and PlatformIO extension
- /designlog.md - My log of findings, learning and obstacles encountered during the design of the alarm clock
