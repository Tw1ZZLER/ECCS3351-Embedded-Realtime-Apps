#include "msp.h"
#include <stdint.h>
#include "Motors.h"
#include "..\inc\SysTick.h"
#include "..\inc\Clock.h"

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();

	SysTick_Init();
	Motor_Init();

	// Drive forward 1 sec
	Motor_ForwardSimple(100);

	// Turn right half sec
	Motor_RightSimple(50);

	// Go forward half sec
	Motor_ForwardSimple(50);

	// Reverse for 1 sec
	Motor_ReverseSimple(100);
}
