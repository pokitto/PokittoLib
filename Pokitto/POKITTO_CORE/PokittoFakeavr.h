/**************************************************************************/
/*!
    @file     PokittoFakeavr.h
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

#ifndef _POKITTO_FAKEAVR_H_
#define _POKITTO_FAKEAVR_H_

#include "PokittoItoa.h"
#include "binary.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef POK_SIM
#include <stdbool.h>
#else
#include <stdbool.h>
#endif // POK_SIM

#define _BV(bit) (1 << (bit)) //jonne

/** data types */
#ifndef _RPCNDR_H
typedef bool boolean;
#endif
typedef uint8_t byte;

/** PROGMEM */
#define PROGMEM
//#define F

/** AVR program memory string functions **/
#define sprintf_P sprintf
#define strstr_P strstr
#define strcpy_P strcpy

#define utoa pokItoa
#define ultoa pokItoa

/** PROGMEM hacks **/
#define PROGMEM
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
    #ifndef WIN32
    #define pgm_read_word(addr) (*(addr))
    #else
    #define pgm_read_word(addr) (*(const unsigned int *)(addr))
    #endif // WIN32

/** min max & others **/


#ifndef DISABLEAVRMIN
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#endif // DISABLEAVRMIN
#define __avrmax(a,b) ((a)>(b)?(a):(b))
#define __avrmin(a,b) ((a)<(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define min2(a,b) (((a)<(b))?(a):(b))



#ifdef __cplusplus
extern "C" {
#endif

void cli();
void sei();

#ifdef __cplusplus
}
#endif
#endif

