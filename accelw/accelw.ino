// Include all of the necessary files
#include <AccelStepper.h>
#include "LegControl.h"
#include <SPI.h>
#include <LoRa.h>
#include "MCP23008.h"
#include "Wire.h"
/// Initialises motors.
///
/// Motors are defined as follows
/// 1 line per leg
///
/// Motor Index : Use
/// 0           : Origin/X-Y Hip.
/// 1           : Z Hip.
/// 2           : Z Knee.
///
/// Motor schema (For Oukeda):
/// pin1, pin3, pin2, pin4
/// libpin1, libpin2, libpin3, libpin4
///

/// LegSchema:
/// StepperMotor, Servo Base, Servo Foot
int legss[36] = {
    0, 1, 2, 3, 0,0,
    0, 1, 2, 3, 0,0,
    4, 5, 6, 7, 0,0,
    4, 5, 6, 7, 0,0,
    4, 5, 6, 7, 0,0,
    0, 1, 2, 3, 0,0
  };

// Initiate the data structures
LegControl lc(legss);

MCP23008 mpx1(0x20);
MCP23008 mpx2(0x21);
MCP23008 mpx3(0x22);

Servo servos[12];

///
/// METAL DETECTOR
///
///
const byte npulse = 3; // number of pulses
const byte pin_pulse=A0; // pins on the ardduino
const byte pin_cap  =A1;

const int nmeas=256;  //measurements to take
long int sumsum=0; //running sum of 64 sums 
long int skip=0;   //number of skipped sums
long int diff=0;        //difference between sum and avgsum
long int flash_period=0;//period (in ms) 
long unsigned int prev_flash=0; //time stamp of previous flash


bool MetalDetect() {
  // Store mins and maxs of Analog data input
  int minval=1023;
  int maxval=0;

  // prepare sum variable
  long unsigned int sum=0;


  for (int imeas=0; imeas<nmeas+2; imeas++){
    //reset the capacitor
    pinMode(pin_cap,OUTPUT);
    digitalWrite(pin_cap,LOW);
    delayMicroseconds(20);
    pinMode(pin_cap,INPUT);
    //apply pulses
    for (int ipulse = 0; ipulse < npulse; ipulse++) {
      digitalWrite(pin_pulse,HIGH); //takes 3.5 microseconds
      delayMicroseconds(3);
      digitalWrite(pin_pulse,LOW);  //takes 3.5 microseconds
      delayMicroseconds(3);
    }
    //read the charge on the capacitor
    int val = analogRead(pin_cap); //takes 13x8=104 microseconds
    minval = min(val,minval);
    maxval = max(val,maxval);
    sum+=val;
 
    //determine if LEDs should be on or off
    long unsigned int timestamp=millis();
    byte ledstat=0;
    if (timestamp<prev_flash+10){
      if (diff>0)ledstat=1;
      if (diff<0)ledstat=2;
    }
    if (timestamp>prev_flash+flash_period){
      if (diff>0)ledstat=1;
      if (diff<0)ledstat=2;
      prev_flash=timestamp;   
    }
    if (flash_period>1000)ledstat=0;

    //switch the LEDs to this setting
    if (ledstat==0){
      // digitalWrite(pin_LED1,LOW);
      // digitalWrite(pin_LED2,LOW);
      // if(sound)noTone(pin_tone);
    }
    if (ledstat==1){
      // digitalWrite(pin_LED1,HIGH);
      // digitalWrite(pin_LED2,LOW);
      // if(sound)tone(pin_tone,2000);
    }
    if (ledstat==2){
      // digitalWrite(pin_LED1,LOW);
      // digitalWrite(pin_LED2,HIGH);
      // if(sound)tone(pin_tone,500);
    }
  
  }


}





/// Start LoRa and Serial.
void setup() {

  Serial.begin(9600); // Begin serial
  while (!Serial); // Wait for serial
  Wire.begin();


  mpx1.begin(); // Begin pin extendor
  mpx2.begin();
  mpx3.begin();

  servos[0].attach(24);
  servos[1].attach(25);

  servos[2].attach(22);
  servos[3].attach(23);

  servos[4].attach(26);
  servos[5].attach(27);

  servos[6].attach(28);
  servos[7].attach(29);

  servos[8].attach(30);
  servos[9].attach(31);

  servos[10].attach(32);
  servos[11].attach(33);

  for (int i = 0; i< 12; i++) {
    servos[i].write(90);
  }

  lc.setup(servos, &mpx1, &mpx2, &mpx3); // Setup the pin extendor

  //delay(1000);
  Serial.print(mpx1.isConnected());
  Serial.print(mpx2.isConnected());
  Serial.print(mpx3.isConnected());
  Serial.println("Didn\'t fail pre boot."); // A quick it worked.
  

  // Begin lora on the correct wavelength, and print if it fails.
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  ///// IGNORE

  WalkEvent e;
  //SyncEvent b;
  lc.submitEvent(&e, false);
  //lc.submitEvent(&b, false);
}

// Process steps every clock cycle.
void loop() {
  lc.procStep();
}
