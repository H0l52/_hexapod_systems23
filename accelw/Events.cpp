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
    Logging::Info("Walk");
    this->timeout = 10000;
  }

  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->MidStage(0)) {
    this->timeout -= 1;
    if (this->timeout == 0) Logging::Info("Die");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  this->End(1);
  return 0;
}

int StopEvent::Proc(Leg *legs, vector2D position){
  return 0;
}