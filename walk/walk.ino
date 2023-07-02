#include "stepper.h"
#include "commandInterpreter.h"
#include "mLoop.h"

const int stepsPerRevolution = 2038;

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
