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

  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("Stage1");
    legs[0].o_xy.setAcceleration(100.0);
    legs[0].o_xy.setMaxSpeed(200.0);
    legs[0].o_xy.moveTo(1000);
    this->timeout = 100000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage2");
    this->timeout = 200000;
  }

  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->MidStage(0) || this->MidStage(1)) {
    this->timeout -= 1;
    if (this->timeout == 0) Logging::Info("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  if (this->stage == 2) {this->stage=0; this->timeout=0; Serial.print("loop");}
  this->End(2);
  return 0;
}

int StopEvent::Proc(Leg *legs, vector2D position){
  return 0;
}