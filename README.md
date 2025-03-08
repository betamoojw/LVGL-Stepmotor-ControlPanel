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
```
## 🛠 Wiring
![schematic](https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/Wiringstepmotor.png)   

## 🖥 UI Preview (LVGL Touchscreen)
<table>
  <tr>
    <td><img src="https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/3.png" alt="Image 1" width="200"/></td>
    <td><img src="https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/4.png" alt="Image 2" width="200"/></td> 
    <td><img src="https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/5.png" alt="Image 2" width="200"/></td> 
  </tr>
</table>  
## 🛠 Lead screw mechanism
  <tr>
    <td><img src="https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/1.png" alt="Image 1" width="600"/></td>
    <td><img src="https://github.com/pangcrd/LVGL-Stepmotor-ControlPanel/blob/main/Images/2.png" alt="Image 1" width="600"/></td>
  </tr>
</table>
## 🔄 Communication Overview  
- The system operates in Master-Slave mode via UART communication:
- Master (ESP32 + LVGL UI) sends control commands in JSON format
- Slave (ESP32 + Stepper Control) receives commands, moves the stepper motor, and sends back the real-time position  
📌 **Example JSON Data Sent from Master**  

```sh
{ "type": "motordata", "distance": 100, "Count": 50 }
```
📌 **Example JSON Data Received from Slave**  

```sh
{ "type": "posdata", "position": 120.5 }
```
## 🎬 Demo Video
