#include "Arrays.h"

const char noteLetters[12][2] = {{'B', '-'},{'C', '-'}, {'C', '#'}, {'D', '-'}, {'D', '#'}, {'E', '-'}, {'F', '-'},
                           {'F', '#'}, {'G', '-'}, {'G', '#'}, {'A', '-'}, {'A', '#'}};

const char* waveNames[]={"OFF", "SQUARE", "SAW", "TRIANGLE", "NOISE", "TONE NOISE"};
const char* arpModes[]={"OFF" ,"M Slow", "M Med", "M Fast", "m Slow", "m Med", "m Fast", "Vib Slow", "Vib Med", "Vib Fast", "Oct Slow", "Oct Med", "Oct Fast", "Funk Slow", "Funk Med", "Funk Fast"};

const char rboyChar[] = "Rboy Tracker song file\n";
const char BPMChar[] = "BPM=";
const char lastPatternChar[] = "Last pattern=";
const char loopToChar[] = "Loop to=";
const char patchesChar[] = "Patches=";
const char blockSeqChar[] = "Block sequence ";
const char blockChar[] = "Block ";
const char rowChar[] = " row ";
const char unknownChar[] = "0.030000";
const char waveChar[] = "Waveform=";
const char volChar[] = "Volume=";
const char pitchRateChar[] = "Pitch bend rate=";
const char pitchMaxChar[] = "Pitch bend max=";
const char vibChar[] = "Vibrato rate=";
const char arpChar[] = "Arpeggio mode=";
const char ADSRChar[] = "ADSR=";
const char attackChar[] = "Attack=";
const char decayChar[] = "Decay=";
const char sustainChar[] = "Sustain=";
const char releaseChar[] = "Release=";
const char loopChar[] = "Loop=";
const char echoChar[] = "Echo=";
const char overdriveChar[] = "Overdrive=";
const char drumChar[] = "Drum kick=";

const uint8_t pauseBitmap[] = {8, 5,
0b00010100,0b00010100,
0b00010100,0b00010100,
0b00010100,0b00010100,
0b00010100,0b00010100,
0b00010100,0b00010100};

const uint8_t playBitmap[] = {8, 5,
0b00010101,0b01000000,
0b00010101,0b01010000,
0b00010101,0b01010100,
0b00010101,0b01010000,
0b00010101,0b01000000};

const uint8_t stopBitmap[] = {8, 5,
0b00010101,0b01010100,
0b00010101,0b01010100,
0b00010101,0b01010100,
0b00010101,0b01010100,
0b00010101,0b01010100};

const uint8_t pointBitmap[] = {8, 5,
0b00000001,0b01010100,
0b00000101,0b01010100,
0b00010101,0b01010100,
0b00000101,0b01010100,
0b00000001,0b01010100};

