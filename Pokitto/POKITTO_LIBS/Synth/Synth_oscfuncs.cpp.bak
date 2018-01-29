/**************************************************************************/
/*!
    @file     Synth_oscfuncs.cpp
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

/** OSCILLATOR FUNCTIONS **/

void setOSC(OSC* o,byte on=1, byte wave=1, byte loop=0, byte echo=0, byte adsr=0,
            uint8_t notenumber=25, uint8_t volume=127,
            uint16_t attack=0, uint16_t decay=0, uint16_t sustain=0, uint16_t release=0,
            int16_t maxbend=0, int16_t bendrate=0, uint8_t arpmode = 0, uint8_t overdrive=0, uint8_t kick=0){
  //Serial.println("SetOsc "); osc1
  o->on = on;
  o->overdrive = overdrive;
  o->kick = kick;
  o->wave = wave;
  o->loop = loop;
  o->echo = echo; //echo shifts left 8 steps to zero
  o->echodiv = 0;
  o->adsr = adsr;
  if (arpmode) {
        if (arpmode < 4) {o->arpmode = 1; o->arpspeed = arpmode;}
        else if (arpmode < 7) {o->arpmode = 2; o->arpspeed = arpmode-3;}
        else if (arpmode < 10) {o->arpmode = 3; o->arpspeed = arpmode-6; } // vibrato trial
        else if (arpmode < 13) {o->arpmode = 4; o->arpspeed = arpmode-9; } // octave trial
        else if (arpmode < 16) {o->arpmode = 5; o->arpspeed = arpmode-12; } // funk trial*/
  } else o->arpmode = 0;
  o->arpstep = 0;
  o->count = 0;
  noiseval = xorshift16(); //random(0,0xFFFF);

  o->cinc = cincs[notenumber]<<POK_CINC_MULTIPLIER; // direct cinc from table, no calculation
  o->tonic = notenumber; // save tonic for arpeggio use
  if (wave == 2) o->cinc >>= 1; // correct pitch for saw wave
  if (wave == 4) o->cinc <<= 1; // enable higher pitch for pure noise
  o->vol = volume << 8;//volume;

  if (adsr) {
    o->attack = attack;
    o->decay = decay;
    o->sustain = sustain;
    o->release = release;
    o->adsrphase = 1;
    if (!o->attack) o->adsrvol = o->vol; // start directly, no attack ramp
    else o->adsrvol = 0;
  } else {
    o->attack = 0;
    o->decay = 0;
    o->sustain = 0;
    o->release = 0;
    o->adsrphase = 0;
    o->adsrvol = o->vol; // will stay same all the time
  }

  if (bendrate != 0) {
        o->bendrate = bendrate; // test value
        o->pitchbend = 0;
        o->maxbend = maxbend;
  }
}

void setOSC(OSC* o,byte on, byte wave, uint16_t frq, uint8_t volume, uint32_t duration){
  o->on = on;
  o->overdrive = 0;
  o->kick = 0;
  o->wave = wave;
  o->loop = 1;
  o->echo = 1; //echo shifts left 8 steps to zero
  o->echodiv = 0;
  o->adsr = 1;
  o->attack = 200;
  o->decay = 200;
  o->sustain = 20;
  o->release = 10;
  o->adsrphase = 1;
  o->arpmode = 0;
  o->count = 0;
  noiseval = xorshift16(); //random(0,0xFFFF);
  o->cinc = (frq/100)*(cincs[18]<<POK_CINC_MULTIPLIER); // its a kludge, i know. cant be bothered.
  if (wave == 2) o->cinc >>= 1; // correct pitch for saw wave
  if (wave == 4) o->cinc <<= 1; // enable higher pitch for pure noise
  o->vol = volume << 8;//volume;
  o->adsrvol = o->vol;
  o->duration = duration*100;
  o->maxbend = -4000;
  o->bendrate = 1000;
}



