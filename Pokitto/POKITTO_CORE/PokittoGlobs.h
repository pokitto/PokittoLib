/**************************************************************************/
/*!
    @file     PokittoGlobs.h
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

#ifndef POKITTO_GLOBS_H
#define POKITTO_GLOBS_H

#include <stdint.h>

#ifdef POK_SIM
    #include "SimLCD.h"
    #include "SimSound.h"
    #include "PokittoSimulator.h"
#else
    #include "mbed.h"
    #include "HWLCD.h"
    #include "HWSound.h"
#endif // POK_SIM

extern int random(int);
extern int random(int,int);

#define HIGH    1
#define LOW     0

#define swapWT(type, a, b)    \
{               \
    type _t_;   \
    _t_ = a;    \
    a = b;      \
    b = _t_;    \
}

#define SAMPLE_RATE POK_AUD_FREQ //was 16000, was 57000
#define NUMFRAMES 570 //1 ms before refresh was 357


// TODO: move these into some suitable place
extern void fakeISR();
extern void audio_IRQ();
extern void update_SDAudioStream();
extern uint16_t soundbyte;

#if POK_STREAMING_MUSIC
    #define SPEAKER 3
   // #define BUFFER_SIZE 512 //5120 // was 256
    extern unsigned char buffers[][BUFFER_SIZE];
    extern volatile int currentBuffer, oldBuffer;
    extern volatile int bufindex, vol;
    extern volatile unsigned char * currentPtr;
    extern volatile unsigned char * endPtr;
    extern int8_t streamvol;
    extern uint32_t streamcounter;
    extern uint8_t streamstep;
#endif

#endif // POKITTO_GLOBS_H

