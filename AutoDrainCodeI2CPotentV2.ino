// Arduino Code for Senior Design Water trap using I2C communication
// The water level sensor has not yet arrived, so the circuit is using a potentiometer as a substitute
#include <Wire.h>  // I2C library

// I2C Address
#define EZO_PMP_ADDRESS 0x67  // Default I2C address (103 in decimal)

// Pin Definitions
const int WaterSensorPin = A0;        // Digital input from water level sensor
const int PinchValvePin = 4;         // Digital output to MOSFET controlling pinch valve

// Timing Constants (in milliseconds)
const unsigned long ValveToPumpDelay = 3000;  // 1 second delay after valve closes (switched to 3)
const unsigned long PumpRunTime = 8000;        // 3 seconds pump operation (switched to 8, should run for set volume)
const unsigned long PumpToValveDelay = 3000;  // 1 second delay before valve reopens (switched to 3)
const unsigned long I2C_Delay = 300;             // Required delay after I2C commands

void setup() {

 Serial.begin(9600);
  
  // Initialize I2C communication
  Wire.begin();
  //I2C_start();
  
  // Configure pin modes
  pinMode(WaterSensorPin, INPUT);
  pinMode(PinchValvePin, OUTPUT);

  // Initialize system to collecting state
  // Normally-open valve: LOW = open (no power), HIGH = closed (energized)
  digitalWrite(PinchValvePin, LOW);   // Valve open
  
  // Wait for EZO-PMP to boot up
  delay(2000);
  
  Serial.println("Water Management System Initialized (I2C Mode)");
  Serial.println("EZO-PMP Address: 0x67");
  Serial.println("Ready to collect water...");
}


void loop() {
  // Check Value of Water Sensor
 int SensorValue = analogRead(WaterSensorPin);
 Serial.println(SensorValue);

if (SensorValue > 600) {
  Serial.println("Starting Draining Sequence");
  //Close Pinch Valve
  digitalWrite(PinchValvePin, HIGH);
  //Give Delay to ensure Pinch Valve is closed
  delay(ValveToPumpDelay);
  Serial.println("Running valve to pump delay");
  //Start up EZO pump
  sendI2CCommand("D,50"); 
  delay(I2C_Delay);
  Serial.println("Running I2C delay");
  //Give delay between Pump stop and Pinch openning to ensure no overlap
  delay(PumpToValveDelay);
  Serial.println("Running pump to valve delay");
  //Reopen Pinch Valve
  digitalWrite(PinchValvePin, LOW);
  // Extra Pause before reading the loop again
  Serial.println("Finished Draining Sequence");
  delay(500);

}

}

// Helper function to send I2C command to EZO-PMP
void sendI2CCommand(const char* command) {
  Serial.print("Sending command: ");
  Serial.println(command);
  
  Wire.beginTransmission(EZO_PMP_ADDRESS);
  Wire.write(command);
  byte error = Wire.endTransmission();
  
  Serial.print("Transmission result: ");
  Serial.println(error);  // 0 = success, other = error
  
  delay(300);  // Wait for pump to process
  
  // Request response
  Wire.requestFrom(EZO_PMP_ADDRESS, 20);
  String response = "";
  while(Wire.available()) {
    char c = Wire.read();
    response += c;
  }
  Serial.print("Pump response: ");
  Serial.println(response);
}