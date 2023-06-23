#ifndef mLoop_h
#define mLoop_h

#include <Arduino.h>
#include "commandInterpreter.h"

class mLoop {
  private:
    const static int commands_max_size = 7;
    String commands[commands_max_size];
    byte lencom = 0;
    unsigned long last_interval;
    const static unsigned long interval_delay = 100;
    commandInterpreter* ci;

  public:
    mLoop(commandInterpreter* ci);
    void UpdateT();
    void PushUpdate(String s);
    void KillUpdate(byte index);
    String readAndKill(byte index);
};



#endif