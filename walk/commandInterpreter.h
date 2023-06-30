#ifndef commandInterpreter_h
#define commandInterpreter_h

#include "stepper.h"
#include <Arduino.h>

enum basicEventTarget {
  walk,
  stop,
  na_b
};

enum eventTarget {
  home,
  pathfind,
  idle,
  na
};

struct event {
  eventTarget l;
  basicEventTarget fallback;

  unsigned long stage;
  unsigned long stage_c;
};



struct Leg {
  Stepper* s_o;
  Stepper* s_m;
  Stepper* s_z;
  float a_o;
  float a_m;
  float a_z;
};

class commandInterpreter {
  private:
    const static int MAXSIZE = 6;

    
    byte leg_c = 0;

    int stepsPerRevolution; 
  public:
    Leg legs[MAXSIZE];

    
    commandInterpreter(int i);
    void addLeg(int i[12]);
    void WalkCycle(int leg);
    void procStep();
    event currentlyManaged;
    void updateMotors();
};



#endif