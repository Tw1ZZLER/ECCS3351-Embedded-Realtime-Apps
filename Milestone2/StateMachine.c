// Milestone 2 State Machine and FSM Table
// Authors: Corbin Hibler, Cody Dusek, and Justin Schiefer
// Date: 2025-10-09

#include <stdint.h>

struct State {
  uint32_t out;
  uint32_t delay;
  uint32_t *Next[16];
};

typedef const struct State State_t;

#define s1 &fsm[0] // Straight
#define s2 &fsm[1] // Stop
#define s3 &fsm[2] // Reverse
#define s4 &fsm[3] // RotateRight
#define s5 &fsm[4] // DriftLeft
#define s6 &fsm[5] // DriftRight

State_t fsm[6] = {
    // Straight
    {0x05, 1, {s1, s5, s6, s1, s2, s2, s2, s2, s2, s2, s2, s2, s2, s2, s2, s2}},
    // Stop
    {0x0A, 1, {s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3}},
    // Reverse
    {0x0F, 1, {s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4, s4}},
    // RotateRight
    {0x0D, 1, {s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1}},
    // DriftLeft
    {0x0D, 1, {s1, s5, s1, s5, s1, s5, s1, s5, s1, s5, s1, s5, s1, s5, s1, s5}},
    // DriftRight
    {0x07, 1, {s1, s1, s6, s6, s1, s1, s6, s6, s1, s1, s6, s6, s1, s1, s6, s6}}
};
