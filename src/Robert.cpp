#include <Servo.h>          //Servo motor library. This is standard library
#include <NewPing.h>        //Ultrasonic sensor function library. You must install this library
//#include <iostream>         // for out stack 
//#include <stack>            // for out stack 

//our L298N control pins
//int i
const int IR_LEFT = 10;
const int IR_RIGHT = 11;
const int LINE_RIGHT = A0;
const int LINE_MIDDLE = A1 ;
const int LINE_LEFT = A2;
const int MOVE = 53;
const int MotorsEnable = 3;
const int LeftMotorEnable = 6;
const int RightMotorEnable = 5;
const int RightMotorDirection = 7;
const int LeftMotorDirection = 8;
const int THRESHOLD = 900;
const int MAX_OBJ_R = 5;
const int MAX_OBJ_L = 5;
const int MAX_OBJ = 5;
const int MAX_OBJ_B = 2;
const int FLAME_LEFT = 25;
const int FLAME_RIGHT = 29;
const int FLAME_MIDDLE = 27;
const int FLAME_LEFT2 = 31;
const int FLAME_RIGHT2 = 30;
const int FAN_INA = 45;
const int FAN_INB = 44;

//sensor pins
#define trig_pin 13//A1 //analog input 1
#define echo_pin 12//A2 //analog input 2

#define trig_r_pin 49//A1 //analog input 1
#define echo_r_pin 47//A2 //analog input 2

#define trig_l_pin 39//A1 //analog input 1
#define echo_l_pin 37//A2 //analog input 2

#define trig_sr_pin 49//A1 //analog input 1
#define echo_sr_pin 47//A2 //analog input 2

#define trig_sl_pin 39//A1 //analog input 1
#define echo_sl_pin 37//A2 //analog input 2

#define trig_b_pin 53//A1 //analog input 1
#define echo_b_pin 52//A2 //analog input 2

#define maximum_distance 200
boolean goesForward = false;
boolean middleLine = false; //for line tracking 
int distance = 100;
int distance_r = 100;
int distance_l = 100;
int distance_sl = 100;
int distance_sr = 100;
int distance_b = 100;

int flameStatus_Left = 1;
int flameStatus_Right = 1;
int flameStatus_Middle = 1;
int flameStatus_Left2 = 1;
int flameStatus_Right2 = 1;
//std::stack<char> lastCliff;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function
NewPing sonar_r(trig_r_pin, echo_r_pin, maximum_distance); //sensor function
NewPing sonar_l(trig_l_pin, echo_l_pin, maximum_distance); //sensor function
NewPing sonar_b(trig_b_pin, echo_b_pin, maximum_distance); //sensor function
//NewPing sonar_sr(trig_r_pin, echo_r_pin, maximum_distance); //sensor function
//NewPing sonar_sl(trig_l_pin, echo_l_pin, maximum_distance); //sensor function
Servo servo_motor; //our servo name

void setup(){
  //std::stack<char>().swap(lastCliff);
  Serial.begin(9600);
  pinMode(MotorsEnable, OUTPUT);
  pinMode(RightMotorEnable, OUTPUT);
  pinMode(LeftMotorEnable, OUTPUT);
  pinMode(LeftMotorDirection, OUTPUT);
  pinMode(RightMotorDirection, OUTPUT);
  pinMode(LINE_RIGHT, INPUT);
  pinMode(LINE_MIDDLE, INPUT);
  pinMode(LINE_LEFT, INPUT);
  pinMode(FAN_INA, OUTPUT); //This is blowing air pin
  pinMode(FAN_INB, OUTPUT); //This is sucking air pin

  //servo_motor.attach(10); //our servo pin

  //servo_motor.write(100);
  delay(2000);
  distance = readPing();
  distance_r = readRightPing();
  distance_l = readLeftPing();
  distance_b = readBackPing();

  //delay(100);

}


