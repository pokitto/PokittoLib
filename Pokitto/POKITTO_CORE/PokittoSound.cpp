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
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
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

#include "PokittoSound.h"
#include "Pokitto_settings.h"
#include "PokittoCore.h"
#include "Synth.h"

#ifndef POK_SIM
#include "HWSound.h"

#ifdef PROJ_SDFS_STREAMING
#include "SDFSDisk.h"
#endif

#else
#include "SimSound.h"
#include "PokittoSimulator.h"
#endif

typedef uint8_t byte;

using namespace Pokitto;

#ifdef PROJ_SND_DEBUG
int __sx=0;
#endif

/** discrete hardware volume control **/

uint8_t Pokitto::discrete_vol = 0;
uint8_t const Pokitto::discrete_vol_levels[8]      {0,32,64,96,128,160,192,224};
uint8_t const Pokitto::discrete_vol_hw_levels[8]   {0,27,64,96,36,117,127,127};
uint8_t const Pokitto::discrete_vol_multipliers[8] {0,127,127,127,192,192,255,255};

Pokitto::Core _soundc;

const uint8_t *Sound::sfxDataPtr = 0;
const uint8_t *Sound::sfxEndPtr = 0;
bool Sound::sfxIs4bitSamples = false;
uint8_t Sound::sfxBytePos = 0;

uint8_t Sound::prescaler;
uint16_t Sound::globalVolume;
uint16_t Sound::volumeMax = VOLUME_HEADPHONE_MAX;
uint8_t Sound::headPhoneLevel=1;

bool Sound::trackIsPlaying[NUM_CHANNELS];
bool Sound::patternIsPlaying[NUM_CHANNELS];
uint8_t Sound::outputPitch[NUM_CHANNELS];
int8_t Sound::outputVolume[NUM_CHANNELS];

uint16_t *Sound::trackData[NUM_CHANNELS];
uint8_t Sound::trackCursor[NUM_CHANNELS];
uint16_t **Sound::patternSet[NUM_CHANNELS];
int8_t Sound::patternPitch[NUM_CHANNELS];

// pattern data
uint16_t *Sound::patternData[NUM_CHANNELS];
uint16_t **Sound::instrumentSet[NUM_CHANNELS];
bool Sound::patternLooping[NUM_CHANNELS];
uint16_t Sound::patternCursor[NUM_CHANNELS];

// note data
uint8_t Sound::notePitch[NUM_CHANNELS];
uint8_t Sound::noteDuration[NUM_CHANNELS];
int8_t Sound::noteVolume[NUM_CHANNELS];
bool Sound::notePlaying[NUM_CHANNELS];

// commands data
int8_t Sound::commandsCounter[NUM_CHANNELS];
int8_t Sound::volumeSlideStepDuration[NUM_CHANNELS];
int8_t Sound::volumeSlideStepSize[NUM_CHANNELS];
uint8_t Sound::arpeggioStepDuration[NUM_CHANNELS];
int8_t Sound::arpeggioStepSize[NUM_CHANNELS];
uint8_t Sound::tremoloStepDuration[NUM_CHANNELS];
int8_t Sound::tremoloStepSize[NUM_CHANNELS];

// instrument data
uint16_t *Sound::instrumentData[NUM_CHANNELS];
uint8_t Sound::instrumentLength[NUM_CHANNELS]; //number of steps in the instrument
uint8_t Sound::instrumentLooping[NUM_CHANNELS]; //how many steps to loop on when the last step of the instrument is reached
uint16_t Sound::instrumentCursor[NUM_CHANNELS]; //which step is being played
uint8_t Sound::instrumentNextChange[NUM_CHANNELS]; //how many frames before the next step

//current step data
int8_t Sound::stepVolume[NUM_CHANNELS];
uint8_t Sound::stepPitch[NUM_CHANNELS];
uint8_t Sound::chanVolumes[NUM_CHANNELS];

#if (POK_ENABLE_SOUND < 1)
 #ifdef NUM_CHANNELS
 #undef NUM_CHANNELS
 #define NUM_CHANNELS 0
 #endif // NUM_CHANNELS
