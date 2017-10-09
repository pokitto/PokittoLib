/**************************************************************************/
/*!
    @file     Synth_osc.h
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

#ifndef SYNTH_OSC_H
#define SYNTH_OSC_H

#include "Pokitto.h"

typedef uint8_t byte;

struct OSC {
  byte on;
  byte wave;
  byte loop;
  byte echo;
  byte echodiv;
  byte adsr;
  byte tonic;

  uint16_t count;

  uint16_t vol;
  uint16_t cinc; // how much to add per cycle
  uint16_t output; // output is stored as  16 bit value and shifted before its put in OCR2B

  uint8_t adsrphase;
  uint16_t adsrvol;
  uint16_t attack; // Attack change
  uint16_t decay; // Attack change
  uint16_t sustain; // Attack change
  uint16_t release; // Attack change

  int16_t pitchbend;   // bends cycle counter (more is higher pitch)
  int16_t maxbend; // maximum bend before stopping
  int16_t bendrate; // how much to bend by every cycle

  uint8_t vibrate;
  uint8_t arpmode;
  uint8_t arpspeed;
  uint8_t arpstep;
  uint8_t overdrive;
  uint8_t kick;

  uint32_t duration;
};



#endif // SYNTH_OSC_H

