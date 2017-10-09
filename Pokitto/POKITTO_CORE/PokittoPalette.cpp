/**************************************************************************/
/*!
    @file     PokittoPalette.cpp
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

#include "PokittoDisplay.h"
#include "Pokitto_settings.h"
#include "GBcompatibility.h"
#include <stdio.h>
#include <string.h>

#ifndef POK_SIM
#include "HWLCD.h"
#else
#include "SimLCD.h"
#endif

using namespace Pokitto;


void Display::loadRGBPalette(const unsigned char* p) {
    for (int i=0;i<PALETTE_SIZE;i++) palette[i] = RGBto565(p[i*3], p[i*3+1],p[i*3+2]);
    paletteptr = palette;
}

void Display::load565Palette(const uint16_t* p) {
    for (int i=0;i<PALETTE_SIZE;i++) palette[i] = p[i];
    paletteptr = palette;
}

void Display::rotatePalette(int8_t step) {
    uint16_t tpal[PALETTE_SIZE];
    if (step == 0) return;
    step = 0-step;
    if (step>0) {
        for (int i=step;i<PALETTE_SIZE;i++) tpal[i]=palette[i-step]; // palette revolves up, new color 1 becomes old color 0
        for (int i=0; i < step; i++) tpal[i]=palette[PALETTE_SIZE-step+i]; // overflow topmost values to bottom of new palette
    } else {
        for (int i=0;i<PALETTE_SIZE+step;i++)
        {
            tpal[i]=palette[i-step];
            }// palette revolves down, new color 0 becomes old color 1
        for (int i=0;i<-step; i++) {
                tpal[PALETTE_SIZE+step+i]=palette[i];
        }
        // overflow bottom values to top of new palette
    }
    for (int i=0; i<PALETTE_SIZE;i++) palette[i] = tpal[i];
}

uint16_t Display::RGBto565(uint8_t R,uint8_t G,uint8_t B) {
    uint16_t color;
    color = B>>3;
    color |= ((G >> 2) << 5);
    color |= ((R >> 3) << 11);
    return color;
}

uint16_t Display::interpolateColor(uint16_t c1, uint16_t c2, uint8_t factor) {
    int16_t R,G,B;
    int16_t dR,dG,dB;
    uint16_t color;

    B = (c1 & 0x1F);
    dB = (c2 & 0x1F)-B;
    dB = (dB*factor)>>8;
    B += dB;
    if (B<0) B = 0;

    G = ((c1>>5) & 0x3F);
    dG = ((c2>>5) & 0x3F)-G;
    dG = (dG*factor)>>8;
    G += dG;
    if (G<0) G=0;

    R = (c1>>11);
    dR = (c2>>11)-R;
    dR = (dR*factor)>>8;
    R += dR;
    if (R<0) R=0;

    color = B;
    color |= (G << 5);
    color |= (R << 11);
    return color;
}

void Display::tweenPalette(uint16_t* ram_pal, const uint16_t* pal_1, const uint16_t* pal_2, uint8_t factor) {
    for (uint8_t i = 0; i<16 ; i++) {
        ram_pal[i] = interpolateColor(pal_1[i],pal_2[i],factor);
    }
}



