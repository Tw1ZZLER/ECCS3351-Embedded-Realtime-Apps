// Lab14_EdgeInterruptsmain.c
// Runs on MSP432, interrupt version
// Main test program for interrupt driven bump switches the robot.
// Daniel Valvano and Jonathan Valvano
// July 11, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

#include <stdint.h>
#include <stdbool.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "../inc/Motor.h"
#include "../inc/BumpInt.h"
#include "../inc/TExaS.h"
#include "../inc/TimerA1.h"
#include "../inc/FlashProgram.h"
#include "../inc/ADC14.h"
#include "../inc/IRDistance.h"

#define SENSOR_SAMPLES 5
#define PL 1
#define PR 1
#define MINIMUM_SPEED 2500
#define MAXIMUM_SPEED 4500
#define CENTER_COLLISION 110
#define LEFT_COLLISION 10
#define RIGHT_COLLISION 10
#define WHEEL_IMBALANCE 300

// Blue arrows (set amount of time)
#define BLUE_1 3072 // 6.0 seconds
#define BLUE_2 1280 // 2.5 seconds
#define BLUE_3 1408 // 2.75 seconds
#define BLUE_4 2176 // 4.25 seconds
#define BLUE_5 3072 // 6.0 seconds
#define BLUE_6 1280 // 2.5 seconds

// Initial setpoint
uint16_t setpoint = 160;

void ExecuteRoute(void);

// Mode based on Launchpad Button
bool mode = 0; // if mode == 0 is mode 1
               // if mode == 1 is mode 2

// Flags based on collisions
uint32_t timerCounter = 0;
uint8_t nextState = 0;
uint8_t pedestrianDetected = 0;
uint8_t collisionFlag = 0;

uint8_t pedestrianFlag = 0;
// 0 = unknown pedestrian location
// 1 - 7 = pedestrian is at 1 - 7 location


void HandleCollision(uint8_t bumpSensor){
   TimerA1_Stop();
   TimerA2_Stop();
   Motor_Stop();
   while(1){}
}

uint32_t readC, readL, readR;
int32_t distC, distL, distR;
int32_t errorL = 0;
int32_t errorR = 0;

int32_t U = 3000;
int32_t O = 3000;

bool leftPIDEnabled = true;
bool rightPIDEnabled = true;

void ResetTimerCounter(void) {
    timerCounter = 0;
}

void TurnLeft(void)
{
    U = 3000;
    O = 3000;

    Motor_Stop();
    Clock_Delay1ms(500);
    Motor_Left(2500,2500);
    Clock_Delay1ms(900);
}

void TurnRight(void)
{
    U = 3000;
    O = 3000;

    Motor_Stop();
    Clock_Delay1ms(500);
    Motor_Right(2500,2500);
    Clock_Delay1ms(900); // 500 = 90 degrees
}

void Turn180(void)
{
    U = 3000;
    O = 3000;

    Motor_Stop();
    Clock_Delay1ms(500);
    Motor_Left(2500,2500);
    Clock_Delay1ms(1800); // 1000 = 180 degrees
}

void PIDEdgeTriggeredInterrupt(void) {
    // IR Sensor Filtering Code
    distC = 0;
    distL = 0;
    distR = 0;
    for (uint8_t i = 0; i < SENSOR_SAMPLES; i++) {
        ADC_In6_14_15(&readC, &readL, &readR);
        distC += CenterConvert(readC);
        distL += LeftConvert(readL);
        distR += RightConvert(readR);
        Clock_Delay1ms(1);
    }
    distC /= SENSOR_SAMPLES;
    distL /= SENSOR_SAMPLES;
    distR /= SENSOR_SAMPLES;

    // IR Sensor collisions
    // Center collision handler
    // Check for pedestrian detection (only if not already detected)
    if (distC < CENTER_COLLISION) {
        collisionFlag = 1;
        ExecuteRoute();
        return;
    }
    else if (distL < LEFT_COLLISION) {
        Motor_Right(3000,3000);
        Clock_Delay1ms(100);
        return;
    }
    else if (distR < RIGHT_COLLISION) {
        Motor_Left(3000,3000);
        Clock_Delay1ms(100);
        return;
    }

    errorL = setpoint - distL;
    errorR = setpoint - distR;

    // Left PID Handler
    if ((errorL >= 5 || errorL <= -5) && leftPIDEnabled ) {
        Motor_Stop();
        if (errorL < 0) {
            U = U + PL * errorL;
        }
        if (errorL > 0) {
            U = U + PL * errorL;
        }
        if (U > MAXIMUM_SPEED) {
            U = MAXIMUM_SPEED;
        }
        if (U < MINIMUM_SPEED) {
            U = MINIMUM_SPEED;
        }
    }
    // Right PID Handler
    else if ((errorR >= 5 || errorR <= -5) && rightPIDEnabled) {
        Motor_Stop();
        if (errorR < 0) {
            O = O + PR * errorR;
        }
        if (errorR > 0) {
            O = O + PR * errorR;
        }
        if (O > MAXIMUM_SPEED) {
            O = MAXIMUM_SPEED;
        }
        if (O < MINIMUM_SPEED) {
            O = MINIMUM_SPEED;
        }
    } else {
        O = U;
    }

    Motor_Forward(O - WHEEL_IMBALANCE, U + WHEEL_IMBALANCE);
}

