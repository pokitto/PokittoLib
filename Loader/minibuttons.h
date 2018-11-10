/**************************************************************************/
/*!
    @file     minibuttons.h
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

#ifndef POKITTO_BUTTONS_H
#define POKITTO_BUTTONS_H

#define BTN_UP      1
#define BTN_RIGHT   2
#define BTN_DOWN    3
#define BTN_LEFT    0
#define BTN_A       4
#define BTN_B       5
#define BTN_C       6
#define NUM_BTN     7

#define UPBIT    0
#define DOWNBIT  1
#define LEFTBIT  2
#define RIGHTBIT 3
#define ABIT     4
#define BBIT     5
#define CBIT     6

extern volatile unsigned char heldStates[];

extern void pokInitButtons();
extern void pokDeleteButtons();
extern void pokPollButtons();
extern unsigned char leftBtn();
extern unsigned char rightBtn();
extern unsigned char upBtn();
extern unsigned char downBtn();
extern unsigned char aBtn();
extern unsigned char bBtn();
extern unsigned char cBtn();

extern DigitalIn ABtn;
extern DigitalIn BBtn;
extern DigitalIn CBtn;
extern DigitalIn UBtn;
extern DigitalIn DBtn;
extern DigitalIn LBtn;
extern DigitalIn RBtn;
/*
extern unsigned char leftHeld();
extern unsigned char rightHeld();
extern unsigned char upHeld();
extern unsigned char downHeld();
extern unsigned char aHeld();
extern unsigned char bHeld();
extern unsigned char cHeld();

extern unsigned char leftReleased();
extern unsigned char rightReleased();
extern unsigned char upReleased();
extern unsigned char downReleased();
extern unsigned char aReleased();
extern unsigned char bReleased();
extern unsigned char cReleased();
*/

#define POK_BTN_A_PIN   P1_9
#define POK_BTN_B_PIN   P1_4
#define POK_BTN_C_PIN   P1_10
#define POK_BTN_UP_PIN   P1_13
#define POK_BTN_DOWN_PIN   P1_3
#define POK_BTN_LEFT_PIN   P1_25
#define POK_BTN_RIGHT_PIN   P1_7

#define UPBIT    0
#define DOWNBIT  1
#define LEFTBIT  2
#define RIGHTBIT 3
#define ABIT     4
#define BBIT     5
#define CBIT     6

#endif
