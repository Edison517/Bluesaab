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

#include <CAN.h>

int cdbutton = 0;
int toggleshuffle = 1;
int mute = 0;
int CDCcmd[] = {
  0xE0,0x00,0x3F,0x31,0xFF,0xFF,0xFF,0xD0};
// first byte needs to be 32 otherwise 2004 9-5 will throw airbag light; it was 0x62
int ninefivecmd[] = {
  0x32,0x00,0x00,0x16,0x01,0x02,0x00,0x00};
int beep[] = {
  0x80,0x04,0x00,0x00,0x00,0x00,0x00,0x00};
int playipod[] = {
  0xFF,0x55,0x04,0x02,0x00,0x00,0x01,0xF9};
int playpauseipod[] = {
  0xFF,0x55,0x03,0x02,0x00,0x01,0xFA};
int stopipod[] = {
  0xFF,0x55,0x04,0x02,0x00,0x00,0x02,0xF8};
int next[] = {
  0xFF,0x55,0x03,0x02,0x00,0x08,0xF3};
int prev[] = {
  0xFF,0x55,0x03,0x02,0x00,0x10,0xEB};
int shuffle[] = {
  0xFF,0x55,0x04,0x02,0x00,0x00,0x80,0x7A};
int repeat[] = {
  0xFF,0x55,0x05,0x02,0x00,0x00,0x00,0x01,0xF8};
int buttonRelease[] = {
  0xFF,0x55,0x03,0x02,0x00,0x00,0xFB};

void setup() {
  // set up CAN
  CAN.begin(47);  // Saab I-Bus is 47.619kbps
  Serial.begin(9600);
  //cdbutton = 0;
  //toggleshuffle = 1;
  CAN_TxMsg.header.rtr=0;     // this value never changes
  CAN_TxMsg.header.length=8;  // this value never changes
  // not sure if this is needed; pauses program before it loops
  Serial.println("9-5 Test Code 2");
  Serial.println("Sky Dailey - 'AUX'");
  Serial.println("18 Mar 2014");
  delay(2000);
}

void loop() {
  //cdbutton = 1;
  //PrintBus();
  // CDC code needs sent every second or less so all loops
  // running added together need to take less than 1000ms
  // but no more or the car won't "see" the CDC
  CDC();
  for (int i = 0; i <= 860; i++) {
    if (CAN.CheckNew()) {
      CAN_TxMsg.data[0]++;
      CAN.ReadFromDevice(&CAN_RxMsg);
      //PrintBus();
      if (CAN_RxMsg.id==0x6A1) {
        CAN_TxMsg.id=0x6A2;     // CD Changer
        for (int c = 0; c < 8; c++) {
          CAN_TxMsg.data[c]=ninefivecmd[c];
        }
        CAN.send(&CAN_TxMsg);
      }

      if (CAN_RxMsg.id==0x3C0) {
        if (CAN_RxMsg.data[0]==0x80) {
          switch (CAN_RxMsg.data[1]) {
          case 0x24:
            cdbutton = 1;
            for (int j = 0; j < 8; j++) {
              CAN_TxMsg.id=0x430;
              CAN_TxMsg.data[j]=beep[j];
            }
            CAN.send(&CAN_TxMsg);
            for (int j = 0; j < 8; j++) {
              Serial.write(byte(playipod[j]));
            }
            delay(3);
            //Serial.println("Release");
            for (int i = 0; i < 7; i++) {
              Serial.write(byte(buttonRelease[i]));
            }
            break;
          case 0x14:
            cdbutton = 0;   
            for (int a = 0; a <=2; a++) {
              for (int j = 0; j < 8; j++) {
                CAN_TxMsg.id=0x430;
                CAN_TxMsg.data[j]=beep[j];
              }
              CAN.send(&CAN_TxMsg);
            }
            for (int j = 0; j < 8; j++) {
              Serial.write(byte(stopipod[j]));
            }
            delay(3);
            //Serial.println("Release");
            for (int i = 0; i < 7; i++) {
              Serial.write(byte(buttonRelease[i]));
            }
            //Serial.println("Radio");
            break;
          }
          if (cdbutton == 1) {
            switch (CAN_RxMsg.data[1]) {
            case 0x59: // NXT button signal
              for (int j = 0; j < 7; j++) {
                Serial.write(byte(playpauseipod[j]));
              }
              break;
            case 0x76: // Long press of CD/RDM button
              if (toggleshuffle > 3) {
                toggleshuffle = 1;
              }
              switch (toggleshuffle) {
              case 1:
                for (int j = 0; j < 9; j++) {
                  Serial.write(byte(repeat[j]));
                }
                break;
              case 2:
                for (int j = 0; j < 9; j++) {
                  Serial.write(byte(repeat[j]));
                }
                break;
              case 3:
                for (int j = 0; j < 9; j++) {
                  Serial.write(byte(repeat[j]));
                }
                for (int j = 0; j < 8; j++) {
                  Serial.write(byte(shuffle[j]));
                }
                break;
              }
              toggleshuffle++;
              //break;
            case 0xB1: // Audio mute on?
              for (int j = 0; j < 8; j++) {
                Serial.write(byte(stopipod[j]));
              }
              break;
            case 0xB0: // Audio mute off?
              for (int j = 0; j < 8; j++) {
                Serial.write(byte(playipod[j]));
              }
              break;
            case 0x35: // Seek next (Seek+)?
              for (int j = 0; j < 7; j++) {
                Serial.write(byte(next[j]));
              }
              break;
            case 0x36: // Seek previous (Seek-)?
              for (int j = 0; j < 7; j++) {
                Serial.write(byte(prev[j]));
              }
              break;
            }
            delay(3);
            //Serial.println("Release");
            for (int i = 0; i < 7; i++) {
              Serial.write(byte(buttonRelease[i]));
            }
          }
        }
      }
      if (CAN_RxMsg.id==0x290) {
        if (CAN_RxMsg.data[0]==0x80) {
          if (cdbutton == 1) {
            switch (CAN_RxMsg.data[2]) {
              //case 0x04: // NXT button on wheel
              //for (int j = 0; j < 9; j++) {
              //Serial.write(byte(repeat[j]));
              //}
              //break;
            case 0x10: // Seek+ button on wheel
              //Serial.println("Next");
              for (int j = 0; j < 7; j++) {
                Serial.write(byte(next[j]));
              }
              break;
            case 0x08: // Seek- button on wheel
              //Serial.println("Prev");
              for (int k = 0; k < 7; k++) {
                Serial.write(byte(prev[k]));
              }
              break;
            }
            delay(3);
            //Serial.println("Release");
            for (int i = 0; i < 7; i++) {
              Serial.write(byte(buttonRelease[i]));
            }
          }
        }
      }
    }
    delay(1);
  }
  if (cdbutton==1) {
    //Serial.println("iPod ON");
    iPodOn();
  }
  else {
    //Serial.println("iPod OFF");
    //iPodOff();
    //delay(400);
  }
}

