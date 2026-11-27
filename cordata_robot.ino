
#include "BluetoothSerial.h"

BluetoothSerial BT;

// L298N motor driver pins
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;

// Ultrasonic sensor pins
const int TRIG = 5;
const int ECHO = 18;

String command = "";

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

void setup() {
  Serial.begin(115200);

  BT.begin("CORDATA");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  stopRobot();

  Serial.println("CORDATA is ready!");
  BT.println("Hello! I am CORDATA.");
}

void loop() {
  if (BT.available()) {
    command = BT.readString();
    command.trim();
    command.toLowerCase();

    Serial.println(command);

    if (command == "forward" ||
        command == "go forward") {

      float distance = getDistance();

      if (distance < 20) {
        stopRobot();
        BT.println("Obstacle detected!");
      } else {
        forward();
        BT.println("Moving forward");
      }
    }

    else if (command == "backward" ||
             command == "back") {
      backward();
    }

    else if (command == "left" ||
             command == "turn left") {
      left();
    }

    else if (command == "right" ||
             command == "turn right") {
      right();
    }

    else if (command == "stop") {
      stopRobot();
    }

    else {
      BT.println("Command not recognized");
    }
  }
}




