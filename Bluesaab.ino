// ----------------------------------------------
// SECUDUINO
// http://secuduino.blogspot.com/
// By Igor Real
// 16/05/2011
//
// Saab CDC Changer Emulator
// http://BlueSaab.blogspot.com/
// By Seth Evans
// 29 July 2013
// 19 Feb 2014 - added name, sid text, date to output
// ----------------------------------------------

#include <Arduino.h>
#include "Bluesaab.h"
#include "SaabCAN.h"
#include "CDChanger.h"

#include <CAN.h>

#include <string.h>

CDChanger CDC;

void setup() {
  // set up CAN
  CAN.begin(47);  // Saab I-Bus is 47.619kbps
  CAN_TxMsg.header.rtr=0;     // this value never changes
  CAN_TxMsg.header.length=8;  // this value never changes

  Serial.begin(9600);

  // not sure if this is needed; pauses program before it loops
  delay(2000);

  Serial.println(PROJECT_NAME);
}

unsigned long CDCGenStatLastBaseline = millis();
void loop() {
  // CDC code needs sent every second or less so all loops
  // running added together need to take less than 1000ms
  // but no more or the car won't "see" the CDC
  if ((millis() - CDCGenStatLastBaseline) > 1000) {
    CDCGenStatLastBaseline = millis();
    sendFrame(CDC_GENERAL_STATUS, CDC.getGeneralStatusFrame());
  }

  if (CAN.CheckNew()) {
    CAN.ReadFromDevice(&CAN_RxMsg);

    printRx();

    if (CAN_RxMsg.id == NODE_STATUS_IHU) {
      if (CAN_RxMsg.data[0] == 0x21) {
        // the message is directed to us. Respond:
        sendFrame(NODE_STATUS_CDC, ninefivecmd);
      }
    } else if (CAN_RxMsg.id == CDC_CONTROL) {
      if (CAN_RxMsg.data[0] & 0x80) { // Sent on event
        switch (CAN_RxMsg.data[1]) {
        case 0x24: // Play
          {
            CDC.play();

            unsigned long timeSinceBaseline = millis() - CDCGenStatLastBaseline;
            if (timeSinceBaseline < 50) {
              delay(50 - timeSinceBaseline);
            }
            sendFrame(CDC_GENERAL_STATUS, CDC.getGeneralStatusFrame(false, true));
          }
          break;

        case 0x14: // Standby
          break;

        case 0x59: // Next CD
          break;

        case 0x76: // Random On/Off
          break;

        case 0xB1: // Pause Off
          break;

        case 0xB0: // Pause On
          break;

        case 0x35: // Track up
          break;

        case 0x36: // Track down
          break;
        }
      }
    } else if (CAN_RxMsg.id == SID_BUTTONS) {
      if (CAN_RxMsg.data[0] & 0x80) { // Sent on event
        //TODO check signal reliability
      }
    }
  }
}

void sendFrame(const int id, const byte* data) {
  CAN_TxMsg.id = id;

  memcpy(CAN_TxMsg.data, data, 8);

  printTx();

  while (CAN.send(&CAN_TxMsg) == 0xFF);
}

void printRx() {
  if (CAN_RxMsg.id == CDC_CONTROL ||
      CAN_RxMsg.id == SID_BUTTONS ||
      CAN_RxMsg.id == NODE_STATUS_IHU) {
    Serial.print(millis());
    Serial.print("  RX: ");
    Serial.print(CAN_RxMsg.id, HEX);
    Serial.print(": ");
    for (int i = 0; i < 8; i++) {
      Serial.print(CAN_RxMsg.data[i], HEX);
      Serial.print(",");
    }
    Serial.println();
  }
}

void printTx() {
  Serial.print(millis());
  Serial.print("  TX: ");
  Serial.print(CAN_TxMsg.id, HEX);
  Serial.print(": ");
  for (int i = 0; i < 8; i++) {
    Serial.print(CAN_TxMsg.data[i], HEX);
    Serial.print(",");
  }
  Serial.println();
}
