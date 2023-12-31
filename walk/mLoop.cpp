#include "mLoop.h"

mLoop::mLoop(commandInterpreter* ci) {
  this->ci = ci;
}

void mLoop::UpdateT() {
  if (this->last_interval + mLoop::interval_delay < micros()) {
    this->last_interval = micros();
    this->ci->procStep();


    String proc = readAndKill(0);
    if (proc != "") {
      if (proc.startsWith("w")) {
        event e;
        e.l = na;
        e.fallback = walk;
        e.stage_c = 0;
        e.stage = 0;
        this->ci->nextManaged = e;
      }
      if (proc.startsWith("e")) {
        event e;
        e.l = na;
        e.fallback = walk;
        e.stage_c = 0;
        e.stage = 0;
        e.ovrd = true;
        this->ci->nextManaged = e;
      }
    }
  }
}

void mLoop::PushUpdate(String s) {
  this->commands[this->lencom] = s;
  this->lencom ++;
}

void mLoop::KillUpdate(byte index) {
  memmove(&this->commands[index], &this->commands[index+1], this->lencom - index - 1);
  this->lencom--;
}

String mLoop::readAndKill(byte index) {
  if (this->lencom == 0) return "";
  if (index > this->lencom -1) return "";

  String out = (this->commands[index]);
  this->KillUpdate(index);
  return out;
}