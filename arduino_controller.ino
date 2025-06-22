// Combined Relay Control and BTS7960B H-Bridge Motor Driver Control
// Relay pin definitions
#define RELAY1 7
#define RELAY2 8

// Pin definitions for RIGHT motor driver
const int R_EN_A = 2;  // Right motor enable pin A
const int L_EN_A = 3;  // Right motor enable pin B
const int RPWM_A = 4;  // Right motor PWM signal - controls forward motion
const int LPWM_A = 5;  // Right motor PWM signal - controls reverse motion

// Pin definitions for LEFT motor driver
const int R_EN_B = 6;  // Left motor enable pin A (changed from 7 to avoid conflict)
const int L_EN_B = 9;  // Left motor enable pin B (changed from 8 to avoid conflict)
const int RPWM_B = 10; // Left motor PWM signal - controls forward motion
const int LPWM_B = 11; // Left motor PWM signal - controls reverse motion

// Fixed speed value
const int MOTOR_SPEED = 200;

// Relay control variables
String command = "";
unsigned long farStartTime = 0;
unsigned long relay2StartTime = 0;
unsigned long relay1StartTime = 0;
unsigned long relay1OffTime = 0;
bool farActive = false;
bool nearAfterFar = false;
bool relay2TimerActive = false;
bool relay1TimerActive = false;
bool sequenceActive = false;
bool relay1OffTimerActive = false;

// Function to control right motor
void rightMotor(int speed, bool forward) {
  if (forward) {
    analogWrite(LPWM_A, 0);      // Ensure reverse PWM is off
    analogWrite(RPWM_A, speed);  // Set forward PWM to desired speed
  } else {
    analogWrite(RPWM_A, 0);      // Ensure forward PWM is off
    analogWrite(LPWM_A, speed);  // Set reverse PWM to desired speed
  }
}

// Function to control left motor
void leftMotor(int speed, bool forward) {
  if (forward) {
    analogWrite(LPWM_B, 0);      // Ensure reverse PWM is off
    analogWrite(RPWM_B, speed);  // Set forward PWM to desired speed
  } else {
    analogWrite(RPWM_B, 0);      // Ensure forward PWM is off
    analogWrite(LPWM_B, speed);  // Set reverse PWM to desired speed
  }
}

// Function to move car forward
void moveForward() {
  rightMotor(MOTOR_SPEED, true);
  leftMotor(MOTOR_SPEED, true);
  digitalWrite(LED_BUILTIN, LOW);  // Turn off LED when moving forward
  Serial.println("Moving Forward");
}

// Function to stop the car
void stopCar() {
  analogWrite(RPWM_A, 0);  // Turn off right motor forward PWM
  analogWrite(LPWM_A, 0);  // Turn off right motor reverse PWM
  analogWrite(RPWM_B, 0);  // Turn off left motor forward PWM
  analogWrite(LPWM_B, 0);  // Turn off left motor reverse PWM
  digitalWrite(LED_BUILTIN, LOW); // Turn off LED when stopped
  Serial.println("Stopped");
}

void setup() {
  // Set relay pins as output
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW); // Initially off
  digitalWrite(RELAY2, LOW);
  
  // Set all motor pins as output
  pinMode(R_EN_A, OUTPUT);
  pinMode(L_EN_A, OUTPUT);
  pinMode(RPWM_A, OUTPUT);
  pinMode(LPWM_A, OUTPUT);
  
  pinMode(R_EN_B, OUTPUT);
  pinMode(L_EN_B, OUTPUT);
  pinMode(RPWM_B, OUTPUT);
  pinMode(LPWM_B, OUTPUT);
  
  // Set LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Enable both sides of both H-bridges
  digitalWrite(R_EN_A, HIGH);
  digitalWrite(L_EN_A, HIGH);
  digitalWrite(R_EN_B, HIGH);
  digitalWrite(L_EN_B, HIGH);
  
  // Initially ensure motors are stopped and LED is off
  stopCar();
  
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Combined Relay and Motor Control System");
}

