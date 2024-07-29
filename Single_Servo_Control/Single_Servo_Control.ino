#include <Arduino.h>
#include <ESP32Servo.h>

// Define GPIO pin for the servo
#define baseX 18

// Define TX2 and RX2 pins for Serial2 (UART1)
#define TX2 17
#define RX2 16

// Create a Servo object for the baseX servo
Servo servo1;

void setup() {
  Serial.begin(115200); // Initialize the primary serial port for debugging at 115200 baud rate
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2); // Initialize Serial2 for UART communication at 115200 baud rate

  // Attach the servo to a specific GPIO pin on the ESP32
  servo1.attach(baseX);

  Serial.println("Setup complete. Waiting for data...");
}

void loop() {
  // Check if there is any data available on Serial2 (UART1)
  if (Serial2.available()) {
    // Read the incoming data as a string until a newline character is detected
    String data = Serial2.readStringUntil('\n');
    Serial.print("Received data: ");
    Serial.println(data);
    
    // Process the received data to control the servo
    processCommand(data);
  }
}

void processCommand(String data) {
  // Remove the square brackets from the data string
  data.remove(0, 1);  // Remove the first character '['
  data.remove(data.length() - 1, 1);  // Remove the last character ']'

  // Split the data string into individual values
  float value = data.toFloat();  // Convert the data to a float

  // Map the received float value to servo position (0 to 180 degrees)
  int servo1Pos = map(value * 100, -100, 100, 0, 180);  // Scaling to -100 to 100 for better precision

  // Write the mapped position to the servo
  servo1.write(servo1Pos);

  // Print the mapped servo position to the serial monitor for debugging
  Serial.print("Mapped value: ");
  Serial.print(value);
  Serial.print(" -> Servo position: ");
  Serial.println(servo1Pos);
}