#endif

#if(NUM_CHANNELS > 0)
    #ifndef POK_SIM
        uint32_t sbyte;
    #else
    uint32_t sbyte;
    float pwm1;
    #endif // POK_SIM

//declare these variables globally for faster access
uint8_t _rand = 1;
uint8_t _chanCount[NUM_CHANNELS]; //counts until the next change of the waveform
bool _chanState[NUM_CHANNELS]; //if the waveform is currently high or low
uint8_t _chanHalfPeriod[NUM_CHANNELS]; //duration of half the period of the waveform
uint8_t _chanOutputVolume[NUM_CHANNELS]; //amplitude of the outputted waveform
uint8_t _chanOutput[NUM_CHANNELS]; //current value of the outputted waveform
bool _chanNoise[NUM_CHANNELS]; //if a random value should be added to the waveform to generate noise

#if POK_GBSOUND > 0
const uint16_t squareWaveInstrument[]  = {0x0101, 0x03F7};
const uint16_t noiseInstrument[]  = {0x0101, 0x03FF};
const uint16_t* const defaultInstruments[]  = {squareWaveInstrument,noiseInstrument};

const uint16_t playOKPattern[]  = {0x0005,0x138,0x168,0x0000};
const uint16_t playCancelPattern[]  = {0x0005,0x168,0x138,0x0000};
const uint16_t playTickP[]  = {0x0045,0x168,0x0000};
#endif
#if(EXTENDED_NOTE_RANGE > 0)
//extended note range
#define NUM_PITCH 59
const uint8_t _halfPeriods[NUM_PITCH] = {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33,31,29,28,26,25,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6};
#else
//regular note range
#define NUM_PITCH 36
const uint8_t _halfPeriods[NUM_PITCH] = {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33};
#endif

#endif

void Pokitto::audio_IRQ() {
    #if POK_STREAMING_MUSIC > 0
        #if POK_STREAMFREQ_HALVE > 0
        streamstep = 1-streamstep;
        #else
        streamstep=1;
        #endif

        streamstep &= streamon; //check if stream is on

        if(streamvol && streamstep) {
            uint8_t output = (*currentPtr++);
            sbyte = output;
        } else {
            sbyte = 0; // duty cycle
        }

        if (currentPtr >= endPtr)
        {
        currentBuffer++;
        if (currentBuffer==4) currentBuffer=0;
        currentPtr = buffers[currentBuffer];
        endPtr = currentPtr + BUFFER_SIZE;
        }

    #endif // POK_STREAMING_MUSIC
    #if (NUM_CHANNELS > 0)
	Sound::generateOutput();
    #endif
}

void Sound::volumeUp() {
    Pokitto::discrete_vol++;
    if (discrete_vol>7) discrete_vol=7;
    globalVolume = discrete_vol_levels[discrete_vol];
    setVolume(globalVolume);
    //if (globalVolume>VOLUME_HEADPHONE_MAX) setVolume(getVolume()+VOLUME_STEP*2);
    //else setVolume(getVolume()+VOLUME_STEP);
}

void Sound::volumeDown() {
    if (discrete_vol) Pokitto::discrete_vol--;
    globalVolume = discrete_vol_levels[discrete_vol];
    setVolume(globalVolume);
    //if (globalVolume>VOLUME_HEADPHONE_MAX) setVolume(getVolume()-VOLUME_STEP*4);
    //else setVolume(getVolume()-VOLUME_STEP);
}


void Sound::setMaxVol(int16_t v) {
    if (v < 0) v = 0; //prevent nasty wraparound
    if (v > VOLUME_SPEAKER_MAX) {
            v = VOLUME_SPEAKER_MAX;
    }
    volumeMax = v;
    setVolume(globalVolume);
}

uint16_t Sound::getMaxVol() {
    return volumeMax;
}

