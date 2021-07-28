/**************************************************************************/
/*!
    @file     PokittoSound.h
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

#pragma once
#include <stdint.h>
#include "Pokitto_settings.h"
#include "GBcompatibility.h"
#include "PokittoFakeavr.h"
#include "PokittoGlobs.h"

#ifndef MAX_VOL_TEST
    #define VOLUME_SPEAKER_MAX 255 //((8<<GLOBVOL_SHIFT)-1)
    #define VOLUME_HEADPHONE_MAX 127
    #define VOLUME_STARTUP VOLUME_HEADPHONE_MAX
#else
    #define VOLUME_SPEAKER_MAX 255
    #define VOLUME_HEADPHONE_MAX VOLUME_SPEAKER_MAX
    #define VOLUME_STARTUP VOLUME_SPEAKER_MAX
#endif // MAXVOLTEST

#ifdef POK_SIM
#define VOLUME_STEP 1
#else
#define VOLUME_STEP 8
#endif

namespace Pokitto {

/** Sound class.
 *  The Sound class is an API-compatible version of the Gamebuino Sound API by Aurelien Rodot.
 *  Because it is a class consisting of only static members, there is only 1 instance running,
 *  no matter how many Sound classes are declared (see example below). This means sound can
 *  be used through a simple Sound class object or as a member of the Core class.
 *
 */

/** discrete_vol* are needed for more accurate volume control levels on hardware **/
inline uint8_t discrete_vol = 0;

class Sound {
protected:
    static inline uint16_t volumeMax = VOLUME_HEADPHONE_MAX;

public:
    static void playSFX( const uint8_t *sfx, uint32_t length );
    static void playSFX4bit( const uint8_t *sfx, uint32_t length );
    static void begin();

    // Headphonemode
    static inline uint8_t headPhoneLevel = 1; // a workaround to disappearing sound at low volume

    static void setMaxVol(int v) {
        if (v < 0) v = 0; //prevent nasty wraparound
        if (v > VOLUME_SPEAKER_MAX) {
            v = VOLUME_SPEAKER_MAX;
        }
        volumeMax = v;
        setVolume(globalVolume);
    }

    static uint16_t getMaxVol() {
        return volumeMax;
    }

    static void volumeUp();
    static void volumeDown();

    // Synth using samples support
    static void loadSampleToOsc(uint8_t os, uint8_t* sampdata, uint32_t sampsize);

    // Original functions
    static void playTone(uint8_t os, int frq, uint8_t amp, uint8_t wav,uint8_t arpmode);
    static void playTone(uint8_t os, uint16_t freq, uint8_t volume, uint32_t duration);

    static uint8_t ampIsOn(){ return true; }
    static void ampEnable(uint8_t){}

    static int playMusicStream(const char* filename, uint8_t options){
        if(!options)
            return playMusicStream(filename);
        return 0;
    }

    static int playMusicStream(const char* filename);
    static int playMusicStream();
    static void pauseMusicStream();
    static uint32_t getMusicStreamElapsedSec();
    static uint32_t getMusicStreamElapsedMilliSec();

    static inline uint16_t globalVolume;
    static void setVolume(int16_t volume){
	if (volume<0) volume = 0;
	globalVolume = volume;
    }

    static uint16_t getVolume(){
	return globalVolume;
    }
};

}
