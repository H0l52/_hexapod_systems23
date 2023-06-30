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

  bool ovrd = false;

  inline bool operator==(const event& other) const
  {
    return 
      (
        this->l == other.l &&
        this->fallback == other.fallback &&
        this->stage != -1 &&
        !other.ovrd
      );
  }

  inline bool operator!=(const event& other) const
  {
    return !(*this == other);
  }

  inline bool operator<=(const event& other) const {
    bool ob = !(
      (*this == other) ||
      (other.fallback == na_b &&
      other.l == na));
    return ob;
  }
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
    event currentlyManaged;
    int stepsPerRevolution; 
  public:
    Leg legs[MAXSIZE];


    commandInterpreter(int i);
    void addLeg(int i[12]);
    void WalkCycle(int leg);
    void procStep();
    
    event nextManaged;
    void updateMotors();
};



#endif