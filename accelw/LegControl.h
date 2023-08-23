#include <AccelStepper.h>
#include "Events.h"

class LegControl {
  private:
    byte leg_c = 0;

    const static int MAX_EVENTLIST_SIZE = 12;
    Event* currentEvent = NULL;

    byte eventListSize = 0;
    Event* eventList[MAX_EVENTLIST_SIZE];

    const static int STEPSPERREVOLUTION = 2038;

  public:
    const static int MAXSIZE = 6;
    
    LegControl();
    void setup(int i[MAXSIZE*12], MCP23008* mcp);
    void procStep();
    void submitEvent(Event *ev, bool override);
    int AttemptEventProc();

    Leg legs[MAXSIZE];

    vector2D currentPosition;
};


