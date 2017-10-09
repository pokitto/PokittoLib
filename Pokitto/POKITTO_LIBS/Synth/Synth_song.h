/**************************************************************************/
/*!
    @file     Synth_song.h
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

#ifndef SYNTH_SONG_H
#define SYNTH_SONG_H

#define CHUNKSIZE 8*3*2 // 48 bytes
//extern uint8_t chunk1[], chunk2[];
extern uint8_t chunk[2][CHUNKSIZE];
extern uint8_t cc; // current chunk

struct SONG {
    byte rb_version; // rbtracker version with which the song was created
    uint16_t song_bpm; // song beats per minute
    byte num_patches; // how many different instruments ie patches
    byte num_channels; // how many channels are used by this song (1-3)
    byte num_patterns; // how many different patterns are used
    byte song_end;  // at what position song ends
    int8_t song_loop; // where to loop at end of song. -1 means no loop
    byte block_sequence[3][10]; //the sequence of blocks for each track
    const uint8_t * instrument_stream[3]; //pointers to the instruments in the track streams
    const uint8_t * note_stream[3]; //pointers to the notes in the track streams
};

struct BLOCK {
    uint8_t notenumber[64]; // was 64
    uint8_t instrument[64]; // was 64
};


#endif // SYNTH_SONG_H

