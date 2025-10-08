#include <stdint.h>
#include <stdio.h>

int main(void) {
  for (int i = 0; i < 2; i++) {
    uint8_t LED1 = 0x00;
    LED1 |= 0x03; // 0x03
    printf("LED1 state: 0x%02X\n", LED1);
    LED1 |= 0x30; // 0x33
    printf("LED1 state: 0x%02X\n", LED1);
    LED1 &= ~0x03; // 0x30
    printf("LED1 state: 0x%02X\n", LED1);
    LED1 |= 0x0C; // 0x3C
    printf("LED1 state: 0x%02X\n", LED1);
    LED1 &= ~0x30; // 0x0C
    printf("LED1 state: 0x%02X\n", LED1);
    LED1 |= 0x03; // 0x0F
    printf("LED1 state: 0x%02X\n", LED1);
  }
  return 0;
}
