#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

// CAN Config
CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was sent
const int interval = 20;            // interval at which to send CAN Messages (milliseconds)
const int rx_queue_size = 10;       // Receive Queue size
bool btEnabled = 0;
CAN_frame_t tx_frame;
CAN_frame_t rx_frame;
unsigned long ulCurrentMicros;
unsigned long ulPreviousMicros;
unsigned long ul5mS = 0;

// Define pins for the servos
#define BASE_X_PIN 5       // Base X axis servo pin
#define BASE_Y_PIN 18      // Base Y axis servo pin
#define WRIST_X_PIN 33     // Wrist X axis servo pin
#define FOREARM_Y_PIN 25   // Forearm Y axis servo pin
#define CLAW_PIN 23        // Claw servo pin

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
const float stepSize = 0.1; // Adjust as needed
const float joystickThreshold = 2; // Threshold to avoid stick drift

void setup() {
  Serial.begin(921600);
  Serial.println("ESP32-CAN");
  CAN_cfg.speed = CAN_SPEED_1000KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_21;
  CAN_cfg.rx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));

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

  ESP32Can.CANInit();

}

void loop() {
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) 
  {
    if (rx_frame.MsgID == 0x0C0C1801) 
    { // Adjusted to handle all inputs in one packet
      // Directly use the received signed bytes to adjust servo positions
      handleBaseX((int8_t)rx_frame.data.u8[0]);
      handleBaseY((int8_t)rx_frame.data.u8[1]);
      handleWristX((int8_t)rx_frame.data.u8[2]);
      handleForearmY((int8_t)rx_frame.data.u8[3]);
      handleRT((int8_t)rx_frame.data.u8[4]);
      handleLT((int8_t)rx_frame.data.u8[5]);
      Serial.println("hi");
    }
  }
}

// Functions to handle the joystick inputs by updating the counters
void handleBaseX(int8_t value) {
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

void handleBaseY(int8_t value) {
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

void handleWristX(int8_t value) {
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

void handleForearmY(int8_t value) {
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

void handleRT(int8_t value) {
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

void handleLT(int8_t value) {
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