void Sound::updateStream() {
     #if POK_STREAMING_MUSIC
    if (oldBuffer != currentBuffer) {
        if (currentBuffer==0) fileReadBytes(&buffers[3][0],BUFFER_SIZE);
        else if (currentBuffer==1) fileReadBytes(&buffers[0][0],BUFFER_SIZE);
        else if (currentBuffer==2) fileReadBytes(&buffers[1][0],BUFFER_SIZE);
        else fileReadBytes(&buffers[2][0],BUFFER_SIZE);
        oldBuffer = currentBuffer;
        streamcounter += BUFFER_SIZE;
    }

    #ifndef POK_SIM
    if ( streamcounter > fs.fsize - (BUFFER_SIZE)) {
    #else
    if ( streamcounter > getFileLength() - (BUFFER_SIZE)) {
    #endif
        streamcounter=0;
        #if POK_STREAM_LOOP > 0
        fileRewind();
        #else
            #ifndef POK_SIM
                streamon=0;
            #endif // POK_SIM
        #endif // POK_STREAM_LOOP
    }
    #endif
}

void Sound::begin() {
#if POK_ENABLE_SOUND > 0
soundInit();
ampEnable(true);
#endif
#if (NUM_CHANNELS > 0)
#if POK_ENABLE_SOUND > 0
#if POK_GBSOUND > 0
	prescaler = 1;
	for(byte i=0; i<NUM_CHANNELS; i++){
		chanVolumes[i] = VOLUME_CHANNEL_MAX;
		changeInstrumentSet(defaultInstruments, i); //load default instruments. #0:square wave, #1: noise
		command(CMD_INSTRUMENT, 0, 0, i); //set the default instrument to square wave
	}
#endif // POK_GBSOUND
#endif //POK_ENABLE_SOUND
#endif
}

void Sound::playTrack(const uint16_t* track, uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	stopTrack(channel);
	trackCursor[channel] = 0;
	trackData[channel] = (uint16_t*)track;
	trackIsPlaying[channel] = true;
#endif
}

void Sound::stopTrack(uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	trackIsPlaying[channel] = false;
	stopPattern(channel);
#endif
}

void Sound::stopTrack(){
#if(NUM_CHANNELS > 0)
	for(uint8_t i=0; i<NUM_CHANNELS; i++){
		stopTrack(i);
	}
#endif
}

void Sound::updateTrack(uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	if(trackIsPlaying[channel] && !patternIsPlaying[channel]){
		uint16_t data = pgm_read_word(trackData[channel] + trackCursor[channel]);
		if(data == 0xFFFF){ //en of the track
			trackIsPlaying[channel] = false;
			return;
		}
		uint8_t patternID = data & 0xFF;
		data >>= 8;
		patternPitch[channel] = data;
		playPattern((const uint16_t*)pgm_read_word(&(patternSet[channel][patternID])), channel);
		trackCursor[channel] ++;
	}
#endif
}

void Sound::updateTrack(){
#if(NUM_CHANNELS > 0)
	for (uint8_t i=0; i<NUM_CHANNELS; i++){
		updateTrack(i);
	}
#endif
}

void Sound::changePatternSet(const uint16_t* const* patterns, uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	patternSet[channel] = (uint16_t**)patterns;
#endif
}

void Sound::playPattern(const uint16_t* pattern, uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	stopPattern(channel);
	patternData[channel] = (uint16_t*)pattern;
	patternCursor[channel] = 0;
	patternIsPlaying[channel] = true;
	noteVolume[channel] = 9;
	//reinit commands
	volumeSlideStepDuration[channel] = 0;
	arpeggioStepDuration[channel] = 0;
	tremoloStepDuration[channel] = 0;
#endif
}

void Sound::updatePattern(){
#if(NUM_CHANNELS > 0)
	for (uint8_t i=0; i<NUM_CHANNELS; i++){
		updatePattern(i);
	}
#endif
}

void Sound::changeInstrumentSet(const uint16_t* const* instruments, uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	instrumentSet[channel] = (uint16_t**)instruments;
#endif
}

void Sound::updatePattern(uint8_t i){
#if(NUM_CHANNELS > 0)
	if(i>=NUM_CHANNELS)
		return;
	if(patternIsPlaying[i]){
		if(noteDuration[i]==0){//if the end of the previous note is reached

			uint16_t data = pgm_read_word(patternCursor[i] + patternData[i]);

			if(data == 0){ //end of the pattern reached
				if(patternLooping[i] == true){
					patternCursor[i] = 0;
					data = pgm_read_word(patternCursor[i] + patternData[i]);
				}
				else{
					patternIsPlaying[i] = false;
					if(trackIsPlaying[i]){ //if this pattern is part of a track, get the next pattern
						updateTrack(i);
						data = pgm_read_word(patternCursor[i] + patternData[i]);
					} else {
						stopNote(i);
						//Serial.print("pattern end\n");
						return;
					}
				}
			}

			while (data & 0x0001){ //read all commands and instrument changes
				data >>= 2;
				//Serial.print("\ncmd\t");
				uint8_t cmd = data & 0x0F;
				data >>= 4;
				uint8_t X = data & 0x1F;
				data >>= 5;
				int8_t Y = data - 16;
				command(cmd,X,Y,i);
				patternCursor[i]++;
				data = pgm_read_word(patternCursor[i] + patternData[i]);
			}
			data >>= 2;

			uint8_t pitch = data & 0x003F;
			data >>= 6;

			uint8_t duration = data;
			if(pitch != 63){
			}

			playNote(pitch, duration, i);

			patternCursor[i]++;
		}
	}
#endif
}

void Sound::stopPattern(uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	stopNote(channel);
	patternIsPlaying[channel] = false;
#endif
}

void Sound::stopPattern(){
#if(NUM_CHANNELS > 0)
	for(uint8_t i=0; i<NUM_CHANNELS; i++){
		stopPattern(i);
	}
#endif
}

void Sound::command(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i){
#if(NUM_CHANNELS > 0)
	if(i>=NUM_CHANNELS)
		return;
	switch(cmd){
	case CMD_VOLUME: //volume
	    X = constrain((int8_t)X, 0, 10);
		noteVolume[i] = X;
		break;
	case CMD_INSTRUMENT: //instrument
		instrumentData[i] = (uint16_t*)pgm_read_word(&(instrumentSet[i][X]));
		instrumentLength[i] = pgm_read_word(&(instrumentData[i][0])) & 0x00FF; //8 LSB
		instrumentLength[i] *= prescaler;
		instrumentLooping[i] = min2((pgm_read_word(&(instrumentData[i][0])) >> 8), instrumentLength[i]); //8 MSB - check that the loop is shorter than the instrument length
		instrumentLooping[i] *= prescaler;
		break;
	case CMD_SLIDE: //volume slide
		volumeSlideStepDuration[i] = X * prescaler;
		volumeSlideStepSize[i] = Y;
		break;
	case CMD_ARPEGGIO:
		arpeggioStepDuration[i] = X * prescaler;
		arpeggioStepSize[i] = Y;
		break;
	case CMD_TREMOLO:
		tremoloStepDuration[i] = X * prescaler;
		tremoloStepSize[i] = Y;
		break;
	default:
		break;
	}
#endif
}

void Sound::playNote(uint8_t pitch, uint8_t duration, uint8_t channel){
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	//set note
	notePitch[channel] = pitch;
	noteDuration[channel] = duration * prescaler;
	//reinit vars
	instrumentNextChange[channel] = 0;
	instrumentCursor[channel] = 0;
	notePlaying[channel] = true;
	_chanState[channel] = true;
	commandsCounter[channel] = 0;
#endif
}

void Sound::stopNote(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	notePlaying[channel] = false;
	//counters
	noteDuration[channel] = 0;
	instrumentCursor[channel] = 0;
	commandsCounter[channel] = 0;
	//output
	_chanOutput[channel] = 0;
	_chanOutputVolume[channel] = 0;
	_chanState[channel] = false;
	updateOutput();
#endif
}

void Sound::stopNote() {
#if(NUM_CHANNELS > 0)
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		stopNote(channel);
	}
#endif
}

