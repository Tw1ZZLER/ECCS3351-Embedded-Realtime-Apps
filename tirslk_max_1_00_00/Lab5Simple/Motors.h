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
