#include "Events.h"

// Base initializer 
Event::Event() :
  Event(0) {}

// Main initializer - creates and id based stage 0 event.
Event::Event(byte id) {
  this->identifier = id;
  this->stage = 0;
  this->timeout = 0;
}

// this is from the event header file - but for some reason it wont compile when its in there
// so i moved it here.
int Logging::msgCount = 0;


/*
  ------- RAISE ------

  legs[1].h_z.write(70);
  legs[1].o_z.write(70);

  legs[1].o_xy.setSpeed(400);
  legs[1].o_xy.setMaxSpeed(400);
  legs[1].o_xy.setAcceleration(400);
  legs[1].o_xy.moveTo(-300);

  ------- DROP ---------
  legs[1].h_z.write(90);
  legs[1].o_z.write(90);

  ------- RETURN BACK ------
  legs[1].o_xy.setSpeed(400);
  legs[1].o_xy.setMaxSpeed(400);
  legs[1].o_xy.setAcceleration(400);
  legs[1].o_xy.moveTo(300);
*/

inline void Raise(Leg *l, int index) {
  int moveAmount = 100; 

  bool RHS = index < 3;

  int directionCoeff = RHS ? -1 : 1;
  moveAmount *= directionCoeff;

  int servoAmount = RHS ? 40 : 140;
  int servoAmoun2 = RHS ? 40 : 140;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);

  l->o_xy.moveTo(moveAmount);
}

inline void Drop(Leg *l, int index, bool move =true) {
  int moveAmount = 100;
  bool RHS = index < 3;
  int directionCoeff = RHS ? 1 : -1;
  moveAmount *= directionCoeff;

  int servoAmount = RHS ? 70 : 120;
  int servoAmoun2 = RHS ? 80 : 110;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);

  if (move) l->o_xy.moveTo(moveAmount);

  if (move) delay(500);
}

int WalkEvent::Proc(Leg *legs, vector2D position) {

  // Right front - 2
  // Right middle - 1
  // Right back - 0
  // Left front = 3
  // Left middle = 4
  // Left back = 5

  unsigned long timeCoeff = 30000;
  unsigned long LargeTimeCoeff = timeCoeff;

  static bool firstPass[] = {false, false, false, false, false, false};

  int Speed = 600;  

  if(this->stage == -1) return 0;
  //Serial.println(this->timeout);


  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("DEBUG STAGE");

    for (int i = 0; i < 6; i++) {
      legs[i].o_xy.setMaxSpeed(Speed);
      legs[i].o_xy.setSpeed(Speed);
      legs[i].o_xy.setAcceleration(Speed);
      legs[i].o_xy.moveTo(100);

      Drop(&legs[i],i);
    }

    this->timeout = 50000;// * 1000000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage 1");
    this->timeout = LargeTimeCoeff;
    
    if (firstPass[3]) Drop(&legs[3],3);
    Raise(&legs[2],2);
    firstPass[3] = true;
  }

  if (this->Stage(2)) {
    Logging::Info("Stage 2");
    this->timeout = timeCoeff;

    if (firstPass[1]) Drop(&legs[1],1);
    Raise(&legs[4],4);
    firstPass[1] = true;
  }

  if (this->Stage(3)) {
    Logging::Info("Stage 3");
    this->timeout = LargeTimeCoeff;

    if (firstPass[5]) Drop(&legs[5],5);
    Raise(&legs[0],0);
    firstPass[5] = true;
  }

  if (this->Stage(4)) {
    Logging::Info("Stage 4");
    this->timeout = timeCoeff;

    Drop(&legs[2],2);
    Raise(&legs[3],3);
    firstPass[2] = true;
  }

  if (this->Stage(5)) {
    Logging::Info("Stage 5");
    this->timeout = LargeTimeCoeff;

    Drop(&legs[4],4);  
    Raise(&legs[1],1);
    firstPass[4] = true;
  }

  if (this->Stage(6)) {
    Logging::Info("Stage 6");
    this->timeout = timeCoeff;
    
    Drop(&legs[0],0);
    Raise(&legs[5],5);
    firstPass[0] = true;
  }





  
  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->timeout != 0) {
    this->timeout -= 1;
    //if(this->timeout % 1000 == 0 && this->stage != 0) Serial.println(firstPass);
    //if(this->timeout % 100 == 0 && this->stage != 0) Serial.println(this->timeout);
    //Logging::Info(this->timeout);
    if (this->timeout == 0) Logging::Info("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  if (this->stage == 7) {this->stage = 1; this->timeout=0;}
  this->End(7);
  return 0;
}

int SyncEvent::Proc(Leg *legs, vector2D position){

  for (int i = 0; i< 6; i++) {
    if (this->Stage(i)) {
      int hval = (i < 3 ) ? 140 : 70;
      int speed = 900;
      legs[i].o_xy.setMaxSpeed(speed);
      legs[i].o_xy.setSpeed(speed);
      legs[i].o_xy.setAcceleration(speed);
      
      legs[i].o_xy.moveTo(200);

      legs[i].h_z.write(hval);
      legs[i].o_z.write(hval);

      for (int e = 0; e < 6; e++) {
        if (e == i) break;
        legs[e].h_z.write(90);
        legs[e].o_z.write(90);
      }

      this->timeout = 100000;
    }
    if (this->MidStage(i)) this->timeout -=1;
  }

  if (this->StageWait()) return 0;
  this->End(6);
  return 0;
}