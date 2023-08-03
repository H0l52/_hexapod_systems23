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

}

/// Processes next events in the leg controller. 
/// This should only be run by the internal timer (void loop),
/// and not called by any other function.
void LegControl::procStep() {

  /// Double event send, assume error due to no override value set.
  while (this->currentEvent == this->eventList[0]) {
    memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
  }

  /// If currentEvent should be take over by eventlist.next, set it, and drop the list size.
  if (this->currentEvent <= this->eventList[0]) {
    this->currentEvent = this->eventList[0];
    memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
  }

  /// Exit code hit
  int x = this->currentEvent.Proc(this->legs, this->currentPosition);
  if(x != 0) {
    Logging::Warning("Event Died - Status: " + x);
    this->currentEvent = this->eventList[0];
    memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
  }
  
}


/// Submit an event to the leg controller safely,
/// prevents crashes from occuring.
void LegControl::submitEvent(Event ev, bool override) {
  /// "Safe" memory practices.
  if (this->eventListSize == this->MAX_EVENTLIST_SIZE) {
    Logging::Ard("EVENT OVERFLOW. DROPPING EVENT."); return;
  }

  /// If override is set, force the event in, and clear memory of the list.
  if (override) {
    this->currentEvent = ev; 
    memset(this->eventList, 0, this->MAX_EVENTLIST_SIZE);
    return;
  }
  
  this->eventList[this->eventListSize++] = ev;
}