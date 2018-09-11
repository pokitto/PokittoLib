/**************************************************************************/
/*!
    @file     minibuttons.cpp
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#include <mbed.h>
#include "minibuttons.h"

using namespace mbed;

#define BS_IDLE 0
#define BS_DOWN 1
#define BS_UP 2

volatile unsigned char heldStates[NUM_BTN];

volatile unsigned char buttons_state, buttons_held, buttons_released; // from LSB up,down,left,right,a,b,c

void pokInitButtons() {
  //enable GPIO AHB clock
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 19) | (1 << 16) | (1 << 7));

  //Disable pin interrupts
  LPC_SYSCON->STARTERP0 = 0;
  LPC_SYSCON->PINTSEL[0] = 0;
  LPC_SYSCON->PINTSEL[1] = 0;
  LPC_SYSCON->PINTSEL[2] = 0;
  LPC_SYSCON->PINTSEL[3] = 0;
  LPC_SYSCON->PINTSEL[4] = 0;
  LPC_SYSCON->PINTSEL[5] = 0;
  LPC_SYSCON->PINTSEL[6] = 0;

  NVIC->ICER[0] = (1 << ((uint32_t)(0) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(1) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(2) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(3) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(4) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(5) & 0x1F));
  NVIC->ICER[0] = (1 << ((uint32_t)(6) & 0x1F));

  // Clear interrupts
  uint32_t* a;
  a = (uint32_t*)0xA000407F;
  *a = 0;
  //if (!(LPC_GPIO_X->ISEL & ch_bit))
  LPC_PINT->IST = 0x0;

  buttons_state = buttons_held = 0;

  heldStates[BTN_LEFT] = 0;
  heldStates[BTN_UP] = 0;
  heldStates[BTN_RIGHT] = 0;
  heldStates[BTN_DOWN] = 0;
  heldStates[BTN_A] = 0;
  heldStates[BTN_B] = 0;
  heldStates[BTN_C] = 0;
}

void pokPollButtons() {
    unsigned char buttons_state_old = buttons_state;
    buttons_state = 0; // clear all
    if (UBtn) buttons_state |= (1<<UPBIT);
    if (DBtn) buttons_state |= (1<<DOWNBIT);
    if (LBtn) buttons_state |= (1<<LEFTBIT);
    if (RBtn) buttons_state |= (1<<RIGHTBIT);
    if (ABtn) buttons_state |= (1<<ABIT);
    if (BBtn) buttons_state |= (1<<BBIT);
    if (CBtn) buttons_state |= (1<<CBIT);
    buttons_held = buttons_state & buttons_state_old; // only if both 1, end result is 1
    buttons_released = ~buttons_state & buttons_state_old; // if now zero, then 1 AND previous 1 = 1
}

unsigned char upHeld() { if (buttons_held & (1<<UPBIT)) return 1; return 0;}
unsigned char downHeld() { if (buttons_held & (1<<DOWNBIT)) return 1; return 0;}
unsigned char leftHeld() { if (buttons_held & (1<<LEFTBIT)) return 1; return 0;}
unsigned char rightHeld() { if (buttons_held & (1<<RIGHTBIT)) return 1; return 0;}
unsigned char aHeld() { if (buttons_held & (1<<ABIT)) return 1; return 0;}
unsigned char bHeld() { if (buttons_held & (1<<BBIT)) return 1; return 0;}
unsigned char cHeld() { if (buttons_held & (1<<CBIT)) return 1; return 0;}

unsigned char upReleased() { if (buttons_released & (1<<UPBIT)) return 1; return 0;}
unsigned char downReleased() { if (buttons_released & (1<<DOWNBIT)) return 1; return 0;}
unsigned char leftReleased() { if (buttons_released & (1<<LEFTBIT)) return 1; return 0;}
unsigned char rightReleased() { if (buttons_released & (1<<RIGHTBIT)) return 1; return 0;}
unsigned char aReleased() { if (buttons_released & (1<<ABIT)) return 1; return 0;}
unsigned char bReleased() { if (buttons_released & (1<<BBIT)) return 1; return 0;}
unsigned char cReleased() { if (buttons_released & (1<<CBIT)) return 1; return 0;}

unsigned char leftBtn(void) {
  if (leftHeld() == 1)
        return true;
    else
        return false;
}

unsigned char rightBtn(void) {
  if (rightHeld() == 1)
        return true;
    else
        return false;
}

unsigned char upBtn(void) {
  if (upHeld() == 1)
        return true;
    else
        return false;
}

unsigned char downBtn(void) {
  if (downHeld() == 1)
        return true;
    else
        return false;
}

unsigned char aBtn(void) {
  if (aHeld() == 1)
        return true;
    else
        return false;
}

unsigned char bBtn(void) {
  if (bHeld() == 1)
        return true;
    else
        return false;
}

unsigned char cBtn(void) {
  if (cHeld() == 1)
        return true;
    else
        return false;
}
