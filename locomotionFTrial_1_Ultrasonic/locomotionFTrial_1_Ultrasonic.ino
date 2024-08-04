#include <SoftwareSerial.h>

#define leftSensor A1
#define rightSensor A0
#define trigPin A3      // Ultrasonic sensor trigger pin
#define echoPin A2     // Ultrasonic sensor echo pin
#define buzzerPin 13   // Piezo buzzer pin

SoftwareSerial mySerial(A5, A4); // RX, TX
String BT_value;
String BT_value_temp;

const int PWM2A = 11;      // M1 motor PWM signal pin
const int PWM2B = 3;       // M2 motor PWM signal pin
const int PWM0A = 6;       // M3 motor PWM signal pin
const int PWM0B = 5;       // M4 motor PWM signal pin
const int DIR_CLK = 4;     // Data input clock line
const int DIR_EN = 7;      // Equip the L293D enabling pins
const int DATA = 8;        // USB cable
const int DIR_LATCH = 12;  // Output memory latch clock

const int Move_Forward = 43;       // Move Forward
const int Move_Backward = 212;     // Move Backward
const int Left_Move = 120;         // Left translation
const int Right_Move = 135;        // Right translation
const int Right_Rotate = 153;      // Rotate Right
const int Left_Rotate = 102;       // Rotate Left
const int Stop = 0;                // Stop

int startingTable;
int Speed1 = 125; // Set the default speed between 1 and 255
int Speed2 = 125; // Set the default speed between 1 and 255
int Speed3 = 125; // Set the default speed between 1 and 255
int Speed4 = 125; // Set the default speed between 1 and 255

void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
  analogWrite(PWM2A, Speed1);  // Motor PWM speed regulation
  analogWrite(PWM2B, Speed2);  // Motor PWM speed regulation
  analogWrite(PWM0A, Speed3);  // Motor PWM speed regulation
  analogWrite(PWM0B, Speed4);  // Motor PWM speed regulation

  digitalWrite(DIR_LATCH, LOW);            // DIR_LATCH sets the low level and writes the direction of motion in preparation
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Write Dir motion direction value
  digitalWrite(DIR_LATCH, HIGH);           // DIR_LATCH sets the high level and outputs the direction of motion
}

long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration / 2) / 29.1;
   delay(5);
  Serial.print("Read distance: ");
  Serial.println(distance);
  return distance;
}

void obstacleDetect(){
  long distance = readUltrasonicDistance();
  Serial.print("Read distance: ");
  Serial.println(distance);
  if (distance < 10) {
    Serial.println("Barrier Detected!");
    Motor(Stop, 0, 0, 0, 0);
     Serial.println(distance);
    digitalWrite(buzzerPin, HIGH);
    while (readUltrasonicDistance() < 10) {
      Serial.println("Stop Loop");
      delay(100);

    }
    digitalWrite(buzzerPin, LOW);
  }
}

void followLine() {
  int leftValue = analogRead(leftSensor);
  int rightValue = analogRead(rightSensor);

  if (leftValue < 500 && rightValue < 500) {
    // Both sensors detect line, move forward
    Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
    delay(21);
  }if (leftValue > 500) {
    // Left sensor detects line, turn left
    Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
    delay(21);
  } if (rightValue > 500) {
    // Right sensor detects line, turn right
    Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
    delay(21);
 } 
  else {
    // No line detected, stop
    Motor(Stop, 0, 0, 0, 0);
  }
    obstacleDetect();
}

void goToTable(int tableNumber) {
  while (true) {
    followLine();

    int leftValue = analogRead(leftSensor);
    int rightValue = analogRead(rightSensor);

    if (leftValue > 500 && rightValue > 500) {
        Motor(Stop, 0, 0, 0, 0); 
        delay(500); 
      if (tableNumber == 1) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(250);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
        delay(800); 
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        delay(180);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        while (true) {
          followLine();
          int leftValue = analogRead(leftSensor);
          int rightValue = analogRead(rightSensor);
            if (leftValue > 500 && rightValue > 500) {
              Motor(Stop, 0, 0, 0, 0);
              break;}
        }
        break;
     
      } else if (tableNumber == 2) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(300);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
        delay(820); 
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        delay(160);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        while (true) {
          followLine();
          int leftValue = analogRead(leftSensor);
          int rightValue = analogRead(rightSensor);
            if (leftValue > 500 && rightValue > 500) {
              Motor(Stop, 0, 0, 0, 0);
              break;}
        }
        break;
      } else if (tableNumber == 3) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(300);
        tableNumber = 2;
      } else if (tableNumber == 4) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(500);
        tableNumber = 1;
      }
    }
  }

  Motor(Stop, 0, 0, 0, 0); // Stop after reaching the table

}