void Sound::updateNote() {
#if(NUM_CHANNELS > 0)
	for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
		updateNote(i);
	}
#endif
}

void Sound::updateNote(uint8_t i) {
#if(NUM_CHANNELS > 0)
	if(i>=NUM_CHANNELS)
		return;
	if (notePlaying[i]) {

		if(noteDuration[i] == 0){
			stopNote(i);
			//Serial.println("note end");
			return;
		} else {
			noteDuration[i]--;
		}

		if (instrumentNextChange[i] == 0) {

			//read the step data from the progmem and decode it
			uint16_t thisStep = pgm_read_word(&(instrumentData[i][1 + instrumentCursor[i]]));

			stepVolume[i] = thisStep & 0x0007;
			thisStep >>= 3;

			uint8_t stepNoise = thisStep & 0x0001;
			thisStep >>= 1;

			uint8_t stepDuration = thisStep & 0x003F;
			thisStep >>= 6;

			stepPitch[i] = thisStep;

			//apply the step settings
			instrumentNextChange[i] = stepDuration * prescaler;

			_chanNoise[i] = stepNoise;


			instrumentCursor[i]++;

			if (instrumentCursor[i] >= instrumentLength[i]) {
				if (instrumentLooping[i]) {
					instrumentCursor[i] = instrumentLength[i] - instrumentLooping[i];
				} else {
					stopNote(i);
				}
			}
		}
		instrumentNextChange[i]--;

		commandsCounter[i]++;

		//UPDATE VALUES
		//pitch
		outputPitch[i] = notePitch[i] + stepPitch[i] + patternPitch[i];
		if(arpeggioStepDuration[i]){
		  outputPitch[i] += commandsCounter[i] / arpeggioStepDuration[i] * arpeggioStepSize[i];
		}
		outputPitch[i] = (outputPitch[i] + NUM_PITCH) % NUM_PITCH; //wrap
		//volume
		outputVolume[i] = noteVolume[i];
		if(volumeSlideStepDuration[i]){
		  outputVolume[i] += commandsCounter[i] / volumeSlideStepDuration[i] * volumeSlideStepSize[i];
		}
		if(tremoloStepDuration[i]){
			outputVolume[i] += ((commandsCounter[i]/tremoloStepDuration[i]) % 2) * tremoloStepSize[i];
		}
		outputVolume[i] = constrain(outputVolume[i], 0, 9);
		if(notePitch[i] == 63){
			outputVolume[i] = 0;
		}
		// jonnehw noInterrupts();
		_chanHalfPeriod[i] = pgm_read_byte(_halfPeriods + outputPitch[i]);
		_chanOutput[i] = _chanOutputVolume[i] = outputVolume[i] * (globalVolume>>GLOBVOL_SHIFT) * chanVolumes[i] * stepVolume[i];
		//_chanOutput[i] = _chanOutputVolume[i] = outputVolume[i] * (globalVolume) * chanVolumes[i] * stepVolume[i];
		//Serial.println(outputVolume[i]);
		// jonnehw interrupts();
	}
#endif
}

