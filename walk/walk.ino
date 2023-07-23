#include "stepper.h"
#include "commandInterpreter.h"
#include "mLoop.h"

#include <SPI.h>
#include <LoRa.h>

const int stepsPerRevolution = 2038;

commandInterpreter cI(stepsPerRevolution);
mLoop _mloop(&cI);

void setup() {
  Serial.begin(9600);

  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  cI.addLeg(new int[12]{ 8,10,9,11, 29,25,27,23, 22,26,24,28});
}

void loop() {
  cI.updateMotors();
  _mloop.UpdateT();
  if (Serial.available() > 0) {
    char r = Serial.read();
    String icString(r);
    _mloop.PushUpdate(icString);
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
