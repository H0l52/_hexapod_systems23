#include "commandInterpreter.h"

#include <Stepper.h>
#include <Arduino.h>


commandInterpreter::commandInterpreter(int i) {
  this->stepsPerRevolution = i;
}

void commandInterpreter::addLeg(int *i) {
  int step = this->stepsPerRevolution;

  Stepper s1 = Stepper(step, i[0], i[1], i[2], i[3]);
  Stepper s2 = Stepper(step, i[4], i[5], i[6], i[7]);
  Stepper s3 = Stepper(step, i[8], i[9], i[10], i[11]);

  Leg l;
  
  l.s_o = &s1;
  l.s_m = &s2;
  l.s_z = &s3;

  l.a_m = 0;
  l.a_o = 0;
  l.a_z = 0;

  this->legs[this->leg_c++] = l;
}

void commandInterpreter::WalkCycle(int leg) {
  Stepper * s = this->legs[leg].s_m;
  s->setSpeed(1);
  s->step(-(this->stepsPerRevolution/4));
  //s->step(this->stepsPerRevolution/4);
}

void commandInterpreter::procStep() {
  this->currentlyManaged.stage_c ++;
  if (this->currentlyManaged.l == na && this->currentlyManaged.fallback != na_b) {
    switch(this->currentlyManaged.fallback) {
      default:
      case walk:
        this->WalkCycle(0);
      break;
    }
  }

}