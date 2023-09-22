// The necessary includes.
#include "LegControl.h"
#include "MCP23008.h"

LegControl::LegControl(int *i) {
  mlist = i;
  for (uint8_t e = 0; e < 6; e++) {
    uint8_t index = e * 6;

    AccelStepper xy(AccelStepper::FULL4WIRE, mlist[index], mlist[index+2], mlist[index+1], mlist[index+3], true);
    Servo z;
    z.attach(index+4);
    z.write(90);
    Servo k;
    k.attach(index+5);
    k.write(90);

    Leg l;
    l.o_xy = xy;
    l.o_z = z;
    l.h_z = k;

    this->legs[this->leg_c++] = l;
  }
}

/// Initialises all the motors,
/// and returns the legcontrol class reference.
void LegControl::setup(MCP23008* mp,MCP23008* mp2,MCP23008* mp3) {

  this->currentPosition = vector2D(0, 0);
  this->legs[0].o_xy.mpcont = mp;
  this->legs[1].o_xy.mpcont = mp;
  this->legs[2].o_xy.mpcont = mp2;
  this->legs[3].o_xy.mpcont = mp2;
  this->legs[4].o_xy.mpcont = mp3;
  this->legs[5].o_xy.mpcont = mp3;
  for (int i = 0; i < 6; i++) {
    this->legs[i].o_xy.enableOutputs();
  }
  // this->legs[0].o_z.mpcont = mp;
  // this->legs[0].o_z.enableOutputs();

  // this->legs[0].h_z.mpcont = mp;
  // this->legs[0].h_z.enableOutputs();
}



int LegControl::AttemptEventProc() {
  if (this->currentEvent == NULL) return 0;
  Event::runMotors(this->legs);
  return this->currentEvent->Proc(this->legs, this->currentPosition);
}


/// Processes next events in the leg controller.
/// This should only be run by the internal timer (void loop),
/// and not called by any other function.
void LegControl::procStep() {
  if (this->currentEvent == NULL) return;
  /// If currentEvent should be take over by eventlist.next, set it, and drop the list size.
  if ((*this->currentEvent) <= (*(this->eventList[0]))) {
    if (this->eventListSize == 0) this->currentEvent = NULL;
    else {
      this->currentEvent = this->eventList[0];
      memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
    }
  }

  /// Double event send, assume error due to no override value set.
  // while (this->eventListSize != 0 && ((*this->currentEvent) == (*(this->eventList[0])))) {
  //   memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
  // }

  /// Exit code hit
  int x = AttemptEventProc();
  if (x != 0) {
    Logging::Warning("Event Died");
    if (this->eventListSize == 0) this->currentEvent = NULL;
    else {
      this->currentEvent = this->eventList[0];
      memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
    }
  }
}

/// Submit an event to the leg controller safely,
/// prevents crashes from occuring.
void LegControl::submitEvent(Event *ev, bool override) {
  /// "Safe" memory practices.
  if (this->eventListSize == this->MAX_EVENTLIST_SIZE) {
    Logging::Ard("EVENT OVERFLOW. DROPPING EVENT.");
    return;
  }

  /// If override is set, force the event in, and clear memory of the list.
  if (override || (this->eventListSize == 0 && this->currentEvent == NULL)) {
    this->currentEvent = ev;
    memset(this->eventList, 0, this->MAX_EVENTLIST_SIZE);
    return;
  }

  this->eventList[this->eventListSize++] = ev;
}