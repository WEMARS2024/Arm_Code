#include <Arduino.h>

// Define GPIO pin for the LED
#define LED_PIN 12

// Define TX2 and RX2 pins for Serial2 (UART1)
#define TX2 17
#define RX2 16

void setup() {
  Serial.begin(115200); // Initialize the primary serial port for debugging at 115200 baud rate
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2); // Initialize Serial2 for UART communication at 115200 baud rate

  // Configure the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Setup complete. Waiting for data...");
}

void loop() {
  // Check if there is any data available on Serial2 (UART1)
  if (Serial2.available()) {
    // Read the incoming data as a string until a newline character is detected
    String data = Serial2.readStringUntil('\n');
    Serial.print("Received data: ");
    Serial.println(data);
    
    // Process the received data to control the LED brightness
    processCommand(data);
  }
}

void processCommand(String data) {
  // Remove the square brackets from the data string
  if (data.startsWith("[") && data.endsWith("]")) {
    data.remove(0, 1);  // Remove the first character '['
    data.remove(data.length() - 1, 1);  // Remove the last character ']'

    // Convert the data to a float
    float value = data.toFloat();
    Serial.print("Converted float value: ");
    Serial.println(value);

    // Map the received float value to LED brightness (0 to 255)
    int brightness = map(value * 100, -100, 100, 0, 255);  // Scaling to -100 to 100 for better precision
    Serial.print("Mapped brightness value: ");
    Serial.println(brightness);

    // Set the LED brightness using analogWrite
    analogWrite(LED_PIN, brightness);

    // Print the mapped brightness to the serial monitor for debugging
    Serial.print("Mapped value: ");
    Serial.print(value);
    Serial.print(" -> LED brightness: ");
    Serial.println(brightness);
  } else {
    Serial.println("Invalid data format");
  }
}
