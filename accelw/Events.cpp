#include "Events.h"

Event::Event() :
  Event(0) {}

Event::Event(byte id) {
  this->identifier = id;
  this->stage = 0;
  this->timeout = 0;
}


int WalkEvent::Proc(Leg *legs, vector2D position) {
  
  if(this->stage == -1) return 0;
  //Serial.println(this->timeout);

  int accel_ar = 50;
  int max_ar = 50;
  int accel_br = 10;
  int max_br = 10;
  int steps_ar = 1000;
  int steps_br = 40;

  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("Stage1");

    for (int i = 0; i<3; i++) {
      if (i == 1) continue;
      legs[i].o_z.setAcceleration(accel_ar);
      legs[i].o_z.setMaxSpeed(max_ar);
      legs[i].o_z.move(-steps_ar);

      legs[i].h_z.setAcceleration(accel_br);
      legs[i].h_z.setMaxSpeed(max_br);
      legs[i].h_z.move(-steps_br);
    }

    for (int i = 3; i<6; i++) {
      if ( i != 4) continue;
      legs[i].o_z.setAcceleration(accel_ar);
      legs[i].o_z.setMaxSpeed(max_ar);
      legs[i].o_z.move(steps_ar);

      legs[i].h_z.setAcceleration(accel_br);
      legs[i].h_z.setMaxSpeed(max_br);
      legs[i].h_z.move(steps_br);
    }
    

    this->timeout = 100000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage2");

    for (int i = 0; i<3; i++) {
      // legs[i].o_z.setAcceleration(accel_ar);
      // legs[i].o_z.setMaxSpeed(max_ar);
      // legs[i].o_z.move(-400);
      
      legs[i].h_z.setAcceleration(accel_br);
      legs[i].h_z.setMaxSpeed(max_br);
      legs[i].h_z.move(-100);
    }

    for (int i = 3; i<6; i++) {
      // legs[i].o_z.setAcceleration(accel_ar);
      // legs[i].o_z.setMaxSpeed(max_ar);
      // legs[i].o_z.move(400);

      legs[i].h_z.setAcceleration(accel_br);
      legs[i].h_z.setMaxSpeed(max_br);
      legs[i].h_z.move(100);
    }
    // for (int i = 0; i<0; i++) {
    //   legs[i].o_z.setAcceleration(accel_ar);
    //   legs[i].o_z.setMaxSpeed(max_ar);
    //   legs[i].o_z.move(steps_ar);

    //   legs[i].h_z.setAcceleration(accel_br);
    //   legs[i].h_z.setMaxSpeed(max_br);
    //   legs[i].h_z.move(steps_br);
    // }

    // for (int i = 3; i<3; i++) {
    //   legs[i].o_z.setAcceleration(accel_ar);
    //   legs[i].o_z.setMaxSpeed(max_ar);
    //   legs[i].o_z.move(-steps_ar);

    //   legs[i].h_z.setAcceleration(accel_br);
    //   legs[i].h_z.setMaxSpeed(max_br);
    //   legs[i].h_z.move(-steps_br);
    // }

    this->timeout = 10000;
  }

  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->MidStage(0) || this->MidStage(1)) {
    this->timeout -= 1;
    if (this->timeout == 0) Logging::Info("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  //if (this->stage == 2) {this->stage = 0; this->timeout=0;}
  this->End(2);
  return 0;
}

int StopEvent::Proc(Leg *legs, vector2D position){
  return 0;
}