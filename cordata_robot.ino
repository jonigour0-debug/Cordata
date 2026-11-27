
#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Bluetooth
BluetoothSerial BT;

// OLED display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// Motor pins
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;

// Ultrasonic sensor
const int TRIG = 5;
const int ECHO = 18;

String movement = "STOP";
String command = "";

float distanceCM = 0;

// Motor control
void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  movement = "STOP";
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  movement = "FORWARD";
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  movement = "BACKWARD";
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  movement = "LEFT";
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  movement = "RIGHT";
}

// Distance measurement
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(
    ECHO,
    HIGH,
    25000
  );

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

// OLED screen
void updateDisplay() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Robot name
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("CORDATA");

  display.drawLine(
    0, 19, 127, 19,
    SSD1306_WHITE
  );

  display.setTextSize(1);

  // Bluetooth status
  display.setCursor(0, 24);
  display.print("Bluetooth: ");

  if (BT.hasClient()) {
    display.println("CONNECTED");
  } else {
    display.println("WAITING");
  }

  // Movement
  display.setCursor(0, 36);
  display.print("Move: ");
  display.println(movement);

  // Distance
  display.setCursor(0, 48);
  display.print("Distance: ");

  if (distanceCM >= 999) {
    display.println("--");
  } else {
    display.print(distanceCM, 1);
    display.println(" cm");
  }

  // Obstacle warning
  if (distanceCM > 0 &&
      distanceCM < 20) {

    display.setCursor(0, 57);
    display.println("OBSTACLE!");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);

  // Bluetooth device name
  BT.begin("CORDATA");

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensor pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // OLED setup
  Wire.begin(21, 22);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED not found!");
  } else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("CORDATA");
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Starting...");
    display.display();
  }

  stopRobot();

  BT.println("Hello! I am CORDATA.");

  Serial.println("CORDATA ready!");
}

void loop() {

  // Receive Bluetooth commands
  if (BT.available()) {

    command = BT.readString();
    command.trim();
    command.toLowerCase();

    Serial.println(command);

    if (command == "forward" ||
        command == "go forward") {

      distanceCM = getDistance();

      if (distanceCM < 20) {
        stopRobot();
        BT.println("Obstacle detected!");
      } else {
        forward();
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
  }

  // Update distance
  distanceCM = getDistance();

  // Stop if obstacle is detected while moving forward
  if (movement == "FORWARD" &&
      distanceCM < 20) {

    stopRobot();
    BT.println("Obstacle detected!");
  }

  // Refresh display
  updateDisplay();

  delay(200);
}
