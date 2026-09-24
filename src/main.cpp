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

#define Left 1
#define Right 0

IRrecv irrecv(pinIr);

vehicle myCar;

ultrasonic myUltrasonic;

Servo servo;

// int mode = 1;
/*
  Modes:
  mode 0 = Automatic
  mode 1 = Manual
  mode 2 = Line follow
  mode 3 = Random
*/

int lastSeen = 0;

enum modes {
  automatic,
  manual,
  line,
  rando
};

modes mode = manual;

// int direction = 0;

enum whichWay {
  GoStop,
  GoForward,
  GoRotateRight,
  GoTurnRight,
  GoRight,
  GoBack,
  GoLeft,
  GoTurnLeft,
  GoRotateLeft
};

whichWay direction = GoForward;

enum ultrasonicWhichWay {
  uGoForward,
  uGoRotateRight,
  uGoRight,
  uGoLeft,
  uGoRotateLeft
};

ultrasonicWhichWay ultrasonicDir = uGoForward;

// int ultrasonicDir = 1;

int lights = 0;

int forward = 0;

int wallDetected = 0;

int randomVal = 0;
int randomLeftVal = 0;
int randomRightVal = 0;
int randomServoVal = 0;
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
  direction = what direct to move (Left and Right are valid) 
  speed = what speed
*/
void carRotate(int direction, int speed, int time, int timePer = 100, int enSerial = 0)
{
  int localTime = 0;
  while (time > localTime)
  {
    localTime += timePer;

    if (direction == 0)
    {
      myCar.Move(Clockwise, speed);
    }
    else if (direction == 1)
    {
      myCar.Move(Contrarotate, speed);
    }
    delay(timePer/3);
    myCar.Move(Forward, speed);
    delay(timePer/2);
    int noTime = (timePer - ((timePer/3) + (timePer/2)));

    myCar.Move(Stop, 0);
    if (enSerial == 1)
    {
      Serial.print("direction; ");
      Serial.println(direction);
      Serial.print("speed; ");
      Serial.println(speed);
      Serial.print("time: ");
      Serial.println(time);
      Serial.print("local time: ");
      Serial.println(localTime);
      Serial.print("time gone: ");
      Serial.println(noTime);
    }
  }
}

void randomDir()
{
  randomVal = rand() % 122;
  randomLeftVal = rand() % 2;
  randomRightVal = rand() % 2;
  randomServoVal = rand() % 181;
  // Serial.print("Left val: ");
  // Serial.println(randomLeftVal);
  digitalWrite(pinLeftLED, randomLeftVal);
  digitalWrite(pinRightLED, randomRightVal);
  servo.write(randomServoVal);
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

  if (mode == 0 && ultrasonicDistance < 40)
  {
    wallDetected = 1;
  }
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
  analogWrite(pinBuzzer, 4);
  delay(400);
  digitalWrite(pinLeftLED, HIGH);
  digitalWrite(pinRightLED, LOW);
  analogWrite(pinBuzzer, 6);
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

  
  if (right > 2700 && left < 2700 && middle > 2500)
  {
    myCar.Move(Move_Right, 128);
    delay(50);
    myCar.Move(Forward, 128);
    Serial.println("Going right");
  }
  else if (left > 2700 && right < 2700 && middle > 2500)
  {
    myCar.Move(Move_Left, 128);
    delay(50);
    myCar.Move(Forward, 128);
    Serial.println("Going left");
  }
  else if (middle > 2500)
  {
    myCar.Move(Forward, 128);
    delay(20);
    myCar.Move(Forward, 64);
    Serial.println("Going forward");
  }
  else if (right > 2700 && left < 2700)
  {
    while (middle < 2500)
    {
      myCar.Move(Clockwise, 128);
      Serial.println("Rotating right");
      middle = analogRead(36);
      delay(50);
    }
    // delay(100);
    myCar.Move(Forward, 128);
  }
  else if (left > 2700 && right < 2700)
  {
    while (middle < 2500)
    {
      myCar.Move(Contrarotate, 128);
      Serial.println("Rotating left");
      middle = analogRead(36);
      delay(50);
    }
    // delay(100);
    myCar.Move(Forward, 96);
  }
  else if (lastSeen > 2500 && middle < 2500 && left < 2700 && right < 2700) 
  {
    myCar.Move(Backward, 128);
    delay(50);
    while (middle < 2500) {
      middle = analogRead(36);
      Serial.println(middle);
      myCar.Move(Backward, 64);
    }
    delay(20);
    if (left > 2700 && right < 2700) {
      myCar.Move(Move_Left, 128);
    }
    else if (left < 2700 && right > 2700) {
      myCar.Move(Move_Right, 128); 
    }
    else {
      myCar.Move(Move_Left, 128);
    }
    delay(120);
  }
  myCar.Move(Forward, 96);
  lastSeen = middle;
}

