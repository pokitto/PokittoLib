/*
    Tracker Arrays
    Updated 5.7.2017
*/

//const char noteLetters[12][2] = {{'C', '-'}, {'C', '#'}, {'D', '-'}, {'D', '#'}, {'E', '-'}, {'F', '-'},
//                           {'F', '#'}, {'G', '-'}, {'G', '#'}, {'A', '-'}, {'A', '#'}, {'B', '-'}};

#ifndef ARRAYS_H
#define ARRAYS H

#include <stdint.h>

extern const char noteLetters[12][2];

extern const char* waveNames[];
extern const char* arpModes[];

extern const char rboyChar[] ;
extern const char BPMChar[] ;
extern const char lastPatternChar[] ;
extern const char loopToChar[];
extern const char patchesChar[]  ;
extern const char blockSeqChar[] ;
extern const char blockChar[] ;
extern const char rowChar[] ;
extern const char unknownChar[] ;
extern const char waveChar[] ;
extern const char volChar[] ;
extern const char pitchRateChar[] ;
extern const char pitchMaxChar[] ;
extern const char vibChar[] ;
extern const char arpChar[] ;
extern const char ADSRChar[] ;
extern const char attackChar[] ;
extern const char decayChar[] ;
extern const char sustainChar[] ;
extern const char releaseChar[] ;
extern const char loopChar[] ;
extern const char echoChar[] ;
extern const char overdriveChar[];
extern const char drumChar[] ;

extern const uint8_t pauseBitmap[];

extern const uint8_t playBitmap[];

extern const uint8_t stopBitmap[];

extern const uint8_t pointBitmap[];


#endif // ARRAYS_H
