
//Haley Warren


#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Stepper.h>

#define GRAVITY 1 // Gravity (to be subtracted from accel magnitude)
#define BTNPIN 2 // Pin to read the button that's our standin for manual override

const int MPU = 0x68; // I'm actually not sure what this is
const int stepsPerRev = 2048; // Motor's steps per revolution

int btnState = 0; // Initialize the variable reading the button's status
int runningAvg = 0; // Initialize the running average 
int averagedOver = 50; // How many samples the function will take the average of

float mag; // Magnitude of acceleration
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; // Declare vars for processed accel/gyro
float SCALE = 16384; // Dcalar by which to divide raw accel values

// Boolean to remember whether or not the brace has been tightened
bool isTightened = false;

// Boolean to remember if the accelerometer has recently seen movement
bool active = false;

// How long the user has been standing still
unsigned long startTime;
unsigned long stillTime = millis();

// How long since the button has been pressed
unsigned long hitTime;
unsigned long prevHit = millis();
unsigned long diffTime;

// creat an MPU object
MPU6050 accelgyro(0x68); 

// declare variables for raw accel and gyro values
int16_t raw_AcX, raw_AcY, raw_AcZ, raw_GyX, raw_GyY, raw_GyZ;

// create the stepper motor object from this tutorial: https://www.makerguides.com/28byj-48-stepper-motor-arduino-tutorial
Stepper braceStepper = Stepper(stepsPerRev, 8, 10, 9, 11);
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver


void setup(){

  // Start talking to the Serial Data Line and Serial Clock Line
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  // Start serial communication
  Serial.begin(9600);

  // initialize the MPU
  accelgyro.initialize();

  // Set motor speed to 5 rpm
  braceStepper.setSpeed(10);

  // Initialize button pin as input
  pinMode(BTNPIN, INPUT);

}


// Function to get the magnitude of acceleration in all axes
float getAccMag(float AcX, float AcY, float AcZ){
  // Good old Pythagoras
  float magnitude = sqrt((AcX*AcX) + (AcY*AcY) + (AcZ*AcZ));
  return magnitude;
}

void tighten(){ // turn clockwise
    braceStepper.step(stepsPerRev);
    isTightened = true;
    delay(500);
}

void loosen(){ //turn counterclockwise
  braceStepper.step(0-stepsPerRev);
  isTightened = false;
  delay(500);
}

void loop(){
  // Start talking to the MPU
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  

  // The "oh fuck please don't cut off my circulation thanks" mode
  // Or, more professionally, the manual override
  // This is still a work in progress, as implementing it interferes with the overall timekeeping in the system
  
  btnState = digitalRead(BTNPIN);
  if (btnState == HIGH) {
    Serial.println('pressed');
    hitTime = millis();
    diffTime = hitTime - prevHit;
    prevHit = hitTime;
    delay(333);
  }

  if (diffTime < 500) {
    tighten();
  }
  else {
    loosen();
  }

  // Read raw accel and gyro on all axes, pass them by reference
  accelgyro.getMotion6(&raw_AcX, &raw_AcY, &raw_AcZ, &raw_GyX, &raw_GyY, &raw_GyZ);

  // Scale the raw acceleration values to deal with gravity
  AcX = raw_AcX/SCALE;
  AcY = raw_AcY/SCALE;
  AcZ = ((raw_AcZ)/SCALE) - GRAVITY;

  // Get the magnitude of acceleration
  mag = getAccMag(AcX, AcY, AcZ);
  //runningAvg = getAccAvg(runningAvg, avgReading);


  // If the acceleration is of sufficient magnitude and the brace is currently loose
  if (((mag > 0.5) || (mag < 0)) && (isTightened == false)) {
    tighten(); // increase the supportiveness of the brace
    isTightened = true;

  // Debugging and entertainment
    Serial.println("============= \n");
    Serial.println("BLAAAAAAAAAH");
    Serial.println("============= \n");

    active = true; // The user is moving, i.e. active
  }

  // If the acceleration magnitude is low enough and the brace is currently tight
  if (((mag >= 0.09) || (mag <= 0.17)) && (isTightened == true)){
    
    // If the user was just moving, startTime is the current time
    if (active == true){
      startTime = millis();
      active = false; // The user is no longer moving
    }

    // Start counting how long the user has been staying still
    stillTime = millis();
    Serial.println(stillTime-startTime);

    // If the user has been stationary for more than 10 seconds, loosen the brace
    if ((stillTime - startTime) >= 10000){
      loosen(); // Remove some support
      isTightened = false;
      Serial.println("loosened");
    }
  } 
  
  // Print out readings of all accel values and their magnitude
  Serial.println("\nAccelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = "); Serial.print(AcZ); 
  Serial.print(" | mag = "); Serial.print(mag);
  Serial.println();

  // Debugging and entertainment
  if (isTightened == true){
    Serial.print("Yep");
  } else{
    Serial.print("Nope");
  }
  
  // Commented out gyro readings
  // We don't use gyro around here because gyro is for nerds
  // But this is here so we could if we wanted to

  // Serial.print("Gyroscope: ");
  // Serial.print("X = "); Serial.print(GyX);
  // Serial.print(" | Y = "); Serial.print(GyY);
  // Serial.print(" | Z = "); Serial.println(GyZ);
  // Serial.println(" ");

  delay(333);
}


// If the user presses the button once
// Loosen

// If the user presses the button twice in succession
// Tighten