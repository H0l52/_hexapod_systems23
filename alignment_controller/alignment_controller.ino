#include <AccelStepper.h>
#include "MCP23008.h"
#include "Wire.h"

const int stepsPerRevolution = 2038;

int motors[20*4] = {
  47,49,51,53, 23,25,27,29, 2,3,4,5,
  46,48,50,52, 31,33,35,37, 14,15,22,24,
  34,36,38,40, 39,41,43,45, 16,17,18,19,
  A4,A5,42,44, A11,A12,A13,A14, 6,7,8,9,
  0,1,2,3, A7,A8,A9,A10, 10,11,12,13, 
  4,5,6,7, A0,A1,A2,A3, 26,28,30,32,
  };

MCP23008 mpx(0x20);

void setup() {
  // put your setup code here, to run once:
  mpx.begin();
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  static int c = 0;
  static int l = 0;
  static int m = 0;
  while (Serial.available()) {
    static char message[256];
    static unsigned int messagePos = 0;
    
    char byte = Serial.read();

    if (byte != '\n' && (messagePos < 255)) {
      message[messagePos] = byte; messagePos++;
    }
    else {
      message[messagePos] = '\0';
      messagePos = 0;

      char lcopy[256];
      memcpy(lcopy, message, sizeof message);
      int i=0;
      char *seperator = ":";
      char response[1][20];
      char *token = strtok(lcopy, seperator);
      while((i<2)&&((token = strtok(NULL, seperator)) != NULL)) {
        strcpy(response[i], token);
        i++;
      }

      m = atoi(lcopy);
      c = atoi(response[0]);
      l = atoi(response[1]);

      Serial.print("---\n");
      Serial.print(m);
      Serial.print("leg/");
      Serial.print(c);
      Serial.print("steps/");
      Serial.println(l);
      l *= 4;
      if (c > stepsPerRevolution) c = stepsPerRevolution;
      if (c < -stepsPerRevolution) c = -stepsPerRevolution;
      

      //memset(message, 0, sizeof message);
    }
  }

  if (c!=0) {
    AccelStepper s(AccelStepper::FULL4WIRE, motors[m*12 + l], motors[m*12 + l+2], motors[m*12 + l+1], motors[m*12 + l+3], true);
    if ((m == 4 || m == 5) && l == 0) {s.mpcont = &mpx; s.enableOutputs();}
    s.setAcceleration(300.0);
    s.setMaxSpeed(300.0);
    s.move(c);
    while (s.isRunning()) {
      s.run();
    }
    c = 0;
  }
}
