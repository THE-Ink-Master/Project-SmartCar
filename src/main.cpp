// Libraries
#include <Arduino.h>
#include <vehicle.h>
#include <ultrasonic.h>
#include <ESP32Servo.h>
#include <IRremote.h>

// Pins
#define pinLeftLED 2
#define pinIr 4
#define pinRightLED 12
#define pinServo 25
#define pinBuzzer 33
#define pinIRLeft 35
#define pinIRMiddle 36
#define pinIRRight 39

// definitions for turning
#define Right 0
#define Left 1

// IR, Car, Ultrasonic and servo setup
IRrecv irrecv(pinIr);
vehicle myCar;
ultrasonic myUltrasonic;
Servo servo;

// Line following setup
int lastSeen = 0;

// Modes and directions setup
enum modes {
  automatic,
  manual,
  line,
  rando
};

modes mode = manual;

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

int lights = 0;

int forward = 0;

int wallDetected = 0;

// Random Value setup
int randomVal = 0;
int randomLeftVal = 0;
int randomRightVal = 0;
int randomServoVal = 0;
int baseVal = 0;

int ultrasonicDistance;

// Sets both LEDs to the brightness given (can be HIGH or LOW)
void setBothLEDs(uint8_t brightness) {
  digitalWrite(pinLeftLED, brightness);
  digitalWrite(pinRightLED, brightness);
}

// Gets distance from ultrasonic sensor
void ultrasonicSens(int enSerial = 0) {
  ultrasonicDistance = myUltrasonic.Ranging();

  if (enSerial == 1) {
    Serial.print("Ultrasonic Distance: ");
    Serial.print(ultrasonicDistance);
    Serial.println("cm");
  }

  if (mode == 0 && ultrasonicDistance < 40) {
    wallDetected = 1;
  }
}

// Returns true if wall, and false if no wall
bool stopIfWall(int distance) {
  if (ultrasonicDistance < distance) {
    myCar.Move(Stop, 0);
    return true;
  }
  return false;
}

// Turns the car (direction = left or right, speed = speed (0-255), time = how long it turns for in ms, timePer = time per loop (default 100ms), enSerial = enable serial output)
void carTurn(int direction, int speed, int time, int timePer = 100, int wallStopDist = 40, int enSerial = 0) {
  // localTime is the codes internal track of time
  int localTime = 0;
  while (time > localTime) {
    // Increase localTime by time per
    localTime += timePer;

    if (direction == 0) {
      myCar.Move(Clockwise, speed);
    } else if (direction == 1) {
      myCar.Move(Contrarotate, speed);
    }
    delay(timePer / 3);
    myCar.Move(Forward, speed);
    delay(timePer / 3);
    delay(timePer / 3);

    myCar.Move(Stop, 0);
    if (enSerial == 1) {
      Serial.print("direction: ");
      Serial.println(direction);
      Serial.print("speed: ");
      Serial.println(speed);
      Serial.print("time: ");
      Serial.println(time);
      Serial.print("local time: ");
      Serial.println(localTime);
    }
    ultrasonicSens();
    bool temp = stopIfWall(wallStopDist);
    if (temp == true) {
      return;
    }
  }
}

// Gets random values for all actuators and sets actuators to them
void randomDir(int enSerial = 0) {
  // Get random value
  randomVal = rand() % 122;
  randomLeftVal = rand() % 2;
  randomRightVal = rand() % 2;
  randomServoVal = rand() % 181;
  // Set LEDs to random values
  digitalWrite(pinLeftLED, randomLeftVal);
  digitalWrite(pinRightLED, randomRightVal);
  // Set servo to random value
  servo.write(randomServoVal);
  // Set car direction based on random value
  if (randomVal <= 20) {
    myCar.Move(Forward, 128);
    delay(1000);
  } else if (randomVal <= 40 && !randomVal <= 20) {
    myCar.Move(Move_Left, 128);
    delay(1000);
  } else if (randomVal <= 60 && !randomVal <= 40) {
    myCar.Move(Move_Right, 128);
    delay(1000);
  } else if (randomVal <= 80 && !randomVal <= 60) {
    myCar.Move(Backward, 128);
    delay(1000);
  } else if (randomVal <= 100 && !randomVal <= 80) {
    myCar.Move(Clockwise, 128);
    delay(1000);
  } else if (randomVal <= 120 && !randomVal <= 100) {
    myCar.Move(Contrarotate, 128);
    delay(1000);
  } else if (randomVal <= 140 && !randomVal <= 120) {
    myCar.Move(Stop, 0);
    delay(1000);
  }
}

