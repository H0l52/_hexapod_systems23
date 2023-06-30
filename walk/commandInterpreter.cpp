#include "commandInterpreter.h"

#include "stepper.h"
#include <Arduino.h>


commandInterpreter::commandInterpreter(int i) {
  this->stepsPerRevolution = i;
}




void commandInterpreter::updateMotors() {
  this->legs[0].s_o->updateMotor();
  /*for (int i =0; i < this->leg_c; i++) {
    this->legs[i].s_m->updateMotor();
    this->legs[i].s_o->updateMotor();
    this->legs[i].s_z->updateMotor();
  }*/
}





void commandInterpreter::addLeg(int *i) {

  Stepper s1 = Stepper(this->stepsPerRevolution, i[0], i[1], i[2], i[3]);
  Stepper s2 = Stepper(this->stepsPerRevolution, i[4], i[5], i[6], i[7]);
  Stepper s3 = Stepper(this->stepsPerRevolution, i[8], i[9], i[10], i[11]);

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
  //Serial.println(this->stepsPerRevolution);
  if (this->currentlyManaged.stage == 0) {
    this->currentlyManaged.stage ++;
    this->currentlyManaged.stage_c = 0;
    
    Stepper * s = this->legs[leg].s_o;
    s->setSpeed(4);
    s->step(this->stepsPerRevolution/2);
  }

  if (this->currentlyManaged.stage == 1) {
    this->currentlyManaged.stage_c = this->legs[leg].s_o->readStage();
    if (this->currentlyManaged.stage_c <= 1) {
      this->currentlyManaged.stage = -1;
    }
  }
}




void commandInterpreter::procStep() {
  if (this->currentlyManaged <= this->nextManaged) {

    Serial.println("---- Switching to ----");
    Serial.print("e.l: "); Serial.print(this->nextManaged.l); 
    Serial.print(" e.fallback: "); Serial.println(this->nextManaged.fallback);
    Serial.print("e.stage: "); Serial.print(this->nextManaged.stage);
    Serial.print(" e.stage_c: "); Serial.println(this->nextManaged.stage_c);
    Serial.println("----------------------");

    this->currentlyManaged = this->nextManaged;
    event e;
    e.l = na;
    e.fallback = na_b;
    e.stage_c = 0;
    e.stage = 0;
    this->nextManaged = e;
  }


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