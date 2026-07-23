#include <Arduino.h>
#include <vehicle.h>
#include <ultrasonic.h>
#include <ESP32Servo.h>

#define pinLeftLED 2
#define pinIr 4
#define pinRightLED 12
#define pinServo 25
#define pinBuzzer 33

vehicle myCar;

Servo servo_0;

float rRotateIndex = 1;
float lRotateIndex = 1;

/*
  myCar.Move(Forward, 255);       Forward
  myCar.Move(Backwards, 255);     Backwards

  myCar.Move(Clockwise, 255);     Rotate Clockwise
  myCar.Move(Contrarotate, 255);  Rotate Anti-clockwise

  myCar.Move(Move_Left, 255);     Strafe Left
  myCar.Move(Move_right, 255);    Strafe Right

  myCar.Move(Stop, 0)             Stop
*/

void led() {
  digitalWrite(pinLeftLED, HIGH);
  digitalWrite(pinRightLED, LOW);
  delay(500);
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, HIGH);
  delay(400);
}

int rightRotate() {
  return 1;
}

int leftRotate() {
  return 1;
}

void move() {
  rRotateIndex = rightRotate();
  lRotateIndex = leftRotate();
}

void setup() {
  Serial.begin(115200);

  pinMode(pinLeftLED, OUTPUT);
  pinMode(pinIr, INPUT);
  pinMode(pinRightLED, OUTPUT);
  pinMode(pinServo, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  myCar.Init();
}

void loop() {
  led();
  // move();

  delay(100);
}