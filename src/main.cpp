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
int mode = 0;
/*
  Modes:
  mode 0 = Automatic
  mode 1 = Manual
  mode 2 = Line follow
  mode 3 = Random
*/

int direction = 0;
int ultrasonicDir = 1;

int lights = 0;

int forward = 0;

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

  Serial.println(ultrasonicDistance);
}

bool stopIfWall(int distance)
{
  if (ultrasonicDistance < distance)
  {
    myCar.Move(Stop, 0);
    return true;
  }
  return false;
}

void reverse()
{
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, HIGH);
  analogWrite(pinBuzzer, 2);
  delay(400);
  digitalWrite(pinLeftLED, HIGH);
  digitalWrite(pinRightLED, LOW);
  analogWrite(pinBuzzer, 4);
  delay(300);
  analogWrite(pinBuzzer, 0);
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
  if (ultrasonicDistance > 40 && forward == 1)
  {
    myCar.Move(Forward, 128);
  }
  else if (ultrasonicDistance > 40 && forward == 0)
  {
    myCar.Move(Forward, 255);
    delay(100);
    myCar.Move(Forward, 128);
    forward = 1;
  }
  else
  {
    forward = 0;
    myCar.Move(Stop, 0);
    servo.write(135);
    digitalWrite(pinLeftLED, HIGH);
    digitalWrite(pinRightLED, LOW);
    delay(1000);
    ultrasonicSens();
    if (ultrasonicDistance > 50)
    {
      myCar.Move(Contrarotate, 255);
      digitalWrite(pinLeftLED, LOW);
      delay(100);
      myCar.Move(Contrarotate, 128);
      delay(100);
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
        myCar.Move(Clockwise, 255);
        digitalWrite(pinRightLED, LOW);
        delay(100);
        myCar.Move(Clockwise, 128);
        delay(100);
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
        myCar.Move(Backward, 255);
        digitalWrite(pinRightLED, LOW);
        delay(100);
        myCar.Move(Backward, 128);
        delay(100);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        delay(200);
        digitalWrite(pinRightLED, LOW);
        delay(200);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Clockwise, 255);
        digitalWrite(pinRightLED, LOW);
        delay(100);
        myCar.Move(Clockwise, 128);
        delay(100);
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

void manualMode()
{
  ultrasonicSens();
  if (ultrasonicDir == 1)
  {
    servo.write(90);
  }
  else if (ultrasonicDir == 2)
  {
    servo.write(45);
  }
  else if (ultrasonicDir == 3)
  {
    servo.write(0);
  }
  else if (ultrasonicDir == 5)
  {
    servo.write(180);
  }
  else if (ultrasonicDir == 6)
  {
    servo.write(135);
  }

  if (direction == ultrasonicDir)
  {
    stopIfWall(30);
  }

  if (direction == 4)
  {
    reverse();
  }
  else if (lights == 0)
  {
    digitalWrite(pinLeftLED, LOW);
    digitalWrite(pinRightLED, LOW);
  }
}

void remote()
{
  if (mode == 0)
  {
    automatic();
  }
  else if (mode == 1)
  {
    manualMode();
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
        direction = 1;
        ultrasonicDir = 1;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true)
        {
          return;
        }
        myCar.Move(Forward, 255);
        delay(100);
        myCar.Move(Forward, 128);
        if (stopSec == 1)
        {
          delay(900);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 21 && mode == 1)
      {
        direction = 4;
        ultrasonicDir = 1;
        myCar.Move(Backward, 255);
        delay(100);
        myCar.Move(Backward, 128);
        if (stopSec == 1)
        {
          reverse();
          delay(100);
          myCar.Move(Stop, 0);
          direction = 1;
        }
      }
      else if (signal == 64 && mode == 1)
      {
        direction = 0;
        ultrasonicDir = 1;
        digitalWrite(pinLeftLED, LOW);
        digitalWrite(pinRightLED, LOW);
        myCar.Move(Stop, 0);
      }
      else if (signal == 67 && mode == 1)
      {
        direction = 3;
        ultrasonicDir = 3;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true)
        {
          return;
        }
        myCar.Move(Move_Right, 255);
        delay(100);
        myCar.Move(Move_Right, 128);
        if (stopSec == 1)
        {
          delay(900);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 68 && mode == 1)
      {
        direction = 5;
        ultrasonicDir = 5;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true)
        {
          return;
        }
        myCar.Move(Move_Left, 255);
        delay(100);
        myCar.Move(Move_Left, 128);
        if (stopSec == 1)
        {
          delay(900);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 13 && mode == 1)
      {
        direction = 2;
        ultrasonicDir = 2;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true)
        {
          return;
        }
        myCar.Move(Clockwise, 255);
        delay(100);
        myCar.Move(Backward, 128);
        if (stopSec == 1)
        {
          delay(900);
          myCar.Move(Stop, 0);
        }
      }
      else if (signal == 22 && (mode == 1))
      {
        direction = 6;
        ultrasonicDir = 6;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true)
        {
          return;
        }
        myCar.Move(Contrarotate, 255);
        delay(100);
        myCar.Move(Contrarotate, 128);
        if (stopSec == 1)
        {
          delay(900);
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
        direction = 0;
        mode = 0;
      }
      else if (signal == 24)
      {
        direction = 0;
        mode = 1;
        myCar.Move(Stop, 0);
      }
      else if (signal == 94)
      {
        direction = 0;
        mode = 2;
      }
      else if (signal == 28)
      {
        direction = 0;
        mode = 3;
      }
      else if (signal == 90)
      {
        lights = 1;
        digitalWrite(pinLeftLED, HIGH);
        digitalWrite(pinRightLED, HIGH);
      }
      else if (signal == 8)
      {
        lights = 0;
        digitalWrite(pinLeftLED, LOW);
        digitalWrite(pinRightLED, LOW);
      }
      else if (signal == 25)
      {
        direction = 0;
        ultrasonicDir = 1;
        digitalWrite(pinLeftLED, LOW);
        digitalWrite(pinRightLED, LOW);
        myCar.Move(Stop, 0);
        delay(5000);
      }
      else if (signal == 82 && mode == 1)
      {
        if (direction == 0)
        {
        }
        else if (direction == 1)
        {
          myCar.Move(Forward, 255);
          bool temp = stopIfWall(50);
          if (temp == true)
          {
            return;
          }
          delay(1000);
          myCar.Move(Forward, 128);
        }
        else if (direction == 2)
        {
          myCar.Move(Clockwise, 255);
          bool temp = stopIfWall(50);
          if (temp == true)
          {
            return;
          }
          delay(1000);
          myCar.Move(Clockwise, 128);
        }
        else if (direction == 3)
        {
          myCar.Move(Move_Right, 255);
          bool temp = stopIfWall(50);
          if (temp == true)
          {
            return;
          }
          delay(1000);
          myCar.Move(Move_Right, 128);
        }
        else if (direction == 4)
        {
          myCar.Move(Backward, 255);
          reverse();
          delay(100);
          reverse();
          delay(100);
          myCar.Move(Backward, 128);
        }
        else if (direction == 5)
        {
          myCar.Move(Move_Left, 255);
          bool temp = stopIfWall(50);
          if (temp == true)
          {
            return;
          }
          delay(1000);
          myCar.Move(Move_Left, 128);
        }
        else if (direction == 6)
        {
          myCar.Move(Contrarotate, 255);
          bool temp = stopIfWall(50);
          if (temp == true)
          {
            return;
          }
          delay(1000);
          myCar.Move(Contrarotate, 128);
        }
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