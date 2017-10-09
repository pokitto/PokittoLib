/**************************************************************************/
/*!
    @file     Synth.h
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#ifndef SYNTH_H
#define SYNTH_H

#include "Synth_osc.h"
#include "Synth_song.h"

/* PROPER WAY
void f() {}

int main()
{
    using FunctionPtr = void (*)();

    FunctionPtr ptr = f;
}
*/

#ifndef boolean
typedef bool boolean;
#endif

//extern void fakeISR(); // was defined in Rboy_soundsim.h

typedef void (*waveFunction)(OSC*);
typedef void (*envFunction)(OSC*);
typedef void (*mixFunction)();

extern waveFunction Farr [];
extern envFunction Earr [];
extern mixFunction Marr []; // counts down
extern mixFunction HWMarr []; // counts down
/** COMMON TO BOTH HW AND SIM SOUND OUTPUT **/

#define RBTRACKER_VERSION 0.03f

#define WOFF     0
#define WSQUARE  1
#define WSAW     2
#define WTRI     3
#define WNOISE   4
#define WSAMPLE  5
#define WPNOISE  5

#define OVERDRIVE 4

#define ARPSTEPMAX 4 // was 5
#define PATTERNLENGTH 64
#define MAXPATTERNS 10
#define MAXBLOCKS 30 // 10 *3

#define VOLTICK 5
#define ARPTICK 50 // 150 // was 200

#define NUMWAVES 5
#define NUMENVELOPES 3
#define NUMMIXES 4

extern void getNoteString(char *, uint8_t);

extern void playNote(uint8_t,uint8_t,uint8_t);
extern void makeSampleInstruments();

extern void setPitch(int);
extern void setWave(int);
extern void setVolume(int);
extern void initAudio();
extern void testOsc();
extern void terminateSound();
extern void killSound();
extern void startSound();
extern void stopSound();
extern void updatePlayback(); // from flash
extern void updatePlaybackSD(uint8_t); // from SD
extern void initStreams(uint8_t);
extern void emptyOscillators();
extern void emptyPatches();
extern void emptyBlocks();
extern void emptySong();
extern int openSongFromSD(char *);
extern void writeChunkToSD(uint8_t *);
extern void readChunkFromSD(uint8_t *);


extern boolean playing, track1on, track2on, track3on, tableRefresh;
extern uint16_t playerpos;
extern uint16_t samplespertick, notetick;
extern long samplesperpattern;

extern long readindex, writeindex;
extern uint8_t tick, sequencepos;

extern SONG song;
extern OSC osc1,osc2,osc3;
extern OSC patch[];
extern BLOCK block[]; // array of blocks

#define MAX_ARPMODE 16

extern int8_t arptable[][5];

extern uint16_t freqs[];
extern uint16_t cincs[];

extern uint8_t xorshift8();
extern uint16_t xorshift16();

extern uint16_t noiseval;

extern void setOSC(OSC*,byte, byte, byte, byte, byte,
            uint8_t, uint8_t,
            uint16_t, uint16_t, uint16_t, uint16_t,
            int16_t, int16_t, uint8_t, uint8_t, uint8_t);

extern void setOSC(OSC*,byte,byte,uint16_t, uint8_t, uint32_t);

extern void waveoff(OSC*);
#endif // SYNTH_H