void goToWaitingArea() {
  Motor(Right_Rotate, 125, 125, 125, 125);
  delay(1410); // Rotate 180 degree

    while (true) {
    followLine();

    int leftValue = analogRead(leftSensor);
    int rightValue = analogRead(rightSensor);

    if (leftValue > 500 && rightValue > 500) {
        Motor(Stop, 0, 0, 0, 0); 
        delay(500); 
      if (startingTable == 1) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(300);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
        delay(700); 
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        delay(200);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        while (true) {
          followLine();
          int leftValue = analogRead(leftSensor);
          int rightValue = analogRead(rightSensor);
            if (leftValue > 500 && rightValue > 500) {
              Motor(Stop, 0, 0, 0, 0);
              delay(900);
              Motor(Right_Rotate, 125, 125, 125, 125);
              delay(1410); // Rotate 180 degree
              Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
              delay(300);
              break;}
        }
        break;
     
      } else if (startingTable == 2) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        delay(300);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
        delay(690); 
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        delay(180);
        Motor(Stop, 0, 0, 0, 0);
        delay(500); 
        while (true) {
          followLine();
          int leftValue = analogRead(leftSensor);
          int rightValue = analogRead(rightSensor);
            if (leftValue > 500 && rightValue > 500) {
              Motor(Stop, 0, 0, 0, 0);
              delay(900);
              Motor(Right_Rotate, 125, 125, 125, 125);
              delay(1410); // Rotate 180 degree
              Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
              delay(300);
              break;}
        }
        break;
      } else if (startingTable == 3) {
          Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
          delay(300);
          Motor(Stop, 0, 0, 0, 0);
          delay(500); 
          Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
          delay(700); 
          Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
          delay(200);
          while (true) {
            followLine();
            int leftValue = analogRead(leftSensor);
            int rightValue = analogRead(rightSensor);
              if (leftValue > 500 && rightValue > 500) {
                Motor(Stop, 0, 0, 0, 0);
                delay(400); 
                Motor(Move_Forward, 125, 125, 125, 125);
                delay(300);
                while (true) {
                  followLine();
                  int leftValue = analogRead(leftSensor);
                  int rightValue = analogRead(rightSensor);
                    if (leftValue > 500 && rightValue > 500) {
                      Motor(Stop, 0, 0, 0, 0);
                      delay(900);
                      Motor(Right_Rotate, 125, 125, 125, 125);
                      delay(1410); // Rotate 180 degree
                      Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
                      delay(300);
                      break;}
                }
                break;}
        }
        break;
      } else if (startingTable == 4) {
          Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
          delay(300);
          Motor(Stop, 0, 0, 0, 0);
          delay(500); 
          Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
          delay(700); 
          Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
          delay(200);
          while (true) {
            followLine();
            int leftValue = analogRead(leftSensor);
            int rightValue = analogRead(rightSensor);
              if (leftValue > 500 && rightValue > 500) {
                Motor(Stop, 0, 0, 0, 0);
                delay(400); 
                Motor(Move_Forward, 125, 125, 125, 125);
                delay(300);
                while (true) {
                  followLine();
                  int leftValue = analogRead(leftSensor);
                  int rightValue = analogRead(rightSensor);
                    if (leftValue > 500 && rightValue > 500) {
                      Motor(Stop, 0, 0, 0, 0);
                      delay(900);
                      Motor(Right_Rotate, 125, 125, 125, 125);
                      delay(1410); // Rotate 180 degree
                      Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
                      delay(300);
                      break;}
                }
                break;}
        }
        break;  }
    }
    }

  Motor(Stop, 0, 0, 0, 0); // Stop after reaching the table
    int startingTable = 0;
}

void setup() {
  BT_value = "";
  BT_value_temp = "";

  mySerial.begin(9600); // Set the myserial port baud rate 9600
  Serial.begin(9600);   // Set the serial port baud rate 9600

  // Configure as output mode
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWM0B, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  pinMode(leftSensor, INPUT);
  pinMode(rightSensor, INPUT);
  pinMode(trigPin, OUTPUT); // Ultrasonic sensor trigger pin
  pinMode(echoPin, INPUT);  // Ultrasonic sensor echo pin
  pinMode(buzzerPin, OUTPUT); // Piezo buzzer pin

  Serial.println("ready");
}

void loop() {

 // readUltrasonicDistance();
  while (mySerial.available() > 0) {
    BT_value_temp += (char)mySerial.read();
    delay(2);
    if (!mySerial.available() > 0) {
      BT_value = BT_value_temp;
      BT_value_temp = "";
    }
  }

  if (BT_value.length() > 0) {
    Serial.println(BT_value);
    if (BT_value.length() == 1) {
      int command = BT_value.toInt();

      if (command == 1) {
        goToTable(1);
        startingTable = command;
      } else if (command == 2) {
        goToTable(2);
        startingTable = command;
      } else if (command == 3) {
        goToTable(3);
        startingTable = command;
      } else if (command == 4) {
        goToTable(4);
        startingTable = command;
      } else if (command == 5) {
        goToWaitingArea(); // Pass the last table number the robot visited
      }
    }

    BT_value = "";
  }
}
