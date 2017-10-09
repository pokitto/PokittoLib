/**************************************************************************/
/*!
    @file     Synth_songfuncs.cpp
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

#include "PokittoDisk.h"
#include "Synth.h"
#ifdef POK_SIM
#include "FileIO.h"
#endif

/** SONG FUNCTIONS **/

//uint8_t chunk1[CHUNKSIZE], chunk2[CHUNKSIZE]; // 8 rows, 3 channels (columns), 2 bytes per entry
uint8_t chunk[2][CHUNKSIZE]; // 8 rows, 3 channels (columns), 2 bytes per entry
uint8_t cc = 0;


#if POK_ENABLE_SOUND > 0
#if POK_ENABLE_SD > 0
void updatePlaybackSD(uint8_t row) {
    // samplespertick determines how long the oscillators are active before they are recalculated (i.e. the next tick
    uint8_t i=0;

    if (notetick > samplespertick ) {
            // TRACK 1
            //if (track1on) i = *song.instrument_stream[0];
            i = 0xF;
            if (track1on) i = chunk[cc][row+1]>>4;
            if (i!=0xF) {
                setOSC(&osc1,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                chunk[cc][row],patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            // TRACK 2
            //if (track2on) i = *song.instrument_stream[1];
            i = 0xF;
            if (track2on) i = chunk[cc][row+3]>>4;
            if (i!=0xF) {
                setOSC(&osc2,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                chunk[cc][row+2],patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            // TRACK 3
            i = 0xF;
            if (track3on) i = chunk[cc][row+5]>>4;
            if (i!=0xF) {
                setOSC(&osc3,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                chunk[cc][row]+4,patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            playerpos++;
            if (playerpos == PATTERNLENGTH) {
                    // move to next song position
                    playerpos = 0;
                    /*sequencepos++;
                    if (sequencepos > song.song_end) {
                        if (song.song_loop == -1) {
                            emptyOscillators();
                            playing = false;
                            sequencepos--;
                        } else {
                            sequencepos = song.song_loop;
                        }
                    }
                    playerpos = 0;
                    initStreams(sequencepos);
                    tableRefresh=true;*/
            }
            notetick =0;
            }
}

void updatePlayback() {
    // samplespertick determines how long the oscillators are active before they are recalculated (i.e. the next tick
    uint8_t i=0;

    if (notetick > samplespertick ) {
            // TRACK 1
            if (track1on) i = *song.instrument_stream[0];
            else i = 0;
            if (i) {
                setOSC(&osc1,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                *song.note_stream[0],patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            // TRACK 2
            if (track2on) i = *song.instrument_stream[1];
            else i = 0;
            if (i) {
                setOSC(&osc2,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                *song.note_stream[1],patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            // TRACK 3
            if (track3on) i = *song.instrument_stream[2];
            else i = 0;
            if (i) {
                setOSC(&osc3,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
                *song.note_stream[2],patch[i].vol,
                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
                }
            playerpos++;
            song.instrument_stream[0]++;
            song.note_stream[0]++;
            song.instrument_stream[1]++;
            song.note_stream[1]++;
            song.instrument_stream[2]++;
            song.note_stream[2]++;
            if (playerpos == PATTERNLENGTH) {
                    // move to next song position
                    sequencepos++;
                    if (sequencepos > song.song_end) {
                        if (song.song_loop == -1) {
                            emptyOscillators();
                            playing = false;
                            sequencepos--;
                        } else {
                            sequencepos = song.song_loop;
                        }
                    }
                    playerpos = 0;
                    initStreams(sequencepos);
                    tableRefresh=true;
            }
            notetick =0;
            }
}

void emptyPatches(){
    for (int i=0; i<16; i++) {
    patch[i].vol = 127;
    patch[i].on = true; patch[i].wave = 1; patch[i].echo = 0; patch[i].count = 0; patch[i].cinc =0;
    patch[i].attack = 0; patch[i].loop = 0; patch[i].adsrphase = 0; patch[i].adsr = 0; patch[i].decay = 20;
    patch[i].pitchbend = 0; patch[i].bendrate = 0; patch[i].maxbend = 0; patch[i].sustain = 0; patch[i].release = 0, patch[i].overdrive = 0, patch[i].kick = 0;
    }
}

void emptyBlocks(){
    for (int i=0; i<MAXBLOCKS; i++) {
            for (int j = 0; j < PATTERNLENGTH; j++) {
                    block[i].instrument[j] = 0;
                    block[i].notenumber[j] = 255;
            }
    }
}

void initStreams(uint8_t seqpos){
    uint8_t blocknum;
    // retarget pointers for track 1
    // byte = pgm_read_byte(&(mydata[i][j]));
    blocknum=song.block_sequence[0][seqpos];
    //blocknum=pgm_read_byte(Song+SONG_SEQUENCE+seqpos);
    song.instrument_stream[0]=&block[blocknum].instrument[0];
    song.note_stream[0]=&block[blocknum].notenumber[0];
    // retarget pointers for track 2
    blocknum=song.block_sequence[1][seqpos];
    song.instrument_stream[1]=&block[blocknum].instrument[0];
    song.note_stream[1]=&block[blocknum].notenumber[0];
    // retarget pointers for track 3
    blocknum=song.block_sequence[2][seqpos];
    song.instrument_stream[2]=&block[blocknum].instrument[0];
    song.note_stream[2]=&block[blocknum].notenumber[0];
}

void emptySong(){
    song.num_channels = 3;
    song.num_patches = 1;
    song.song_bpm = 120;
    song.num_patterns = 1;
    song.song_end = 0;
    song.song_loop = 0; // loop back to start
    song.rb_version = RBTRACKER_VERSION;
    for (uint8_t i = 0; i<10; i++) {
        song.block_sequence[0][i]=i*3; // track 1
        song.block_sequence[1][i]=i*3+1; // track 2
        song.block_sequence[2][i]=i*3+2; // track 3
    }
    song.instrument_stream[0] = &block[0].instrument[0];
    song.note_stream[0] = &block[0].notenumber[0];
    song.instrument_stream[1] = &block[1].instrument[0];
    song.note_stream[1] = &block[1].notenumber[0];
    song.instrument_stream[2] = &block[2].instrument[0];
    song.note_stream[2] = &block[2].notenumber[0];
    sequencepos = 0;
}

int openSongFromSD(char* buffer) {
    if (!isThisFileOpen(buffer)) {
        fileClose(); // close any open files
        fileOpen(buffer,FILE_MODE_OVERWRITE | FILE_MODE_BINARY);
    }
    return isThisFileOpen(buffer);
}

void writeChunkToSD(uint8_t* buffer) {
    if (fileOK()) {
        fileWriteBytes(buffer, CHUNKSIZE);
    }
}

void readChunkFromSD(uint8_t* buffer) {
    if (fileOK()) {
        fileReadBytes(buffer, CHUNKSIZE);
    }
}
#endif
#endif // POK_ENABLE_SOUND

