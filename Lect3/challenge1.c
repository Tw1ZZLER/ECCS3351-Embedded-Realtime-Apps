#include <stdint.h>
#include <stdio.h>

int main(void) {
  uint8_t x = 4; // 0000 0100
  x = x << 4;    // 0100 0000
  printf("%x\n", x);
  x = x << 4; // 0000 0000
  printf("%x\n", x);
  x = x >> 4; // 0000 0000
  printf("%x\n", x);
  x = x >> 4; // 0000 0000
  printf("%x\n", x);
  x = x >> 4; // 0000 0000
  printf("%x\n", x);
}
