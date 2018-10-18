/**************************************************************************/
/*!
    @file     JoyHat.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Jonne Valola
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

#ifndef __JOYHAT_H__
#define __JOYHAT_H__

#include "Pokitto.h"

extern DigitalOut* rumblepin;

#define _X_PIN EXT2
#define _Y_PIN EXT1

extern mbed::DigitalOut _rumble;
extern mbed::DigitalIn _b1;
extern mbed::DigitalIn _b2;
extern mbed::AnalogIn _joyx;
extern mbed::AnalogIn _joyy;

extern mbed::Timeout _rTimer;

extern void stoprumble();

class JoyHat {
public:
    int joyScale = 100;
    bool portraitOrientation=false;

    bool Button1() { return _b1.read();}
    bool Button2() { return _b2.read();}

    int JoyX() {
        if (portraitOrientation) { return _joyx.read()*joyScale;}
        else {
                return _joyy.read()*joyScale;
        }
    };

    int JoyY() {
        if (portraitOrientation) { return _joyy.read()*joyScale;}
        else {
                mbed::AnalogIn t(_X_PIN);
                volatile float a = 0;
                a = t.read();
                if (a) {
                    wait_ms(1); //
                }
                return a*joyScale;
        }
    };

    void Rumble(float t) {
        if (t==0) _rumble.write(0);
        else {
                _rumble.write(1);
                rumblepin = &_rumble;
                _rTimer.attach(&stoprumble, t); //workaround for attach
        }
    };

};

extern JoyHat joyhat;

#endif //__JOYHAT_H__
