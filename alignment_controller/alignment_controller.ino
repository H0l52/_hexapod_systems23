#include <Stepper.h>
const int stepsPerRevolution = 2038;

int motors[20][4] = {8,10,9,11, 29,25,27,23, 22,26,24,28};

void setup() {
  // put your setup code here, to run once:

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
    Stepper s(stepsPerRevolution, motors[m][l], motors[m][l+1], motors[m][l+2], motors[m][l+3]);
    s.setSpeed(5);
    s.step(c);
    c = 0;
  }
}