// Code that handles the LED and buzzer when reversing
void reverse() {
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

// Line following mode code
void lineFollow(int enSerial = 0) {
  int left = analogRead(35);
  int middle = analogRead(36);
  int right = analogRead(39);
  if (enSerial == 1) {
    Serial.println("Left: ");
    Serial.println(left);
    Serial.println("Middle: ");
    Serial.println(middle);
    Serial.println("Right ");
    Serial.println(right);
  }

  if (right > 2700 && left < 2700 && middle > 2500) {
    myCar.Move(Move_Right, 128);
    delay(50);
    myCar.Move(Forward, 128);
    if (enSerial == 1) {
      Serial.println("Going right");
    }
  } else if (left > 2700 && right < 2700 && middle > 2500) {
    myCar.Move(Move_Left, 128);
    delay(50);
    myCar.Move(Forward, 128);
    if (enSerial == 1) {
      Serial.println("Going left");
    }
  } else if (middle > 2500) {
    myCar.Move(Forward, 128);
    delay(20);
    myCar.Move(Forward, 64);
    if (enSerial == 1) {
      Serial.println("Going forward");
    }
  } else if (right > 2700 && left < 2700) {
    while (middle < 2500) {
      myCar.Move(Clockwise, 128);
      if (enSerial == 1) {
        Serial.println("Rotating right");
      }
      middle = analogRead(36);
      delay(50);
    }
    // delay(100);
    myCar.Move(Forward, 128);
  } else if (left > 2700 && right < 2700) {
    while (middle < 2500) {
      myCar.Move(Contrarotate, 128);
      if (enSerial == 1) {
        Serial.println("Rotating left");
      }
      middle = analogRead(36);
      delay(50);
    }
    // delay(100);
    myCar.Move(Forward, 96);
  } else if (lastSeen > 2500 && middle < 2500 && left < 2700 && right < 2700) {
    myCar.Move(Backward, 128);
    delay(50);
    while (middle < 2500) {
      middle = analogRead(36);
      if (enSerial == 1) {
        Serial.println(middle);
      }
      myCar.Move(Backward, 64);
    }
    delay(20);
    if (left > 2700 && right < 2700) {
      myCar.Move(Move_Left, 128);
    } else if (left < 2700 && right > 2700) {
      myCar.Move(Move_Right, 128);
    } else {
      myCar.Move(Move_Left, 128);
    }
    delay(120);
  }
  myCar.Move(Forward, 96);
  lastSeen = middle;
}

// Cycles the LEDs on/off switching them each half cycle
void cycle(uint8_t starting, int pin, int cycles, int enSerial) {
  int currentCycle = 0;
  uint8_t Ending = LOW;
  if (starting == LOW) {
    Ending = HIGH;
  } while (cycles > currentCycle) {
    digitalWrite(pin, starting);
    delay(200);
    digitalWrite(pin, Ending);
    delay(200);
    currentCycle += 1;
    if (enSerial == 1) {
      Serial.print("Current cycle: ");
      Serial.println(currentCycle);
      Serial.print("Cycles: ");
      Serial.println(cycles);
      Serial.print("Starting with: ");
      Serial.println(starting);
      Serial.print("Ending with: ");
      Serial.println(Ending);
    }
  }
}

// Moves the car automatically if it detects a wall
void automaticMode(int enSerial = 0) {
  servo.write(90);
  delay(500);
  ultrasonicSens();
  if (wallDetected == 0) {
    servo.write(45);
    delay(500);
    ultrasonicSens(enSerial);
  }
  if (wallDetected == 0) {
    servo.write(135);
    delay(500);
    ultrasonicSens(enSerial);
  }
  if (ultrasonicDistance > 40 && forward == 1 && wallDetected == 0) {
    myCar.Move(Forward, 128);
  } else if (ultrasonicDistance > 40 && forward == 0 && wallDetected == 0) {
    myCar.Move(Forward, 255);
    delay(100);
    myCar.Move(Forward, 128);
    forward = 1;
  } else {
    forward = 0;
    myCar.Move(Stop, 0);
    servo.write(135);
    digitalWrite(pinLeftLED, HIGH);
    digitalWrite(pinRightLED, LOW);
    delay(1000);
    ultrasonicSens();
    if (ultrasonicDistance > 50) {
      myCar.Move(Contrarotate, 255);
      digitalWrite(pinLeftLED, LOW);
      delay(100);
      myCar.Move(Contrarotate, 128);
      delay(100);
      cycle(HIGH, pinLeftLED, 2, enSerial);
      digitalWrite(pinLeftLED, HIGH);
      myCar.Move(Stop, 0);
    } else {
      servo.write(45);
      digitalWrite(pinLeftLED, LOW);
      digitalWrite(pinRightLED, HIGH);
      delay(1000);
      ultrasonicSens();
      if (ultrasonicDistance > 50) {
        myCar.Move(Clockwise, 255);
        digitalWrite(pinRightLED, LOW);
        delay(100);
        myCar.Move(Clockwise, 128);
        delay(100);
        cycle(HIGH, pinRightLED, 2, enSerial);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Stop, 0);
      } else {
        myCar.Move(Backward, 255);
        digitalWrite(pinRightLED, LOW);
        delay(100);
        myCar.Move(Backward, 128);
        delay(100);
        cycle(HIGH, pinRightLED, 2, enSerial);
        digitalWrite(pinRightLED, HIGH);
        myCar.Move(Clockwise, 255);
        delay(100);
        myCar.Move(Clockwise, 128);
        delay(100);
        cycle(LOW, pinRightLED, 2, enSerial);
        myCar.Move(Stop, 0);
      }
    }
  }
  servo.write(90);
  setBothLEDs(LOW);
  wallDetected = 0;
}

