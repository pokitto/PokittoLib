/**************************************************************************/
/*!
    @file     PokittoFonts.h
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

#ifndef POK_FONTS_H
#define POK_FONTS_H

#include "Pokitto_settings.h"

extern const unsigned char fontMini[];
extern const unsigned char fontDonut[];
extern const unsigned char fontDragon[];
extern const unsigned char fontKarateka[];
extern const unsigned char fontKoubit[];
extern const unsigned char fontRunes[];
extern const unsigned char fontTight[];
extern const unsigned char fontTiny[];
extern const unsigned char fontAdventurer[];
extern const unsigned char font5x7[];
extern const unsigned char font3x5[];
extern const unsigned char font3x3[];
extern const unsigned char fontC64[];
extern const unsigned char fntC64UIGfx[];
extern const unsigned char fontZXSpec[];
extern const unsigned char fontTIC806x6[];

#if POK_GAMEBUINO_SUPPORT > 0
#define DEFAULT_FONT font3x5
#else
#define DEFAULT_FONT font5x7
#endif // POK_GAMEBUINO_SUPPORT


#endif

