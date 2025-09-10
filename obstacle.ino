
// Define motor driver pins
const int motorPin1 = 10; // IN1 on the L293D
const int motorPin2 = 11; // IN2 on the L293D
const int motorPin3 = 12; // IN3 on the L293D
const int motorPin4 = 13; // IN4 on the L293D
const int enablePin1 = 5;  // Enable 1 on the L293D
const int enablePin2 = 6;  // Enable 2 on the L293D

// Define IR sensor pin
const int irSensorPin = 7;

// Variable to store IR sensor state
int irSensorState = 0;



void setup() {
  // Set motor pins as output
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);

  // Set IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Initialize LED matrix


  // Initialize serial communication (for debugging)
  Serial.begin(9600);

  // Set initial motor speed to a moderate value
  analogWrite(enablePin1, 200); // Set speed for motor A
  analogWrite(enablePin2, 200); // Set speed for motor B
}

void loop() {
  // Read the state of the IR sensor
  irSensorState = digitalRead(irSensorPin);

  // Print IR sensor state to the Serial Monitor
  Serial.print("IR Sensor State: ");
  Serial.println(irSensorState);



  if (irSensorState == HIGH) {
    // No obstacle detected, move forward
    Serial.println("No obstacle detected. Moving forward.");
    moveForward();
  } else {
    // Obstacle detected, stop and turn right
    Serial.println("Obstacle detected. Stopping.");
    stopCar();
    delay(500); // Short delay before turning
    Serial.println("Turning right.");
    turnRight();
    delay(1500); // Turn for 1.5 seconds
    Serial.println("Stopping after turn.");
    stopCar();
  }

  delay(100); // Add a small delay for stability
}

void moveForward() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void stopCar() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

void turnRight() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}
