/**************************************************************************/
/*!
    @file     Synth_mixfuncs.cpp
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
#include "Synth.h"

/** MIXING FUNCTIONS **/

char voltick=0; // i need to make volume changes even slower
uint16_t arptick=0; // i need to make volume changes even slower
int8_t bendtick = 0; // ditto for bend.


void mix1(){
    // Track 1
    if (osc1.on) {
    Farr[osc1.wave](&osc1);
    #if PROJ_ARDUBOY > 0
    if (osc1.duration) {
        /**this is special for osc1 and is only used to emulate arduino Tone(); */
        osc1.duration--;
    } else osc1.on = 0;
    #endif

    #ifdef POK_SIM
    soundbyte = (((osc1.output>>8) * (osc1.adsrvol >>8 )) >> 8) >> osc1.echodiv; // To output, shift back to 8-bit
    if (osc1.overdrive) soundbyte *= OVERDRIVE;
    if (osc1.kick ) soundbyte >>= 2;
    osc1.output = soundbyte;
    #else
    //OCR2B = osc1.output>>8;
    #if POK_ENABLE_SOUND > 1
    soundbyte = (((osc1.output>>8) * (osc1.adsrvol >>8 )) >> 8) >> osc1.echodiv; // To output, shift back to 8-bit
    if (osc1.overdrive) soundbyte *= OVERDRIVE;
    if (osc1.kick ) soundbyte >>= 2;
    osc1.output = soundbyte;
    #endif
    #endif
    }
}

void mix2(){
    // Track 2
    if (osc2.on) {
    Farr[osc2.wave](&osc2);
    #ifdef POK_SIM
    soundbyte = (((osc2.output>>8) * (osc2.adsrvol >>8 )) >> 8) >> osc2.echodiv;
    if (osc2.overdrive) soundbyte *= OVERDRIVE;
    if (osc2.kick ) soundbyte >>= 2;
    osc2.output = soundbyte;
    #else
    //OCR2B = osc2.output>>8;
    #if POK_ENABLE_SOUND > 1
    soundbyte = (((osc2.output>>8) * (osc2.adsrvol >>8 )) >> 8) >> osc2.echodiv;
    if (osc2.overdrive) soundbyte *= OVERDRIVE;
    if (osc2.kick ) soundbyte >>= 2;
    osc2.output = soundbyte;
    #endif
    #endif
    }
}

void mix3(){
    // Track 3
    if (osc3.on) {
    Farr[osc3.wave](&osc3);
    #ifdef POK_SIM
    soundbyte = (((osc3.output>>8) * (osc3.adsrvol >>8 )) >> 8) >> osc3.echodiv;
    if (osc3.overdrive) soundbyte *= OVERDRIVE;
    if (osc3.kick ) soundbyte >>= 2;
    osc3.output = soundbyte;
    #else
    //OCR2B = osc3.output>>8;
    #if POK_ENABLE_SOUND > 1
    soundbyte = (((osc3.output>>8) * (osc3.adsrvol >>8 )) >> 8) >> osc3.echodiv;
    if (osc3.overdrive) soundbyte *= OVERDRIVE;
    if (osc3.kick ) soundbyte >>= 2;
    osc3.output = soundbyte;
    #endif
    #endif
    }
}

void updateEnvelopes(){
    //calculate volume envelopes, I do this to save cpu power
    if (arptick) --arptick;
    else {
            if (osc1.arpmode && osc1.on) {
                osc1.cinc = cincs[osc1.tonic+arptable[osc1.arpmode][osc1.arpstep]];
                osc1.arpstep++;
                if (osc1.arpstep==ARPSTEPMAX) osc1.arpstep = 0;
                arptick = ARPTICK << (3-osc1.arpspeed);
            }
            if (osc2.arpmode && osc2.on) {
                osc2.cinc = cincs[osc2.tonic+arptable[osc2.arpmode][osc2.arpstep]];
                osc2.arpstep++;
                if (osc2.arpstep==ARPSTEPMAX) osc2.arpstep = 0;
                arptick = ARPTICK << (3-osc2.arpspeed);
            }
            if (osc3.arpmode && osc3.on) {
                osc3.cinc = cincs[osc3.tonic+arptable[osc3.arpmode][osc3.arpstep]];
                osc3.arpstep++;
                if (osc3.arpstep==ARPSTEPMAX) osc3.arpstep = 0;
                arptick = ARPTICK << (3-osc3.arpspeed);
            }

    }

    if (voltick) --voltick;
    else {
            bendtick = !bendtick;
            if (osc1.on) Earr[osc1.adsrphase](&osc1);
            if (bendtick) osc1.pitchbend += osc1.bendrate; //slow bend to every second beat
            if (osc1.bendrate > 0 && osc1.pitchbend > osc1.maxbend) {
                    osc1.pitchbend = osc1.maxbend;
                    osc1.bendrate = 0; // STOP BENDING !
            }
            else if (osc1.bendrate < 0 && osc1.pitchbend < osc1.maxbend) {
                    osc1.pitchbend = osc1.maxbend;
                    osc1.bendrate = 0; // STOP BENDING !
            }

            if (osc2.on) Earr[osc2.adsrphase](&osc2);
            if (bendtick) osc2.pitchbend += osc2.bendrate;
            if (osc2.bendrate > 0 && osc2.pitchbend > osc2.maxbend) osc2.pitchbend = osc2.maxbend;
            else if (osc2.bendrate < 0 && osc2.pitchbend < osc2.maxbend) osc2.pitchbend = osc2.maxbend;

            if (osc3.on) Earr[osc3.adsrphase](&osc3);
            if (bendtick) osc3.pitchbend += osc3.bendrate;
            if (osc3.bendrate > 0 && osc3.pitchbend > osc3.maxbend) osc3.pitchbend = osc3.maxbend;
            else if (osc3.bendrate < 0 && osc3.pitchbend < osc3.maxbend) osc3.pitchbend = osc3.maxbend;

            voltick = VOLTICK;
    }
    tick = 4;
}

