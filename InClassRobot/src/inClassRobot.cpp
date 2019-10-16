#include <Arduino.h>
#include <L293D.h> // motor library
#include <math.h>
#include <Servo.h>


Servo sonarServo;

L293D rmotor(11,9,10); // Right motor when facing same direction as the car
L293D lmotor(4,5,6); // Left

const int pingPin = 7; //we may change this later

// #define boundary 520;
// #define randTurnBase 120
// #define randTurnCeiling





void setup() {
  Serial.begin(9600);
  sonarServo.attach(3);
  sonarServo.write(0);

 // Assigning pins for sonar

}

int getDistance() {
  int cm;
  long duration;


  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // Now pingPin becomes input to read the pulse
  pinMode(pingPin, INPUT);

  // Convert time to distance
  duration = pulseIn(pingPin, HIGH);
  cm = duration / 29 / 2;

  // Print distance read
  // Serial.println(cm);

  return cm;


}

void goForward() {
  rmotor.set(191); //255 * 0.75
  lmotor.set(191);
}

void goBack() {
  rmotor.set(-255);
  lmotor.set(-255);
}

void goLeft() { // make it a rotation later
  rmotor.set(191);
  lmotor.set(25);
}

void goRight (){
  rmotor.set(0.10);
  lmotor.set(0.75);
}

// void randomTurn(){
//   long randNum1 = random(1,10);

//   long randNum2 = random(rand_turn_base)

// }

void loop() {
  int distances[20];
  for(int i=0;i<20;i++){
    sonarServo.write(9*i);
    delay(100);
    distances[i]=getDistance();
  }
  sonarServo.write(0);

  // find direction of minimum distance
  int minDistance = distances[0];
  int minDistanceDir = 0;

  for(int i=1;i<20;i++){
    if(distances[i]>minDistance){
      minDistance=distances[i];
      minDistanceDir = i;
    }
  }

  Serial.println(minDistance);
  Serial.println(minDistanceDir);

  delay(200);
  // Set motor speed and direction with a double value between -1 and 1
  
  //if object detected within X distance
    //
  


  
  
  // // Use motor.get() to get current speed and then update it from that
  // rmotor.set(-255);
  // lmotor.set(-255);

  // while(rmotor.get() != 255 || lmotor.get() != 255)
  // {
  //   rmotor.set(rmotor.get() + 5); // Increment motor speed from previous speed
  //   delay(100); // Delay 100ms to see the speed changing
  //   lmotor.set(lmotor.get() + 5);
  //   delay(100);
  // }
}

//testing to see if github sees this change