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
int steppers[6*12] = {//8,10,9,11, 29,25,27,23, 22,26,24,28,
                      4,6,5,7, 4,6,5,7, 0,0,0,0,
                      0,0,0,0, 0,0,0,0, 0,0,0,0,
                      0,0,0,0, 0,0,0,0, 0,0,0,0,
                      0,0,0,0, 0,0,0,0, 0,0,0,0,
                      0,0,0,0, 0,0,0,0, 0,0,0,0,
                      0,0,0,0, 0,0,0,0, 0,0,0,0
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