void emptyOscillators(){
    osc1.on = false; osc1.wave = 0; osc1.echo = 0; osc1.count = 0; osc1.cinc =0;
    osc1.attack = 0; osc1.loop = 0; osc1.adsrphase = 1; osc1.adsr = 1; osc1.decay = 100;
    osc1.pitchbend = 0; osc1.bendrate = 0; osc1.maxbend = 0; osc1.sustain = 0; osc1.release = 0;

    osc2.on = false; osc2.wave = 0; osc2.echo = 0; osc2.count = 0; osc2.cinc =0;
    osc2.attack = 0; osc2.loop = 0; osc2.adsrphase = 1; osc2.adsr = 1; osc2.decay = 100;
    osc2.pitchbend = 0; osc2.bendrate = 0; osc2.maxbend = 0; osc2.sustain = 0; osc2.release = 0;

    osc3.on = false; osc3.wave = 0; osc3.echo = 0; osc3.count = 0; osc3.cinc =0;
    osc3.attack = 0; osc3.loop = 0; osc3.adsrphase = 1; osc3.adsr = 1; osc3.decay = 100;
    osc3.pitchbend = 0; osc3.bendrate = 0; osc3.maxbend = 0; osc3.sustain = 0; osc3.release = 0;
}


void testOsc(){
    setOSC(&osc1,1,WTRI,1,0,1,25,127,10,10,20,2,0,0,0,0,0); // C3 = 25
    setOSC(&osc2,1,WTRI,1,0,1,29-12,63,2,1,20,2,0,0,14,0,0); // E3 = 29
    setOSC(&osc3,1,WSAW,1,0,1,25,15,30,30,20,2,-1,-1000,12,0,0); // G3 = 32
}

void playNote(uint8_t oscnum, uint8_t notenum, uint8_t i) {
    OSC* o;
    if (oscnum == 1) o = &osc1; else if (oscnum == 2) o = &osc2; else o = &osc3;
    setOSC(o,1,patch[i].wave,patch[i].loop,patch[i].echo,patch[i].adsr,notenum,patch[i].vol,
                          patch[i].attack,patch[i].decay,patch[i].sustain,patch[i].release,
                          patch[i].maxbend,patch[i].bendrate,patch[i].arpmode,patch[i].overdrive,patch[i].kick);
}

void makeSampleInstruments() {
    /* sample instruments for testing */
    patch[0].wave = WSQUARE;
    patch[0].on = 1;
    patch[0].vol = 127;
    patch[0].loop = 0;
    patch[0].echo = 0;

    patch[0].adsr = 0;
    patch[0].attack = 0;
    patch[0].decay = 0;
    patch[0].sustain = 0;
    patch[0].release = 0;

    patch[0].maxbend = -1000;
    patch[0].bendrate = 100;
    patch[0].arpmode = 3;
    patch[0].overdrive = 0;
    patch[0].kick = 0;

    patch[1].wave = WSAW;
    patch[1].on = 1;
    patch[1].vol = 200;
    patch[1].loop = 0;
    patch[1].echo = 0;

    patch[1].adsr = 0;
    patch[1].attack = 0;
    patch[1].decay = 0;
    patch[1].sustain = 0;
    patch[1].release = 0;

    patch[1].maxbend = 0;
    patch[1].bendrate = 0;
    patch[1].arpmode = 1;
    patch[1].overdrive = 0;
    patch[1].kick = 0;

    patch[2].wave = WTRI;
    patch[2].on = 1;
    patch[2].vol = 127;
    patch[2].loop = 0;
    patch[2].echo = 0;

    patch[2].adsr = 1;
    patch[2].attack = 10;
    patch[2].decay = 0;
    patch[2].sustain = 0;
    patch[2].release = 0;

    patch[2].maxbend = 0;
    patch[2].bendrate = 0;
    patch[2].arpmode = 1;
    patch[2].overdrive = 0;
    patch[2].kick = 0;

    patch[3].wave = WNOISE;
    patch[3].on = 1;
    patch[3].vol = 127;
    patch[3].loop = 1;
    patch[3].echo = 1;

    patch[3].adsr = 1;
    patch[3].attack = 0;
    patch[3].decay = 30;
    patch[3].sustain = 30;
    patch[3].release = 5;

    patch[3].maxbend = 0;
    patch[3].bendrate = 0;
    patch[3].arpmode = 0;
    patch[3].overdrive = 0;
    patch[3].kick = 0;

    patch[4].wave = WPNOISE;
    patch[4].on = 1;
    patch[4].vol = 127;
    patch[4].loop = 0;
    patch[4].echo = 0;

    patch[4].adsr = 1;
    patch[4].attack = 0;
    patch[4].decay = 30;
    patch[4].sustain = 30;
    patch[4].release = 5;

    patch[4].maxbend = 0;
    patch[4].bendrate = 0;
    patch[4].arpmode = 1;
    patch[4].overdrive = 0;
    patch[4].kick = 0;
}

