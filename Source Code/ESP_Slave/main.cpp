#include "Arduino.h"
#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(2);  // UART2

const byte limitSwitch = 33;

bool limitTriggered = false; 

AccelStepper stepper(1, 26, 27);

// config microstep
#define MICROSTEP 1  //(1,2,4,8,16)
#define STEPS_PER_REV 200 // 200 steps/rev in full-step mode
#define STEPS_1_REV (STEPS_PER_REV * MICROSTEP) 


int steps = 0;
bool homed = false; 

typedef struct RecvData{

  float distance;
  int count;
  float pos;
  bool home;
  bool reset;

} RecvData1;

RecvData MotorData;


void SendPositionToMaster(int pos) {

  JsonDocument Position;

  Position["type"] = "posdata"; 
  Position["position"] = MotorData.pos;

  String output;
  serializeJson(Position, output);
  mySerial.print(output);  // Send to master

  // Serial.print("Send position to master: ");
  // Serial.print(output);
}

void updateDistance(float newDistance) { 

  static float lastDistance = 0;  

  float stepPerMM = STEPS_1_REV / 2.0;  // Steps/mm for T8 2mm lead screw
  float delta = newDistance - lastDistance;  // Tolerance to move

  if (delta != 0) {  
      stepper.move(delta * stepPerMM);  
      lastDistance = newDistance;  // Update new Distance
  }
}



void RuntoSetpoint(){

  unsigned long lastPrintTime = 0; 
  if (!homed) return;  // If not homed, do nothing

  updateDistance(MotorData.distance);  // Update distance from master 
  // for (int i = 0; i < MotorData.count; i++) {  // Repeat COUNT times.
  //   Serial.print("Count time: "); Serial.println(i + 1);
    
    while (stepper.distanceToGo() != 0) {

      stepper.run();
    if (millis() - lastPrintTime > 100) {  
      //Serial.print("Position: ");

      float newPos = stepper.currentPosition() / (STEPS_1_REV / 2.0); 
      //Serial.print(MotorData.pos);  // Convert to mm
      //Serial.println(" mm");
      if (newPos != MotorData.pos) {  // Only update if the position changes.
        MotorData.pos = newPos;  
        lastPrintTime = millis();
        SendPositionToMaster(MotorData.pos);
    }

    }
 // }
 


  }
  stepper.move(stepper.currentPosition());

}
void goHome() {

  Serial.println("Homing...");

  // 1. Move to the right at a customizable speed.
  stepper.setSpeed(-500);  //Go home
  while (digitalRead(limitSwitch) == LOW) {  
    stepper.runSpeed();
  }

  // 2. When the limit switch is triggered, back off slightly to avoid sticking.
  stepper.move(40);  
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }

  // 3. Set the Home position to 0.
  stepper.setCurrentPosition(0);
  Serial.println("Homed reset to 0.");
  homed = true;  // Mark Home as found.



}

void ReceiveMotorData() {

  if (mySerial.available() > 0) {

    String input = mySerial.readString();
    //Serial.print("Nhận dữ liệu thô: "); Serial.println(input);
    JsonDocument RecvData;
    DeserializationError error = deserializeJson(RecvData, input);

    if (!error) {

        String type = RecvData["type"].as<String>();  // Check data type.

        if (type == "HomeBTN") {
          
            MotorData.home = RecvData["Home"];
            Serial.print("Recv: Home = "); Serial.println(MotorData.home);
            if (MotorData.home) {
                goHome();  // Move to origin.
            }
        } 
        else if (type == "ResetBTN") {
          MotorData.reset = RecvData["Reset"];
          Serial.print("Recv: Reset = "); Serial.println(MotorData.reset);
            
            if (MotorData.reset) {
                ESP.restart(); 
              // MotorData.distance = 0;  // Reset distance.
              // MotorData.count = 0;  // Reset count.
              // stepper.setCurrentPosition(0);
          }
        }
        else if (type == "motordata") {

            MotorData.distance = RecvData["distance"].as<float>();
            MotorData.count = RecvData["Count"].as<int>();

            Serial.print("Recv: Distance = "); Serial.println(MotorData.distance);
            Serial.print("Recv: Count = "); Serial.println(MotorData.count);

            RuntoSetpoint();
        }
    }
}
}



void LimitCheck() {

  if (digitalRead(limitSwitch) == LOW && !limitTriggered) {  
    limitTriggered = true;  
    //goingForward = !goingForward;  
  } else if (digitalRead(limitSwitch) == HIGH) {
    limitTriggered = false; 
  }

}

void setup() {

Serial.begin(115200);
mySerial.begin(115200, SERIAL_8N1,16, 17); // Initialize with baud rate and configuration

pinMode(limitSwitch, INPUT_PULLUP);

stepper.setMaxSpeed(4000); //Set speed LOW MED HIGH
stepper.setAcceleration(1000);




}

void loop() {
  LimitCheck();
  ReceiveMotorData();
}
