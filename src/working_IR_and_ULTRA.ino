#include <Servo.h>          //Servo motor library. This is standard library
#include <NewPing.h>        //Ultrasonic sensor function library. You must install this library
//#include <iostream>         // for out stack 
//#include <stack>            // for out stack 

//our L298N control pins
//int i;
const int IR_LEFT = A1;
const int IR_RIGHT = 11;
// const int IR_LEFT_F = ;
// const int IR_RIGHT_F = ;
const int MOVE = 70;
const int MotorsEnable = 3;
const int LeftMotorEnable = 6;
const int RightMotorEnable = 5;
const int RightMotorDirection = 7;
const int LeftMotorDirection = 8;

//sensor pins
#define trig_pin 13//A1 //analog input 1
#define echo_pin 12//A2 //analog input 2

#define maximum_distance 200
boolean goesForward = false;
int distance = 100;
//std::stack<char> lastCliff;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function
Servo servo_motor; //our servo name

void setup(){
  //std::stack<char>().swap(lastCliff);
  Serial.begin(9600);
  pinMode(MotorsEnable, OUTPUT);
  pinMode(RightMotorEnable, OUTPUT);
  pinMode(LeftMotorEnable, OUTPUT);
  pinMode(LeftMotorDirection, OUTPUT);
  pinMode(RightMotorDirection, OUTPUT);
  

  servo_motor.attach(10); //our servo pin

  servo_motor.write(100);
  delay(2000);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
}


void loop(){
  int distanceRight = 0;
  int distanceLeft = 0;
  delay(50);
  moveForward();
  cliffCheck();
  if (distance <= 25){
    moveStop();
    delay(300);
    moveBackward();
    delay(10);
    moveStop();
    delay(300);
    distanceRight = lookRight();
    delay(300);
    distanceLeft = lookLeft();
    delay(300);

    if (distance >= distanceLeft){
      turnRight();
      moveStop();
    }
    else{
      turnLeft();
      moveStop();
    }
  }
  else{
    moveForward();
  }
    distance = readPing();
    //std::stack<char>().swap(lastCliff); 
} 

void cliffCheck(){
  int sensorStatus_Left = digitalRead(IR_LEFT);
  int sensorStatus_Right = digitalRead(IR_RIGHT);
  Serial.println(sensorStatus_Left);
  Serial.println(sensorStatus_Right);
  if (sensorStatus_Left == 1){
    //lastCliff.push('L');
    Serial.println("LEFT CLIFF!"); // print Motion Detected! on the serial monitor window
    clifMoveBackward();
    //moveBackwardCliff();
    cliffTurnRight();
  }
  else if (sensorStatus_Right == 1){
    Serial.println("RIGHT CLIFF!"); // print Motion Detected! on the serial monitor window
    //lastCliff.push('R');
    //moveBackwardCliff();
    clifMoveBackward();
    cliffTurnLeft();
  }else{
    Serial.println("LAND!");
    moveForward();
  }
}


int lookRight(){ 
  servo_motor.write(25);
  delay(500);
  int distance = readPing();
  delay(100);
  Serial.print("Right Distance, ");
  Serial.println(distance);
  servo_motor.write(95);
  return distance;
}


int lookLeft(){
  
  servo_motor.write(160);
  delay(500);
  int distance = readPing();
  delay(100);
  servo_motor.write(95);
  Serial.print("Left Distance, ");
  Serial.println(distance);
  return distance;
  delay(100);
}


int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  Serial.println(cm);
  return cm;
}


void moveStop(){
  digitalWrite(MotorsEnable, HIGH);
  digitalWrite(LeftMotorEnable, LOW);
  digitalWrite(RightMotorEnable, LOW);
  digitalWrite(RightMotorDirection, LOW);
  digitalWrite(LeftMotorDirection, LOW);

}



void moveForward(){
  if(!goesForward){
    goesForward=true;
    digitalWrite(3, HIGH); //Motors Enable
    analogWrite(5, MOVE); //Right motor enable
    analogWrite(6, MOVE); //Left motor enable
    digitalWrite(7, HIGH); //Right motor direction
    digitalWrite(8, HIGH); //Left motor direction
  }
}

void moveBackward(){
  goesForward=false;

  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(RightMotorDirection, LOW);
  analogWrite(LeftMotorDirection, LOW);
  delay(100);
}

void clifMoveBackward(){
  goesForward=false;

  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(RightMotorDirection, LOW);
  analogWrite(LeftMotorDirection, LOW);
  delay(200);
}

void turnRight(){
  Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(1000);
}

void cliffTurnRight(){
  Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(600);
}

void turnLeft(){
  Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(1000);
}

void cliffTurnLeft(){
  Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(600);
}