void loop() {
  // Check for serial input
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading/trailing whitespace
    command.toLowerCase();
    
    if (command == "no human") {
      // Relay control
      digitalWrite(RELAY1, LOW);
      digitalWrite(RELAY2, LOW);
      farActive = false;
      nearAfterFar = false;
      relay2TimerActive = false;
      relay1TimerActive = false;
      sequenceActive = false;
      relay1OffTimerActive = false;
      
      // Motor control - move forward for "no human"
      moveForward();
    }
    else if (command == "far") {
      // Relay control
      if (!farActive) {
        farStartTime = millis();
        farActive = true;
      }
      digitalWrite(RELAY1, HIGH);
      digitalWrite(RELAY2, LOW);
      nearAfterFar = false;
      relay2TimerActive = false;
      relay1TimerActive = false;
      sequenceActive = false;
      relay1OffTimerActive = false;
      
      // Motor control - move forward for "far"
      moveForward();
    }
    else if (command == "near") {
      // Motor control - stop car for "near"
      stopCar();
      
      // Relay control
      if (farActive) {
        // Near command came within 4 seconds of Far
        if (millis() - farStartTime <= 4000) {
          // Keep relay1 on, turn on relay2 for 6 seconds
          digitalWrite(RELAY1, HIGH);
          digitalWrite(RELAY2, HIGH);
          relay2StartTime = millis();
          relay2TimerActive = true;
          nearAfterFar = true;
        } else {
          // Far expired, treat as normal near command
          digitalWrite(RELAY1, HIGH);
          digitalWrite(RELAY2, LOW);
          relay1StartTime = millis();
          relay1TimerActive = true;
          sequenceActive = true;
        }
      } else {
        // Near command came without Far before
        // Turn on relay1 for 2 seconds, then turn on relay2 for 6 seconds
        digitalWrite(RELAY1, HIGH);
        digitalWrite(RELAY2, LOW);
        relay1StartTime = millis();
        relay1TimerActive = true;
        sequenceActive = true;
      }
    // Handle Node-RED manual control commands
    else if (command == "moveforward") {
      moveForward();
    }
    else if (command == "movebackward") {
      // Add backward movement function
      rightMotor(MOTOR_SPEED, false);
      leftMotor(MOTOR_SPEED, false);
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Moving Backward");
    }
    else if (command == "turnleft") {
      // Turn left: right motor forward, left motor stop/slow
      rightMotor(MOTOR_SPEED, true);
      leftMotor(MOTOR_SPEED/2, true);
      Serial.println("Turning Left");
    }
    else if (command == "turnright") {
      // Turn right: left motor forward, right motor stop/slow
      leftMotor(MOTOR_SPEED, true);
      rightMotor(MOTOR_SPEED/2, true);
      Serial.println("Turning Right");
    }
    else if (command == "stopcar") {
      stopCar();
    }
  }
  
  // Check if Far has been active for more than 4 seconds without Near
  if (farActive && (millis() - farStartTime > 4000)) {
    digitalWrite(RELAY1, LOW); // Turn off relay1
    digitalWrite(RELAY2, LOW); // Keep relay2 off
    farActive = false;
    // Keep motor moving forward (no change to motor state)
  }
  
  // Handle relay2 timer for near after far (6 seconds)
  if (nearAfterFar && relay2TimerActive && (millis() - relay2StartTime > 6000)) {
    digitalWrite(RELAY2, LOW); // Turn off relay2
    relay2TimerActive = false;
    // Start 2-second timer for relay1 to stay on after relay2 is off
    relay1OffTime = millis();
    relay1OffTimerActive = true;
  }
  
  // Handle relay1 off timer after relay2 is off (near after far case)
  if (nearAfterFar && relay1OffTimerActive && (millis() - relay1OffTime > 2000)) {
    digitalWrite(RELAY1, LOW); // Turn off relay1
    relay1OffTimerActive = false;
    nearAfterFar = false;
  }
  
  // Handle sequence for near without far
  if (sequenceActive && relay1TimerActive) {
    // After 2 seconds, turn on relay2 while keeping relay1 on
    if (millis() - relay1StartTime > 2000) {
      digitalWrite(RELAY2, HIGH);
      relay1TimerActive = false;
      relay2StartTime = millis();
      relay2TimerActive = true;
    }
  }
  
  // Handle relay2 off for near without far sequence (6 seconds)
  if (sequenceActive && relay2TimerActive && (millis() - relay2StartTime > 6000)) {
    digitalWrite(RELAY2, LOW); // Turn off relay2
    relay2TimerActive = false;
    // Start 2-second timer for relay1 to stay on after relay2 is off
    relay1OffTime = millis();
    relay1OffTimerActive = true;
  }
  
  // Handle final relay1 shutdown for near without far sequence
  if (sequenceActive && relay1OffTimerActive && (millis() - relay1OffTime > 2000)) {
    digitalWrite(RELAY1, LOW); // Turn off relay1
    relay1OffTimerActive = false;
    sequenceActive = false;
  }
}