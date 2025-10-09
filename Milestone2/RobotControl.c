// Milestone 2 Robot Control Functions
// Authors: Corbin Hibler, Cody Dusek, and Justin Schiefer
// Date: 2025-10-09

#include "StateMachine.c"
#include <stdint.h>

void MotorPinInit() {
  P3->SEL0 &= ~0xC0;
  P3->SEL1 &= ~0xC0;
  P3->DIR |= 0xC0;
  P3->OUT |= 0xC0;
}

void RobotForward() {
  P5->OUT &= ~0x30;
  P2->OUT |= 0xC0;
}

void RobotReverse() {
  P5->OUT |= 0x30;
  P2->OUT |= 0xC0;
}

void RobotTurnLeft() {
  P5->OUT |= 0x10;
  P5->OUT &= ~0x20;
  P2->OUT |= 0xC0;
}

void RobotTurnRight() {
  P5->OUT |= 0x20;
  P5->OUT &= ~0x10;
  P2->OUT |= 0xC0;
}

uint8_t ReadBumperValues() {
  // assume BMP5 and BMP0 are already set
  return (BMP5 << 5) | BMP0;
}

uint8_t ReadSensorValues() {
  // assume QTR5 and QTR1 are already set
  return (QTR5 << 5) | (QTR1 << 1);
}
