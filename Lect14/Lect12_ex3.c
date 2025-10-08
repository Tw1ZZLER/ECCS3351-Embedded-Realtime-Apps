#include <stdint.h>
void SysTick_Wait(uint32_t n) {
  SysTick->LOAD = n - 1;
  SysTick->VAL = 0;
  while ((SysTick->CTRL & 0x00010000) == 0)
    ;
}
void SysTick_Wait10ms(uint32_t n) {
  for (uint16_t i = 0; i < n; i++) {
    SysTick_Wait(240000); // wait 10ms (assumes 24 MHz clock)
  }
}
