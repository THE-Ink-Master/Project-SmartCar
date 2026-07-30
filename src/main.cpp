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
  myCar.Move(Move_right, 255);    Strafe Right

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
      Serial.print("IR Code Received: 0x");
      int signal = (irrecv.decodedIRData.command, HEX);
      Serial.println(signal);
      if (signal == 46) {
        myCar.Move(Forward, 255);
      } else if (signal == 15) {
        myCar.Move(Backward, 255);
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