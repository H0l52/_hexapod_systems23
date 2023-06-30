#include "stepper.h"
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
  Serial.begin(9600);
  cI.addLeg(new int [12] {8, 10, 9, 11, 0, 0, 0, 0, 0, 0, 0, 0});
}

void loop() {
  cI.updateMotors();
  _mloop.UpdateT();
  if (Serial.available() > 0) {
    char r = Serial.read();
    String icString(r);
    _mloop.PushUpdate(icString);
  }
}
