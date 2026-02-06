#include "msp.h"
#include <stdint.h>
#include "..\inc\Clock.h"


/**
 * main.c
 */
void LED_Init(void) {
    P1->SEL0 &= ~0x01;
    P1->SEL1 &= ~0x01;
    P1->DIR |= 0x01;
}

void LED_On(void) {
    P1->OUT |= 0x01;
}

void LED_Off(void) {
    P1->OUT &= ~0x01;
}

int main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();
	LED_Init();
	while(1){
	    LED_On();
	    Clock_Delay1ms(100);
	    LED_Off();
	    Clock_Delay1ms(100);
	}
}