void Sound::setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	_chanHalfPeriod[channel] = halfPeriod;
	_chanState[channel] = false;
	_chanCount[channel] = 0;
	updateOutput();
#endif
}


void Sound::generateOutput() {
#if(NUM_CHANNELS > 0)
	bool outputChanged = false;
	//no for loop here, for the performance sake (this function runs 15 000 times per second...)
	//CHANNEL 0
	if (_chanOutputVolume[0]) {
		_chanCount[0]++;
		if (_chanCount[0] >= _chanHalfPeriod[0]) {
			outputChanged = true;
			_chanState[0] = !_chanState[0];
			_chanCount[0] = 0;
			if (_chanNoise[0]) {
				_rand = 67 * _rand + 71;
				_chanOutput[0] = _rand % _chanOutputVolume[0];
			}
		}
	}


	//CHANNEL 1
	#if (NUM_CHANNELS > 1)
	if (_chanOutputVolume[1]) {
		_chanCount[1]++;
		if (_chanCount[1] >= _chanHalfPeriod[1]) {
			outputChanged = true;
			_chanState[1] = !_chanState[1];
			_chanCount[1] = 0;
			if (_chanNoise[1]) {
				_rand = 67 * _rand + 71;
				_chanOutput[1] = _rand % _chanOutputVolume[1];
			}
		}
	}
	#endif

	//CHANNEL 2
	#if (NUM_CHANNELS > 2)
	if (_chanOutputVolume[2]) {
		_chanCount[2]++;
		if (_chanCount[2] >= _chanHalfPeriod[2]) {
			outputChanged = true;
			_chanState[2] = !_chanState[2];
			_chanCount[2] = 0;
			if (_chanNoise[2]) {
				_rand = 67 * _rand + 71;
				_chanOutput[2] = _rand % _chanOutputVolume[2];
			}
		}
	}
	#endif

	//CHANNEL 3
	#if (NUM_CHANNELS > 3)
	if (_chanOutputVolume[3]) {
		_chanCount[3]++;
		if (_chanCount[3] >= _chanHalfPeriod[3]) {
			outputChanged = true;
			_chanState[3] = !_chanState[3];
			_chanCount[3] = 0;
			if (_chanNoise[3]) {
				_rand = 67 * _rand + 71;
				_chanOutput[3] = _rand % _chanOutputVolume[3];
			}
		}
	}
	#endif

    #if POK_STREAMING_MUSIC
        if (streamstep) {
            outputChanged=true;
        }
    #endif

	if (outputChanged) {
		updateOutput();
	}
#endif
}

