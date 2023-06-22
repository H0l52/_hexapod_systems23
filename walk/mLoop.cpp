#include "mLoop.h"

mLoop::mLoop(commandInterpreter* ci) {
  this->ci = ci;
}

void mLoop::UpdateT() {
  if (this->last_interval + mLoop::interval_delay < millis()) {
    this->last_interval = millis();

    String proc = readAndKill(0);
    Serial.println(proc);
    if (proc=="w") {
      event e;
      e.l = na;
      e.fallback = walk;
      e.stage_c = 0;
      e.stage = 0;
      this->ci->currentlyManaged = e;
    }
    //do something here if readandkill is something

    this->ci->procStep();
  } 
}

void mLoop::PushUpdate(String* s) {
  this->commands[this->lencom] = s;
  this->lencom ++;
}

void mLoop::KillUpdate(byte index) {
  memmove(&this->commands[index], &this->commands[index+1], this->lencom - index - 1);
  this->lencom--;
}

String mLoop::readAndKill(byte index) {
  if (index > this->lencom) return "";

  String out = *(this->commands[index]);
  this->KillUpdate(index);
  return out;
}