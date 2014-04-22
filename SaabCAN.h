#ifndef __SAABCAN_H__
#define __SAABCAN_H__

// Node addresses:
#define CDC_ADDR    0x12

// Frames:
#define CDC_CONTROL         0x3c0 // Base time 1000 ms, Update time 100 ms
#define CDC_GENERAL_STATUS  0x3c8 // Base time 1000 ms, Update time 50 ms

#define SID_BUTTONS         0x290 // Base time 1000 ms, Update time 100 ms

#define NODE_STATUS_IHU     0x6a1 // Update time 140 ms
#define NODE_STATUS_CDC     0x6a2 // Update time 140 ms

#endif