void Sound::updateOutput() {
#if(NUM_CHANNELS > 0)
	uint32_t output = 0;

	//CHANNEL 0
	if (_chanState[0]) {
		output += _chanOutput[0];
	}

	//CHANNEL 1
	#if (NUM_CHANNELS > 1)
	if (_chanState[1]) {
		output += _chanOutput[1];
	}
	#endif

	//CHANNEL 2
	#if (NUM_CHANNELS > 2)
	if (_chanState[2]) {
		output += _chanOutput[2];
	}
	#endif

	//CHANNEL 3
	#if (NUM_CHANNELS > 3)
	if (_chanState[3]) {
		output += _chanOutput[3];
	}
	#endif

    #ifndef POK_SIM
    #if POK_ENABLE_SOUND
    /** HARDWARE **/

        #if POK_STREAMING_MUSIC
            if (streamstep) {
                //pwmout_write(&audiopwm,(float)(sbyte>>headPhoneLevel)/(float)255);
                sbyte *= discrete_vol_multipliers[discrete_vol];
                sbyte >>= 8;
                pwmout_write(&audiopwm,(float)(sbyte)/(float)255);
            }
        #endif
            output *= discrete_vol_multipliers[discrete_vol];
            output >>= 8;
            dac_write((uint8_t)output); //direct hardware mixing baby !
    soundbyte = output;

    #endif //POK_ENABLE_SOUND
    #else
    /** SIMULATOR **/
        #if POK_STREAMING_MUSIC
            if (streamstep) {
                uint16_t o = output + sbyte;
                output = (o/2);//>>headPhoneLevel;
            }
        #endif
        soundbyte = output;//<<headPhoneLevel;
    #endif // POK_SIM

#endif
}

void Sound::setPatternLooping(bool loop, uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	patternLooping[channel] = loop;
#endif
}

void Sound::playOK(){
    #if POK_GBSOUND
#if(NUM_CHANNELS > 0)
	playPattern(playOKPattern,0);
#endif
#endif // POK_GBSOUND
}

void Sound::playCancel(){
#if POK_GBSOUND
#if(NUM_CHANNELS > 0)
	playPattern(playCancelPattern,0);
#endif
#endif
}

