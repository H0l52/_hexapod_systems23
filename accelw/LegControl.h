#include <AccelStepper.h>

struct event {
  unsigned long timeout = 0;
  byte stage = 0;
  byte identifier = 0;

  inline bool operator==(const event& other) const {
    return this->identifier == other.identifier && 
          this->stage == other.stage && 
          this->timeout == other.timeout;
  };

  inline bool operator<=(const event& other) const {
    return this->stage == -1;
  }
};

struct vector2D {
  unsigned int x;
  unsigned int y;

  vector2D(int a=0, int b=0) {
    x = a; y = b;
  };

  int magnitude(vector2D& other) {
    int l=(this->x - other.x)^2;
    int f=(this->y - other.y)^2;
    return sqrt(l + f);
  };

  int length() {
    return sqrt(this->x^2 + this->y^2);
  }

  void normalise() {
    int len = this->length();
    this->x /= len;
    this->y /= len;
  }
};

struct Leg {
  AccelStepper* o_xy;
  AccelStepper* o_z;
  AccelStepper* h_z;
};

class LegControl {
  private:
    byte leg_c = 0;
    const static int MAXSIZE = 6;

    const static int MAX_EVENTLIST_SIZE = 12;
    event currentEvent;

    byte eventListSize = 0;
    event eventList[MAX_EVENTLIST_SIZE];

    const static int STEPSPERREVOLUTION = 2038;

  public:
    LegControl(int i[MAXSIZE*12]);
    void procStep();
    void submitEvent(event ev, bool override);

    Leg legs[MAXSIZE];

    vector2D currentPosition();
};


