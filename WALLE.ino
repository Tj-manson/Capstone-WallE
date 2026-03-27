#include <Servo.h>

// Servo
Servo headRotate, headTilt, armLeft, armRight, frontDoor;

const int HEAD_ROTATE_PIN = 3;
const int HEAD_TILT_PIN = 5;
const int ARM_RIGHT_PIN = 6;
const int ARM_LEFT_PIN = 9;
const int DOOR_PIN = 10;

// Motor driver
const int IN1 = 22;
const int IN2 = 23;
const int ENA = 2;  // PWM
const int IN3 = 24;
const int IN4 = 25;
const int ENB = 4;  // PWM

// Ultrasonic
const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

// Buzzer
const int BUZZER_PIN = 13;

// Time 
unsigned long lastActionTime = 0;
const unsigned long actionInterval = 2500; // increased slightly

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  bootUpAnimation();
}

void loop() {
  if (millis() - lastActionTime > actionInterval) {
    if (readDistance() < 20) {
      reactToObstacle();
    } else {
      doRandomAction();
    }
    lastActionTime = millis();
  }
}

// Distance Sensing
long readDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}

// Startup 
void bootUpAnimation() {
  attachAll();
  headTilt.write(130);
  headRotate.write(90);
  armLeft.write(180);
  armRight.write(0);
  frontDoor.write(120);
  delay(1200);

  tone(BUZZER_PIN, 1000, 300);
  headRotate.write(60); delay(400);
  headRotate.write(120); delay(400);
  headRotate.write(90); delay(200);

  frontDoor.write(0); delay(700);
  frontDoor.write(130); delay(300);

  detachAll();
}

// Random Action 
void doRandomAction() {
  int r = random(0, 6);
  switch (r) {
    case 0: moveHead(); break;
    case 1: nodHead(); break;
    case 2: moveArms(); break;
    case 3: openDoor(); break;
    case 4: moveForward(); break;
    case 5: lookAround(); break;
  }
}

// Actions 

void moveHead() {
  headRotate.attach(HEAD_ROTATE_PIN);
  for (int pos = 90; pos >= 50; pos -= 2) {
    headRotate.write(pos); delay(25);
  }
  for (int pos = 50; pos <= 130; pos += 2) {
    headRotate.write(pos); delay(25);
  }
  for (int pos = 130; pos >= 90; pos -= 2) {
    headRotate.write(pos); delay(25);
  }
  headRotate.detach();
}

void nodHead() {
  headTilt.attach(HEAD_TILT_PIN);
  for (int pos = 130; pos <= 180; pos += 2) {
    headTilt.write(pos); delay(20);
  }
  for (int pos = 180; pos >= 130; pos -= 2) {
    headTilt.write(pos); delay(20);
  }
  headTilt.detach();
}

void moveArms() {
  armLeft.attach(ARM_LEFT_PIN);
  armRight.attach(ARM_RIGHT_PIN);

  // Main mirrored arm movement
  for (int i = 180; i >= 90; i -= 2) {
    armLeft.write(i);
    armRight.write(180 - i); // reversed motion
    delay(20);
  }

  delay(300);

  for (int i = 90; i <= 180; i += 2) {
    armLeft.write(i);
    armRight.write(180 - i);
    delay(20);
  }

  // Right arm wave (3 quick waves)
  for (int wave = 0; wave < 3; wave++) {
    armRight.write(90);   // wave forward
    delay(200);
    armRight.write(120);  // wave backward slightly
    delay(200);
  }

  armLeft.detach();
  armRight.detach();
}

void openDoor() {
  frontDoor.attach(DOOR_PIN);

  // Open the door (from closed 120 to open 0)
  for (int i = 130; i >= 0; i -= 3) {
    frontDoor.write(i);
    delay(20);
  }

  delay(800); // door stays open

  // Close the door (back to 120)
  for (int i = 0; i <= 130; i += 3) {
    frontDoor.write(i);
    delay(20);
  }

  frontDoor.detach();
}

void moveForward() {
  analogWrite(ENA, 255); // stronger
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENB, 255);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  delay(3000);
  stopMotors();
}

void lookAround() {
  headRotate.attach(HEAD_ROTATE_PIN);
  for (int pos = 90; pos >= 60; pos -= 2) {
    headRotate.write(pos); delay(25);
  }
  delay(200);
  for (int pos = 60; pos <= 120; pos += 2) {
    headRotate.write(pos); delay(25);
  }
  delay(200);
  for (int pos = 120; pos >= 90; pos -= 2) {
    headRotate.write(pos); delay(25);
  }
  headRotate.detach();
}

void reactToObstacle() {
  Serial.println("Obstacle detected!");
  tone(BUZZER_PIN, 800, 250);

  headRotate.attach(HEAD_ROTATE_PIN);
  headTilt.attach(HEAD_TILT_PIN);

  // Look left and right
  for (int pos = 90; pos <= 110; pos += 2) {
    headRotate.write(pos); delay(20);
  }
  for (int pos = 110; pos >= 70; pos -= 2) {
    headRotate.write(pos); delay(20);
  }
  for (int pos = 70; pos <= 90; pos += 2) {
    headRotate.write(pos); delay(20);
  }

  // Tilt up, pause, and reset
  for (int pos = 130; pos <= 180; pos += 2) {
    headTilt.write(pos); delay(15);
  }
  delay(300);
  for (int pos = 180; pos >= 130; pos -= 2) {
    headTilt.write(pos); delay(15);
  }

  headRotate.detach();
  headTilt.detach();

  // Rotate away from obstacle
  analogWrite(ENA, 255);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENB, 255);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  delay(4000);
  stopMotors();
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}

// Other stuff
void attachAll() {
  headRotate.attach(HEAD_ROTATE_PIN);
  headTilt.attach(HEAD_TILT_PIN);
  armLeft.attach(ARM_LEFT_PIN);
  armRight.attach(ARM_RIGHT_PIN);
  frontDoor.attach(DOOR_PIN);
}

void detachAll() {
  headRotate.detach();
  headTilt.detach();
  armLeft.detach();
  armRight.detach();
  frontDoor.detach();
}