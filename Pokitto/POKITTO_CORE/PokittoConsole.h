/**************************************************************************/
/*!
    @file     PokittoConsole.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifndef POKITTO_CONSOLE_H
#define POKITTO_CONSOLE_H

#include "Pokitto_settings.h"
#include "PokittoButtons.h"
#include <stdint.h>

struct consmsg {
  uint32_t val;
  uint8_t valtype;
  uint8_t msgsource;
  uint8_t msgtype;
};

/** VAL TYPES **/
#define V_UINT8     0
#define V_INT8      1
#define V_UINT16    2
#define V_INT16     3
#define V_UINT32    4
#define V_INT32     5
#define V_FLOAT     6
#define V_STRING    7
#define V_NONE      8

/** MESSAGE TYPE **/
#define MSG_NULL        0
#define MSG_OK          1
#define MSG_WARNING     2
#define MSG_FATAL       255
#define MSG_INIT_OK     3
#define MSG_INIT_FAIL   4
#define MSG_NOT_ENOUGH_MEM  5
#define MSG_GFX_MODE_CHANGE 6
#define MSG_GFX_MODE_INVALID 7
#define MSG_UP          8
#define MSG_DOWN        9
#define MSG_PRINT       10
#define MSG_BREAK       11
#define MSG_YESNO       12
#define MSG_YES         13
#define MSG_NO          14
#define MSG_OBJECT      15
#define MSG_OBJECT2     16

/** MESSAGE SOURCE **/
#define MSOURCE_NULL    0
#define MSOURCE_SD      1
#define MSOURCE_LCD     2
#define MSOURCE_SOUND   3
#define MSOURCE_TIMER   4
#define MSOURCE_BTNA    5
#define MSOURCE_BTNB    6
#define MSOURCE_BTNC    7
#define MSOURCE_BTNU    8
#define MSOURCE_BTND    9
#define MSOURCE_BTNL    10
#define MSOURCE_BTNR    11
#define MSOURCE_BATT    12
#define MSOURCE_APP     13
#define MSOURCE_USER    14
#define MSOURCE_COLLISION 15

/** CONSOLE MODES **/
#define CONS_OVERLAY    0x1
#define CONS_PAUSE      0x2
#define CONS_STEP       0x4
#define CONS_VISIBLE    0x8


/** CONSOLE DEBOUNCE **/
#define CONS_TIMEOUT   20
extern uint16_t conscounter;



namespace Pokitto {

class Display;

class Console {
public:
    /** Console class constructor */
    Console();

    static uint8_t mode;
    static uint8_t visible;
    static unsigned char* font;
    static void Toggle();
    static void AddMessage(uint8_t, uint8_t, uint8_t, uint32_t);
    static void AddMessage(uint8_t, uint8_t);
    static void AddMessage(uint8_t, char*);
    static void AddMessage(uint8_t, uint8_t, uint32_t);
    static void Last();
    static void First();
    static void RemoveLast();
    static void Previous();
    static void Next();
    static consmsg GetMessage();
    static void PrintMessage();
    static void Purge();
    static void Draw(); // pokConsole

private:
    /** Message buffer */
    static consmsg msgbuf[CONSOLEBUFSIZE];
    static uint8_t conslast;
    static uint8_t consfirst;
    static uint8_t conspointer;
public:
    static uint16_t conscounter;
    static uint16_t color;
    static Display* _display;
    static Buttons* _buttons;
};


} // namespace

// this is the console used by the core if POK_USE_CONSOLE is nonzero
extern Pokitto::Console console;

#endif // POKITTO_CONSOLE_H

