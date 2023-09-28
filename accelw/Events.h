#include <AccelStepper.h>
#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>


// LoRa and Serial logging class structure.
//
// LoRa packet structure:
//
// Channel Address
// My Address
// Message Count/Id since turn on
// Info/Warning , 0/1
// Message length
// The Message
//

class Logging {
  public:
    // Important information for LoRa packets
    const static byte MyAddress = 0xBB;
    const static byte ChannelAddress = 0xFF;
    static int msgCount;

    // Should we use LoRa right now?
    const static bool isLoRa = false;

    // An info function for easy use.
    static void Info(String message) { 
      Serial.println(message); // Serial print message
      if (!isLoRa) return;

      // Write the packet from above structure if LoRa is allowed.
      LoRa.beginPacket();
      LoRa.write(ChannelAddress);
      LoRa.write(MyAddress);
      LoRa.write(msgCount++);
      LoRa.write(0);
      LoRa.write(message.length());
      LoRa.print(message);
      LoRa.endPacket(true);
    };

    // A Warning function for easy use
    static void Warning(String message) {
      Serial.print("Warning: ");
      Serial.println(message); // Serial print message
      if (!isLoRa) return;

      // Write the packet from above structure if LoRa is allowed.
      LoRa.beginPacket();
      LoRa.write(ChannelAddress);
      LoRa.write(MyAddress);
      LoRa.write(msgCount++);
      LoRa.write(1);
      LoRa.write(message.length());
      LoRa.print(message);
      LoRa.endPacket(true);
    };

    // Basically just Serial.print - except with from the same logging system.
    static void Ard(String message) {
      Serial.println(message);
    }
};

// Basic leg structure in memory - just makes it easy to access everything.
struct Leg {
  AccelStepper o_xy; // The Origin XY (Left/Right) stepper motor
  Servo o_z; // The origin Z (Up/Down) servo
  Servo h_z; // The hip Z (Up/Down) servo
};

// Vector2D direction and positional computation system - designed for easy extension later.
struct vector2D {
  
  // Structure initializer.
  int x;
  int y;
  
  vector2D(int a=0, int b=0) {
    x = a; y = b;
  };

  // The size of the vector compared to another - (basically pythag diff)
  int magnitude(vector2D& other) {
    int l=(this->x - other.x)^2;
    int f=(this->y - other.y)^2;
    return sqrt(l + f);
  };

  // Its pythag. 
  int length() {
    return sqrt(this->x^2 + this->y^2);
  }

  // Make a unit Vector.
  void normalise() {
    int len = this->length();
    this->x /= len;
    this->y /= len;
  }
};



//
// THE EVENT SYSTEM
//

class Event {
  public:
    unsigned long timeout = 0; // A defined timeout system - lets the event determine how far along it is.
    int stage = 0; // A defined stage system - a rank higher than timeout - allows for staging events.
    byte identifier = 0; // An idenitifer system - the highest rank - determines which event this is respective to the others.

    // are the events the same 
    inline bool operator==(const Event& other) const {
      // compare everything about them.
      return this->identifier == other.identifier && 
            this->stage == other.stage && 
            this->timeout == other.timeout;
    };

    // should event to right replace me.
    inline bool operator<=(const Event& other) const {
      // Basically just a check to see if the event to the right is ready to die yet.
      return this->stage == -1;
    };

    // The base intializer - required for virtual lookup tables.
    Event();
    // The proper id initializer
    Event(byte id);

    // Process the events step.
    // This is a virtual function - which adds itself to the virtual table.
    // If this is ever reached, very bad things are happening. This code should never run.
    // Other events are to override this code on the virtual table.
    // If that is not happening you either arent using a virtual table capable board (You are meant to be using a Mega or Mega 2560).
    // or things have gone terribly wrong in either your code or compiler.
    virtual int Proc(Leg legs[6], vector2D position) {Logging::Warning("UnReachable Code");};

    //
    // Utility functions for in-event use.
    //

    // Are we at the start of stage ID?
    bool Stage(byte id) {
      return (id == this->stage) && (this->timeout == 0);
    };

    // Are we currently in the middle of stage ID?
    bool MidStage(byte id) {
      return (id == this->stage) && (this->timeout != 0);
    }

    // Are we ready to move on from this stage yet?
    bool StageWait() {
      if (this->timeout == 0) this->stage++;
      return this->timeout != 0;
    }

    // Finally we can move on from this stage - 
    // as long as we are on the stage intended_stage, and are allowed to from a passed boolean expression.
    void ProgressStage(byte intended_stage, bool doI) {
      if (this->Stage(intended_stage) && doI) { this->stage++; }
    };

    // Are we on the stage last_stage?
    // If so, tell the other events we are ready to die.
    void End(byte last_stage) {
      if (last_stage == this->stage) this->stage = -1;
    }

    // As servos can be - stupid -
    // A user may need to map their angle values.
    // They can do this here.
    void WriteServoSafe(Servo s, int angle ) {
      s.write(map(angle,0,180,0,180));
    }


    // Not for use beyond the LegControl system. Attempts to run the motors, then Procs the event.
    static void runMotors(Leg legs[6]) {
      for (int i = 0; i < 6; i++) {
        // Run each legs stepper motor.
        legs[i].o_xy.runSpeedToPosition();
      }
    } 

};


// The walk event class - a great example of how to setup an event.
class WalkEvent : public Event {
  public:
    WalkEvent() : Event(1) { // Choose Id 1
    };
    int Proc(Leg legs[6], vector2D position); // Process the event - code in the cpp file.
};


// Same as above, but id2 and for syncing the legs - used this to align my motors with the servos.
class SyncEvent : public Event {
  public:
    SyncEvent() : Event(2) {
    };
    int Proc(Leg legs[6], vector2D position);
};
















