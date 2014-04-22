#include "CDChanger.h"

CDChanger::CDChanger() {
  discMode = STOP;
  discNumber = 0x1;
  changerStatus = CDC_ANTI_THEFT_MARRIED |
                  CDC_ANTI_THEFT_PASSED |
                  MAGAZINE_PRESENT;
}

byte* CDChanger::getGeneralStatusFrame(boolean basetime, boolean commandReply) {
  // new data, command, validity:
  frame[0] = basetime ? 0x00 : 0x80;
  frame[0] |= commandReply ? 0x40 : 0x00;
  frame[0] |= 0x20; // disc presence signal is valid

  // pretend there are 16 cds in the "magazine":
  frame[1] = 0xff;
  frame[2] = 0xff;

  // disc mode:
  frame[3] = discMode << 4;
  // disc number:
  frame[3] |= (discNumber & 0x0f);
  
  // track or error number. Use ff during disc presence check, disc change etc.:
  frame[4] = (discMode == DISC_PRESENCE_CHECK) || (discMode == DISC_LOAD) ? 0xff : 0x01;

  // minutes (0xff means no data available):
  frame[5] = 0xff;

  // seconds (0xff means no data available):
  frame[6] = 0xff;

  // changer status:
  frame[7] = changerStatus;

  return frame;
}

void CDChanger::play() {
  discMode = PLAY;
}
