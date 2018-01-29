/**************************************************************************/
/*!
    @file     Synth_wavefuncs.cpp
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

uint16_t noiseval, noiseval2;

void waveoff(OSC* o); void sqwave(OSC* o); void sawwave(OSC* o); void triwave(OSC* o); void noise(OSC* o); void tonenoise(OSC* o); void sample(OSC* o);
void noADSR(OSC* o); void attackFunc(OSC* o); void decayFunc(OSC* o); void releaseFunc(OSC* o);
void mix1(); void mix2(); void mix3(); void updateEnvelopes();

waveFunction Farr []  = {waveoff, sqwave, sawwave, triwave, noise, tonenoise};
envFunction Earr [] = {noADSR, attackFunc, decayFunc, releaseFunc};
mixFunction Marr [] = {updateEnvelopes,mix3,mix2,mix1}; // counts down
mixFunction HWMarr [] = {updateEnvelopes,mix3,mix2,mix1}; // counts down

/** SOUND FUNCTIONS **/

void waveoff(OSC* o){
  o->output = 0;
}

void sqwave(OSC* o){
// square. If bit 16 set, its 2nd half of cycle and then output. if not, silence.
 if (o->count & 0x8000) o->output = 0;
 else  o->output = 0xFFFF;
}

void sawwave(OSC* o){
 // saw is just twice the count, so it happens 2 times in a cycle.
 o->output = o->count << 1; // simple, eh ?
}

void triwave(OSC* o){
    // exploit the fact that above 0x7FFF bit 16 will be set (ie. when on second side of cycle)
    if (o->count & 0x8000) o->output = (~o->count) << 1; // counts down because complement goes other way
    else o->output = o->count << 1; // count up on first side of cycle
}

void noise(OSC* o){
  // Standard libc random gives 10-12 fps
  // Xorshift16 gives 32-39 fps
  // Xorshift8 gives

  if (o->count > 0x8000) {
    o->output = noiseval2;
    //noiseval = random(0,0xFFFF);
    noiseval = xorshift16();
    //noiseval = xorshift8()*256;
  }
  else  {
    o->output = noiseval;
    //noiseval2 = random(0,0xFFFF);
    noiseval2 = xorshift16();
    //noiseval2 = xorshift8()*256;
  }
}

void tonenoise(OSC* o){
  // square. If bit 16 set, its 2nd half of cycle and then output. if not, silence.
 if (o->count & 0x8000) o->output = (xorshift16()>>6); //was 0
 else  o->output = (xorshift16()>>1) + 0x4000;//random(0,0xFFFF);
}

void sample(OSC* o) {

    /*if (o->samplepos > o->samplelength ) o->samplepos = 0;

    if (o->count > o->wcycle) {
        o->count=0;
        if (o->output) o->output = 0;
        //else o->output = o->output=pgm_read_byte((uint32_t)(sfxBike) + o->inccount);
    }*/
}

