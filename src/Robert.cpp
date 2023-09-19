#include <Arduino.h>
//www.elegoo.com
//2016.12.12

/************************
Exercise the motor using
the L293D chip
************************/

#define ENABLE 6
#define ENB 5
#define DIRA 5
#define DIRB 4


int i;
int IR1 = 10;
int LED = 13;

void turn_left();
void turn_right();
void reverse();
void stop();
void sethi();
 
void setup() {
  Serial.begin(9600);
  Serial.print("Start! \n");
  pinMode(IR1, INPUT);
  pinMode(LED, OUTPUT);
  //---set pin direction
  //sethi();
 
    
  
  
  //delay(10000);
}
void sethi(){
  for(i=0; i< 11; i++){
  pinMode(i, HIGH);
  analogWrite(i, 150);
}
}
void forward(){
  Serial.print("Forward! \n");
  sethi();
  //delay(500);
  
}

void loop(){
 /* reverse();
  delay(2000);
  forward();
  delay(2000);
  turn_left();
  delay(2000);
  turn_right();
  delay(2000);
  stop();*/
  int sensorStatus = digitalRead(IR1);
  if (sensorStatus == 1) // Check if the pin high or not
  {
    // if the pin is high turn off the onboard Led
    digitalWrite(LED, LOW); // LED LOW
    Serial.println("Motion Detected!"); // print Motion Detected! on the serial monitor window
  }
  else  {
    //else turn on the onboard LED
    digitalWrite(LED, HIGH); // LED High
    Serial.println("Motion Ended!"); // print Motion Ended! on the serial monitor window
  }
}



/*void loop(){
  for(i=0; i< 11; i++){
    digitalWrite(i, LOW);
    Serial.print("Setting " );
    Serial.print(i);
    Serial.println(" low.");
    delay(10000);
    digitalWrite(i,HIGH);
  delay(2000);



  }*/
void reverse(){
  Serial.print("Reverse! \n");
  sethi();
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  //delay(500);
  //delay(10000);
}

void turn_left(){
  Serial.print("Turning Left! \n");
  sethi();
  digitalWrite(8, LOW);
  //delay(500);
}

void turn_right(){
  Serial.print("Turning Right! \n");
  sethi();
  digitalWrite(7, LOW);
  //delay(500);
}

void stop(){
  Serial.print("Stop! \n");
  sethi();
  digitalWrite(3, LOW);
  exit(0);
  
}