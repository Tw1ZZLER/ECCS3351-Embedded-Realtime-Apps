#include <stdint.h>
void SysTick_Wait10ms(uint32_t n) {
  for (uint16_t i = 0; i < n; i++) {
    SysTick_Wait(480000); // wait 10ms (assumes 48 MHz clock)
  }
}
