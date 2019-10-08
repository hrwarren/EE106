#include <Arduino.h>
#include <L293D.h>

L293D rmotor(11,9,10); // Right motor when facing same direction as the car
L293D lmotor(3,5,6); // Left

void setup() {

}

void goForward() {
  rmotor.set(0.75);
  lmotor.set(0.75);
}

void goBack() {
  rmotor.set(-0.75);
  lmotor.set(-0.75);
}

void goLeft() { // make it a rotation later
  rmotor.set(0.75);
  lmotor.set(0.10);
}

void goRight (){
  rmotor.set(0.10);
  lmotor.set(0.75);
}

void loop() {
  // Set motor speed and direction with a double value between -1 and 1
  
  goForward();
  delay(1000);
  goRight();
  delay(1000);
  goForward();
  delay(1000);
  goLeft();
  delay(1000);
  goBack();
  delay(1000);
  


  
  
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