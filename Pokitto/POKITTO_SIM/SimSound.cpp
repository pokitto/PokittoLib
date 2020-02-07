/**************************************************************************/
/*!
    @file     SimSound.cpp
    @author   Jonne Valola

    @section  LICENSE

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

#include "SimSound.h"
#include "PokittoSimulator.h"
#include "PokittoGlobs.h"
#include "Synth.h"
#include <stdint.h>

uint16_t soundbyte;

#ifdef SOUNDCAPTURE
FILE* soundfile;
uint8_t soundfilebuffer[2][SFBUFSIZE];
uint8_t activesfbuf=0, writesfbuf=0; uint16_t sfbufindex=0;
#endif // SOUNDCAPTURE

using namespace Pokitto;

void Pokitto::soundInit() {
    soundInit(false);
}


void Pokitto::soundInit(uint8_t reinit) {
    if (!reinit) {
    simulator.initSDLAudio();
    simulator.simSoundEnabled(true);
    pokPauseStream();
    }
}

void Pokitto::pauseAudio(uint8_t v) {
    SDL_PauseAudio(v);
}


#if (POK_STREAMING_MUSIC > 0)
    unsigned char buffers[4][BUFFER_SIZE];
    volatile int currentBuffer = 0, oldBuffer = 0;
    volatile int bufindex = 0, vol=1;
    volatile unsigned char * currentPtr;
    volatile unsigned char * endPtr;
    int8_t streamvol=3;
    uint32_t streamcounter=0;
    uint8_t streamstep=0;
#endif

uint8_t streambyte,output,streamon=0;
uint8_t test=false;
float test2=0;

uint8_t soundbuf[SBUFSIZE];
uint16_t soundbufindex=0;

uint8_t pokStreamPaused() {
    return !streamon;
}

/** pokPauseStream
* Used for pausing sound stream from SD card.
*/
void pokPauseStream() {
    streamon=0;
}

void pokPlayStream() {
    streamon=1;
}

// *** INCLUDE THE COMMON IMPLEMENTATION FOR HWSound.cpp and SimSound.cpp.
#include "../POKITTO_CORE/SoundCommonImp.h"

/** THE FAKE ISR **/

void fakeISR(void){
  #if POK_GBSOUND > 0
  /** GAMEBUINO SOUND **/
    #ifdef POK_SIM
        Pokitto::audio_IRQ();
    #endif // POK_SIM
  #else
  /** NOT GAMEBUINO SOUND **/
    pokSoundIRQ();
  #endif // POK_GAMEBUINO_SUPPORT
}

#ifndef WIN32

#define SAVECPU  4
uint8_t savecpu=1;

#endif // WIN32
