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
  
  }
  //subtract minimum and maximum value to remove spikes
  sum-=minval; sum-=maxval;
  
  //process
  if (sumsum==0) sumsum=sum<<6; //set sumsum to expected value
  long int avgsum=(sumsum+32)>>6; 
  diff=sum-avgsum;
  if (abs(diff)<avgsum>>10){      //adjust for small changes
    sumsum=sumsum+sum-avgsum;
    skip=0;
  } else {
    skip++;
  }
  if (skip>64){     // break off in case of prolonged skipping
    sumsum=sum<<6;
    skip=0;
  }

  // one permille change = 2 ticks/s
  if (diff==0) flash_period=1000000;
  else flash_period=avgsum/(2*abs(diff));    

  return flash_period < 10;
}





/// Start LoRa and Serial.
void setup() {

  Serial.begin(9600); // Begin serial
  while (!Serial); // Wait for serial
  Wire.begin(); // Begin the wire connection on the SCL SDA pins


  mpx1.begin(); // Begin pin extendor
  mpx2.begin();
  mpx3.begin();


  // Attach all the servos
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

  // Set all the servos to default.
  for (int i = 0; i< 12; i++) {
    servos[i].write(90);
  }

  lc.setup(servos, &mpx1, &mpx2, &mpx3); // Setup the pin extendor

  Serial.print(mpx1.isConnected());
  Serial.print(mpx2.isConnected());
  Serial.print(mpx3.isConnected());
  Serial.println("Didn\'t fail pre boot."); // A quick it worked message.
  

  // Begin lora on the correct wavelength, and print if it fails.
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // FOR DEBUGGING.
  WalkEvent e;
  lc.submitEvent(&e, false);
}

// Process steps every clock cycle.
void loop() {
  lc.procStep(); // Tell the control to process a step.

  // If we detect a mine lets tell LoRa.
  if (MetalDetect()) Logging::Info("+" + String(lc.currentPosition.x) + ":" + String(lc.currentPosition.y));


  // Process a lora packet.
  if (LoRa.parsePacket() > 0) {
    
    // Read basic packet information.
    // See more about packet info in the events.h file.

    byte recipient = LoRa.read();
    byte sender = LoRa.read();
    byte messageID = LoRa.read();

    byte messageType = LoRa.read();

    byte incomingLength = LoRa.read();

    String incoming = "";


    // Read from the LoRa serial.
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) { // Something went wrong ohno.
      Logging::Warning("RESEND:" + String(messageID));
      return;
    }

    if (recipient != Logging::MyAddress && recipient != Logging::ChannelAddress) return; // Oops we touched a bad packet.

    if (messageType == 1) { // A create event packet

      // The walk event
      if (incoming == "WALK") {
        WalkEvent tmp;
        lc.submitEvent(&tmp, false);
      }

    } 

    if (messageType == 2) { // Override create event

      // The walk event
      if (incoming == "WALK") {
        WalkEvent tmp;
        lc.submitEvent(&tmp, true);
      }

    }

  }
}
