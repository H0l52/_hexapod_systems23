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

inline void SetUpMotors(Leg *l) {
  int Speed = 400; 
  for (int i = 0; i < 6; i++) {
    l[i].o_xy.setSpeed(Speed);
    l[i].o_xy.setMaxSpeed(Speed);
    l[i].o_xy.setAcceleration(Speed);
  }
}

inline void Raise(Leg *l, int index) {
  int moveAmount = 100; 

  bool RHS = index < 3;

  int directionCoeff = RHS ? -1 : 1;
  moveAmount *= directionCoeff;
  int servoAmount = RHS ? 70 : 110;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmount);

  l->o_xy.moveTo(moveAmount);
}

inline void Drop(Leg *l) {
  l->h_z.write(90);
  l->o_z.write(90);
}

inline void ReturnBack(Leg *l, int index) {
  int moveAmount = 100;
  bool RHS = index < 3;
  int directionCoeff = RHS ? 1 : -1;
  moveAmount *= directionCoeff;
  l->o_xy.moveTo(moveAmount);
}

int WalkEvent::Proc(Leg *legs, vector2D position) {

  // Right front - 2
  // Right middle - 1
  // Right back - 0
  // Left front = 3
  // Left middle = 4
  // Left back = 5

  unsigned long timeCoeff = 10000 / 2;
  
  int moveAmount = 100;
  int Speed = 400;  

  if(this->stage == -1) return 0;
  //Serial.println(this->timeout);


  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("DEBUG STAGE");

    SetUpMotors(legs);

    legs[1].h_z.write(70);
    legs[1].o_z.write(70);
    legs[1].o_xy.moveTo(-moveAmount);

    this->timeout = 100000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage 1");
    // RF back
    // RM mid move
    // RB return back
    // LF drop
    // LM back
    // LB raise

    ReturnBack(&legs[0],0);
    Drop(&legs[3]);
    Raise(&legs[5],5);

    this->timeout = timeCoeff;
  }

  if (this->Stage(2)) {
    Logging::Info("Stage 2");

    // RF raise
    // RM drop
    // RB back
    // LF Return back
    // LM back
    // LB move

    Raise(&legs[2],2);
    Drop(&legs[1]);
    ReturnBack(&legs[3],3);


    this->timeout = timeCoeff;
  }

  if (this->Stage(3)) {
    Logging::Info("Stage 3");

    // RM back
    // LM raise
    // LB drop

    ReturnBack(&legs[1],1);
    Raise(&legs[4],4);
    Drop(&legs[5]);

    this->timeout = timeCoeff;
  }

  if (this->Stage(4)) {
    Logging::Info("Stage 4");

    // RM back
    // LM move

    // RF drop
    // RB Raise
    // LB return back

    Drop(&legs[2]);
    Raise(&legs[0],0);
    ReturnBack(&legs[5],5);
    

    this->timeout = timeCoeff;
  }

  if (this->Stage(5)) {
    Logging::Info("Stage 5");

    // RM back
    // LM drop

    // RF return back
    // LF raise

    ReturnBack(&legs[2],2);
    Drop(&legs[4]);
    Raise(&legs[3],3);

    this->timeout = timeCoeff;
  }

  if (this->Stage(6)) {
    Logging::Info("Stage 6");
    
    // RM Raise
    // LM back
    Raise(&legs[1],1);
    ReturnBack(&legs[4],4);

    // RB drop
    Drop(&legs[0]);

    this->timeout = timeCoeff;
  }





  
  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->timeout != 0) {
    this->timeout -= 1;
    //if(this->timeout % 1000 == 0 ) Serial.println(this->timeout);
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
      int hval = (i < 3 ) ? 180 : 0;
      legs[i].o_xy.setAcceleration(300);
      legs[i].o_xy.setMaxSpeed(300);
      legs[i].o_xy.move(100);

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