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

const int Speed = 100;

// The code to directly raise a leg.
inline void Raise(Leg *l, int index) {
  bool RHS = index < 3; // Is it the Right hand side?
  int moveAmount = RHS ? 100 : 80;  // Are we moving foward in relation to the motor or back - this has to be done due to the servos being on flipped sides.

  

  int servoAmount = RHS ? 40 : 140; // Set to high up - same as above
  int servoAmoun2 = RHS ? 40 : 140;

  // Write those values
  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);

  l->o_xy.write(moveAmount);
}

// The drop code, for dropping a leg.
inline void Drop(Leg *l, int index) {

  bool RHS = index < 3; // Is it the right hand side

  // write the standing values.
  int servoAmount = RHS ? 70 : 110;
  int servoAmoun2 = RHS ? 80 : 100;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);
}

// Are we returning?
inline void Return(Leg *l, int index) {
  int moveAmount = 90; // set back to 90.

  l->o_xy.write(moveAmount);
}
  
int WalkEvent::Proc(Leg *legs, vector2D position) {
  // Right front - 2
  // Right middle - 1
  // Right back - 0
  // Left front = 3
  // Left middle = 4
  // Left back = 5

  // The time coeff defines how long there should be of clockcycles, before we move on.
  // This isn't in seconds or millis due to the nature of the high intensity workload.
  unsigned long timeCoeff = 6000;
  unsigned long LargeTimeCoeff = timeCoeff;

  // Is this the first time we've touched this leg?
  static bool firstPass[] = {false, false, false, false, false, false};

  
  // Check if we should end the event.
  if(this->stage == -1) return 0;


  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("DEBUG STAGE");

    for (int i = 0; i < 6; i++) {
      // Set all the legs to the stand position.
      Drop(&legs[i],i);
    }

    this->timeout = 5000;// * 1000000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage 1");
    this->timeout = LargeTimeCoeff;

    // If we have already raised the legs, drop them
    if (firstPass[3]) Drop(&legs[3],3);
    if (firstPass[5]) Drop(&legs[5],5);
    if (firstPass[1]) Drop(&legs[1],1);
    delay(400);

    // Raise the new legs
    Raise(&legs[2],2);
    Raise(&legs[0],0);
    Raise(&legs[4],4);

    delay(400);

    // If we just dropped those legs, lets move them back.
    if (firstPass[3]) Return(&legs[3],3);
    if (firstPass[5]) Return(&legs[5],5);
    if (firstPass[1]) Return(&legs[1],1);

    
    // Confirm this was the first time we touched those legs.
    firstPass[1] = true;
    firstPass[3] = true;
    firstPass[5] = true;
  }

  if (this->Stage(2)) {
    Logging::Info("Stage 2");
    this->timeout = timeCoeff;


    //Drop the currently raised legs
    Drop(&legs[2],2);
    Drop(&legs[0],0);
    Drop(&legs[4],4);
    delay(400);

    //Raise the new legs
    Raise(&legs[3],3);
    Raise(&legs[5],5);
    Raise(&legs[1],1);

    delay (400);

    // Push the legs back
    Return(&legs[2],2);
    Return(&legs[4],4);
    Return(&legs[0],0);

    
    
  }






  
  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->timeout != 0) {
    this->timeout -= 1;
    if (this->timeout == 0) Logging::Ard("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;

  // If stage 3 is hit, we shoud loop back to stage one. Comment this out if this is unintended.
  if (this->stage == 3) {this->stage = 1; this->timeout=0;}
  // end the stage on stage 3.
  this->End(3);
  return 0;
}


// DEPRICATED.
int SyncEvent::Proc(Leg *legs, vector2D position){

  /*for (int i = 0; i< 6; i++) {
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
  return 0;*/
}


// DEPRICATED.
int TestEvent::Proc(Leg *legs, vector2D position) {
  if (this->Stage(0)) {
    for (int i =0; i<6; i++) {
      legs[i].o_z.write((i<3)?0:160);
      legs[i].h_z.write((i<3)?180:0);
      // legs[i].o_xy.setAcceleration(100);
      // legs[i].o_xy.setMaxSpeed(100);
      // legs[i].o_xy.setSpeed(100);
      
      //legs[i].o_xy.moveTo((i<3) ? -100: 100);
    }
    this->timeout = 10000;
  }
  if (this->Stage(1)) {
    for (int i =0; i<6; i++) {
      //legs[i].o_xy.moveTo((i<3) ? 100: -100);
      legs[i].o_z.write(90);
      legs[i].h_z.write(90);
      //legs[i].h_z.write((i<3)?180:0);
    }
    this->timeout = 10000;
  }

  if (this->MidStage(0) || this->MidStage(1) ) this->timeout -=1;
  if (this->StageWait()) return 0;
  if (this->stage == 2) {this->stage = 0; this->timeout = 0;}
  this->End(2);
  return 0;
}