void automaticMode()
{
  servo.write(90);
  delay(500);
  ultrasonicSens();
  if (wallDetected == 0)
  {
    servo.write(45);
    delay(500);
    ultrasonicSens();
  }
  if (wallDetected == 0)
  {
    servo.write(135);
    delay(500);
    ultrasonicSens();
  }
  if (ultrasonicDistance > 40 && forward == 1 && wallDetected == 0)
  {
    myCar.Move(Forward, 128);
  }
  else if (ultrasonicDistance > 40 && forward == 0 && wallDetected == 0)
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
  wallDetected = 0;
}

void manualMode()
{
  ultrasonicSens();
  if (ultrasonicDir == uGoForward)
  {
    servo.write(90);
  }
  else if (ultrasonicDir == uGoRotateRight)
  {
    servo.write(45);
  }
  else if (ultrasonicDir == uGoRight)
  {
    servo.write(0);
  }
  else if (ultrasonicDir == uGoLeft)
  {
    servo.write(180);
  }
  else if (ultrasonicDir == uGoRotateLeft)
  {
    servo.write(135);
  }


  if ((direction == GoForward && ultrasonicDir == uGoForward) || (direction == GoRotateRight && ultrasonicDir == uGoRotateRight) || (direction == GoRight && ultrasonicDir == uGoRight) || (direction == GoLeft && ultrasonicDir == uGoLeft) || (direction == GoRotateLeft && ultrasonicDir == uGoRotateLeft))
  {
    stopIfWall(30);
  }


  if (direction == GoBack)
  {
    reverse();
  }

  else if (lights == 0)
  {
    digitalWrite(pinLeftLED, LOW);
    digitalWrite(pinRightLED, LOW);
  }
}

void whichMode()
{
  if (mode == automatic)
  {
    automaticMode();
  }
  else if (mode == manual)
  {
    manualMode();
  }
  else if (mode == line)
  {
    lineFollow();
  }
  else if (mode == rando)
  {
    randomDir();
  }
}

void remote()
{
  if (irrecv.decode())
  {
    if (!irrecv.decodedIRData.flags)
    {
      Serial.print("IR Code Received: ");
      int signal = irrecv.decodedIRData.command;
      Serial.println(signal);
      Serial.print("IR Code Received (Hex): 0x");
      Serial.println(irrecv.decodedIRData.command, HEX);
      if (signal == 70 && mode == manual)
      {
        direction = GoForward;
        ultrasonicDir = uGoForward;
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
      }
      else if (signal == 21 && mode == manual)
      {
        direction = GoBack;
        ultrasonicDir = uGoForward;
        myCar.Move(Backward, 255);
        delay(100);
        myCar.Move(Backward, 128);
      }
      else if (signal == 64 && mode == manual)
      {
        direction = GoStop;
        ultrasonicDir = uGoForward;
        digitalWrite(pinLeftLED, LOW);
        digitalWrite(pinRightLED, LOW);
        myCar.Move(Stop, 0);
      }
      else if (signal == 67 && mode == manual)
      {
        direction = GoRight;
        ultrasonicDir = uGoRight;
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
      }
      else if (signal == 68 && mode == manual)
      {
        direction = GoLeft;
        ultrasonicDir = uGoLeft;
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
      }
      else if (signal == 13 && mode == manual)
      {
        direction = GoRotateRight;
        ultrasonicDir = uGoRotateRight;
        myCar.Move(Clockwise, 255);
        delay(100);
        myCar.Move(Clockwise, 128);
      }
      else if (signal == 22 && mode == manual)
      {
        direction = GoRotateLeft;
        ultrasonicDir = uGoRotateLeft;
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
      }
      else if (signal == 66)
      {
        direction = GoTurnLeft;
        ultrasonicDir = uGoRotateLeft;
        manualMode();
        carRotate(Left, 256, 2000);
        direction = GoStop;
        ultrasonicDir = uGoForward;
      }
      else if (signal == 74)
      {
        direction = GoTurnRight;
        ultrasonicDir = uGoRotateRight;
        manualMode();
        carRotate(Right, 256, 2000);
        direction = GoStop;
        ultrasonicDir = uGoForward;
      }
      else if (signal == 12)
      {
        direction = GoStop;
        mode = automatic;
      }
      else if (signal == 24)
      {
        direction = GoStop;
        mode = manual;
        myCar.Move(Stop, 0);
      }
      else if (signal == 94)
      {
        direction = GoStop;
        mode = line;
      }
      else if (signal == 28)
      {
        direction = GoStop;
        mode = rando;
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
    }
    irrecv.resume();
  }
}

void setup()
{
  Serial.begin(115200);

  servo.attach(pinServo, 500, 2500);
  servo.setPeriodHertz(50);
  servo.write(90);

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
  whichMode();
  remote();

  delay(100);
}