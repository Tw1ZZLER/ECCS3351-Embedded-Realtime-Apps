#include "msp.h"
#include <stdint.h>
#include "Motors.h"
#include "..\inc\SysTick.h"
#include "..\inc\Clock.h"

//Initializes the 6 GPIO lines and puts DRV8838 drivers to sleep
//Returns right away
void Motor_Init(void) {
    // nSLEEP
    P3->SEL0 &= ~0xC0;
    P3->SEL1 &= ~0xC0;
    P3->DIR |= 0xC0;

    // PH (phase thingy)
    P5->SEL0 &= ~0x30;
    P5->SEL1 &= ~0x30;
    P5->DIR |= 0x30;

    // EN (enable)
    P2->SEL0 &= ~0xC0;
    P2->SEL1 &= ~0xC0;
    P2->DIR |= 0xC0;
    Motor_Stop();
    return;
}

//Stops both motors, puts DRV8838 drivers to sleep
//Returns right away
void Motor_Stop(void) {
    P5->OUT &= ~0x30;
    P2->OUT &= ~0xC0;
    P3->OUT &= ~0xC0; // nSLEEP
    return;
}

//Drives both motors forward at full speed
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_ForwardSimple(uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT &= ~0x30;
    P2->OUT |= 0xC0;
    SysTick_Wait10ms(time);
    Motor_Stop();
    return;
}

//Drives both motors backward at full speed
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_ReverseSimple(uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x30;
    P2->OUT |= 0xC0;
    SysTick_Wait10ms(time);
    Motor_Stop();
    return;
}

//Drives just the right motor forward at full speed
//Left motor is stopped (sleeping)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_LeftSimple(uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x10;
    P5->OUT &= ~0x20;
    P2->OUT |= 0xC0;
    SysTick_Wait10ms(time);
    Motor_Stop();
    return;
}

//Drives just the left motor forward at full speed
//Right motor is stopped (sleeping)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_RightSimple(uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x20;
    P5->OUT &= ~0x10;
    P2->OUT |= 0xC0;
    SysTick_Wait10ms(time);
    Motor_Stop();
    return;
}

uint32_t i = 0;

//Drives both motors forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Forward(uint16_t duty, uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT &= ~0x30;
    for (i = 0; i < time; i++) {
        P2->OUT |= 0xC0;
        SysTick_Wait(duty*48);
        P2->OUT &= ~0xC0;
        SysTick_Wait((10000-duty)*48);
    }
    Motor_Stop();
    return;
}

//Drives both motors backward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Reverse(uint16_t duty, uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x30;
    for (i = 0; i < time; i++) {
        P2->OUT |= 0xC0;
        SysTick_Wait(duty*48);
        P2->OUT &= ~0xC0;
        SysTick_Wait((10000-duty)*48);
    }
    Motor_Stop();
    return;
}

//Drives the right motor forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Left motor is going in reverse at duty to complete a ‘zero-turn’
//Runs for time duration (units=10ms), and then stops
//Returns after time*10ms
void Motor_Left(uint16_t duty, uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x10;
    P5->OUT &= ~0x20;
    for (i = 0; i < time; i++) {
        P2->OUT |= 0xC0;
        SysTick_Wait(duty*48);
        P2->OUT &= ~0xC0;
        SysTick_Wait((10000-duty)*48);
    }
    Motor_Stop();
    return;
}

//Drives the left motor forward at duty (acceptable duty inputs are 100 to 9900, units of us)
//Right motor is going in reverse at duty (100 to 9900) to complete a ‘zero-turn’
//Runs for time duration (units=10ms), and then stops
//Returns after time*10m
void Motor_Right(uint16_t duty, uint32_t time) {
    P3->OUT |= 0xC0; // nSLEEP
    P5->OUT |= 0x20;
    P5->OUT &= ~0x10;
    for (i = 0; i < time; i++) {
        P2->OUT |= 0xC0;
        SysTick_Wait(duty*48);
        P2->OUT &= ~0xC0;
        SysTick_Wait((10000-duty)*48);
    }
    Motor_Stop();
    return;
}


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();

	SysTick_Init();
	Motor_Init();

	// Wait 2 sec
	SysTick_Wait10ms(200);

	// Drive forward full speed for 1 second
	Motor_Forward(9900, 100);

	// Turn left at half speed for 90 degrees
	Motor_Left(5000, 40);

	// Go forward at PWM duty 1000 for 2 sec
	Motor_Forward(1000, 200);

	// Right at half speed for 180 degrees
	Motor_Right(5000, 50);

	// Drive in reverse for 3 sec at PWM value of 2000
	Motor_Reverse(2000, 300);

	// Drive forward for 1 second
	Motor_Forward(7000, 100);
}
