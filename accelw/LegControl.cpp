// The necessary includes.
#include "LegControl.h"
#include "MCP23008.h"


// Initialize the leg control system
LegControl::LegControl(int i) {
  // for (uint8_t e = 0; e < 6; e++) {
  //   uint8_t index = e * 6;
  //   Leg l;
  //   this->legs[this->leg_c++] = l;
  // }
}

/// Initialises all the motors,
/// and returns the legcontrol class reference.
void LegControl::setup(Servo *s){//, MCP23008* mp,MCP23008* mp2,MCP23008* mp3) {

  // Connect the servos up to their respective lists
  for (int e = 0; e < 6; e++) {
    this->legs[e].o_z = s[e*2];
    this->legs[e].h_z = s[(e*2)+1];
  }

  for (int e = 12; e < 18; e++) {
    this->legs[e-12].o_xy = s[e];
  }

  this->currentPosition = vector2D(0, 0);

  // Connect all the mcp pin expandors to the right legs in the code.
  // this->legs[0].o_xy.mpcont = mp2;
  // this->legs[1].o_xy.mpcont = mp3;
  // this->legs[2].o_xy.mpcont = mp2;
  // this->legs[3].o_xy.mpcont = mp3;
  // this->legs[4].o_xy.mpcont = mp;
  // this->legs[5].o_xy.mpcont = mp;

  // Connect all the outputs 
  // for (int i = 0; i < 6; i++) {
  //   this->legs[i].o_xy.enableOutputs();
  // }
}



// Attempt to process an event from the list
int LegControl::AttemptEventProc() {
  // Check if there is an event to process, if not die
  if (this->currentEvent == NULL) return 0;

  // If event, then we can first process the stepper motors
  //Event::runMotors(this->legs);

  // Now we can process the event.
  return this->currentEvent->Proc(this->legs, this->currentPosition);
}


/// Processes next events in the leg controller.
/// This should only be run by the internal timer (void loop),
/// and not called by any other function.
void LegControl::procStep() {
  if (this->currentEvent == NULL) return;
  /// If currentEvent should be take over by eventlist.next, set it, and drop the list size.
  if ((*this->currentEvent) <= (*(this->eventList[0]))) {
    if (this->eventListSize == 0) this->currentEvent = NULL; //Is there actually another event.
    else {
      // Replace the current with the next one.
      this->currentEvent = this->eventList[0];
      // Move the memory of the entire list over by 1 in the stack, effectively removing the first element and reassigning the first element.
      memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
    }
  }

  /// Double event send, assume error due to no override value set.
  /// Same as above, but just override the memory.
  // while (this->eventListSize != 0 && ((*this->currentEvent) == (*(this->eventList[0])))) {
  //   memmove(&this->eventList[0], &this->eventList[1], this->eventListSize-- - 1);
  // }

  // Attempt to proc the event, take the status code
  int x = AttemptEventProc();
  if (x != 0) {
    // If status is not 0, it died somehow, lets change over to a new event.
    // Same techniques as above.
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

  /// If override is set, force the event in, and clear memory of the list, then return.
  if (override || (this->eventListSize == 0 && this->currentEvent == NULL)) {
    this->currentEvent = ev;
    memset(this->eventList, 0, this->MAX_EVENTLIST_SIZE);
    return;
  }

  // Add the event to the end of the list
  this->eventList[this->eventListSize++] = ev;
}