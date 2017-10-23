/**************************************************************************/
/*!
    @file     SimSound.h
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

#ifndef POKSOUND_H
#define POKSOUND_H

#define SPEAKER 3
#define BUFFER_SIZE 512*8 //*8 // 512 // was 512 (works really well with crabator) was 256

#include "PokittoSimulator.h"
#include <stdint.h>
#include <stdio.h>

extern FILE* soundfile;
extern uint8_t soundfilebuffer[2][SFBUFSIZE];
extern uint8_t activesfbuf, writesfbuf; extern uint16_t sfbufindex;

namespace Pokitto {

extern void soundInit();
extern void pauseAudio(uint8_t);
}

#if POK_STREAMING_MUSIC > 0
    extern unsigned char buffers[][BUFFER_SIZE];
    extern volatile int currentBuffer, oldBuffer;
    extern volatile int bufindex, vol;
    extern volatile unsigned char * currentPtr;
    extern volatile unsigned char * endPtr;
    extern int8_t streamvol;
    extern uint32_t streamcounter;
    extern uint8_t streamstep, streamon;
#endif

#endif // POKSOUND_H

