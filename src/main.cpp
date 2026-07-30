#include <Arduino.h>
#include <vehicle.h>
#include <ultrasonic.h>
#include <ESP32Servo.h>
#include <IRremote.h>

#define pinLeftLED 2
#define pinIr 4
#define pinRightLED 12
#define pinServo 25
#define pinBuzzer 33

IRrecv irrecv(pinIr);

vehicle myCar;

Servo servo;

float rRotateIndex = 1;
float lRotateIndex = 1;

/*
  myCar.Move(Forward, 255);       Forward
  myCar.Move(Backwards, 255);     Backwards

  myCar.Move(Clockwise, 255);     Rotate Clockwise
  myCar.Move(Contrarotate, 255);  Rotate Anti-clockwise

  myCar.Move(Move_Left, 255);     Strafe Left
  myCar.Move(Move_Right, 255);    Strafe Right

  myCar.Move(Stop, 0)             Stop
*/

// void buzz() {
//   tone(pinBuzzer, 262);
//   tone(pinBuzzer, 494);
//   noTone(pinBuzzer);
// }

void remote() {
  if (irrecv.decode()) {
    if (!irrecv.decodedIRData.flags) {
      Serial.print("IR Code Received: ");
      int signal = irrecv.decodedIRData.command;
      Serial.println(signal);
      Serial.print("IR Code Received (Hex): 0x");
      Serial.println(irrecv.decodedIRData.command, HEX);
      if (signal == 70) {
        myCar.Move(Forward, 128);
      } else if (signal == 21) {
        myCar.Move(Backward, 128);
      } else if (signal == 64) {
        myCar.Move(Stop, 0);
      } else if (signal == 67) {
        myCar.Move(Move_Right, 128);
      } else if (signal == 68) {
        myCar.Move(Move_Left, 128);
      } else if (signal == 13) {
        myCar.Move(Clockwise, 128);
      } else if (signal == 22) {
        myCar.Move(Contrarotate, 128);
      }
    }
    irrecv.resume();
  }
}

void led() {
  digitalWrite(pinLeftLED, HIGH);
  digitalWrite(pinRightLED, LOW);
  servo.write(135);
  delay(500);
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, HIGH);
  servo.write(45);
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

  servo.attach(pinServo, 500, 2500);
  servo.setPeriodHertz(50);
  // servo.write(90);

  pinMode(pinLeftLED, OUTPUT);
  pinMode(pinIr, INPUT);
  pinMode(pinRightLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  myCar.Init();

  irrecv.enableIRIn();
}

void loop() {
  remote();
  // led();
  // move();

  delay(100);
}