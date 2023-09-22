// include the neccessary libararies
#include <AccelStepper.h>
#include <Servo.h>
#include "Events.h"

// The legcontrol datastructure
class LegControl {
  // Private, unnaccessable data
  private:
    // The current size of the pointer "list" (fake array extension)
    byte leg_c = 0;

    // The max size of the list
    const static int MAX_EVENTLIST_SIZE = 12;

    // The current event being processed
    Event* currentEvent = NULL;

    // The size of the event list currently (fake array extension)
    byte eventListSize = 0;
    // The pointer list of events
    Event* eventList[MAX_EVENTLIST_SIZE];

    // The steps per revolution on the oukeda stepper motors.
    const static int STEPSPERREVOLUTION = 2038;

    // The pointer list of leg integers.
    int *mlist;

  // Public data structure
  public:
    // The max size of the leg list
    const static int MAXSIZE = 6;
    
    // The initiator for the class, takes a 6*6 2d array int input.
    LegControl(int i[MAXSIZE*6]);

    // Setup function for the MCP pin expander
    void setup(MCP23008* mp,MCP23008* mp2,MCP23008* mp3);

    // A generic process step function.
    void procStep();

    // A submit event utility function for adding to the list.
    void submitEvent(Event *ev, bool override);

    // Attempt to proc the event.
    int AttemptEventProc();

    // The actual legs list.
    Leg legs[MAXSIZE];

    // Vector2D position for utilities.
    vector2D currentPosition;
};


