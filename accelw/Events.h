#include <AccelStepper.h>
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

class Logging {
  public:
    const static bool isLoRa = false;
    static void Info(const char *message) { 
      Serial.println(message);
      if (!isLoRa) return;
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
    };
    static void Warning(const char *message) {
      Serial.print("Warning: ");
      Serial.println(message);
      if (!isLoRa) return;
      LoRa.beginPacket();
      LoRa.print("Warning: ");
      LoRa.print(message);
      LoRa.endPacket();
    };
    static void Ard(const char *message) {
      Serial.println(message);
    }
};

struct Leg {
  AccelStepper* o_xy;
  AccelStepper* o_z;
  AccelStepper* h_z;
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







class Event {
  public:
    unsigned long timeout = 0;
    int stage = 0;
    byte identifier = 0;

    // are the events the same 
    inline bool operator==(const Event& other) const {
      return this->identifier == other.identifier && 
            this->stage == other.stage && 
            this->timeout == other.timeout;
    };

    // should event to right replace me.
    inline bool operator<=(const Event& other) const {
      return this->stage == -1;
    };
    Event();
    Event(byte id);

    // Process the events step.
    virtual int Proc(Leg legs[6], vector2D position) {Logging::Warning("UnReachable Code");};

    bool Stage(byte id) {
      return (id == this->stage) && (this->timeout == 0);
    };

    bool MidStage(byte id) {
      return (id == this->stage) && (this->timeout != 0);
    }

    bool StageWait() {
      if (this->timeout == 0) this->stage++;
      return this->timeout != 0;
    }

    void ProgressStage(byte intended_stage, bool doI) {
      if (this->Stage(intended_stage) && doI) { this->stage++; }
    };

    void End(byte last_stage) {
      if (last_stage == this->stage) this->stage = -1;
    }

};

class WalkEvent : public Event {
  public:
    WalkEvent() : Event(1) {
    };
    int Proc(Leg legs[6], vector2D position);
};



class StopEvent : public Event {
  public:
    StopEvent() : Event(2) {
    };
    int Proc(Leg legs[6], vector2D position);
};
