void loop(){
  delay(25);
  moveForward();
  wallCheck();
  lineCheck();
  lineFollow();


  while(flameSensing() == 0){
      if((flameStatus_Left == 0 || flameStatus_Left2 == 0) && (flameStatus_Right == 1 && flameStatus_Right2 == 1)){
        flameLeft();
      } else if((flameStatus_Left == 1 && flameStatus_Left2 == 1) && (flameStatus_Right == 0 || flameStatus_Right2 == 0)) {
        flameRight();
      } else if (flameStatus_Left == 1 && flameStatus_Right == 1 && flameStatus_Middle ==0){
        moveStop();
        engageFan(1);
        goesForward=false;
        fireMoveBackward();
        turnLeft();
      }
  }

  if(distance_b <= MAX_OBJ_B){
    Serial.print("Distance Back!");
    Serial.print(distance_b);
    //moveForward();
    //moveStop();
    //goesForward=false;
    //delay(500);
    intruderTurnLeft();
    //moveStop();
    goesForward=false;
    //engageFan(1);
    //delay(1000);
  }

  //fireMoveBackward();
  engageFan(0);
  moveForward();

  if (distance <= MAX_OBJ || distance_l <= MAX_OBJ_L || distance_r <= MAX_OBJ_R){ 
    if (distance > distance_l && distance > distance_r && distance <= MAX_OBJ) {
    // More space in front
    goesForward = true;
    //moveBackward();
    moveForward();
    lineCheck();
    lineFollow();
  } else if (distance_l > distance_r) {
    // More space on the left
    //Serial.print("OBJECT ON RIGHT!!!!");
    //obstructionMoveBackward();
    turnLeft();
    goesForward = false;
    moveForward();
    lineCheck();
    lineFollow();
  } else {
    // More space on the right or equal space on both sides
    //Serial.print("OJECT ON LEFT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    //obstructionMoveBackward();
    turnRight();
    goesForward = false;
    moveForward();
    lineCheck();
    lineFollow();
}

  }
  else{
    moveForward();
    lineCheck();
    lineFollow();
  } 
    distance = readPing();
    distance_l = readLeftPing();
    distance_r = readRightPing();
    distance_b = readBackPing();

} 


int flameSensing(){
  flameStatus_Left = digitalRead(FLAME_LEFT);
  flameStatus_Right = digitalRead(FLAME_RIGHT);
  flameStatus_Middle = digitalRead(FLAME_MIDDLE);
  flameStatus_Left2 = digitalRead(FLAME_LEFT2);
  flameStatus_Right2 = digitalRead(FLAME_RIGHT2);

  if(flameStatus_Left == 0 || flameStatus_Right == 0 || flameStatus_Middle == 0 || flameStatus_Left2 == 0 || flameStatus_Right2 == 0){
    return 0;
  }
  return 1;
}

void wallCheck(){
  int sensorStatus_Left = digitalRead(IR_LEFT);
  int sensorStatus_Right = digitalRead(IR_RIGHT);
  //Serial.println(sensorStatus_Left);
  //Serial.println(sensorStatus_Right);
  if (sensorStatus_Left == 0){
    //Serial.println("LEFT CLIFF!"); // print Motion Detected! on the serial monitor window
    clifMoveBackward();
    cliffTurnRight();
  }
  else if (sensorStatus_Right == 0){
    //Serial.println("RIGHT CLIFF!"); // print Motion Detected! on the serial monitor window
    clifMoveBackward();
    cliffTurnLeft();
  }else{
    //Serial.println("LAND!");
    moveForward();
  }
}

bool lineCheck(){
  int ss_L = digitalRead(LINE_LEFT); 
  int ss_R = digitalRead(LINE_RIGHT);
  int ss_M = digitalRead(LINE_MIDDLE);
  if (middleLine == false){
    if (ss_R == 1){
      //Serial.println("Right Line Detect!");
      //Check for middle sensor
      if (ss_M == 1){
        if (!middleLine){
          middleLine = true;
        }
      }
    }
    else if (ss_L == 1){
      //Serial.println("Left Line Detect!");
      //Check for middle sensor
      if (ss_M == 1){   
        if (!middleLine){
          middleLine = true;
        }
      }
    }

     //Serial.println("looking for line (forward)");
    //Serial.println(ss_M);
  }
}

unsigned long operationStartTime =0;
unsigned long tempTime;

int someOffset = 150;

bool lineOnLeft = false;
bool lineOnRight = false;


