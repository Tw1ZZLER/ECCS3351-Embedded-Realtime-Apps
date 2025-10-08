#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

struct State {
  uint32_t out;
  uint32_t delay;
  uint32_t *Next[4];
};

typedef const struct State State_t;

#define s1 &fsm[0]
#define s2 &fsm[1]

State_t fsm[2] = {
    {0, 1000000, {s1, s1, s2, s2}}, // State 1
    {1, 1000000, {s2, s1, s2, s1}}  // State 2
};
