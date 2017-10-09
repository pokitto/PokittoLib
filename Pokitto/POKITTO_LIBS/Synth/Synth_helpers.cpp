/**************************************************************************/
/*!
    @file     Synth_envfuncs.cpp
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

#include "Pokitto.h"
#include "Synth.h"

#define PROGMEM
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
    #ifndef WIN32
    #ifndef pgm_read_word
    #define pgm_read_word(addr) (*(const unsigned short *)(addr))
    #endif
    #else
    #define pgm_read_word(addr) (*(const unsigned int *)(addr))
    #endif // WIN32

// http://www.arklyffe.com/main/2010/08/29/xorshift-pseudorandom-number-generator/
static uint8_t y8 = 1;
static uint16_t y16 = 1;

// returns values from 1 to 255 inclusive, period is 255
uint8_t xorshift8(void) {
    //y8 ^= (y8 << 7);
    y8 ^= (y8 * 128);
    y8 ^= (y8 >> 5);
    //return y8 ^= (y8 << 3);
    return y8 ^= (y8 * 8);
}

// returns values from 1 to 65535 inclusive, period is 65535
uint16_t xorshift16(void) {
    //y16 ^= (y16 << 13);
    y16 ^= (y16 * 8192);
    y16 ^= (y16 >> 9);
    //return y16 ^= (y16 << 7);
    return y16 ^= (y16 * 128);
}


int8_t arptable[MAX_ARPMODE][5] = {
{0,0,0,0,0}, // Off
{0,4,7,4,0}, // Major tonic, 3rd, 5th
{0,3,7,3,0}, // Minor tonic, 3rd, 5th
{0,1,0,-1,0}, // vibrato test
{0,12,0,12,0}, // octave test
{0,11,14,17,21}, // funky test
};

/** VITAL TABLES **/

uint16_t cincs[] ={
36,  // 0 - NOTE_B0
38,  // 1 - NOTE_C1
40,  // 2 - NOTE_CS1
43,  // 3 - NOTE_D1
45,  // 4 - NOTE_DS1
47,  // 5 - NOTE_E1
51,  // 6 - NOTE_F1
53,  // 7 - NOTE_FS1
56,  // 8 - NOTE_G1
60,  // 9 - NOTE_GS1
63,  // 10 - NOTE_A1
67,  // 11 - NOTE_AS1
71,  // 12 - NOTE_B1
75,  // 13 - NOTE_C2
79,  // 14 - NOTE_CS2
84,  // 15 - NOTE_D2
90,  // 16 - NOTE_DS2
94,  // 17 - NOTE_E2
100,  // 18 - NOTE_F2
107,  // 19 - NOTE_FS2
113,  // 20 - NOTE_G2
120,  // 21 - NOTE_GS2
127,  // 22 - NOTE_A2
135,  // 23 - NOTE_AS2
142,  // 24 - NOTE_B2
151,  // 25 - NOTE_C3
160,  // 26 - NOTE_CS3
169,  // 27 - NOTE_D3
180,  // 28 - NOTE_DS3
190,  // 29 - NOTE_E3
201,  // 30 - NOTE_F3
213,  // 31 - NOTE_FS3
225,  // 32 - NOTE_G3
239, //239,  // 33 - NOTE_GS3
253,  // 34 - NOTE_A3
267,  // 35 - NOTE_AS3
284,  // 36 - NOTE_B3
301,  // 37 - NOTE_C4
318,  // 38 - NOTE_CS4
338,  // 39 - NOTE_D4
358,  // 40 - NOTE_DS4
379,  // 41 - NOTE_E4
402,  // 42 - NOTE_F4
426,  // 43 - NOTE_FS4
452,  // 44 - NOTE_G4
478,  // 45 - NOTE_GS4
504,  // 46 - NOTE_A4 // 440Hz
537,  // 47 - NOTE_AS4
570,  // 48 - NOTE_B4
601,  // 49 - NOTE_C5
636,  // 50 - NOTE_CS5
676,  // 51 - NOTE_D5
712,  // 52 - NOTE_DS5
762,  // 53 - NOTE_E5
799,  // 54 - NOTE_F5
851,  // 55 - NOTE_FS5
898,  // 56 - NOTE_G5
950,  // 57 - NOTE_GS5
1008,  // 58 - NOTE_A5
1074,  // 59 - NOTE_AS5
1130,  // 60 - NOTE_B5
1214,  // 61 - NOTE_C6
1285,  // 62 - NOTE_CS6
1337,  // 63 - NOTE_D6
1425,  // 64 - NOTE_DS6
1524,  // 65 - NOTE_E6
1598,  // 66 - NOTE_F6
1680,  // 67 - NOTE_FS6
1820,  // 68 - NOTE_G6
1928,  // 69 - NOTE_GS6
2048,  // 70 - NOTE_A6
2114,  // 71 - NOTE_AS6
2260,  // 72 - NOTE_B6
2427,  // 73 - NOTE_C7
2521,  // 74 - NOTE_CS7
2731,  // 75 - NOTE_D7
2849,  // 76 - NOTE_DS7
2979,  // 77 - NOTE_E7
3277,  // 78 - NOTE_F7
3449,  // 79 - NOTE_FS7
3641,  // 80 - NOTE_G7
3855,  // 81 - NOTE_GS7
4096,  // 82 - NOTE_A7
4369,  // 83 - NOTE_AS7
4681,  // 84 - NOTE_B7
4681,  // 85 - NOTE_C8
5041,  // 86 - NOTE_CS8
5461,  // 87 - NOTE_D8
5958,  // 88 - NOTE_DS8
};


