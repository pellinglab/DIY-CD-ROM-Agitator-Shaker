/* 

  DIY BENCH TOP AGITATOR/SHAKER
  BY: Andrew Pelling, MAY 2015.
      http://www.pellinglab.net
      
  - Running on Arduino Pro Mini (5V/16MHz) and wired to an old CD ROM drive. 
  - Motor control with the TI SN754410NE driver chip.

*/

#include <Stepper.h>
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8,9,10,11);   
int speedRead; // Motor speed, controlled by pot on A0
int ampRead; // Motor steps, controlled by pot on A1

int shakerOn = 0; // 0 = Shaker Off; 1 = Shaker On
const int stopPin = 3;       // the Start pushbutton pin
const int startPin = 2;      // the Stop pushbutton pin
int stopButtonState = HIGH;  // the current reading from the input pin
int startButtonState = LOW;  // the current reading from the input pin
int state = LOW;         

void setup() {
  pinMode(stopPin, INPUT);
  pinMode(startPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  buttonCheck();  // Check Start/Stop Buttons
  if (state == HIGH) {
    for (int i = 0; i<2; i++) {
      if (i == 0) {
        potCheck();  // Check shaker settings
        myStepper.step(ampRead);
      } else {
        potCheck();  // Check shaker settings
        myStepper.step(-ampRead);
      }
    }
  } else if (state == LOW) {
    myStepper.setSpeed(0); 
    myStepper.step(0);
  }
}

void buttonCheck() {
  stopButtonState = digitalRead(stopPin);  // Check state of Stop button
  startButtonState = digitalRead(startPin);  // Check state of Start button
  if (stopButtonState == HIGH && startButtonState == LOW && shakerOn == 1) {     // Stopping Routine
    for (int i=1; i<5; i++) { // To avoid an abrupt stop, the speed is sequentially decreased 
        myStepper.setSpeed(speedRead/i);
        myStepper.step(ampRead);
        myStepper.step(-ampRead); 
    }
    state = LOW; // Shaker Off 
    shakerOn = 0; // Shaker Off
  } else if (stopButtonState == LOW && startButtonState == HIGH && shakerOn == 0) {      // Startup Routine
    myStepper.setSpeed(40);  // Initial speed
    myStepper.step(-205);  // Move backwards by more than 1 revolution in order to find the start position
    myStepper.step(100); // Move to middle position of the full range of motion
    state = HIGH;  // Shaker On
    shakerOn = 1;
  }
}

void potCheck() {
  // A0 = Frequency Pot and A1 = Amplitude Pot
  speedRead = map(analogRead(0), 1023, 0, 30, 140);
  ampRead = map(analogRead(1), 1023, 0, 100, 25);
  myStepper.setSpeed(speedRead);
}
