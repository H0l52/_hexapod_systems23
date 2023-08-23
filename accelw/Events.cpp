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

  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("Stage1");
    legs[0].o_xy.setAcceleration(300.0);
    legs[0].o_xy.setMaxSpeed(300.0);
    legs[0].o_xy.move(1000);

    legs[0].o_z.setAcceleration(500);
    legs[0].o_z.setMaxSpeed(500);
    legs[0].o_z.move(-1000);
    this->timeout = 10000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage2");
    this->timeout = 10000;
  }

  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->MidStage(0) || this->MidStage(1)) {
    this->timeout -= 1;
    if (this->timeout == 0) Logging::Info("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  if (this->stage == 2) {this->stage = 0; this->timeout=0;}
  this->End(2);
  return 0;
}

int StopEvent::Proc(Leg *legs, vector2D position){
  return 0;
}