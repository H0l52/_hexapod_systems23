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
int legss[(4 + 2)*6] = {
  
  };

// Initiate the data structures
LegControl lc(legss);


MCP23008 mpx1(0x20);
MCP23008 mpx2(0x21);
MCP23008 mpx3(0x22);


/// Start LoRa and Serial.
void setup() {
  mpx1.begin(); // Begin pin extendor
  mpx2.begin();
  mpx3.begin();
  lc.setup(&mpx1, &mpx2, &mpx3); // Setup the pin extendor
  Serial.begin(9600); // Begin serial
  while (!Serial); // Wait for serial

  Serial.println("Didn\'t fail pre boot."); // A quick it worked.
  

  // Begin lora on the correct wavelength, and print if it fails.
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  ///// IGNORE

  WalkEvent e;
  //WalkEvent b;
  lc.submitEvent(&e, false);
  //lc.submitEvent(&b, false);
}

// Process steps every clock cycle.
void loop() {
  lc.procStep();
}
