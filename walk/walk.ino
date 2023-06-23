#include <Stepper.h>
#include "commandInterpreter.h"
#include "mLoop.h"

const int stepsPerRevolution = 2038;
// Rotate CW slowly at 5 RPM
// myStepper.setSpeed(1);
// myStepper.step(stepsPerRevolution/4);
// delay(500);

// // Rotate CCW quickly at 10 RPM
// myStepper.setSpeed(1);
// myStepper.step(-stepsPerRevolution/4);
// delay(500);



commandInterpreter cI(stepsPerRevolution);
mLoop _mloop(&cI);

void setup() {
  cI.addLeg(new int [12] {8, 10, 9, 11, 0, 0, 0, 0, 0, 0, 0, 0});
  Serial.begin(9600);
}

void loop() {
  _mloop.UpdateT();
  if (Serial.available() > 0) {
    String icString = Serial.readString();
    _mloop.PushUpdate(icString);
  }
}
