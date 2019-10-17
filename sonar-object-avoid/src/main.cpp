// Willem Hillier
// Rose Warren

#include <Arduino.h>
#include <L293D.h> // motor library
#include <math.h>
#include <Servo.h>

#define SCAN_SAMPLE_DELAY_MS 150

#define NUM_SAMPLES 10

long lastMillis = 0;

//Scan direction, 0->right to left, 1->left to right
bool scanDir = 0;

// Declare servo where sonar is mounted
Servo sonarServo;

// Motor pin assignments
L293D rmotor(11,9,10); // Right motor when facing same way as the car
L293D lmotor(4,5,6); // Left

// Sonar pin
const int pingPin = 7;

void setup() {

  // Start the serial
  Serial.begin(9600);

  // Assigning pins for servo
  sonarServo.attach(3);
  sonarServo.write(0);
 
}

int getDistance() {
  int cm;
  long duration;

  // Assign the sonar pin to output 
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);

  // Wait, then send out a sonar ping
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // Now pingPin becomes input to read the pulse
  pinMode(pingPin, INPUT);

  // Convert time to distance
  duration = pulseIn(pingPin, HIGH);
  cm = duration / 29 / 2;

  // Return the distance
  return cm;
}


void loop() {

  // Start with the motors off
  rmotor.set(0);
  lmotor.set(0);
  

  int distances[NUM_SAMPLES];

  for(int i=0;i<NUM_SAMPLES;i++){

    // choose scan direction
    if(scanDir){
      sonarServo.write(175-i*175/NUM_SAMPLES);
    }else{
      sonarServo.write(i*175/NUM_SAMPLES);
    }
    lastMillis = millis();

    // timer-less delay to wait for servo to get to commanded position
    while(millis() < lastMillis + SCAN_SAMPLE_DELAY_MS){}
    if(scanDir){
      distances[NUM_SAMPLES-i]=getDistance();
    }else{
      distances[i]=getDistance();
    }
  }

  // change the next scan direction
  scanDir = ! scanDir;

  // find direction of minimum distance
  int minDistance = distances[0];
  int minDistanceDir = 0;

  for(int i=1;i<NUM_SAMPLES;i++){
    if(distances[i]>minDistance){
      minDistance=distances[i];
      minDistanceDir = i;
    }
  }

  // Print shortest distance and the direction where it was found
  Serial.println(minDistance);
  Serial.println(minDistanceDir);

  // Calculate direction to go, where 500 is straight ahead, 0 is all the way right, 1000 is all the way left
  long dir = 1000*minDistanceDir/NUM_SAMPLES;

  int lMotorSpeed = min(255,(max(0,map(dir,1000,0,0,255))));
  int rMotorSpeed = min(255,(max(0,map(dir,0,1000,0,255))));

  // Print values  
  Serial.println(dir);
  Serial.println(lMotorSpeed);
  Serial.println(rMotorSpeed);

  // Set the motors to determined speed
  lmotor.set(lMotorSpeed);
  rmotor.set(rMotorSpeed);

  // Pause so the servo, sonar and motors aren't all running at once
  delay(3000);

}