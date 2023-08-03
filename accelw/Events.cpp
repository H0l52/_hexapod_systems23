#include "Events.h"

Event::Event() :
  Event(0) {}

Event::Event(byte id) {
  this->identifier = id;
  this->stage = 0;
  this->timeout = 0;
}

int WalkEvent::Proc(Leg *legs, vector2D position) {
  
}