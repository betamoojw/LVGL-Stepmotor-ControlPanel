# 🚀 ESP32 Stepper Motor Control with LVGL & UART | Real-time Position Feedback

This project demonstrates how to control a **stepper motor** using an **ESP32**, with an interactive **LVGL UI** for real-time monitoring. The system communicates via **UART**, sending and receiving **position data in JSON format**.

## 📌 Features
✅ Stepper motor control with **AccelStepper** library  
✅ **LVGL UI** for interactive touchscreen control  
✅ **Real-time position feedback** via UART  
✅ Home & Reset functions for precise calibration  
✅ Adjustable **microstepping & lead screw settings**  

## 🛠 Hardware Requirements
- **ESP32** (Master & Slave communication setup)  
- **Stepper motor (e.g., NEMA17)**  
- **Stepper motor driver (e.g., A4988, DRV8825, or TMC2209)**  
- **2mm lead screw mechanism**  
- **Touchscreen display (ESP32 Cheap Yellow display, e.g., 2.8" TFT)**  
- **Limit switch for homing**  

## 🔗 Software & Libraries
- **LVGL** (UI framework)  
- **ArduinoJson** (Data serialization)  
- **AccelStepper** (Stepper motor control)  
- **HardwareSerial** (ESP32 UART communication)  

## 📜 Setup & Installation
1️⃣ **Clone the repository**  
```sh
git clone https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel.git  
## Wiring