// Changes the ultrasonic sensor servo direction based on what the ultrasonicDir enum is 
void manualMode(int enSerial = 0) {
  ultrasonicSens(enSerial);
  switch (ultrasonicDir) {
  case uGoForward:
    servo.write(90);
    break;
  case uGoRotateRight:
    servo.write(45);
    break;
  case uGoRight:
    servo.write(0);
    break;
  case uGoLeft:
    servo.write(180);
    break;
  case uGoRotateLeft:
    servo.write(135);
    break;
  default:
    break;
  }

  // If the direction matches the ultrasonics direction, stop the car if there is a wall
  if ((direction == GoForward && ultrasonicDir == uGoForward) || (direction == GoRotateRight && ultrasonicDir == uGoRotateRight) || (direction == GoRight && ultrasonicDir == uGoRight) || (direction == GoLeft && ultrasonicDir == uGoLeft) || (direction == GoRotateLeft && ultrasonicDir == uGoRotateLeft)) {
    stopIfWall(30);
  }

  // Reverse if car is going backwards
  if (direction == GoBack) {
    reverse();
  }

  // Turn the lights off if they aren't meant to be on
  if (lights == 0) {
    setBothLEDs(LOW);
  }
}

// Gets the current mode and runs the corresponding function
void whichMode(int enSerial = 0) {
  switch (mode) {
  case automatic:
    automaticMode(enSerial);
    break;
  case manual:
    manualMode(enSerial);
    break;
  case line:
    lineFollow(enSerial);
    break;
  case rando:
    randomDir(enSerial);
    break;
  default:
    break;
  }
}

// Speeds up the car temporarily
void speedUp(int direction, int initSpeed = 255, int endSpeed = 128, int wallStopDist = 50, int time = 1000, int enSerial = 0) {
  myCar.Move(direction, initSpeed);
  bool wall = stopIfWall(wallStopDist);
  if (wall == true) {
    return;
  }
  delay(time);
  myCar.Move(direction, endSpeed);
}