void CDC() {
  CAN_TxMsg.id=0x3C8;     // CD Changer
  for (int c = 0; c < 8; c++) {
    CAN_TxMsg.data[c]=CDCcmd[c];
  }
  CAN.send(&CAN_TxMsg);
}

void iPodOn() {
  // This loop takes 50ms
  CAN_TxMsg.id=0x328;     // SID audio text
  CAN_TxMsg.data[0]=0x42; // message 2
  CAN_TxMsg.data[1]=0x96;
  CAN_TxMsg.data[2]=0x02; // Row 2
  CAN_TxMsg.data[3]=0x20; // _
  CAN_TxMsg.data[4]=0x20; // _
  CAN_TxMsg.data[5]=0x20; // _
  CAN_TxMsg.data[6]=0x20; // _
  CAN_TxMsg.data[7]=0x41; // A
  CAN.send(&CAN_TxMsg);
  delay(10);

  CAN_TxMsg.data[0]=0x01; // message 1
  CAN_TxMsg.data[3]=0x55; // U
  CAN_TxMsg.data[4]=0x58; // X
  CAN_TxMsg.data[5]=0x20; // _
  CAN_TxMsg.data[6]=0x20; // _
  CAN_TxMsg.data[7]=0x20; // _
  CAN.send(&CAN_TxMsg);
  delay(10);

  CAN_TxMsg.data[0]=0x00; // message 0
  CAN_TxMsg.data[3]=0x20; // 
  CAN_TxMsg.data[4]=0x20; // _
  CAN_TxMsg.data[5]=0x20; //
  CAN_TxMsg.data[6]=0x20; //
  CAN_TxMsg.data[7]=0x20; //
  CAN.send(&CAN_TxMsg);
  delay(10);

  CAN_TxMsg.id=0x348;     // audio text control
  CAN_TxMsg.data[0]=0x11; // 11
  CAN_TxMsg.data[1]=0x02; // Row 2?
  CAN_TxMsg.data[2]=0x05; // 05 used to work, now SID turns off when engine is off; 01 seems to work 
  CAN_TxMsg.data[3]=0x18; // priority 18?
  CAN_TxMsg.data[4]=0x00;
  CAN_TxMsg.data[5]=0x00;
  CAN_TxMsg.data[6]=0x00;
  CAN_TxMsg.data[7]=0x00;
  CAN.send(&CAN_TxMsg);
  delay(10);

  /*CAN_TxMsg.id=0x368;     // SID text priority
   CAN_TxMsg.data[0]=0x02; // Row 2
   CAN_TxMsg.data[1]=0x18; // priority 18?
   CAN_TxMsg.data[2]=0x00;
   CAN_TxMsg.data[3]=0x00;
   CAN_TxMsg.data[4]=0x00;
   CAN_TxMsg.data[5]=0x00;
   CAN_TxMsg.data[6]=0x00;
   CAN_TxMsg.data[7]=0x00;
   CAN.send(&CAN_TxMsg);
   delay(10);
   */
}

void PrintBus() {
  if (CAN_RxMsg.id==0x6A2) {
  //if (CAN_RxMsg.data[0]==0x80) {
    Serial.print(CAN_RxMsg.id,HEX);
    Serial.print(";");
    for (int i = 0; i < 8; i++) {
      Serial.print(CAN_RxMsg.data[i],HEX);
      Serial.print(";");
    }
    Serial.println("");
  }
}
