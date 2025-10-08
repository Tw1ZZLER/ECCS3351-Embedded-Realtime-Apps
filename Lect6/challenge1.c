#include <stdint.h>
#include <stdio.h>

int main(void) {
  uint8_t LED1 = 0x00;
  LED1 |= 0x03; // Set LED1 to binary 00000011
  printf("LED1 state: 0x%02X\n", LED1);
  LED1 <<= 2; // Shift left by 2 bits, LED1 becomes 00001100
  printf("LED1 state: 0x%02X\n", LED1);
  LED1 <<= 2;
  printf("LED1 state: 0x%02X\n", LED1);
  LED1 <<= 2;
  printf("LED1 state: 0x%02X\n", LED1);
  return 0;
}
