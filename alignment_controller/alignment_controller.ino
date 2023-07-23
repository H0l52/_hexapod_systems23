#include <Stepper.h>
const int stepsPerRevolution = 2038;


Stepper leg1Origin(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  // put your setup code here, to run once:
  leg1Origin.setSpeed(4);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  static int c = 0;
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
      c = atoi(message);
      if (c > stepsPerRevolution) c = stepsPerRevolution;
      if (c < -stepsPerRevolution) c = -stepsPerRevolution;
      

      memset(message, 0, sizeof message);
    }
  }

  if (c!=0) {
    leg1Origin.step(c);
    c=0;
  }
}
