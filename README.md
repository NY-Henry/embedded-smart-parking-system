# Smart Parking Slot Availability and Warning System

## 📌 Project Overview

This project is a real-time embedded system designed to solve parking inefficiency by automating the detection of vehicle presence in a parking slot. Built using an Arduino Uno and an Ultrasonic Distance Sensor, the system continuously monitors a parking space and provides real-time visual feedback to drivers.

This project was developed for the **Real-Time Embedded Systems** coursework at the Faculty of Science & Computing.

## 🎯 Problem Statement

Parking inefficiency leads to congestion, wasted fuel, and lost time. Drivers often circle parking lots searching for empty spaces. An automated parking availability system directly improves traffic flow by visually guiding drivers to free slots and alerting management to unauthorized obstructions.

## 🏗️ System Architecture & Hardware

The system acts as a localized state machine, utilizing the following hardware components:

- **Microcontroller:** Arduino Uno R3 (The central processing unit)
- **Sensor:** HC-SR04 Ultrasonic Distance Sensor (Detects vehicle presence)
- **Status Indicators:** \* Green LED (Indicates the slot is FREE)
  - Red LED (Indicates the slot is OCCUPIED)
- **Warning Indicator:** Yellow LED (Alerts if an obstruction exceeds the permitted time limit)

## ⚙️ System Logic & State Management

The system operates continuously in a non-blocking event loop, shifting between three distinct states based on sensor input and time calculations:

1. **State A: Free**
   - **Trigger:** The ultrasonic sensor measures a distance > 100 cm.
   - **Action:** The Green LED is activated. Red and Yellow LEDs are deactivated. System timers are kept at zero.

2. **State B: Occupied (Normal Parking)**
   - **Trigger:** A vehicle enters the slot (distance <= 100 cm).
   - **Action:** The system transitions state. The Green LED turns off, and the Red LED turns on.
   - **Background Process:** Using the Arduino's internal `millis()` function, the exact timestamp of arrival is recorded to begin a non-blocking timer.

3. **State C: Obstruction Warning**
   - **Trigger:** The vehicle (or object) remains in the slot (distance <= 100 cm) for a duration exceeding the defined warning threshold (10 seconds for simulation purposes).
   - **Action:** The Red LED remains solid, while the Yellow Warning LED flashes to indicate a persistent obstruction. The flashing is executed using modulo arithmetic on the `millis()` timer to ensure the microcontroller never halts sensor readings (avoiding the `delay()` trap).

## 🚀 Simulation

The system was fully simulated and tested using Autodesk Tinkercad.
_(Note: View the `circuit_design.png` file in this repository to see the hardware layout)._
