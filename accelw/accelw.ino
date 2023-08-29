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
int steppers[6*12] = {
  47,49,51,53, 23,25,27,29, 2,3,4,5,
  46,48,50,52, 31,33,35,37, 14,15,22,24,
  34,36,38,40, 39,41,43,45, 16,17,18,19,
  A4,A5,42,44, A11,A12,A13,A14, 6,7,8,9,
  0,1,2,3, A7,A8,A9,A10, 10,11,12,13, 
  4,5,6,7, A0,A1,A2,A3, 26,28,30,32,
  };

LegControl lc(steppers);

MCP23008 mpx(0x20);
/// Start LoRa and Serial.
void setup() {
  mpx.begin();
  lc.setup(&mpx);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Didn\'t fail pre boot.");
  

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  WalkEvent e;
  WalkEvent b;
  lc.submitEvent(&e, false);
  //lc.submitEvent(&b, false);
}

void loop() {
  lc.procStep();
}