void Sound::playTick(){
#if POK_GBSOUND
#if(NUM_CHANNELS > 0)
	playPattern(playTickP,0);
#endif
#endif // POK_GBSOUND
}

void Sound::setVolume(int16_t volume) {
//#if NUM_CHANNELS > 0
	if (volume<0) volume = 0;
	//if (volume>volumeMax) volume = volumeMax;
	globalVolume = volume;
	//#if POK_ENABLE_SOUND > 0
	discrete_vol = (volume>>5);
	#ifndef POK_SIM
	#if POK_ENABLE_SOUND > 0
	setHWvolume(discrete_vol_hw_levels[discrete_vol]); //boost volume if headphonelevel
	#endif
	#endif
	//#endif
	#if POK_SHOW_VOLUME > 0
	_soundc.volbar_visible = VOLUMEBAR_TIMEOUT;
	#endif
//#endif
}

uint16_t Sound::getVolume() {
//#if NUM_CHANNELS > 0
	return globalVolume;
//#else
//	return 0;
//#endif
}

void Sound::setVolume(int8_t volume, uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
	volume = (volume > VOLUME_CHANNEL_MAX) ? VOLUME_CHANNEL_MAX : volume;
	volume = (volume < 0) ? 0 : volume;
	chanVolumes[channel] = volume;
#endif
}

uint8_t Sound::getVolume(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return 255;
	return (chanVolumes[channel]);
#else
	return 0;
#endif
}

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

uint8_t Sound::ampIsOn()
{
    #ifdef POK_SIM
    return core.ampIsOn();
    #else
    #if POK_ENABLE_SOUND > 0
    return Pokitto::ampIsOn();
    #endif
    #endif // POK_SIM
    return 0;
}

void Sound::ampEnable(uint8_t v) {
    #ifdef POK_SIM
    core.ampEnable(v);
    #else
    #if POK_ENABLE_SOUND > 0
    Pokitto::ampEnable(v);
    #endif
    #endif // POK_SIM

}

int Sound::playMusicStream(char* filename)
{
    return playMusicStream(filename,FILE_MODE_READONLY | FILE_MODE_BINARY);
}

int Sound::playMusicStream()
{
    #if POK_STREAMING_MUSIC >0
    if (currentPtr) {
            pokPlayStream();
            return 1;
    }
    #endif // POK_STREAMING_MUSIC
    return 0; //no stream
}

void Sound::pauseMusicStream() {
    #if POK_ENABLE_SOUND > 0
    pokPauseStream();
    #endif
}

int Sound::playMusicStream(char* filename, uint8_t options)
{
    #if POK_STREAMING_MUSIC

        uint8_t result;
        result = pokInitSD();
        if (!isThisFileOpen(filename)) {
            fileClose(); // close any open files
            result = fileOpen(filename,FILE_MODE_READONLY | FILE_MODE_BINARY);
        }else{
	    fileRewind();
	    result = 0;
	}

        if (result) {
                currentPtr = 0; // mark that no stream is available
                return 0; // opening music file failed
        }

        fileReadBytes(&buffers[0][0],BUFFER_SIZE);
        fileReadBytes(&buffers[1][0],BUFFER_SIZE);
        fileReadBytes(&buffers[2][0],BUFFER_SIZE);
        fileReadBytes(&buffers[3][0],BUFFER_SIZE);
        currentBuffer = 0;
        currentPtr = buffers[currentBuffer];
        endPtr = currentPtr + BUFFER_SIZE;

        //streaming = STR_PLAYING|options;

        if (!options) pokPlayStream(); // activate stream
    #endif //POK_STREAMING_MUSIC
    return 1; // opening music file succeeded
}

uint32_t Sound::getMusicStreamElapsedSec() {
    #if POK_STREAMING_MUSIC
    return streamcounter/POK_AUD_FREQ;
    #endif
    return 0;
}

uint32_t Sound::getMusicStreamElapsedMilliSec() {
    #if POK_STREAMING_MUSIC
    return streamcounter/(POK_AUD_FREQ/1000);
    #endif
    return 0;
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
