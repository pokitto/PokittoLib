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
    simulator.initSDLAudio();
    simulator.simSoundEnabled(true);
    pokPauseStream();
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

uint8_t soundbuf[256], soundbufindex=0;

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

void pokSoundIRQ() {
    #ifndef POK_SIM
    pwmout_t* obj = &audiopwm;
    #endif
    #if POK_STREAMING_MUSIC > 0
        #if POK_STREAMFREQ_HALVE
        streamstep = 1-streamstep;
        #else
        streamstep = 1;
        #endif // POK_STREAMFREQ_HALVE
        streamstep &= streamon; // streamon is used to toggle SD music streaming on and off
        if (streamstep) {
            uint8_t output = (*currentPtr++);
            if(streamvol) {
                output >>= 3-streamvol;
                streambyte = output;
            } else {
                streambyte = 0; // duty cycle
                output = 0;
            }
            if (currentPtr >= endPtr)
            {
            currentBuffer++;
            if (currentBuffer==4) currentBuffer=0;
            currentPtr = buffers[currentBuffer];
            endPtr = currentPtr + BUFFER_SIZE;
            }
        }
    #endif // POK_STREAMING_MUSIC

    /** DO ADDITIONAL SOUND PROCESSING (NOT STREAM) OF SOUND HERE **/

    #if POK_ENABLE_SYNTH
        /** if song is being played from sd **/
        if (playing) {
                notetick++;
                updatePlaybackSD(playerpos&7);
        }
        /** oscillators update **/
        osc1.count += osc1.cinc + (osc1.pitchbend >> 4); // counts to 65535 and overflows to zero WAS 8 !
        osc2.count += osc2.cinc + (osc2.pitchbend >> 4); // counts to 65535 and overflows to zero
        osc3.count += osc3.cinc + (osc3.pitchbend >> 4); // counts to 65535 and overflows to zero
        Marr[tick](); // call mixing function
        --tick;

        /** mixing oscillator output **/
        #ifdef POK_SIM
        uint16_t op = (uint16_t) ((osc1.output)*(osc1.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc2.output)*(osc2.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc3.output)*(osc3.vol>>8))>>9;// >> 2 osc1.vol Marr;
        #else
        uint16_t op = (uint16_t) ((osc1.output)*(osc1.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc2.output)*(osc2.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc3.output)*(osc3.vol>>8))>>9;// >> 2 osc1.vol Marr;
        #endif //POK_SIM
        output = (uint8_t) op;

    #endif // POK_ENABLE_SYNTH

    #ifndef POK_SIM
    /** HARDWARE **/
        #if POK_ENABLE_SOUND
            #if POK_STREAMING_MUSIC > 0
                #if POK_STREAM_TO_DAC > 0
                    #if POK_USE_DAC > 0
                        if (streamstep) dac_write((uint8_t)streambyte); // duty cycle
                    #endif // POK_USE_DAC
                #else
                    if (streamstep) {
                            uint32_t t_on = (uint32_t)(((obj->pwm->MATCHREL0)*streambyte)>>8); //cut out float
                            obj->pwm->MATCHREL1 = t_on;
                    }
                #endif // POK_STREAM_TO_DAC
            #endif // POK_STREAMING_MUSIC

            #if POK_STREAM_TO_DAC == 0
                #if POK_USE_DAC > 0
                    dac_write((uint8_t)output); // write synth output to DAC
                #endif
            #else
                uint32_t t_on = (uint32_t)(((obj->pwm->MATCHREL0)*output)>>8); //cut out float
                obj->pwm->MATCHREL1 = t_on;
            #endif // POK_SYNTH_TO_DAC
            soundbuf[soundbufindex++]=output;
        #endif //POK_ENABLE_SOUND
    #endif // HARDWARE

    #ifdef POK_SIM
    /** SIMULATOR **/
        #if POK_STREAMING_MUSIC
            //if (streamstep) {
                uint16_t o = output + streambyte;
                output = o/2;
            //}
        #endif // STREAMING
        soundbyte = output;
        soundbuf[soundbufindex++]=output;
    #endif // POK_SIM
}


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