void FollowRightWall(void) {
    setpoint = 100;
    leftPIDEnabled = false;
    rightPIDEnabled = true;
}

void FollowLeftWall(void) {
    setpoint = 100;
    leftPIDEnabled = true;
    rightPIDEnabled = false;
}

void ResetWallFollow(void) {
    setpoint = 140;
    rightPIDEnabled = true;
    leftPIDEnabled = true;
}

// Main routing state machine
void ExecuteRoute(void) {
    // MODE 1 STATE MACHINE
    if (mode == 0) { // MODE 1 - Pedestrian at 1, 3, 5, or 7
        switch(nextState) {
            case 0: // State 0 (Start)
                if (collisionFlag == 1) {
                    nextState = 1;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 7;

                    TimerA2_Stop();
                    TurnLeft();
                    TimerA2_Init(&ExecuteRoute, BLUE_1);
                } else { // Complete
                    nextState = 2;
                    collisionFlag = 0;

                    TimerA2_Stop();
                    TurnRight();
                    FollowLeftWall();
                    TimerA2_Init(&ExecuteRoute, BLUE_2);
                }
                return;
            case 1: // State 1 (EC after blue arrow)
                // execute route 7 somehow
                nextState = 20;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                TimerA2_Init(&ExecuteRoute, BLUE_2);
                return;
            case 2: // State 2 (Completed first blue arrow)
                if (collisionFlag == 1) { // Early collision
                    nextState = 3;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 5;

                    TimerA2_Stop();
                    Turn180();
                } else { // Complete
                    nextState = 4;
                    collisionFlag = 0;

                    TimerA2_Stop();
                    TurnRight();
                }
                return;
            case 3: // State 3 (Early collision second blue arrow)
                // execute route 5 somehow
                nextState = 17;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 4: // State 4 (Completed second blue arrow)
                nextState = 5;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 5: // State 5 (Expected collision)
                nextState = 6;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 6: // State 6 (Expected collision)
                nextState = 7;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                TimerA2_Init(&ExecuteRoute, BLUE_3);
                return;
            case 7: // State 7 (Expected collision)
                if (collisionFlag == 1) { // Early Collision
                    nextState = 8;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 1;

                    TimerA2_Stop();
                    TurnLeft();
                    TimerA2_Init(&ExecuteRoute, BLUE_4);
                } else { // Complete
                    nextState = 9;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 3;
                    TimerA2_Stop();
                    TurnLeft();
                    ResetWallFollow();
                }
                return;
            case 8: // State 8 (Early collision third blue arrow)
                // execute route 1
                nextState = 10;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                TimerA2_Init(&ExecuteRoute, BLUE_6);
                return;
            case 9: // State 9 (Completed third blue arrow)
                // execute route 3
                nextState = 14;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                ResetWallFollow();
                return;
            case 10: // ROUTE 1 START
                nextState = 11;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                return;
            case 11:
                nextState = 12;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 12:
                nextState = 13;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 13: // STOP EVERYTHING!!!
                HandleCollision(0);
            case 14: // ROUTE 3 START
                nextState = 15;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                FollowLeftWall();
                return;
            case 15:
                nextState = 16;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnLeft();
                FollowRightWall();
                return;
            case 16:
                nextState = 13;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnLeft();
                FollowRightWall();
                return;
            case 17: // ROUTE 5 START
                nextState = 18;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                TimerA2_Init(&ExecuteRoute, BLUE_3);
                return;
            case 18: // State 18
                nextState = 19;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnLeft();
                ResetWallFollow();
                return;
            case 19:
                nextState = 4;
                collisionFlag = 0;

                Turn180();
                ResetWallFollow();
                return;
            case 20: // ROUTE 7 START
                nextState = 21;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                ResetWallFollow();
                return;
            case 21:
                nextState = 22;
                collisionFlag = 0;

                Turn180();
                ResetWallFollow();
                return;
            case 22:
                nextState = 23;
                collisionFlag = 0;

                TurnRight();
                ResetWallFollow();
                return;
            case 23:
                nextState = 24;
                collisionFlag = 0;

                TurnRight();
                FollowLeftWall();
                return;
            case 24:
                nextState = 5;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
        }
    // MODE 2 STATE MACHINE
    } else { // MODE 2 - Pedestrian at 2, 4, or 6
        switch(nextState) {
            case 0: // State 0 (Start)
                if (collisionFlag == 1) {
                    nextState = 1;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 6;

                    TimerA2_Stop();
                    TurnLeft();
                    ResetWallFollow();
                } else { // Complete
                    nextState = 2;
                    TimerA2_Stop();
                    TurnRight();
                    TimerA2_Init(&ExecuteRoute, BLUE_2);
                }
                return;
            case 1: // State 1 (EC after blue arrow)
                // execute route 6 somehow
                nextState = 16;
                collisionFlag = 0;

                TurnRight();
                ResetWallFollow();
                return;
            case 2: // State 2 (Completed first blue arrow)
                nextState = 3;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                return;
            case 3: // State 3 (Completed second blue arrow)
                nextState = 4;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 4: // State 4 (Expected collision)
                nextState = 5;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 5: // State 5 (Expected collision)
                nextState = 6;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                TimerA2_Init(&ExecuteRoute, BLUE_3);
                return;
            case 6: // State 6 (Expected collision)
                if (collisionFlag == 1) { // Early Collision
                    nextState = 7;
                    collisionFlag = 0;

                    P2->OUT |= 0x01;
                    pedestrianFlag = 2;

                    TimerA2_Stop();
                    TurnLeft();
                    ResetWallFollow();
                    TimerA2_Init(&ExecuteRoute, BLUE_5);
                } else { // Complete
                    nextState = 15;
                    collisionFlag = 0;

                    if (pedestrianFlag == 6) {
                        nextState = 10;
                        FollowLeftWall();
                    } else {
                        P2->OUT |= 0x01;
                        pedestrianFlag = 4;
                        ResetWallFollow();
                    }

                    TimerA2_Stop();
                    TurnLeft();
                }
                return;
            case 7: // ROUTE 2 START
                nextState = 8;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                ResetWallFollow();
                TimerA2_Init(&ExecuteRoute, BLUE_6);
                return;
            case 8:
                nextState = 9;
                collisionFlag = 0;

                TimerA2_Stop();
                TurnRight();
                ResetWallFollow();
                return;
            case 9:
                nextState = 10;
                collisionFlag = 0;

                Turn180();
                ResetWallFollow();
                return;
            case 10:
                nextState = 11;
                collisionFlag = 0;

                TurnRight();
                ResetWallFollow();
                return;
            case 11:
                nextState = 12;
                collisionFlag = 0;

                TurnRight();
                FollowLeftWall();
                return;
            case 12:
                nextState = 13;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 13:
                nextState = 14;
                collisionFlag = 0;

                TurnLeft();
                FollowRightWall();
                return;
            case 14: // STOP
                HandleCollision(0);
            case 15: // ROUTE 4 START
                nextState = 12;
                collisionFlag = 0;

                Turn180();
                ResetWallFollow();
                return;
            case 16:
                nextState = 3;
                collisionFlag = 0;

                TurnRight();
                FollowLeftWall();
                return;
        }
    }
}

int main(void) {
    Clock_Init48MHz();
    ADC0_InitSWTriggerCh6();
    Motor_Init();
    LaunchPad_Init();
    BumpInt_Init(&HandleCollision);

    P2->SEL0 &=~ 0x01;
    P2->SEL1 &=~ 0x01;
    P2->DIR |= 0x01;

    while (LaunchPad_Input() == 0) {}
    if (LaunchPad_Input() == 1) { // if right button
        mode = 1; // mode is mode 2
    } else {
        mode = 0; // else is mode 1
    }

    Motor_Forward(3000, 3000); // Start Driving Immediately
    TimerA1_Init(&PIDEdgeTriggeredInterrupt, 10000);  // Run PID every 20ms
    TimerA2_Init(&ExecuteRoute, BLUE_1); // First blue arrow, same for all possibilities and modes
    EnableInterrupts();

    while (1) {}
}
