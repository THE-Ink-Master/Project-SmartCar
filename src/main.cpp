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
#define pinIRLeft 35
#define pinIRMiddle 36
#define pinIRRight 39

IRrecv irrecv(pinIr);

vehicle myCar;

ultrasonic myUltrasonic;

Servo servo;

int stopSec = 0;
int mode = 1;

int randomVal = 0;
int baseVal = 0;

int ultrasonicDistance;

/*
  myCar.Move(Forward, 255);       Forward
  myCar.Move(Backwards, 255);     Backwards

  myCar.Move(Clockwise, 255);     Rotate Clockwise
  myCar.Move(Contrarotate, 255);  Rotate Anti-clockwise

  myCar.Move(Move_Left, 255);     Strafe Left
  myCar.Move(Move_Right, 255);    Strafe Right

  myCar.Move(Stop, 0)             Stop
*/

/*
 void buzz() {
   tone(pinBuzzer, 262);
   tone(pinBuzzer, 494);
   noTone(pinBuzzer);
 }
*/

void randomDir()
{
  randomVal = rand() % 121;
  if (randomVal <= 20)
  {
    myCar.Move(Forward, 128);
    delay(1000);
  }
  else if (randomVal <= 40 && !randomVal <= 20)
  {
    myCar.Move(Move_Left, 128);
    delay(1000);
  }
  else if (randomVal <= 60 && !randomVal <= 40)
  {
    myCar.Move(Move_Right, 128);
    delay(1000);
  }
  else if (randomVal <= 80 && !randomVal <= 60)
  {
    myCar.Move(Backward, 128);
    delay(1000);
  }
  else if (randomVal <= 100 && !randomVal <= 80)
  {
    myCar.Move(Clockwise, 128);
    delay(1000);
  }
  else if (randomVal <= 120 && !randomVal <= 100)
  {
    myCar.Move(Contrarotate, 128);
    delay(1000);
  }
  else if (randomVal <= 140 && !randomVal <= 120)
  {
    myCar.Move(Stop, 0);
    delay(1000);
  }
}

void led()
{
  digitalWrite(pinLeftLED, HIGH);
  digitalWrite(pinRightLED, LOW);
  delay(500);
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, HIGH);
  delay(400);
}

void ultrasonicSens()
{
  ultrasonicDistance = myUltrasonic.Ranging();

  if (ultrasonicDistance < 30)
  {
    myCar.Move(Stop, 0);
  }
  Serial.println(ultrasonicDistance);
}

void lineFollow()
{
  int left = analogRead(35);
  int middle = analogRead(36);
  int right = analogRead(39);
  Serial.println("Left: ");
  Serial.println(left);
  Serial.println("Middle: ");
  Serial.println(middle);
  Serial.println("Right ");
  Serial.println(right);

  if (middle > 2500)
  {
    myCar.Move(Forward, 64);
  }
  else if (right > 2700 && left < 2700)
  {
    myCar.Move(Clockwise, 128);
    delay(100);
    myCar.Move(Forward, 32);
  }
  else if (left > 2700 && right < 2700)
  {
    myCar.Move(Contrarotate, 128);
    delay(100);
    myCar.Move(Forward, 32);
  }
  myCar.Move(Forward, 128);
}

void automatic()
{
  ultrasonicSens();
  if (ultrasonicDistance > 40)
  {
    myCar.Move(Forward, 128);
  }
  else
  {
    myCar.Move(Stop, 0);
    servo.write(135);
    digitalWrite(pinLeftLED, HIGH);
    digitalWrite(pinRightLED, LOW);
    delay(1000);
    ultrasonicSens();
    if (ultrasonicDistance > 50)
    {
      myCar.Move(Contrarotate, 128);
      digitalWrite(pinLeftLED, LOW);
      delay(200);
      digitalWrite(pinLeftLED, HIGH);
      delay(200);
      digitalWrite(pinLeftLED, LOW);
      delay(200);
      digitalWrite(pinLeftLED, HIGH);
      delay(200);
      digitalWrite(pinLeftLED, LOW);
      delay(200);
      digitalWrite(pinLeftLED, HIGH);
      myCar.Move(Stop, 0);
    }
    else
    {
      servo.write(45);
      digitalWrite(pinLeftLED, LOW);
      digitalWrite(pinRightLED, HIGH);
      delay(1000);
      ultrasonicSens();
      if (ultrasonicDistance > 50)
      {
        myCar.Move(Clockwise, 128);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Stop, 0);
      }
      else
      {
        myCar.Move(Backward, 128);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Clockwise, 128);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Stop, 0);
      }
    }
  }
  servo.write(90);
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, LOW);
}

void remote()
{
  if (mode == 0)
  {
    automatic();
  }
  else if (mode == 1)
  {
    ultrasonicSens();
  }
  else if (mode == 2)
  {
    lineFollow();
  }
  else if (mode == 3)
  {
    randomDir();
  }
  if (irrecv.decode())
  {
    if (!irrecv.decodedIRData.flags)
    {
      Serial.print("IR Code Received: ");
      int signal = irrecv.decodedIRData.command;
      Serial.println(signal);
      Serial.print("IR Code Received (Hex): 0x");
      Serial.println(irrecv.decodedIRData.command, HEX);
      if (signal == 70 && mode == 1)
      {
        myCar.Move(Forward, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 21 && mode == 1)
      {
        myCar.Move(Backward, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if ((signal == 64 || signal == 82) && mode == 1)
      {
        myCar.Move(Stop, 0);
      }
      else if (signal == 67 && mode == 1)
      {
        myCar.Move(Move_Right, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 68 && mode == 1)
      {
        myCar.Move(Move_Left, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 13 && mode == 1)
      {
        myCar.Move(Clockwise, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 22 && (mode == 1))
      {
        myCar.Move(Contrarotate, 128);
        if (stopSec == 1)
        {
          delay(1000);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 66)
      {
        stopSec = 0;
      }
      else if (signal == 74)
      {
        stopSec = 1;
      }
      else if (signal == 12)
      {
        mode = 0;
      }
      else if (signal == 24)
      {
        mode = 1;
        myCar.Move(Stop, 0);
      }
      else if (signal == 94)
      {
        mode = 2;
      }
      else if (signal == 28)
      {
        mode = 3;
      }
      else if (signal == 90)
      {
        digitalWrite(pinLeftLED, HIGH);
        digitalWrite(pinRightLED, HIGH);
      }
      else if (signal == 8)
      {
        digitalWrite(pinLeftLED, LOW);
        digitalWrite(pinRightLED, LOW);
      }
      else if (signal == 25)
      {
        myCar.Move(Stop, 0);
        delay(5000);
      }
      // Serial.println(stopSec);
    }
    irrecv.resume();
  }
  // Serial.println(mode);
}

void setup()
{
  Serial.begin(115200);

  servo.attach(pinServo, 500, 2500);
  servo.setPeriodHertz(50);
  // servo.write(90);

  pinMode(pinLeftLED, OUTPUT);
  pinMode(pinIr, INPUT);
  pinMode(pinRightLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinIRLeft, INPUT);
  pinMode(pinIRMiddle, INPUT);
  pinMode(pinIRRight, INPUT);

  myCar.Init();

  myUltrasonic.Init(13, 14);

  irrecv.enableIRIn();

  randomSeed(analogRead(0));
}

void loop()
{
  remote();
  // ultrasonicSens();
  // lineFollow();
  // led();

  delay(100);
}