/** NOTE TO TEXT TABLES **/

const char note_0[] PROGMEM = "B-0";  // 0 - NOTE_B0
const char note_1[] PROGMEM = "C-1";  // 1 - NOTE_C1
const char note_2[] PROGMEM = "C#1";  // 2 - NOTE_CS1
const char note_3[] PROGMEM = "D-1";  // 3 - NOTE_D1
const char note_4[] PROGMEM = "D#1";  // 4 - NOTE_DS1
const char note_5[] PROGMEM = "E-1";  // 5 - NOTE_E1
const char note_6[] PROGMEM = "F-1";  // 6 - NOTE_F1
const char note_7[] PROGMEM = "F#1";  // 7 - NOTE_FS1
const char note_8[] PROGMEM = "G-1";  // 8 - NOTE_G1
const char note_9[] PROGMEM = "G#1";  // 9 - NOTE_GS1
const char note_10[] PROGMEM = "A-1";  // 10 - NOTE_A1
const char note_11[] PROGMEM = "A#1";  // 11 - NOTE_AS1
const char note_12[] PROGMEM = "B-1";  // 12 - NOTE_B1
const char note_13[] PROGMEM = "C-2";  // 2 - NOTE_C2
const char note_14[] PROGMEM = "C#2";  // 2 - NOTE_CS2
const char note_15[] PROGMEM = "D-2";  // 3 - NOTE_D2
const char note_16[] PROGMEM = "D#2";  // 4 - NOTE_DS2
const char note_17[] PROGMEM = "E-2";  // 5 - NOTE_E2
const char note_18[] PROGMEM = "F-2";  // 6 - NOTE_F2
const char note_19[] PROGMEM = "F#2";  // 7 - NOTE_FS2
const char note_20[] PROGMEM = "G-2";  // 8 - NOTE_G2
const char note_21[] PROGMEM = "G#2";  // 9 - NOTE_GS2
const char note_22[] PROGMEM = "A-2";  // 20 - NOTE_A2
const char note_23[] PROGMEM = "A#2";  // 22 - NOTE_AS2
const char note_24[] PROGMEM = "B-2";  // 22 - NOTE_B2
const char note_25[] PROGMEM = "C-3";  // 3 - NOTE_C3
const char note_26[] PROGMEM = "C#3";  // 3 - NOTE_CS3
const char note_27[] PROGMEM = "D-3";  // 3 - NOTE_D3
const char note_28[] PROGMEM = "D#3";  // 4 - NOTE_DS3
const char note_29[] PROGMEM = "E-3";  // 5 - NOTE_E3
const char note_30[] PROGMEM = "F-3";  // 6 - NOTE_F3
const char note_31[] PROGMEM = "F#3";  // 7 - NOTE_FS3
const char note_32[] PROGMEM = "G-3";  // 8 - NOTE_G3
const char note_33[] PROGMEM = "G#3";  // 9 - NOTE_GS3
const char note_34[] PROGMEM = "A-3";  // 30 - NOTE_A3
const char note_35[] PROGMEM = "A#3";  // 33 - NOTE_AS3
const char note_36[] PROGMEM = "B-3";  // 33 - NOTE_B3
const char note_37[] PROGMEM = "C-4";  // 4 - NOTE_C4
const char note_38[] PROGMEM = "C#4";  // 4 - NOTE_CS4
const char note_39[] PROGMEM = "D-4";  // 3 - NOTE_D4
const char note_40[] PROGMEM = "D#4";  // 4 - NOTE_DS4
const char note_41[] PROGMEM = "E-4";  // 5 - NOTE_E4
const char note_42[] PROGMEM = "F-4";  // 6 - NOTE_F4
const char note_43[] PROGMEM = "F#4";  // 7 - NOTE_FS4
const char note_44[] PROGMEM = "G-4";  // 8 - NOTE_G4
const char note_45[] PROGMEM = "G#4";  // 9 - NOTE_GS4
const char note_46[] PROGMEM = "A-4";  // 40 - NOTE_A4
const char note_47[] PROGMEM = "A#4";  // 44 - NOTE_AS4
const char note_48[] PROGMEM = "B-4";  // 44 - NOTE_B4
const char note_49[] PROGMEM = "C-5";  // 5 - NOTE_C5
const char note_50[] PROGMEM = "C#5";  // 5 - NOTE_CS5
const char note_51[] PROGMEM = "D-5";  // 3 - NOTE_D5
const char note_52[] PROGMEM = "D#5";  // 4 - NOTE_DS5
const char note_53[] PROGMEM = "E-5";  // 5 - NOTE_E5
const char note_54[] PROGMEM = "F-5";  // 6 - NOTE_F5
const char note_55[] PROGMEM = "F#5";  // 7 - NOTE_FS5
const char note_56[] PROGMEM = "G-5";  // 8 - NOTE_G5
const char note_57[] PROGMEM = "G#5";  // 9 - NOTE_GS5
const char note_58[] PROGMEM = "A-5";  // 50 - NOTE_A5
const char note_59[] PROGMEM = "A#5";  // 55 - NOTE_AS5
const char note_60[] PROGMEM = "B-5";  // 55 - NOTE_B5
const char note_61[] PROGMEM = "C-6";  // 6 - NOTE_C6
const char note_62[] PROGMEM = "C#6";  // 6 - NOTE_CS6
const char note_63[] PROGMEM = "D-6";  // 3 - NOTE_D6
const char note_64[] PROGMEM = "D#6";  // 4 - NOTE_DS6
const char note_65[] PROGMEM = "E-6";  // 5 - NOTE_E6
const char note_66[] PROGMEM = "F-6";  // 6 - NOTE_F6
const char note_67[] PROGMEM = "F#6";  // 7 - NOTE_FS6
const char note_68[] PROGMEM = "G-6";  // 8 - NOTE_G6
const char note_69[] PROGMEM = "G#6";  // 9 - NOTE_GS6
const char note_70[] PROGMEM = "A-6";  // 60 - NOTE_A6
const char note_71[] PROGMEM = "A#6";  // 66 - NOTE_AS6
const char note_72[] PROGMEM = "B-6";  // 66 - NOTE_B6
const char note_73[] PROGMEM = "C-7";  // 7 - NOTE_C7
const char note_74[] PROGMEM = "C#7";  // 7 - NOTE_CS7
const char note_75[] PROGMEM = "D-7";  // 3 - NOTE_D7
const char note_76[] PROGMEM = "D#7";  // 4 - NOTE_DS7
const char note_77[] PROGMEM = "E-7";  // 5 - NOTE_E7
const char note_78[] PROGMEM = "F-7";  // 6 - NOTE_F7
const char note_79[] PROGMEM = "F#7";  // 7 - NOTE_FS7
const char note_80[] PROGMEM = "G-7";  // 8 - NOTE_G7
const char note_81[] PROGMEM = "G#7";  // 9 - NOTE_GS7
const char note_82[] PROGMEM = "A-7";  // 70 - NOTE_A7
const char note_83[] PROGMEM = "A#7";  // 77 - NOTE_AS7
const char note_84[] PROGMEM = "B-7";  // 77 - NOTE_B7
const char note_85[] PROGMEM = "C-8";  // 8 - NOTE_C8
const char note_86[] PROGMEM = "C#8";  // 8 - NOTE_CS8
const char note_87[] PROGMEM = "D-8";  // 3 - NOTE_D8
const char note_88[] PROGMEM = "D#8";  // 4 - NOTE_DS8