// When a button is pressed this function gets that signal and does what it needs to do with it, such as changing the mode from automatic to manual when 
void remote(int enSerial = 0) {
  if (irrecv.decode()) {
    if (!irrecv.decodedIRData.flags) {
      int signal = irrecv.decodedIRData.command;
      if (enSerial == 1) {
        Serial.print("IR Code Received: ");
        Serial.println(signal);
        Serial.print("IR Code Received (Hex): 0x");
        Serial.println(irrecv.decodedIRData.command, HEX);
      }
      if (signal == 70 && mode == manual) {
        // Up button
        direction = GoForward;
        ultrasonicDir = uGoForward;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        myCar.Move(Forward, 255);
        delay(100);
        myCar.Move(Forward, 128);
      } else if (signal == 21 && mode == manual) {
        // Down button
        direction = GoBack;
        ultrasonicDir = uGoForward;
        myCar.Move(Backward, 255);
        delay(100);
        myCar.Move(Backward, 128);
      } else if (signal == 64 && mode == manual) {
        // OK button
        direction = GoStop;
        ultrasonicDir = uGoForward;
        setBothLEDs(LOW);
        myCar.Move(Stop, 0);
      } else if (signal == 67 && mode == manual) {
        // Right button
        direction = GoRight;
        ultrasonicDir = uGoRight;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        myCar.Move(Move_Right, 255);
        delay(100);
        myCar.Move(Move_Right, 128);
      } else if (signal == 68 && mode == manual) {
        // Left button
        direction = GoLeft;
        ultrasonicDir = uGoLeft;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        myCar.Move(Move_Left, 255);
        delay(100);
        myCar.Move(Move_Left, 128);
      } else if (signal == 13 && mode == manual) {
        // button 3
        direction = GoRotateRight;
        ultrasonicDir = uGoRotateRight;
        myCar.Move(Clockwise, 255);
        delay(100);
        myCar.Move(Clockwise, 128);
      } else if (signal == 22 && mode == manual) {
        // button 1
        direction = GoRotateLeft;
        ultrasonicDir = uGoRotateLeft;
        manualMode();
        delay(200);
        bool temp = stopIfWall(30);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        myCar.Move(Contrarotate, 255);
        delay(100);
        myCar.Move(Contrarotate, 128);
      } else if (signal == 66) {
        // * button
        direction = GoTurnLeft;
        ultrasonicDir = uGoRotateLeft;
        manualMode();
        bool temp = stopIfWall(40);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        carTurn(Left, 256, 2000, 100, 40, enSerial);
        direction = GoStop;
        ultrasonicDir = uGoForward;
      } else if (signal == 74) {
        // # button
        direction = GoTurnRight;
        ultrasonicDir = uGoRotateRight;
        manualMode();
        bool temp = stopIfWall(40);
        if (temp == true) {
          irrecv.resume();
          myCar.Move(Stop, 0);
          direction = GoStop;
          ultrasonicDir = uGoForward;
          return;
        }
        carTurn(Right, 256, 2000, 100, 40, enSerial);
        direction = GoStop;
        ultrasonicDir = uGoForward;
      } else if (signal == 12) {
        // button 4
        direction = GoStop;
        mode = automatic;
      } else if (signal == 24) {
        // button 5
        direction = GoStop;
        mode = manual;
        myCar.Move(Stop, 0);
      } else if (signal == 94) {
        // button 6
        direction = GoStop;
        mode = line;
      } else if (signal == 28) {
        // button 8
        direction = GoStop;
        mode = rando;
      } else if (signal == 90) {
        // button 9
        lights = 1;
        setBothLEDs(HIGH);
      } else if (signal == 8) {
        // button 7
        lights = 0;
        setBothLEDs(LOW);
      } else if (signal == 25) {
        // Button 2 (Not Used)
      } else if (signal == 82 && mode == 1) {
        // button 0
        if (direction == GoForward) {
          speedUp(Forward, 255, 128, 50, 1000, enSerial);
        } else if (direction == GoRotateRight) {
          speedUp(Clockwise, 255, 128, 50, 1000, enSerial);
        } else if (direction == GoRight) {
          speedUp(Move_Right, 255, 128, 50, 1000, enSerial);
        } else if (direction == GoBack) {
          myCar.Move(Backward, 255);
          reverse();
          delay(100);
          reverse();
          delay(100);
          myCar.Move(Backward, 128);
        } else if (direction == GoLeft) {
          speedUp(Move_Left, 255, 128, 50, 1000, enSerial);
        } else if (direction == GoRotateLeft) {
          speedUp(Contrarotate, 255, 128, 50, 1000, enSerial);
        }
      }
    }
    irrecv.resume();
    // direction = GoStop;
    // ultrasonicDir = uGoForward;
  }
}

// Setup code
void setup() {
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

// Main loop
void loop() {
  // enSerial = enable serial
  int enSerial = 1;
  whichMode(enSerial);
  remote(enSerial);

  delay(100);
}