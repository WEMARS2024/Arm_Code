#include <Arduino.h>
#include <ESP32Servo.h>

// Define pins for the servos
#define BASE_X_PIN 5       // Base X axis servo pin
#define BASE_Y_PIN 18      // Base Y axis servo pin
#define WRIST_X_PIN 19     // Wrist X axis servo pin
#define FOREARM_Y_PIN 21   // Forearm Y axis servo pin
#define CLAW_PIN 22        // Claw servo pin

// Define pins for Serial2 communication (UART with Raspberry Pi)
#define TX2_PIN 17
#define RX2_PIN 16

// Define servo objects
Servo baseX;
Servo baseY;
Servo wristX;
Servo forearmY;
Servo claw;

// Define counters for each axis
float counterBaseX = 90;
float counterBaseY = 90;
float counterWristX = 90;
float counterForearmY = 90;
float counterClaw = 90; // Use this counter to combine RT and LT movements

// Define a step size for how much each joystick movement changes the counter
const float stepSize = 1.0; // Adjust as needed
const float joystickThreshold = 0.1; // Threshold to avoid stick drift

void setup() {
  Serial.begin(115200); // Serial for debugging on the laptop
  Serial2.begin(115200, SERIAL_8N1, RX2_PIN, TX2_PIN); // Serial2 for UART with Raspberry Pi

  // Attach servos to their respective pins with custom pulse widths
  baseX.setPeriodHertz(50);
  baseX.attach(BASE_X_PIN, 500, 2400);

  baseY.setPeriodHertz(50);
  baseY.attach(BASE_Y_PIN, 500, 2400);

  wristX.setPeriodHertz(50);
  wristX.attach(WRIST_X_PIN, 500, 2400);

  forearmY.setPeriodHertz(50);
  forearmY.attach(FOREARM_Y_PIN, 500, 2400);

  claw.setPeriodHertz(50);
  claw.attach(CLAW_PIN, 500, 2400);

  // Initialize servos to midpoint position (90 degrees)
  baseX.write(90);
  baseY.write(90);
  wristX.write(90);
  forearmY.write(90);
  claw.write(90);
}

void loop() {
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n'); // Read the incoming data until newline character
    Serial.println("Received data: " + receivedData); // Print the received data for debugging
    
    // Parse the received data
    if (receivedData.startsWith("(RX,")) {
      float value = receivedData.substring(4).toFloat();
      handleBaseX(value);
    } else if (receivedData.startsWith("(RY,")) {
      float value = receivedData.substring(4).toFloat();
      handleBaseY(value);
    } else if (receivedData.startsWith("(LX,")) {
      float value = receivedData.substring(4).toFloat();
      handleWristX(value);
    } else if (receivedData.startsWith("(LY,")) {
      float value = receivedData.substring(4).toFloat();
      handleForearmY(value);
    } else if (receivedData.startsWith("(RT,")) {
      float value = receivedData.substring(4).toFloat();
      handleRT(value);
    } else if (receivedData.startsWith("(LT,")) {
      float value = receivedData.substring(4).toFloat();
      handleLT(value);
    }
  }
}

// Functions to handle the joystick inputs by updating the counters
void handleBaseX(float value) {
  if (abs(value) > joystickThreshold) {
    counterBaseX += value * stepSize;
    // Ensure the counter stays within the valid range for servo positions
    counterBaseX = constrain(counterBaseX, 0, 180);
    Serial.print("Counter Base X: ");
    Serial.println(counterBaseX);
    // Control the servo
    baseX.write(counterBaseX);
  }
}

void handleBaseY(float value) {
  if (abs(value) > joystickThreshold) {
    counterBaseY += value * stepSize;
    // Ensure the counter stays within the valid range for servo positions
    counterBaseY = constrain(counterBaseY, 0, 180);
    Serial.print("Counter Base Y: ");
    Serial.println(counterBaseY);
    // Control the servo
    baseY.write(counterBaseY);
  }
}

void handleWristX(float value) {
  if (abs(value) > joystickThreshold) {
    counterWristX += value * stepSize;
    // Ensure the counter stays within the valid range for servo positions
    counterWristX = constrain(counterWristX, 0, 180);
    Serial.print("Counter Wrist X: ");
    Serial.println(counterWristX);
    // Control the servo
    wristX.write(counterWristX);
  }
}

void handleForearmY(float value) {
  if (abs(value) > joystickThreshold) {
    counterForearmY += value * stepSize;
    // Ensure the counter stays within the valid range for servo positions
    counterForearmY = constrain(counterForearmY, 0, 180);
    Serial.print("Counter Forearm Y: ");
    Serial.println(counterForearmY);
    // Control the servo
    forearmY.write(counterForearmY);
  }
}

void handleRT(float value) {
  if (abs(value) > joystickThreshold) {
    counterClaw += value * stepSize;
    // Ensure the counter stays within the valid range for servo positions
    counterClaw = constrain(counterClaw, 0, 180);
    Serial.print("Counter Claw (RT): ");
    Serial.println(counterClaw);
    // Control the fifth servo
    claw.write(counterClaw);
  }
}

void handleLT(float value) {
  if (abs(value) > joystickThreshold) {
    counterClaw -= value * stepSize; // Subtract value to rotate the other way
    // Ensure the counter stays within the valid range for servo positions
    counterClaw = constrain(counterClaw, 0, 180);
    Serial.print("Counter Claw (LT): ");
    Serial.println(counterClaw);
    // Control the fifth servo
    claw.write(counterClaw);
  }
}
