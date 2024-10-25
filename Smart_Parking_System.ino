#include <Servo.h>

// Pin definitions
const int entrySensorPin = 2;     // Pin for the entry IR sensor (IR1)
const int exitSensorPin = 3;      // Pin for the exit IR sensor (IR2)
const int lm393Pin = 5;            // Pin for the LM393 sensor (detects parking slot occupation)
const int servoPin = 4;            // Pin for the servo motor

int totalSlots = 4;               // Total number of parking slots
int availableSlots = totalSlots;  // Start with all slots available

Servo parkingServo;               // Create a Servo object

// Variables to debounce the sensors
int entryFlag = 0;                // To track entry sensor status
int exitFlag = 0;                 // To track exit sensor status
unsigned long debounceTime = 500; // Time to debounce the sensors (in ms)
unsigned long lastTriggerTime = 0;// To store the time of the last sensor trigger

void setup() {
  Serial.begin(9600);               // Initialize serial communication
  pinMode(entrySensorPin, INPUT);   // Set entry sensor pin as input
  pinMode(exitSensorPin, INPUT);    // Set exit sensor pin as input
  pinMode(lm393Pin, INPUT);         // Set LM393 sensor pin as input
  parkingServo.attach(servoPin);    // Attach the servo motor to the specified pin
  parkingServo.write(100);          // Start with the gate closed (servo at 100 degrees)
  
  // Initial status
  Serial.println("Parking System Initialized");
  Serial.print("Available slots: ");
  Serial.println(availableSlots);
}

void loop() {
  // Read sensor values
  int entrySensorValue = digitalRead(entrySensorPin);
  int exitSensorValue = digitalRead(exitSensorPin);
  int lm393Value = digitalRead(lm393Pin);  // LM393 sensor to check if slot is occupied

  // Handle entry detection
  if (entrySensorValue == LOW && entryFlag == 0 && (millis() - lastTriggerTime) > debounceTime) {
    entryFlag = 1;
    lastTriggerTime = millis();

    if (availableSlots > 0) {
      Serial.println("Car detected at entry.");
      Serial.println("Opening gate...");
      parkingServo.write(0);       // Open the gate (servo at 0 degrees)
      delay(3000);                 // Keep the gate open for 3 seconds
      parkingServo.write(100);     // Close the gate
      availableSlots--;            // Decrease available slots
      Serial.print("Slots left: ");
      Serial.println(availableSlots);
    } else {
      Serial.println("Parking full, cannot open gate.");
      // Close the gate immediately if it's open
      parkingServo.write(100);     // Ensure the gate is closed if full
    }
  }

  // Handle exit detection
  if (exitSensorValue == LOW && exitFlag == 0 && (millis() - lastTriggerTime) > debounceTime) {
    exitFlag = 1;
    lastTriggerTime = millis();

    if (availableSlots < totalSlots) {
      Serial.println("Car detected at exit.");
      Serial.println("Opening gate...");
      parkingServo.write(0);       // Open the gate (servo at 0 degrees)
      delay(3000);                 // Keep the gate open for 3 seconds
      parkingServo.write(100);     // Close the gate
      availableSlots++;            // Increase available slots
      Serial.print("Slots left: ");
      Serial.println(availableSlots);
    } else {
      Serial.println("No cars in the parking lot, cannot increase slots.");
      // Close the gate immediately if it's open
      parkingServo.write(100);     // Ensure the gate is closed if no cars
    }
  }

  // Reset entry flag after processing
  if (entrySensorValue == HIGH) {
    entryFlag = 0;
  }

  // Reset exit flag after processing
  if (exitSensorValue == HIGH) {
    exitFlag = 0;
  }

  // // Optional: Monitor the LM393 sensor to check if the parking spot is occupied
  // if (lm393Value == LOW) {
  //   Serial.println("Parking spot is occupied.");
  // } else {
  //   Serial.println("Parking spot is free.");
  // }

  delay(1000); // Short delay to prevent rapid polling
}