void lineFollow(){
int ss_L = digitalRead(LINE_LEFT); 
int ss_R = digitalRead(LINE_RIGHT);
int ss_M = digitalRead(LINE_MIDDLE);

 while (ss_R == 1 && ss_M==1 && ss_L == 1){
    moveBackward();
    delay(50);
    lineLeft();
    delay(270);
    moveForward();
    delay(50);
    ss_L = digitalRead(LINE_LEFT); 
    ss_R = digitalRead(LINE_RIGHT);
    ss_M = digitalRead(LINE_MIDDLE);
  }

  if(ss_L == 1){
    operationStartTime=millis();
    lineLeft();
    lineOnRight = false;
    lineOnLeft =  true;
  }

  if(ss_R == 1){
    tempTime = millis();
    if(tempTime >= operationStartTime + someOffset){
      lineRight();
      lineOnRight = true;
      lineOnLeft =  false;
    }
  }
}


int lookRight(){ 
  servo_motor.write(160);
  delay(500);
  int distance = readPing();
  delay(100);
  //Serial.print("Right Distance, ");
  //Serial.println(distance);
  servo_motor.write(95);
  return distance;
}


int lookLeft(){
  
  servo_motor.write(25);
  delay(500);
  int distance = readPing();
  delay(100);
  servo_motor.write(95);
  //Serial.print("Left Distance, ");
  //Serial.println(distance);
  return distance;
  delay(100);
}


int readPing(){
  //delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  Serial.println(cm);
  return cm;
}

int readRightPing(){
  //delay(70);
  int cm = sonar_r.ping_cm();
  if (cm==0){
    cm=250;
  }
  //Serial.println(cm);
  return cm;
}

int readLeftPing(){
  //delay(70);
  int cm = sonar_l.ping_cm();
  if (cm==0){
    cm=250;
  }
  //Serial.println(cm);
  return cm;
}



int readBackPing(){
  //delay(70);
  int cm = sonar_b.ping_cm();
  if (cm==0){
    cm=250;
  }
  //Serial.println(cm);
  return cm;
}

void engageFan(int i){
  if(i == 1){
  digitalWrite(FAN_INA, 255);
  digitalWrite(FAN_INB, 0);
  delay(500);
  //fireMoveBackward();
  }
  if(i == 0){
  digitalWrite(FAN_INA, 0);
  digitalWrite(FAN_INB, 0);
  }

}

void fireMoveBackward(){
  goesForward=false;

  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(RightMotorDirection, LOW);
  analogWrite(LeftMotorDirection, LOW);
  delay(300);
}

void moveStop(){
  //goesForward=false;
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

void moveBackwardNoDelay(){
  goesForward=false;

  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(RightMotorDirection, LOW);
  analogWrite(LeftMotorDirection, LOW);

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
  //Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(1000);
}

void turnLeft(){
  //Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(1000);
}

void intruderTurnLeft(){
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(3200);
}

void lineRight(){
  goesForward=false;
  Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  //delay(50);
}

void lineLeft(){
  goesForward=false;
  //Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  //delay(50);
}


void cliffTurnRight(){
  //Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(500);
}


void cliffTurnLeft(){
  //Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(500);
}

void flameLeft(){
  goesForward = false;
  //Serial.println("TURNING LEFT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(300);
}

void flameRight(){
  goesForward = false;
  //Serial.println("TURNING RIGHT!");
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(300);
}

void obstructionTurnLeft(){
  goesForward = false;
  Serial.println("TURNING LEFT!");
  //moveBackward();
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorDirection, HIGH);
  delay(400);
}

void obstructionTurnRight(){
  goesForward = false;
  //Serial.println("TURNING RIGHT!");
  //moveBackward();
  digitalWrite(MotorsEnable, HIGH);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(LeftMotorEnable, MOVE);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorDirection, LOW);
  delay(400);
}

void obstructionMoveBackward(){
  goesForward=false;

  digitalWrite(MotorsEnable, HIGH);
  analogWrite(LeftMotorEnable, MOVE);
  analogWrite(RightMotorEnable, MOVE);
  analogWrite(RightMotorDirection, LOW);
  analogWrite(LeftMotorDirection, LOW);
  delay(400);  //Increasing delay Oct 17 from 50 - 200
}

