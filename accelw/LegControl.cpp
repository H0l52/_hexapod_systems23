#include "LegControl.h"


/// Initialises all the motors,
/// and returns the legcontrol class reference.
LegControl::LegControl(int *i) {
  
  for (uint8_t e = 0; e < 6; e++) {
    uint8_t index = e * 12;
    AccelStepper xy(AccelStepper::FULL4WIRE, i[index++], i[index++], i[index++], i[index++], true);
    AccelStepper z(AccelStepper::FULL4WIRE, i[index++], i[index++], i[index++], i[index++], true);
    AccelStepper k(AccelStepper::FULL4WIRE, i[index++], i[index++], i[index++], i[index++], true);

    Leg l;
    l.o_xy = &xy;
    l.o_z = &z;
    l.h_z = &k;

    this->legs[this->leg_c++] = l;
  }

  this->currentPosition = vector2D(0,0);

}



int LegControl::AttemptEventProc() {
  if (this->currentEvent == NULL) return 0;
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
  if(x != 0) {
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
    Logging::Ard("EVENT OVERFLOW. DROPPING EVENT."); return;
  }

  /// If override is set, force the event in, and clear memory of the list.
  if (override || (this->eventListSize == 0 && this->currentEvent == NULL)) {
    this->currentEvent = ev; 
    memset(this->eventList, 0, this->MAX_EVENTLIST_SIZE);
    return;
  }
  
  this->eventList[this->eventListSize++] = ev;
}