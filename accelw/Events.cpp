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
const int Speed = 100;

inline void Raise(Leg *l, int index) {
  return;
  bool RHS = index < 3;
  int moveAmount = RHS ? 100 : 80; 

  

  int servoAmount = RHS ? 40 : 140;
  int servoAmoun2 = RHS ? 40 : 140;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);

  l->o_xy.write(moveAmount);
}

inline void Drop(Leg *l, int index) {
  return;
  bool RHS = index < 3;
  int moveAmount = 90;//600 : 200; //600, 200

  int servoAmount = RHS ? 70 : 110;
  int servoAmoun2 = RHS ? 80 : 100;

  l->h_z.write(servoAmount);
  l->o_z.write(servoAmoun2);
}

inline void Return(Leg *l, int index) {
  bool RHS = index < 3;
  int moveAmount = 90;

  l->o_xy.write(moveAmount);
}
  
int WalkEvent::Proc(Leg *legs, vector2D position) {
  // Right front - 2
  // Right middle - 1
  // Right back - 0
  // Left front = 3
  // Left middle = 4
  // Left back = 5

  unsigned long timeCoeff = 6000;
  unsigned long LargeTimeCoeff = timeCoeff;

  static bool firstPass[] = {false, false, false, false, false, false};

  

  if(this->stage == -1) return 0;
  //Serial.println(this->timeout);


  // One time stage code for motor information... etc.
  if (this->Stage(0)) {
    Logging::Info("DEBUG STAGE");

    for (int i = 0; i < 6; i++) {

      Drop(&legs[i],i);
    }

    this->timeout = 5000;// * 1000000;
  }

  if (this->Stage(1)) {
    Logging::Info("Stage 1");
    this->timeout = LargeTimeCoeff;
    
    if (firstPass[3]) Drop(&legs[3],3);
    if (firstPass[5]) Drop(&legs[5],5);
    if (firstPass[1]) Drop(&legs[1],1);
    delay(400);

    Raise(&legs[2],2);
    Raise(&legs[0],0);
    Raise(&legs[4],4);

    delay(400);

    if (firstPass[3]) Return(&legs[3],3);
    if (firstPass[5]) Return(&legs[5],5);
    if (firstPass[1]) Return(&legs[1],1);

    

    firstPass[1] = true;
    firstPass[3] = true;
    firstPass[5] = true;
  }

  if (this->Stage(2)) {
    Logging::Info("Stage 2");
    this->timeout = timeCoeff;

    Drop(&legs[2],2);
    Drop(&legs[0],0);
    Drop(&legs[4],4);
    delay(400);

    Raise(&legs[3],3);
    Raise(&legs[5],5);
    Raise(&legs[1],1);

    delay (400);
    Return(&legs[2],2);
    Return(&legs[4],4);
    Return(&legs[0],0);

    
    
  }






  
  // Interpolate from infinity down to 0 (when stage wait will call).
  if (this->timeout != 0) {
    this->timeout -= 1;
    //if(this->timeout % 1000 == 0 && this->stage != 0) Serial.println(firstPass);
    //if(this->timeout % 100 == 0 && this->stage != 0) Serial.println(this->timeout);
    //Logging::Info(this->timeout);
    if (this->timeout == 0) Logging::Ard("Timedout");
  }

  // Waits for next stage
  if (this->StageWait()) return 0;
  if (this->stage == 3) {this->stage = 1; this->timeout=0;}
  this->End(3);
  return 0;
}

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