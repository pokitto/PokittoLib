/**************************************************************************/
/*!
    @file     minibuttons.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Jonne Valola
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

#include "miniGFX.h"
#include "Pokitto_settings.h"
#include "binary.h"

//pok_gfxdepth pok_depth = (pok_gfxdepth) POK_COLORDEPTH;
//pok_gfxscaling pok_scale = (pok_gfxscaling) POK_STRETCH;
unsigned int pok_directcolor = 0xFFFF;
unsigned int palette[16];
unsigned int* paletteptr;

void pokDirectChar(int x, int y, const unsigned char *bitmap, unsigned int index)
{
    unsigned char w = *bitmap;
	unsigned char h = *(bitmap + 1);
	unsigned char xtra=0;
	if (w&0x7) xtra=1;
	bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
	signed char i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;

	for (i = 0; i < w; i++) {
            byteNum = i / 8;
            bitNum = i % 8;
            for (j = 0; j < h; j++) {
                if (*(bitmap + j * byteWidth + byteNum) & (B10000000 >> bitNum)) {
                    //pokDirectPixel(x + i, y + j,pok_directcolor);
                    pokDirectPixel(x + j, y + 8-i,pok_directcolor);
                }
            }
    }
}

void pokDirectBitmap(int x, int y, const unsigned char *bitmap, unsigned char depth, unsigned scale) {
    unsigned char w = *bitmap;
	unsigned char h = *(bitmap + 1);
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    signed char i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;

    if (depth == 1) {
        for (i = 0; i < w; i++) {
            byteNum = i / 8;
            bitNum = i % 8;
            for (j = 0; j < h; j++) {
                if (*(bitmap + j * byteWidth + byteNum) & (B10000000 >> bitNum)) {
                    pokDirectPixel(x + i, y + j,pok_directcolor);
                }
            }
        }
    } else if (depth == 4) {
        for (j = 0; j < h; j+=1) {
            for (i = 0; i < w; i+=2) {
                unsigned int col = paletteptr[*bitmap>>4]; //higher nibble
                if (scale==2) {
                        pokDirectPixel(x + (i<<1), y + (j<<1),col);
                        pokDirectPixel(x + (i<<1) + 1, y + (j<<1),col);
                        pokDirectPixel(x + (i<<1) + 1, y + (j<<1) + 1,col);
                        pokDirectPixel(x + (i<<1), y + (j<<1) + 1,col);
                } else pokDirectPixel(x + i, y + j,col);
                col = paletteptr[*bitmap&0xF]; // lower nibble
                if (scale==2) {
                        pokDirectPixel(x + (i<<1) + 2, y + (j<<1),col);
                        pokDirectPixel(x + (i<<1) + 1 + 2, y + (j<<1),col);
                        pokDirectPixel(x + (i<<1) + 1 + 2, y + (j<<1) + 1,col);
                        pokDirectPixel(x + (i<<1) + 2 , y + (j<<1) + 1,col);
                } else pokDirectPixel(x + i + 1, y + j,col);
                bitmap++;
            }
        }
    }

}


const unsigned char Pokitto_logo[] = {
84,24,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0xF0,0x07,0x80,0x1F,0xFE,0x00,0x00,
0x00,0x00,0x00,0x00,0xF0,0x07,0x83,0xDF,0xFE,0x00,0x00,
0x03,0x80,0x00,0x00,0xF0,0x07,0x83,0xDF,0xFE,0x00,0x00,
0x1F,0xF0,0x00,0x00,0xF0,0x07,0x83,0xDF,0xFE,0x00,0x00,
0x3F,0xF8,0x00,0x00,0xF0,0x00,0x03,0xC1,0xE0,0x00,0x00,
0x7F,0xFC,0x07,0xC0,0xF3,0xE7,0xBF,0xFD,0xE0,0x7C,0x00,
0x7C,0x7C,0x1F,0xF0,0xF7,0xC7,0xBF,0xFD,0xE1,0xFF,0x00,
0xF8,0x3E,0x3F,0xF8,0xFF,0x87,0xBF,0xFD,0xE3,0xFF,0x80,
0xF0,0x1E,0x7F,0xFC,0xFF,0x07,0xBF,0xFD,0xE7,0xFF,0xC0,
0xF0,0x1E,0x7C,0x7C,0xFE,0x07,0x83,0xC1,0xE7,0xC7,0xC0,
0xF0,0x1E,0xF8,0x3E,0xFC,0x07,0x83,0xC1,0xEF,0x83,0xE0,
0xF8,0x3E,0xF0,0x1E,0xFC,0x07,0x83,0xC1,0xEF,0x01,0xE0,
0xFC,0x7C,0xF0,0x1E,0xFE,0x07,0x83,0xC1,0xF7,0x01,0xE0,
0xFF,0xFC,0xF0,0x1E,0xFF,0x07,0x83,0xC0,0xFF,0x01,0xE0,
0xFF,0xF8,0xF8,0x3E,0xFF,0x87,0x83,0xC0,0xFF,0x83,0xE0,
0xFF,0xE0,0x7C,0x7C,0xF7,0xC7,0x83,0xC0,0x7F,0xC7,0xC0,
0xF3,0x80,0x7F,0xFC,0xF3,0xE7,0x83,0xC0,0x1F,0xFF,0xC0,
0xF0,0x00,0x3F,0xF8,0xF1,0xF7,0x83,0xC0,0x07,0xFF,0x80,
0xF0,0x00,0x1F,0xF0,0x00,0x07,0x83,0xC0,0x01,0xFF,0x00,
0xF0,0x00,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x7C,0x00,
};
