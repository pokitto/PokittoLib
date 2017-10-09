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


#ifndef POKITTOSOUND_H
#define POKITTOSOUND_H

#include <stdint.h>
#include "Pokitto_settings.h"
#include "GBcompatibility.h"
#include "PokittoFakeavr.h"
#include "PokittoGlobs.h"

extern void pokPauseStream();
extern void pokPlayStream();
extern uint8_t pokStreamPaused();

//volume levels
#define GLOBVOL_SHIFT 5 //shift global volume to allow for finer increments
#ifndef MAX_VOL_TEST
    #define VOLUME_SPEAKER_MAX 255 //((8<<GLOBVOL_SHIFT)-1)
    #define VOLUME_HEADPHONE_MAX (1<<GLOBVOL_SHIFT)
    #define VOLUME_STARTUP ((1<<GLOBVOL_SHIFT)/2)
#else
    #define VOLUME_SPEAKER_MAX ((8<<GLOBVOL_SHIFT)-1)
    #define VOLUME_HEADPHONE_MAX VOLUME_SPEAKER_MAX
    #define VOLUME_STARTUP VOLUME_SPEAKER_MAX
#endif // MAXVOLTEST

#ifdef POK_SIM
#define VOLUME_STEP 1
#else
#define VOLUME_STEP 8
#endif


//commands
#define CMD_VOLUME 0
#define CMD_INSTRUMENT 1
#define CMD_SLIDE 2
#define CMD_ARPEGGIO 3
#define CMD_TREMOLO 4

#define STR_PLAYING 0x1
#define STR_VISUALIZER  0x2
#define STR_LOOP    0x4
#define STR_PAUSED  0x8

namespace Pokitto {

/** Sound class.
 *  The Sound class is an API-compatible version of the Gamebuino Sound API by Aurelien Rodot.
 *  Because it is a class consisting of only static members, there is only 1 instance running,
 *  no matter how many Sound classes are declared (see example below). This means sound can
 *  be used through a simple Sound class object or as a member of the Core class.
 *
 */

extern void audio_IRQ();  // audio interrupt

class Sound {
private:
    static uint16_t volumeMax;
public:
	static void begin();

	// Headphonemode
	static void setMaxVol(int16_t);
    static uint16_t getMaxVol();
    static void volumeUp();
    static void volumeDown();

	// Original functions
	static void updateStream();
    static void playTone(uint8_t os, int frq, uint8_t amp, uint8_t wav,uint8_t arpmode);
    static void playTone(uint8_t os, uint16_t freq, uint8_t volume, uint32_t duration);
    static uint8_t ampIsOn();
    static void ampEnable(uint8_t);
    static int playMusicStream(char* filename, uint8_t options);
    static int playMusicStream(char* filename);
    static int playMusicStream();
    static void pauseMusicStream();

	// GB compatibility functions
	static void playTrack(const uint16_t* track, uint8_t channel);
	static void updateTrack(uint8_t channel);
	static void updateTrack();
	static void stopTrack(uint8_t channel);
	static void stopTrack();
	static void changePatternSet(const uint16_t* const* patterns, uint8_t channel);
	static bool trackIsPlaying[NUM_CHANNELS];

	static void playPattern(const uint16_t* pattern, uint8_t channel);
	static void changeInstrumentSet(const uint16_t* const* instruments, uint8_t channel);
	static void updatePattern(uint8_t i);
	static void updatePattern();
	static void setPatternLooping(bool loop, uint8_t channel);
	static void stopPattern(uint8_t channel);
	static void stopPattern();
	static bool patternIsPlaying[NUM_CHANNELS];

	static void command(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i);
	static void playNote(uint8_t pitch, uint8_t duration, uint8_t channel);
	static void updateNote();
	static void updateNote(uint8_t i);
	static void stopNote(uint8_t channel);
	static void stopNote();

	static uint8_t outputPitch[NUM_CHANNELS];
	static int8_t outputVolume[NUM_CHANNELS];

	static void setMasterVolume(uint8_t);
	static uint8_t GetMasterVolume();
	static void setVolume(int16_t volume);
	static uint16_t getVolume();
	static void setVolume(int8_t volume, uint8_t channel);
	static uint8_t getVolume(uint8_t channel);

	static void playOK();
	static void playCancel();
	static void playTick();

	static uint8_t prescaler;

	static void setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

	static void generateOutput(); //!\\ DO NOT USE
	static uint16_t globalVolume;


#if (NUM_CHANNELS > 0)
	//tracks data
	static uint16_t *trackData[NUM_CHANNELS];
	static uint8_t trackCursor[NUM_CHANNELS];
	static uint16_t **patternSet[NUM_CHANNELS];
	static int8_t patternPitch[NUM_CHANNELS];

	// pattern data
	static uint16_t *patternData[NUM_CHANNELS];
	static uint16_t **instrumentSet[NUM_CHANNELS];
	static bool patternLooping[NUM_CHANNELS];
	static uint16_t patternCursor[NUM_CHANNELS];

	// note data
	static uint8_t notePitch[NUM_CHANNELS];
	static uint8_t noteDuration[NUM_CHANNELS];
	static int8_t noteVolume[NUM_CHANNELS];
	static bool notePlaying[NUM_CHANNELS];

	// commands data
	static int8_t commandsCounter[NUM_CHANNELS];
	static int8_t volumeSlideStepDuration[NUM_CHANNELS];
	static int8_t volumeSlideStepSize[NUM_CHANNELS];
	static uint8_t arpeggioStepDuration[NUM_CHANNELS];
	static int8_t arpeggioStepSize[NUM_CHANNELS];
	static uint8_t tremoloStepDuration[NUM_CHANNELS];
	static int8_t tremoloStepSize[NUM_CHANNELS];


	// instrument data
	static uint16_t *instrumentData[NUM_CHANNELS];
	static uint8_t instrumentLength[NUM_CHANNELS]; //number of steps in the instrument
	static uint8_t instrumentLooping[NUM_CHANNELS]; //how many steps to loop on when the last step of the instrument is reached
	static uint16_t instrumentCursor[NUM_CHANNELS]; //which step is being played
	static uint8_t instrumentNextChange[NUM_CHANNELS]; //how many frames before the next step

	//current step data
	static int8_t stepVolume[NUM_CHANNELS];
	static uint8_t stepPitch[NUM_CHANNELS];

	static uint8_t chanVolumes[NUM_CHANNELS];
#endif
	static void updateOutput();
};

}

#endif // POKITTOSOUND_H





