// Include all of the necessary files
#include <AccelStepper.h>
#include "LegControl.h"
#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

// Initiate the data structures
LegControl lc(0);
Servo servos[18];

///
/// METAL DETECTOR
/// Code partially taken from https://www.instructables.com/Simple-Arduino-Metal-Detector/
///
const byte NumberOfPulses = 3; // number of pulses
const byte PulsePin =A0; // pins on the ardduino
const byte CapacitorPin  =A1;

const int NumberOfMeasurements=256;  //measurements to take
long int SumOf64=0; //running sum of 64 sums 
long int NumberOfSkippedSums=0; //number of skipped sums
long int DifferenceInAvgSum=0; //difference between sum and avgsum

long unsigned int LastDetect = 0;
long unsigned int hVal = 0;
int averageOfAverages = 0;
int avCount = 0;

bool MetalDetect() {
  long unsigned int ts=millis();
  if (ts < LastDetect + 200) return false;
  LastDetect = ts;
  // Store mins and maxs of Analog data input
  int minval=1023;
  int maxval=0;

  // prepare sum variable
  long unsigned int sum=0;


  for (int imeas=0; imeas<NumberOfMeasurements+2; imeas++){
    long unsigned int timestamp=millis();

    //reset the capacitor
    pinMode(CapacitorPin,OUTPUT);


    // cleanse capacitor
    digitalWrite(CapacitorPin,LOW);
    delayMicroseconds(20);

    // turn capcitor back to normal
    pinMode(CapacitorPin,INPUT);

    // pulse the metal detector
    for (int ipulse = 0; ipulse < NumberOfPulses; ipulse++) {
      digitalWrite(PulsePin,HIGH);
      delayMicroseconds(3);
      digitalWrite(PulsePin,LOW); 
      delayMicroseconds(3);
    }

    //read the changes on the capacitor
    int val = analogRead(CapacitorPin);

    // find the mins and maxs
    minval = min(val,minval);
    maxval = max(val,maxval);
    sum+=val;
  
  }
  //subtract minimum and maximum value to remove spikes
  sum-=minval; sum-=maxval;
  
  //process
  if (SumOf64==0) SumOf64=sum<<6; //back bitshift sum, and set sumsum to expected value
  long int avgsum=(SumOf64+32)>>6;  // bitshift average the sum with some hacky math.
  DifferenceInAvgSum=sum-avgsum; // find the average sum difference
  if (abs(DifferenceInAvgSum)<avgsum>>10){      //adjust for small changes
    SumOf64=SumOf64+sum-avgsum; //this is fancy math that removes spikes
    NumberOfSkippedSums=0;
  } else {
    NumberOfSkippedSums++; // this indicates if we have skipped a reading.
  }
  if (NumberOfSkippedSums>64){     // break off in case of prolonged skipping
    SumOf64=sum<<6;
    NumberOfSkippedSums=0;
  }
  
  // Timing based average system to remove errors - will still show a few false positives,
  // but no wehre near as many.

  if (millis() > hVal + 1000) {
    hVal = millis();
    averageOfAverages = 0;
    avCount = 0;
  } 
  averageOfAverages += avgsum/(2*abs(DifferenceInAvgSum)); 
  avCount++;

  // return a mine only if the average is less than 80. This should be tuned to the environment.
  return ((averageOfAverages/avCount) < 80);
}





/// Start LoRa and Serial.
void setup() {
  //attach all the servos
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

  servos[12].attach(39);
  servos[13].attach(37);
  servos[14].attach(35);
  servos[15].attach(34);
  servos[16].attach(36);
  servos[17].attach(38);

  pinMode(PulsePin, OUTPUT); 
  digitalWrite(PulsePin, LOW);
  pinMode(CapacitorPin, INPUT);  

  Serial.begin(9600); // Begin serial
  while (!Serial); // Wait for serial

  // Set all the servos to default.
  for (int i = 0; i< 18; i++) {
    servos[i].write(90);
  }

  lc.setup(servos); // Setup the servos

  // Begin lora on the correct wavelength, and print if it fails.
  if (!LoRa.begin(915E6)) {
   Serial.println("Starting LoRa failed!");
   while (1);
  }

  Logging::Info("AWOKEN");
  
  // FOR DEBUGGING.
  WalkEvent e;
  //delay(10000);
  lc.submitEvent(&e, false);
}


// Process steps every clock cycle.
void loop() {
  lc.procStep(); // Tell the control to process a step.

  
  // If we detect a mine lets tell LoRa.
  if (MetalDetect()) Serial.println("mine");//Logging::Info("mine" + lc.currentPosition.toString());

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