const char* note_table[] PROGMEM = {
 note_0,
 note_1,
 note_2,
 note_3,
 note_4,
 note_5,
 note_6,
 note_7,
 note_8,
 note_9,
 note_10,
 note_11,
 note_12,
 note_13,
 note_14,
 note_15,
 note_16,
 note_17,
 note_18,
 note_19,
 note_20,
 note_21,
 note_22,
 note_23,
 note_24,
 note_25,
 note_26,
 note_27,
 note_28,
 note_29,
 note_30,
 note_31,
 note_32,
 note_33,
 note_34,
 note_35,
 note_36,
 note_37,
 note_38,
 note_39,
 note_40,
 note_41,
 note_42,
 note_43,
 note_44,
 note_45,
 note_46,
 note_47,
 note_48,
 note_49,
 note_50,
 note_51,
 note_52,
 note_53,
 note_54,
 note_55,
 note_56,
 note_57,
 note_58,
 note_59,
 note_60,
 note_61,
 note_62,
 note_63,
 note_64,
 note_65,
 note_66,
 note_67,
 note_68,
 note_69,
 note_70,
 note_71,
 note_72,
 note_73,
 note_74,
 note_75,
 note_76,
 note_77,
 note_78,
 note_79,
 note_80,
 note_81,
 note_82,
 note_83,
 note_84,
 note_85,
 note_86,
 note_87,
 note_88,
};

void getNoteString(char * buffer, uint8_t i) {
    if (i>88) strcpy(buffer,"@@@");
    else strcpy(buffer, note_table[i]);
}

