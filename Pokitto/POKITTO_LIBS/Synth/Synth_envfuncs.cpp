/**************************************************************************/
/*!
    @file     Synth_envfuncs.cpp
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

#include "Synth.h"

/** ENVELOPE FUNCTIONS **/

void noADSR(OSC* o){
}

void attackFunc(OSC* o){
    if (o->adsrvol >= o->vol) {
        if (o->decay) {
            o->adsrvol = o->vol;
            o->adsrphase = 2;
        } else {
            o->adsrvol = o->sustain;
            o->adsrphase = 3;
        }
        return;
    }
    o->adsrvol += o->attack;
}

void decayFunc(OSC* o){
    if (o->adsrvol >= o->sustain + o->decay) {
        o->adsrvol -= o->decay;
        return;
    } else {
        o->adsrvol = o->sustain;
        o->adsrphase = 3;
    }
}

void releaseFunc(OSC* o){
    if (o->adsrvol > o->release) o->adsrvol -= o->release;
    else o->adsrvol = 0;
    if (!o->adsrvol) { // we have hit zero volume level
        if (o->loop) {
                if (o->attack) {
                    o->adsrvol = 0;
                } else {
                    o->adsrvol = o->vol;
                }
                if (o->echo) o->echodiv++;
                o->adsrphase = 1;
                return;
        }
        else {
                o->adsrphase = 0;
                o->adsrvol = 0;
                return;
        }
    }
}




