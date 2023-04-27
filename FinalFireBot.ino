#include <Servo.h>

// Servo Motor Configuration
Servo myservo;

// Ultrasonic Pin Configuration
const int trigPinL = 7; // Left sensor trigger pin
const int echoPinL = 6; // Left sensor echo pin
const int trigPinC = 9; // Centre sensor trigger pin
const int echoPinC = 10; // Centre sensor echo pin
const int trigPinR = 12; // Right sensor trigger pin
const int echoPinR = 13; // Right sensor echo pin

// Flame Sensor Pin Configuration Left to Right
// Threshold point at 500
const int NUM_SENSORS = 5;
int sensorPins[NUM_SENSORS] = {A1, A2, A3, A4, A5};
int sensorReadings[NUM_SENSORS];

// Motor Driver
const int LeftMotor1 = 2;
const int LeftMotor2 = 3;
const int RightMotor1 = 4;
const int RightMotor2 = 5;

// Relay
const int relay_pump = 8;

// Servo Motor
const int servo_pump = 8;

// Gas Sensor
// Threshold point at = 450
const int gas = A0;

// Buzzer 
const int buzz = 11;

void setup() {
  Serial.begin(9600); // Initialize serial communication

  pinMode(LeftMotor1,OUTPUT);
  pinMode(LeftMotor2,OUTPUT);
  pinMode(RightMotor1,OUTPUT);
  pinMode(RightMotor2,OUTPUT);


// Initialize for Ultrasonic
  pinMode(trigPinL, OUTPUT); // Set trigger pin as output
  pinMode(echoPinL, INPUT); // Set echo pin as input
  pinMode(trigPinC, OUTPUT); // Set trigger pin as output
  pinMode(echoPinC, INPUT); // Set echo pin as input
  pinMode(trigPinR, OUTPUT); // Set trigger pin as output
  pinMode(echoPinR, INPUT); // Set echo pin as input

// Initialize for Flame Sensor
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(sensorPins[i], INPUT);
  }

// Initialize for Relay and Servo
  pinMode(relay_pump,OUTPUT);
  pinMode(servo_pump,OUTPUT);

// Initialize for Gas sensor
  pinMode(gas,INPUT);

// Initialize Servo Motor
  myservo.attach(servo_pump); // attach the servo to pin 12
}


bool hurdleLeft(){

  digitalWrite(trigPinL, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);
  long duration1 = pulseIn(echoPinL, HIGH);
  float distance1 = duration1 * 0.034 / 2;

  if(distance1 <= 15){
    return true;
  }
  return false;
}

bool hurdleCentre(){

  // Second sensor
  digitalWrite(trigPinC, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinC, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinC, LOW);
  long duration2 = pulseIn(echoPinC, HIGH);
  float distance2 = duration2 * 0.034 / 2;

   if(distance2 <= 15){
    return true;
  }
  return false;
}

bool hurdleRight(){

  // Third sensor
  digitalWrite(trigPinR, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);
  long duration3 = pulseIn(echoPinR, HIGH);
  float distance3 = duration3 * 0.034 / 2;

   if(distance3 <= 15){
    return true;
  }
  return false;
}

bool gasDetect(){

  if(analogRead(gas) >= 450){
    return true;
  }
  return false;
}


void servoAndRelayStop(){

  digitalWrite(relay_pump,LOW);
}


void servoAndRelayStart(){

  digitalWrite(relay_pump,HIGH);
  myservo.write(90);
  for (int pos = 90; pos <= 135; pos += 10) { // go from 0 to 180 degrees
    myservo.write(pos); // set the servo position
    delay(100); // wait for the servo to move
  }

  for (int pos = 135; pos >= 45; pos -= 10) { // go from 0 to 180 degrees
    myservo.write(pos); // set the servo position
    delay(100); // wait for the servo to move
  }

  for (int pos = 45; pos <= 90; pos += 10) { // go from 0 to 180 degrees
    myservo.write(pos); // set the servo position
    delay(100); // wait for the servo to move
  }
}

void goFront(){
Serial.print("Forward");
digitalWrite(LeftMotor1,HIGH);
digitalWrite(LeftMotor2,LOW);
digitalWrite(RightMotor1,HIGH);
digitalWrite(RightMotor2,LOW);

}

void goBack(){

digitalWrite(LeftMotor1,LOW);
digitalWrite(LeftMotor2,HIGH);
digitalWrite(RightMotor1,LOW);
digitalWrite(RightMotor2,HIGH);

}

void goStop(){

digitalWrite(LeftMotor1,HIGH);
digitalWrite(LeftMotor2,HIGH);
digitalWrite(RightMotor1,HIGH);
digitalWrite(RightMotor2,HIGH);

}

void goLeft(){

digitalWrite(LeftMotor1,HIGH);
digitalWrite(LeftMotor2,LOW);
digitalWrite(RightMotor1,LOW);
digitalWrite(RightMotor2,HIGH);

}

void goRight(){

digitalWrite(LeftMotor1,LOW);
digitalWrite(LeftMotor2,HIGH);
digitalWrite(RightMotor1,HIGH);
digitalWrite(RightMotor2,LOW);

}
 
void fireAndDistance(){

  bool distanceL = hurdleLeft();
  bool distanceC = hurdleCentre();
  bool distanceR = hurdleRight();
  bool gas = gasDetect();

  Serial.print(distanceL);
   Serial.print(" ");
   Serial.print(distanceC);
   Serial.print(" ");
   Serial.print(distanceR);
   Serial.print(" ");
   Serial.print(gas);
   Serial.println(" ");

  servoAndRelayStop();

 for (int i = 0; i < NUM_SENSORS; i++) {
    sensorReadings[i] = analogRead(sensorPins[i]);
  }

  if(distanceL || distanceC || distanceR){

    if(distanceL){
      goStop();
      goRight();
    }

    if(distanceC){
      goStop();
      goBack();
      goRight();
    }

    if(distanceR){
      goStop();
      goLeft();
    }

     if(distanceL && distanceC){
      goStop();
      goRight();
    }

    if(distanceR && distanceC){
      goStop();
      goLeft();
    }

    if(distanceR && distanceL){
      goStop();
      goFront();
    }

    if(distanceR && distanceL && distanceC){
      goStop();
      goBack();
    }
  }
    
   else if(sensorReadings[0] >= 500 || sensorReadings[1] >= 500 || sensorReadings[2] >= 500 || sensorReadings[3] >= 500 || sensorReadings[4] >= 500 || gas){
    Serial.print("Entering Flame");
      digitalWrite(buzz,HIGH);
      delay(300);
      digitalWrite(buzz,LOW);
      delay(300);
      goStop();
      servoAndRelayStart();
  
    }
    else{
    goFront();
    servoAndRelayStop();
  }

}

void loop() {

  fireAndDistance();
 
}

