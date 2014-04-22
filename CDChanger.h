#ifndef __CDC_H__
#define __CDC_H__

#include "Arduino.h"

class CDChanger {
  public:
    enum DiscMode {
      STOP = 0x0,
      DISC_PRESENCE_CHECK = 0x1,
      DISC_LOAD = 0x2,
      TOC_READ = 0x3,
      PLAY = 0x4,
      TRACK_SEEK = 0x5,
      FF = 0x6,
      REW = 0x7,
      EJECT = 0x8,
      INTERRUPT_STOP = 0x9,
      PAUSE = 0xa,
      TEST_MODE = 0xe,
      ERROR = 0xf
    };
    
    enum ChangerStatus {
      CDC_ANTI_THEFT_MARRIED = 0x80,
      CDC_ANTI_THEFT_PASSED = 0x40,
      RANDOM_ON = 0x20,
      MAGAZINE_PRESENT = 0x10,
      DISC_SCAN = 0x08,
      MAGAZINE_SCAN = 0x04,
      DISC_REPEAT = 0x02,
      TRACK_REPEAT = 0x01    
    };

    CDChanger();
    
    byte* getGeneralStatusFrame(boolean basetime = true, boolean commandReply = false);
    
    void play();

  private:
    byte frame[8];
    DiscMode discMode;
    byte discNumber;
    byte changerStatus;
};

#endif
