/**************************************************************************/
/*!
    @file     PokittoConsole.cpp
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

#include "PokittoGlobs.h"
#include "PokittoConsole.h"
#include "PokittoFonts.h"
#include "PokittoDisplay.h"

Pokitto::Console pConsole;

using namespace Pokitto;

consmsg Console::msgbuf[CONSOLEBUFSIZE];
uint8_t Console::conslast;
uint8_t Console::consfirst;
uint8_t Console::conspointer;
uint16_t Console::conscounter;
uint16_t Console::color;
Display* Console::_display;
uint8_t Console::mode;
uint8_t Console::visible;
unsigned char* Console::font;

Console::Console() {
    conslast=consfirst=conspointer=conscounter=0;
    color=0xFFFF;
    font=(unsigned char*)fontMini;
    #if POK_CONSOLE_VISIBLE_AT_STARTUP > 0
        mode=CONS_OVERLAY|CONS_VISIBLE;
    #else
        mode=CONS_OVERLAY;//|CONS_VISIBLE;
    #endif
}

void Console::Toggle() {
    if (!conscounter) // conscounter is a debounce feature for console toggle
    {
        mode ^= CONS_VISIBLE;
        conscounter = CONS_TIMEOUT;
    }
}


void Console::AddMessage(uint8_t source, uint8_t msgtype) {
    AddMessage(source,msgtype, V_NONE,0);
}


void Console::AddMessage(uint8_t type, char* stringptr) {
    AddMessage(MSOURCE_USER,type,V_STRING, (uint32_t) stringptr);
}


void Console::AddMessage(uint8_t msgtype, uint8_t valtype, uint32_t val) {
    AddMessage(MSOURCE_USER,msgtype,valtype,val);
}

void Console::AddMessage(uint8_t source, uint8_t msgtype, uint8_t valtype, uint32_t val) {
    consmsg c;
    c.msgsource = source;
    c.msgtype = msgtype;
    c.valtype = valtype;
    c.val = val;
    msgbuf[conslast] = c;
    conslast++;
    if (conslast==CONSOLEBUFSIZE) conslast = 0; // wrap around the buffer
    consfirst = conslast + 1;
    if (consfirst == CONSOLEBUFSIZE) consfirst = 0;
}

void Console::Last() {
    conspointer = conslast;
}

void Console::RemoveLast() {
    Last();
    Previous();
    msgbuf[conspointer].msgsource = MSOURCE_USER;
    msgbuf[conspointer].msgtype = MSG_OK;
    msgbuf[conspointer].val = 0;
    msgbuf[conspointer].valtype = 0;
    conslast = conspointer;
}

void Console::First() {
    consmsg c;
    uint16_t outofbounds=0;
    conspointer = consfirst;
    c=GetMessage();
    while (!c.msgsource) {
        Next();
        c=GetMessage();
        outofbounds++;
        if (outofbounds==CONSOLEBUFSIZE) {
            conspointer = consfirst;
            break;
        }
    }
}

void Console::Previous() {
    if (conspointer==0) conspointer = CONSOLEBUFSIZE-1;
    else conspointer--;
}

void Console::Next() {
    if (conspointer==CONSOLEBUFSIZE-1) conspointer = 0;
    else conspointer++;
}

consmsg Console::GetMessage() {
    return msgbuf[conspointer];
}

void Console::PrintMessage() {
    consmsg c = msgbuf[conspointer];
    switch (c.msgsource) {
    case MSOURCE_APP:
        _display->print("App:");break;
    case MSOURCE_BATT:
        _display->print("Batt:");break;
    case MSOURCE_BTNA:
        _display->print("A:");break;
    case MSOURCE_BTNB:
        _display->print("B:");break;
    case MSOURCE_BTNC:
        _display->print("C:");break;
    case MSOURCE_BTNU:
        _display->print("Up:");break;
    case MSOURCE_BTND:
        _display->print("Down:");break;
    case MSOURCE_BTNL:
        _display->print("Left:");break;
    case MSOURCE_BTNR:
        _display->print("Right:");break;
    case MSOURCE_LCD:
        _display->print("LCD:");break;
    case MSOURCE_SD:
        _display->print("SD:");break;
    case MSOURCE_SOUND:
        _display->print("Sound:");break;
    case MSOURCE_TIMER:
        _display->print("Timer:");break;
    case MSOURCE_USER:
        _display->print("User:");break;
    case MSOURCE_COLLISION:
        _display->print("Hit:");break;
    default:
        return;
    }

    switch (c.msgtype) {
    case MSG_OK:
        _display->print("OK ");break;
    case MSG_INIT_OK:
        _display->print("INIT OK ");break;
    case MSG_INIT_FAIL:
        _display->print("INIT FAIL ");break;
    case MSG_WARNING:
        _display->print("WARNING ");break;
    case MSG_FATAL:
        _display->print("FATAL ");break;
    case MSG_GFX_MODE_CHANGE:
        _display->print("MODE ");break;
    case MSG_GFX_MODE_INVALID:
        _display->print("INVALID!");break;
    case MSG_NOT_ENOUGH_MEM:
       _display->print("NOT ENOUGH MEM ");break;
    case MSG_UP:
        _display->print("KEYUP");break;
    case MSG_DOWN:
        _display->print("KEYDOWN");break;
    case MSG_BREAK:
        _display->print("BREAK ");break;
    case MSG_YESNO:
        _display->print("YES(A)/NO(B)");break;
    case MSG_YES:
        _display->print("YES");break;
    case MSG_NO:
        _display->print("NO");break;
    case MSG_OBJECT:
        _display->print("OBJ1: ");break;
    case MSG_OBJECT2:
        _display->print("OBJ2: ");break;
    default:
        _display->print(" ");
    }

    switch (c.valtype) {
    case V_NONE:
        _display->println();break;
    case V_UINT8:
        _display->println((uint16_t)c.val);break;
    case V_INT8:
        _display->println((int16_t)c.val);break;
    case V_UINT16:
        _display->println((uint16_t)c.val);break;
    case V_INT16:
        _display->println((int16_t)c.val);break;
    case V_UINT32:
        _display->println((uint32_t)c.val);break;
    case V_INT32:
        _display->println((int32_t)c.val);break;
    case V_FLOAT:
        _display->println((float)c.val);break;
    case V_STRING:
        _display->println((char*)c.val);break;
    default:
        _display->println();
    }

    if (c.msgtype == MSG_BREAK || c.msgtype == MSG_YESNO) {
        #ifndef POK_SIM
        __disable_irq();    // Disable Interrupts
        #else
        pauseAudio(1);
        visible = false; // avoid loop in loop inside refreshDisplay
        simulator.wait_ms(300);
        #endif
        int rval=0;
        while(!rval) {
                _buttons->pollButtons();
                #ifdef POK_SIM
                    simulator.refreshDisplay();
                #endif // POK_SIM
                if (_buttons->aBtn()) rval = 1;
                if (_buttons->bBtn()) rval = -1;
                }
        /* now remove this message to prevent it from rerunning */
        if (c.msgtype == MSG_YESNO) {
            int8_t* v;
            v = (int8_t*) c.val; //c.val is a pointer to the memory location that receives answer
            *v = rval; // return value through pointer
            msgbuf[conspointer].msgsource = MSOURCE_USER;
            if (rval==1) msgbuf[conspointer].msgtype = MSG_YES;
            else msgbuf[conspointer].msgtype = MSG_NO;
        } else msgbuf[conspointer].msgsource = MSOURCE_NULL;

        #ifndef POK_SIM
        __enable_irq();     // Enable Interrupts
        #else
        pauseAudio(0);
        visible = true;
        #endif
    }
}

void Console::Draw() {
    unsigned char* oldfont;
    int16_t oldx,oldy;
    uint16_t oldcolor;
    uint8_t oldsize;
    oldfont = (unsigned char*) _display->font;
    oldx = _display->cursorX;
    oldy = _display->cursorY;
    oldcolor = _display->directcolor;
    oldsize = _display->fontSize;
    if (!(mode & CONS_OVERLAY)) _display->clearLCD();
    _display->setFont(font5x7);
    _display->fontSize=1;
    _display->setCursor(0,0);
    _display->directcolor=color;
    First();
    while(conspointer!=conslast) {
        PrintMessage();
        Next();
        if (GetMessage().msgsource==MSOURCE_NULL) break;
    }
    _display->font = oldfont;
    _display->fontSize = oldsize;
    _display->cursorX = oldx;
    _display->cursorY = oldy;
    _display->directcolor = oldcolor;
}






