/**************************************************************************/
/*!
    @file     PokittoSound.cpp
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

/*
 * NOTE:
 * API of the Pokitto Sound library is partially identical to the Gamebuino Sound API.
 * Due to the difference in architecture (ARM Cortex-M0+ in mbed environment vs. 8-bit AVR)
 * large parts are not identical. Most functions were rewritten, with only API remaining.
 * We want to give attribution to the original author's project:
 *
 * License for Gamebuino-identical code:
 *
 * (C) Copyright 2014 Aur�lien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "PokittoCore.h"
#include "PokittoSound.h"

#if (POK_ENABLE_SYNTH == 1)
#include "Synth.h"
#endif

using namespace Pokitto;

#include <LibAudio>

Audio::Sink<NUM_CHANNELS, POK_AUD_FREQ> audioSink;

/** discrete hardware volume control **/
static const uint8_t discrete_vol_levels[] = {0,32,64,96,128,160,192,224};

void Sound::begin(){
    Audio::setVolume(globalVolume);
}

void Sound::volumeUp() {
    if (discrete_vol<7) discrete_vol++;
    setVolume(discrete_vol_levels[discrete_vol]);
}

void Sound::volumeDown() {
    if (discrete_vol) discrete_vol--;
    setVolume(discrete_vol_levels[discrete_vol]);
}

void Sound::playSFX( const uint8_t *sfx, uint32_t length ){
    Audio::SFX8Source::play<1>(sfx, length);
}

void Sound::playSFX4bit( const uint8_t *sfx, uint32_t length ){
    // to-do: implement SFX4Source
    // Audio::SFX4Source::play<1>(sfx, length);
}

int Sound::playMusicStream(const char* filename){
    return Audio::play<0>(filename) != nullptr;
}

int Sound::playMusicStream(){
    auto& inst = Audio::RAWFileSource::getSourceInstance<0>();
    inst.setPaused(false);
    return 1;
}

void Sound::pauseMusicStream(){
    auto& inst = Audio::RAWFileSource::getSourceInstance<0>();
    inst.setPaused(true);
}

uint32_t Sound::getMusicStreamElapsedSec(){
    auto& inst = Audio::RAWFileSource::getSourceInstance<0>();
    return inst.getPosition() / POK_AUD_FREQ;
}

uint32_t Sound::getMusicStreamElapsedMilliSec(){
    auto& inst = Audio::RAWFileSource::getSourceInstance<0>();
    return inst.getPosition() * 1000 / POK_AUD_FREQ;
}

#if (POK_ENABLE_SYNTH == 1)
void Sound::playTone(uint8_t os, int frq, uint8_t amp, uint8_t wav,uint8_t arpmode)
{
    if (wav>MAX_WAVETYPES) wav=0;
    if (arpmode>MAX_ARPMODE) arpmode=MAX_ARPMODE;
    if (os==1) setOSC(&osc1,1,wav,1,0,0,frq,amp,0,0,0,0,0,0,arpmode,0,0);
    else if (os==2) setOSC(&osc2,1,wav,1,0,0,frq,amp,0,0,0,0,0,0,arpmode,0,0);
    else if (os==3) setOSC(&osc3,1,wav,1,0,0,frq,amp,0,0,0,0,0,0,arpmode,0,0);
}

void Sound::playTone(uint8_t os, uint16_t frq, uint8_t volume, uint32_t duration)
{
    if (os==1) setOSC(&osc1,1,WSQUARE,frq,volume,duration);
    else if (os==2) setOSC(&osc2,1,WTRI,frq,volume,duration);
    else if (os==3) setOSC(&osc3,1,WTRI,frq,volume,duration);
}

void Sound::loadSampleToOsc(uint8_t os, uint8_t* data, uint32_t datasize) {
    OSC* o;
    if (os==3) o = &osc3;
    else if (os==2) o = &osc2;
    else o = &osc1;
    o->sample = data;
    o->samplelength = datasize;
    o->samplepos = 0;
}

#endif
