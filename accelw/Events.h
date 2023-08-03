#include <AccelStepper.h>
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

class Logging {
  public:
    static void Info(const char *message) { 
      Serial.println(message);
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
    };
    static void Warning(const char *message) {
      char dest[256];
      char bufWarn[10] = "WARNING: ";
      strcpy(dest, bufWarn);
      strcat(dest, message);
      Logging::Info(dest);
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
    byte stage = 0;
    byte identifier = 0;

    inline bool operator==(const Event& other) const {
      return this->identifier == other.identifier && 
            this->stage == other.stage && 
            this->timeout == other.timeout;
    };

    inline bool operator<=(const Event& other) const {
      return this->stage == -1;
    };
    Event();
    Event(byte id);
    virtual int Proc(Leg legs[6], vector2D position) {};
};

class WalkEvent : public Event {
  public:
    int Proc(Leg legs[6], vector2D position);
};



















