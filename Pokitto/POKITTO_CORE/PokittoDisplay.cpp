/**************************************************************************/
/*!
    @file     PokittoDisplay.cpp
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


/* THE SEGMENT BELOW PERTAINS TO CIRCLE DRAWING FUNCTIONS ONLY
*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).
Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
Copyright (c) 2013 Adafruit Industries.  All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "PokittoDisplay.h"
#include "Pokitto_settings.h"
#include "GBcompatibility.h"
#include "PokittoCore.h"
#include "PokittoSound.h"
#include <stdio.h>
#include <string.h>

#ifndef POK_SIM
#include "HWLCD.h"
#else
#include "SimLCD.h"
#endif

Pokitto::Core core;
Pokitto::Sound _pdsound;

using namespace Pokitto;



uint8_t* Display::m_scrbuf;
uint8_t* Display::m_tileset;
uint8_t* Display::m_tilebuf;
uint8_t* Display::m_tilecolorbuf;
uint8_t Display::m_mode, Display::m_colordepth;
uint8_t Display::palOffset;
SpriteInfo Display::m_sprites[SPRITE_COUNT];
uint8_t Display::fontSize=1;
int16_t Display::cursorX,Display::cursorY;
uint16_t Display::m_w,Display::m_h;
uint8_t Display::fontWidth, Display::fontHeight;
bool Display::textWrap=true;

uint8_t Display::persistence = 0;
uint16_t Display::color = 1;
uint16_t Display::bgcolor = 0;
uint16_t Display::invisiblecolor = 17;
uint16_t Display::directcolor=0xFFFF;
uint16_t Display::directbgcolor=0x0;
bool Display::directtextrotated=false;
int16_t Display::clipX = 0;
int16_t Display::clipY = 0;
int16_t Display::clipW = LCDWIDTH;
int16_t Display::clipH = LCDHEIGHT;

uint16_t* Display::paletteptr;
uint16_t Display::palette[PALETTE_SIZE];
const unsigned char* Display::font;
int8_t Display::adjustCharStep = 1;
int8_t Display::adjustLineStep = 1;
bool Display::fixedWidthFont = false;

/** drawing canvas **/
//uint8_t* Display::canvas; // points to the active buffer. if null, draw direct to screen

/** screenbuffer **/
uint8_t Display::bpp = POK_COLORDEPTH;
#ifndef POK_TILEDMODE
#if (POK_SCREENMODE == MODE_HI_MONOCHROME)
    uint8_t Display::width = POK_LCD_W;
    uint8_t Display::height = POK_LCD_H;
    uint8_t Display::screenbuffer[((POK_LCD_H+1)*POK_LCD_W)*POK_COLORDEPTH/8]; // maximum resolution
#elif (POK_SCREENMODE == MODE_HI_4COLOR)
    uint8_t Display::width = POK_LCD_W;
    uint8_t Display::height = POK_LCD_H;
    uint8_t __attribute__((section (".bss"))) Display::screenbuffer[((POK_LCD_H)*POK_LCD_W)/4]; // maximum resolution
#elif (POK_SCREENMODE == MODE_FAST_16COLOR)
    uint8_t Display::width = POK_LCD_W/2;
    uint8_t Display::height = POK_LCD_H/2;
    uint8_t Display::screenbuffer[(((POK_LCD_H/2)+1)*POK_LCD_W/2)*POK_COLORDEPTH/8]; // half resolution
#elif (POK_SCREENMODE == MODE_HI_16COLOR)
    uint8_t Display::width = POK_LCD_W;
    uint8_t Display::height = POK_LCD_H;
    uint8_t Display::screenbuffer[POK_LCD_H*POK_LCD_W/2]; // 4 bits per pixel
#elif (POK_SCREENMODE == MODE_LAMENES)
    uint8_t Display::width = 128;
    uint8_t Display::height = 120;
    uint8_t Display::screenbuffer[((121)*128)*POK_COLORDEPTH/8]; // half resolution
#elif (POK_SCREENMODE == MODE_GAMEBOY)
    uint8_t Display::width = 160;
    uint8_t Display::height = 144;
    uint8_t Display::screenbuffer[160*144/4];
#else
    uint8_t Display::width = 84;
    uint8_t Display::height = 48;
    uint8_t Display::screenbuffer[128*64]; // not needed because Gamebuino and Arduboy have their own buffer
#endif
#else //Tiledmode
#if (POK_SCREENMODE == MODE_TILED_1BIT)
    uint8_t Display::width = POK_LCD_W;
    uint8_t Display::height = POK_LCD_H;
    uint8_t Display::screenbuffer[0];
#else
    uint8_t Display::width = POK_LCD_W;
    uint8_t Display::height = POK_LCD_H;
    uint8_t Display::screenbuffer[0];
#endif
#endif //tiledmode

// RLE decoding
#define RLE_ESC_EOL 0
#define RLE_ESC_EOB 1
#define RLE_ESC_OFFSET 2

Display::Display() {
    m_scrbuf = screenbuffer;
    setDefaultPalette();
    m_mode = 1; // direct printing on by default
    m_w = POK_LCD_W;
    m_h = POK_LCD_H;
    setFont(DEFAULT_FONT);
    invisiblecolor=17;
    bgcolor=0;
    if (POK_COLORDEPTH) m_colordepth = POK_COLORDEPTH;
    else m_colordepth = 4;
    #if POK_GAMEBUINO_SUPPORT
    setColorDepth(1);
    #endif // POK_GAMEBUINO_SUPPORT

    // Reset sprites
    m_tilecolorbuf = NULL; //!!HV
    for (uint8_t s = 0; s < SPRITE_COUNT; s++) m_sprites[s].bitmapData = NULL;
}

uint16_t Display::getWidth() {
    return width;
}

uint8_t Display::getNumberOfColors() {
    return 1<<POK_COLORDEPTH;
}

uint16_t Display::getHeight() {
    return height;
}

uint8_t Display::getColorDepth() {
    return m_colordepth;
}

void Display::setColorDepth(uint8_t v) {
    if (v > POK_COLORDEPTH) v=POK_COLORDEPTH;
    m_colordepth = v;
}

void Display::clearLCD() {
    lcdFillSurface(0);
    setCursor(0,0); // old basic computer style
}

void Display::fillLCD(uint16_t c) {
    lcdFillSurface(c);
}

void Display::directPixel(int16_t x, int16_t y, uint16_t color) {
    if ((invisiblecolor < PALETTE_SIZE) && (invisiblecolor < 16) && (color == palette[invisiblecolor])) return;
    lcdPixel(x,y,color);
}

void Display::directTile(int16_t x, int16_t y, int16_t x2, int16_t y2, uint16_t* gfx) {
    lcdTile(x,y,x2,y2,gfx);
}

void Display::directRectangle(int16_t x, int16_t y,int16_t x2, int16_t y2, uint16_t color) {
    lcdRectangle(x,y,x2,y2,color);
}

void Display::begin() {
    lcdInit();
}

void Display::setCursor(int16_t x,int16_t y) {
    cursorX = x;
    cursorY = y;
}

/**
 * Update the display.
 * The update rect is used for drawing only part of the screen buffer to LCD. Because of speed optimizations, the
 * x, y, and width of the update rect must be dividable by 4 pixels, and the height must be dividable by 8 pixels.
 * Note: The update rect is currently used for 220x176, 4 colors, screen mode only.
 * @param useDirectMode True, if only direct screen drawing is used. False, if the screen buffer is drawn. Note: If sprites are enabled, they are drawn in both modes.
 * @param updRectX The update rect.
 * @param updRectY The update rect.
 * @param updRectW The update rect.
 * @param updRectH The update rect.
 */
void Display::update(bool useDirectDrawMode, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH) {

    #if POK_SCREENMODE == MODE_HI_4COLOR
    // If there is one or more sprites, use sprite enabled drawing.
    if (m_sprites[0].bitmapData != NULL)
        lcdRefreshMode1Spr(m_scrbuf, updRectX, updRectY, updRectW, updRectH, paletteptr, m_sprites, useDirectDrawMode);
    else if (!useDirectDrawMode)
        lcdRefreshMode1(m_scrbuf, updRectX, updRectY, updRectW, updRectH, paletteptr);
    #endif

    // For the screen modes that do not support sprites, return if the direct draw mode is used.
    if (! useDirectDrawMode) {

        #if POK_SCREENMODE == MODE_GAMEBOY
        lcdRefreshModeGBC(m_scrbuf, paletteptr);
        #endif

        #if POK_SCREENMODE == MODE_HI_16COLOR
        lcdRefreshMode3(m_scrbuf, paletteptr);
        #endif

        #if POK_SCREENMODE == MODE_FAST_16COLOR
        lcdRefreshMode2(m_scrbuf, paletteptr);
        #endif

        #if POK_SCREENMODE == MODE_GAMEBUINO_16COLOR
        lcdRefreshGB(m_scrbuf, paletteptr);
        #endif

        #if POK_SCREENMODE == MODE_ARDUBOY_16COLOR
        lcdRefreshAB(m_scrbuf, paletteptr);
        #endif

        #if POK_SCREENMODE == MODE_TILED_1BIT
        lcdRefreshT1(m_tilebuf, m_tilecolorbuf, m_tileset, paletteptr);
        #endif
    }

    if (!persistence) clear();

    /** draw volume bar if visible **/
    #if POK_SHOW_VOLUME > 0
    if (core.volbar_visible) {
            core.drawvolbar(4,20,_pdsound.getVolume(),true);
            core.volbar_visible--;
    }
    #endif // POK_SHOW_VOLUME

    /** draw FPS if visible **/
    #ifdef PROJ_USE_FPS_COUNTER

	// Store current state
    bool temp = isDirectPrintingEnabled();
    uint16_t oldcol = directcolor;
    uint16_t oldinvisiblecolor = invisiblecolor;
    uint16_t oldbgcol = directbgcolor;
    bool olddirecttextrotated = directtextrotated;
    int8_t oldadjustCharStep = adjustCharStep;
    const unsigned char * oldFont = font;

    // Print FPS
    char str[16];
    sprintf(str,"FPS:%d ", (int)core.fps_counter);
    directcolor = COLOR_WHITE;
    invisiblecolor = COLOR_BLACK;
    directbgcolor = 0x0001; // Cannot be black as that is transparent color
    directtextrotated = true;
    adjustCharStep = 0;
    setFont(fontC64);
    enableDirectPrinting(true);
    print(0,0, str);

	// Restore state
    enableDirectPrinting(temp);
    directcolor = oldcol;
    invisiblecolor =  oldinvisiblecolor;
    directbgcolor = oldbgcol;
    directtextrotated = olddirecttextrotated;
    adjustCharStep = oldadjustCharStep;
    setFont(font);
    #endif
}

void Display::directBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t depth, uint8_t scale) {
    uint8_t w = *bitmap;
	uint8_t h = *(bitmap + 1);
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    int16_t i, j;
    int8_t byteNum, bitNum, byteWidth = (w + 7) >> 3;

    if (depth == 1) {
        for (i = 0; i < w; i++) {
            byteNum = i / 8;
            bitNum = i % 8;
            for (j = 0; j < h; j++) {
                if (*(bitmap + j * byteWidth + byteNum) & (0x80 >> bitNum)) { //0x80 = B10000000
                    if (scale==1) directPixel(x + i, y + j,directcolor);
                    else {
                        directPixel(x + i + i, y + j + j,directcolor);
                        directPixel(x + 1 + i + i, y + j + j,directcolor);
                        directPixel(x + i + i, y + j + j + 1,directcolor);
                        directPixel(x + i + i + 1 , y + j + j + 1,directcolor);
                    }
                }
            }
        }
    } else if (depth == 4) {
        for (j = 0; j < h; j+=1) {
            for (i = 0; i < w; i+=2) {
                uint16_t col = paletteptr[*bitmap>>4]; //higher nibble
                if (scale==2) {
                        directPixel(x + (i<<1), y + (j<<1),col);
                        directPixel(x + (i<<1) + 1, y + (j<<1),col);
                        directPixel(x + (i<<1) + 1, y + (j<<1) + 1,col);
                        directPixel(x + (i<<1), y + (j<<1) + 1,col);
                } else directPixel(x + i, y + j,col);
                col = paletteptr[*bitmap&0xF]; // lower nibble
                if (scale==2) {
                        directPixel(x + (i<<1) + 2, y + (j<<1),col);
                        directPixel(x + (i<<1) + 1 + 2, y + (j<<1),col);
                        directPixel(x + (i<<1) + 1 + 2, y + (j<<1) + 1,col);
                        directPixel(x + (i<<1) + 2 , y + (j<<1) + 1,col);
                } else directPixel(x + i + 1, y + j,col);
                bitmap++;
            }
        }
    }

}

int Display::directChar(int16_t x, int16_t y, uint16_t index){
    const uint8_t* bitmap = font;
    uint8_t w = *bitmap;
	uint8_t h = *(bitmap + 1);
	uint8_t hbytes=0, xtra=1;
	if (h==8 || h==16) xtra=0; //don't add if exactly on byte limit
	hbytes=(h>>3)+xtra; //GLCD fonts are arranged w+1 times h/8 bytes
	//bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
	bitmap = bitmap + 4 + index * (w * hbytes + 1); //add an offset to the pointer (fonts !)
	//int8_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    int8_t i, j, numBytes;
    numBytes = *bitmap++; //first byte of char is the width in bytes
    // GLCD fonts are arranged LSB = topmost pixel of char, so its easy to just shift through the column
    uint16_t bitcolumn; //16 bits for 2x8 bit high characters

	for (i = 0; i < numBytes; i++) {
            bitcolumn = *bitmap++;
            if (hbytes == 2) bitcolumn |= (*bitmap++)<<8; // add second byte for 16 bit high fonts
            for (j = 0; j < h; j++) {
                if (bitcolumn&0x1) {
                    if (fontSize==2) {
                        directPixel(x + (i<<1)  , y + (j<<1),directcolor);
                        directPixel(x + (i<<1)+1, y + (j<<1),directcolor);
                        directPixel(x + (i<<1)  , y + (j<<1)+1,directcolor);
                        directPixel(x + (i<<1)+1, y + (j<<1)+1,directcolor);
                    } else {
                        if(directtextrotated) directPixel(y + h - j - 1, x + i,directcolor);
                        else directPixel(x + i, y + j,directcolor);
                    }

                } else if (directbgcolor != invisiblecolor) {
                    if (fontSize==2) {
                        directPixel(x + (i<<1)  , y + (j<<1),directbgcolor);
                        directPixel(x + (i<<1)+1, y + (j<<1),directbgcolor);
                        directPixel(x + (i<<1)  , y + (j<<1)+1,directbgcolor);
                        directPixel(x + (i<<1)+1, y + (j<<1)+1,directbgcolor);
                    } else {
                        if(directtextrotated) directPixel(y + h - j - 1, x + i,directbgcolor);
                        else directPixel(x + i, y + j,directbgcolor);
                    }
                }
                bitcolumn>>=1;
            }
    }
    return (numBytes+adjustCharStep)*fontSize; // for character stepping
}

int Display::bufferChar(int16_t x, int16_t y, uint16_t index){
    const uint8_t* bitmap = font;
    uint8_t w = *bitmap;
	uint8_t h = *(bitmap + 1);
	uint8_t hbytes=0, xtra=1;
	if (h==8 || h==16) xtra=0; //don't add if exactly on byte limit
	hbytes=(h>>3)+xtra; //GLCD fonts are arranged w+1 times h/8 bytes
	//bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
	bitmap = bitmap + 4 + index * (w * hbytes + 1); //add an offset to the pointer (fonts !)
	//int8_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    int8_t i, j, numBytes;
    numBytes = *bitmap++; //first byte of char is the width in bytes
    // GLCD fonts are arranged LSB = topmost pixel of char, so its easy to just shift through the column
    uint16_t bitcolumn; //16 bits for 2x8 bit high characters

	for (i = 0; i < numBytes; i++) {
            bitcolumn = *bitmap++;
            if (hbytes == 2) bitcolumn |= (*bitmap++)<<8; // add second byte for 16 bit high fonts
            for (j = 0; j <= h; j++) { // was j<=h
                #if PROJ_ARDUBOY > 0
                if (bitcolumn&0x1) {
                    drawPixel(x + i, y + 7 - j,color);
                } else drawPixel(x + i, y + 7 - j,bgcolor);
                bitcolumn>>=1;
                #else
                if (bitcolumn&0x1) {
                    drawPixel(x + i, y + j,color);
                } else drawPixel(x + i, y + j,bgcolor);
                bitcolumn>>=1;
                #endif // PROJ_ARDUBOY

            }
    }

    return numBytes+adjustCharStep; // for character stepping
}

void Display::clear() {

    uint8_t c=0;
    c = bgcolor & (PALETTE_SIZE-1) ; //don't let palette go out of bounds
    if (bpp==1 && bgcolor) c=0xFF; // bgcolor !=0, set all pixels
    else if (bpp==2) {
        c = bgcolor & 0x3;
        c = c | (c << 2);
        c = c | (c << 4);
    } else {
        c = (c & 0x0F) | (c << 4);
    }
    uint16_t j = sizeof(screenbuffer);
    memset((void*)m_scrbuf,c,j);

    setCursor(0,0);

}

void Display::scroll(int16_t pixelrows) {
    uint16_t index = 0, index2=0,oc;
    if (pixelrows==0) return;
    if (pixelrows >= height) pixelrows=height-1;
    if (bpp == 4) index2 = pixelrows*width/2;
    else if (bpp == 2) index2 = pixelrows*width/4;
    else return;
    oc = color;
    color = bgcolor;
    if (pixelrows>0) {
    for (uint16_t y=0;y<height-pixelrows;y++) {
            for (uint16_t x=0;x<(width/8)*bpp;x++) screenbuffer[index++]=screenbuffer[index2++];
    }
    fillRect(0,cursorY,width,height);
    } else {
    for (uint16_t y=pixelrows;y<height;y++) {
            for (uint16_t x=0;x<(width*bpp)/8;x++) screenbuffer[index2++]=screenbuffer[index++];
    }
    fillRect(0,0,width,pixelrows);
    }
    color=oc;
}

void Display::fillScreen(uint16_t c) {
    c = c & (PALETTE_SIZE-1) ; //don't let palette go out of bounds
    if (bpp==1 && c) c=0xFF; // set all pixels
    else if (bpp==2) {
        c = c & 0x3;
        c = c | (c << 2);
        c = c | (c << 4);
    } else {
        c = (c & 0x0F) | (c << 4);
    }
    memset((void*)m_scrbuf,c,sizeof(screenbuffer));
}

void Display::setDefaultPalette() {
    #if PICOPALETTE
        loadRGBPalette(palettePico);
    #else
        loadRGBPalette(POK_DEFAULT_PALETTE);
    #endif //PICOPALETTE
}

void Display::setColor(uint8_t c) {
    color = c & ((1<<POK_COLORDEPTH)-1); // cut out colors that go above palette limit
}

void Display::setColor(uint8_t c,uint8_t bgc){
    color = c & ((1<<POK_COLORDEPTH)-1); // cut out colors that go above palette limit
    bgcolor = bgc & ((1<<POK_COLORDEPTH)-1); // cut out colors that go above palette limit
}

void Display::setInvisibleColor(uint16_t c){
    invisiblecolor = c; // invisible color can have values beyond 255 for identification purposes
}

uint8_t Display::getColor() {
    return color;
}

uint8_t Display::getBgColor() {
    return bgcolor;
}

uint16_t Display::getInvisibleColor() {
    return invisiblecolor;
}

void Display::setClipRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    clipX = x; clipY = y; clipW = w; clipH = h;
}


void Display::drawPixel(int16_t x,int16_t y, uint8_t col) {
    if (col==invisiblecolor) return; // do not draw transparent pixels
    if ((uint16_t)x >= width || (uint16_t)y >= height) return;
    col &= (PALETTE_SIZE-1);

    #if POK_COLORDEPTH == 8
        m_scrbuf[x+width*y] = col;
    #endif

    #if POK_GAMEBUINO_SUPPORT >0

	uint8_t c = col;
	uint8_t ct = col;

    uint16_t bitptr=0;
    for (uint8_t cbit=0;cbit<POK_COLORDEPTH;cbit++) {
	c = ct & 1; // take the lowest bit of the color index number
	if(c == 0){ //white - or actually "Clear bit"
		m_scrbuf[x + (y / 8) * LCDWIDTH + bitptr] &= ~_BV(y % 8);
	} else { //black - or actually "Set bit"
		m_scrbuf[x + (y / 8) * LCDWIDTH + bitptr] |= _BV(y % 8);
	}
	ct >>=1; // shift to get next bit
	bitptr += POK_BITFRAME; // move one screen worth of buffer forward to get to the next color bit
    } // POK_COLOURDEPTH

    #else
    #if POK_COLORDEPTH == 1
        if (col) {m_scrbuf[(y >> 3) * width + x] |= (0x80 >> (y & 7)); return;}
        m_scrbuf[(y >> 3) * width + x] &= ~(0x80 >> (y & 7));
    #elif POK_COLORDEPTH == 2
        if (col) {
                col &= 3;
        }
        uint16_t i = y*(width>>2) + (x>>2);
        uint8_t pixel = m_scrbuf[i];
        uint8_t column = x&0x03;
        if (column==3) pixel = (pixel&0xFC)|(col); // bits 0-1
        else if (column==2) pixel = (pixel&0xF3)|(col<<2); // bits 2-3
        else if (column==1) pixel = (pixel&0xCF)|(col<<4); // bits 4-5
        else pixel = (pixel&0x3F)|(col<<6); // bits 6-7
        m_scrbuf[i] = pixel;
    #elif POK_COLORDEPTH == 3
    #elif POK_COLORDEPTH == 4
    uint16_t i = y*(width>>1) + (x>>1);
    uint8_t pixel = m_scrbuf[i];
    if (x&1) pixel = (pixel&0xF0)|(col);
    else pixel = (pixel&0x0F) | (col<<4);
    m_scrbuf[i] = pixel;
    #endif // POK_COLORDEPTH
    #endif // POK_GAMEBUINO_SUPPORT
}

void Display::drawPixel(int16_t x,int16_t y) {
    if ((uint16_t)x >= width || (uint16_t)y >= height) return;

    #if POK_COLORDEPTH == 8
        m_scrbuf[x+width*y] = color;
    #endif

    #if POK_GAMEBUINO_SUPPORT > 0

	uint8_t c = color;
	uint8_t ct = color;
	if(ct == INVERT){
	 ct = !getPixel(x, y); //jonne - was c = !getP...
	}

    uint16_t bitptr=0;
    for (uint8_t cbit=0;cbit<POK_COLORDEPTH;cbit++) {
	c = ct & 1; // take the lowest bit of the color index number
	if(c == 0){ //white - or actually "Clear bit"
    #if DISPLAY_ROT == NOROT
		m_scrbuf[x + (y / 8) * LCDWIDTH + bitptr] &= ~_BV(y % 8);
    #elif DISPLAY_ROT == ROTCCW
		m_scrbuf[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT + bitptr] &= ~_BV(x % 8);
    #elif DISPLAY_ROT == ROT180
		m_scrbuf[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT + bitptr] &= ~_BV((LCDHEIGHT - y - 1) % 8);
    #elif DISPLAY_ROT == ROTCW
		m_scrbuf[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT + bitbtr] &= ~_BV((LCDWIDTH - x - 1) % 8);
    #endif
		//return; //jonne
	} else { //black - or actually "Set bit"
    #if DISPLAY_ROT == NOROT
		m_scrbuf[x + (y / 8) * LCDWIDTH + bitptr] |= _BV(y % 8);
    #elif DISPLAY_ROT == ROTCCW
		m_scrbuf[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT + bitptr] |= _BV(x % 8);
    #elif DISPLAY_ROT == ROT180
		m_scrbuf[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT + bitptr] |= _BV((LCDHEIGHT - y - 1) % 8);
    #elif DISPLAY_ROT == ROTCW
		m_scrbuf[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT + bitptr] |= _BV((LCDWIDTH - x -1) % 8);
    #endif
		//return; //jonne
	}
	ct >>=1; // shift to get next bit
	bitptr += POK_BITFRAME; // move one screen worth of buffer forward to get to the next color bit
    } // POK_COLOURDEPTH

    #else

    /** NOT Gamebuino */
    #if POK_COLORDEPTH == 1
        if (color) {m_scrbuf[(y >> 3) * width + x] |= (0x80 >> (y & 7)); return;}
        m_scrbuf[(y >> 3) * width + x] &= ~(0x80 >> (y & 7));
    #elif POK_COLORDEPTH == 2
        uint16_t i = y*(width>>2) + (x>>2);
        uint8_t pixel = m_scrbuf[i];
        uint8_t column = x&0x03;
        if (column==3) pixel = (pixel&0xFC)|(color); // bits 0-1
        else if (column==2) pixel = (pixel&0xF3)|(color<<2); // bits 2-3
        else if (column==1) pixel = (pixel&0xCF)|(color<<4); // bits 4-5
        else pixel = (pixel&0x3F)|(color<<6); // bits 6-7
        m_scrbuf[i] = pixel;
    #elif POK_COLORDEPTH == 3
    #elif POK_COLORDEPTH == 4
            uint16_t i = y*(width>>1) + (x>>1);
            uint8_t pixel = m_scrbuf[i];
            if (x&1) pixel = (pixel&0xF0)|(color);
            else pixel = (pixel&0x0F) | (color<<4);
            m_scrbuf[i] = pixel;
    #endif // POK_COLORDEPTH
    #endif // POK_GAMEBUINO_SUPPORT
}

uint8_t Display::getPixel(int16_t x,int16_t y) {
    if ((uint16_t)x >= width || (uint16_t)y >= height) return 0;
    #if POK_GAMEBUINO_SUPPORT
    uint8_t color=0; //jonne
	for (uint8_t cbit=0; cbit<POK_COLORDEPTH;cbit++) {
    	color |= (m_scrbuf[x + (y / 8) * LCDWIDTH+POK_BITFRAME*cbit] >> (y % 8)) & 0x1 ; //jonne - added +504*cbit
	}
	return color;
    #else
    /** not gamebuino */
    #if POK_COLORDEPTH == 1
        return (m_scrbuf[(y >> 3) * width + x] & (0x80 >> (y & 7))) ? 1:0;
    #elif POK_COLORDEPTH == 2
        uint16_t i = y*(width>>2) + (x>>2);
        uint8_t pixel = m_scrbuf[i];
        uint8_t column = x&0x03;
        if (column==0) return pixel & 0x03; // bits 0-1
        else if (column==1) return (pixel & 0x0C)>>2; // bits 2-3
        else if (column==2) return (pixel & 0x30)>>4; // bits 4-5
        else return pixel>>6;; // bits 6-7
    #elif POK_COLORDEPTH == 3
    #elif POK_COLORDEPTH == 4
    uint16_t i = y*(width>>1) + (x>>1);
    uint8_t pixel = m_scrbuf[i];
    if (x&1) return pixel & 0x0F;
    else return pixel>>4;
    #endif // POK_COLORDEPTH
    #endif // POK_GAMEBUINO_SUPPORT
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    if ((uint16_t)x0 >= width || (uint16_t)y0 >= height || (uint16_t)x1 >= width || (uint16_t)y1 >= height ) {
        if (clipLine (&x0,&y0,&x1,&y1)==0) return; // line out of bounds
    }
	if (x0 == x1)
		drawColumn(x0,y0,y1);
	else if (y0 == y1)
		drawRow(x0,x1,y0);
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;

		x = x0;
		y = y0;

		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		}

		e = ((int)dy<<1) - dx;

		for (j=0; j<=dx; j++) {
			drawPixel(x,y);

			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
}

uint8_t Display::clipLine(int16_t *x0, int16_t *y0, int16_t *x1, int16_t *y1){
    // Check X bounds
	if (*x1<*x0) {
        //std::swap (*x1,*x0); // swap so that we dont have to check x1 also
        swapWT(int16_t*,x1,x0);
        //std::swap (*y1,*y0); // y needs to be swaaped also
        swapWT(int16_t*,y1,y0);
	}

	if (*x0>=width) return 0; // whole line is out of bounds

	// Clip against X0 = 0
	if (*x0 < 0) {
        if ( *x1 < 0) return 0; // nothing visible
        int16_t dx = (*x1 - *x0);
        int16_t dy = ((*y1 - *y0) << 8); // 8.8 fixed point calculation trick
        int16_t m = dy/dx;
        *y0 = *y0 + ((m*-*x0)>>8); // get y0 at boundary
        *x0 = 0;
	}

	// Clip against x1 = 83
	if (*x1 >= width) {
        int16_t dx = (*x1 - *x0);
        int16_t dy = ((*y1 - *y0) << 8); // 8.8 fixed point calculation trick
        int16_t m = dy/dx;
        //*y1 = *y1 + ((m*(*x1-XMAX))>>8); // get y0 at boundary
        *y1 = *y1 + ((m*(width-1-*x1))>>8); // get y0 at boundary
        *x1 = width-1;
	}

    // Check Y bounds
	if (*y1<*y0) {
        //std::swap (*x1,*x0); // swap so that we dont have to check x1 also
        swapWT(int16_t*,x1,x0);
        //std::swap (*y1,*y0); // y needs to be swaaped also
        swapWT(int16_t*,y1,y0);
	}

	if (*y0>=height) return 0; // whole line is out of bounds

    if (*y0 < 0) {
        if ( *y1 < 0) return 0; // nothing visible
        int16_t dx = (*x1 - *x0) << 8;
        int16_t dy = (*y1 - *y0); // 8.8 fixed point calculation trick
        int16_t m = dx/dy;
        *x0 = *x0 + ((m*-*y0)>>8); // get x0 at boundary
        *y0 = 0;
	}

    // Clip against y1 = 47
	if (*y1 >= height) {
        int16_t dx = (*x1 - *x0) << 8;
        int16_t dy = (*y1 - *y0); // 8.8 fixed point calculation trick
        int16_t m = dx/dy;
        *x1 = *x1 + ((m*(height-1-*y1))>>8); // get y0 at boundary
        //*x1 = *x1 + ((m*(*y1-YMAX))>>8); // get y0 at boundary
        *y1 = height-1;
	}
	return 1; // clipped succesfully
}

void Display::map1BitColumn(int16_t x, int16_t sy, int16_t ey, const uint8_t* bitmap, uint16_t column){
    if ((uint16_t)sy>=height && (uint16_t)ey>=height) return; //completely out of bounds
    if ((uint16_t)x>=width) return; //completely out of bounds
    if (sy>ey) {
            int y=sy;
            sy=ey;
            ey=y; // swap around so that x0 is less than x1
    }
    uint16_t bmw,bmh;
    float texelstep, texelindex;
    bmw = *(bitmap);
    bmh = *(bitmap+1);
    if (column>bmw-1) column=bmw-1;
    bitmap += 2;
    bitmap += column;
    texelstep = (float)bmh/((float)ey-(float)sy);
    texelindex = 0;
    for (int y=sy; y <= ey; y++, texelindex += texelstep) {
        uint8_t texel;
        uint8_t currbyte, bit;
        currbyte = texelindex / 8;
        bit = 7-((uint16_t) texelindex & 0x7);
        texel=*(bitmap+currbyte*bmw);
        if (texel & (1<<bit)) drawPixel(x,y);
        else if (bgcolor != invisiblecolor) drawPixel(x,y,bgcolor);
    }
};

void Display::drawColumn(int16_t x, int16_t sy, int16_t ey){
    if ((uint16_t)sy>=height && (uint16_t)ey>=height) return; //completely out of bounds
    if ((uint16_t)x>=width) return; //completely out of bounds
    if (sy>ey) {
            int y=sy;
            sy=ey;
            ey=y; // swap around so that x0 is less than x1
    }
    for (int y=sy; y <= ey; y++) {
        drawPixel(x,y);
    }
}

void Display::drawRow(int16_t x0, int16_t x1, int16_t y){
    if ((uint16_t)x0>=width && (uint16_t)x1>=width) return; //completely out of bounds
    if ((uint16_t)y>=height) return; //completely out of bounds

    if (x0>x1) {
            int x=x0;
            x0=x1;
            x1=x; // swap around so that x0 is less than x1
    }
    for (int x=x0; x <= x1; x++) {
        drawPixel(x,y);
    }
}

void Display::drawFastVLine(int16_t x, int16_t y, int16_t h){
    if (h<0) {y += h; h = -h;}
    drawColumn(x,y,y+h);
}

void Display::drawFastHLine(int16_t x, int16_t y, int16_t w){
    if (w<0) {x += w; w = -w;}
    drawRow(x,x+w-1,y);
}

void Display::drawRectangle(int16_t x0, int16_t y0, int16_t w, int16_t h) {
    drawColumn(x0,y0,y0+h);
    drawColumn(x0+w,y0,y0+h);
    drawRow(x0,x0+w,y0);
    drawRow(x0,x0+w,y0+h);
}

void Display::fillRectangle(int16_t x0,int16_t y0, int16_t w, int16_t h){
    int16_t x,y,x1,y1;
    x1=x0+w;y1=y0+h;
    if ((x0<0 && x1<0) || (x0>=width && x1 >=width)) return; //completely out of bounds
    if ((y0<0 && y1<0) || (y0>=height && y1 >=height)) return; //completely out of bounds
    if (x0>x1) {x=x1;x1=x0;}
    else x=x0;
    if (y0>y1) {y=y1;y1=y0;}
    else y=y0;
    if (x<0) x=0;
    if (y<0) y=0;
    for (;x<x1;x++) drawColumn(x,y,y1);
}

void Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    fillRectangle(x,y,w,h);
}

void Display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    drawRectangle(x,y,w,h);
}

void Display::drawCircle(int16_t x0, int16_t y0, int16_t r) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r);
    drawPixel(x0, y0 - r);
    drawPixel(x0 + r, y0);
    drawPixel(x0 - r, y0);

    while (x < y) {
        if (f >= 0) {

            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y);
        drawPixel(x0 - x, y0 + y);
        drawPixel(x0 + x, y0 - y);
        drawPixel(x0 - x, y0 - y);
        drawPixel(x0 + y, y0 + x);
        drawPixel(x0 - y, y0 + x);
        drawPixel(x0 + y, y0 - x);
        drawPixel(x0 - y, y0 - x);

    }
}

void Display::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y);
            drawPixel(x0 + y, y0 + x);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y);
            drawPixel(x0 + y, y0 - x);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x);
            drawPixel(x0 - x, y0 + y);
        }
        if (cornername & 0x1) {

            drawPixel(x0 - y, y0 - x);
            drawPixel(x0 - x, y0 - y);
        }
    }
}

void Display::fillCircle(int16_t x0, int16_t y0, int16_t r) {
    drawFastVLine(x0, y0 - r, 2 * r );
    fillCircleHelper(x0, y0, r, 3, 0);
}

void Display::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername, int16_t delta) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta-1); //added -1 here, jonne
            drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta-1); //added -1 here, jonne
        }
        if (cornername & 0x2) {

            drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta-1); //added -1 here, jonne
            drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta-1); //added -1 here, jonne
        }
    }
}

void Display::drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r) {
    if (r<2) {drawRectangle(x,y,w,h);return;}
    // smarter version
    drawFastHLine(x + r, y, w - 2 * r); // Top
    drawFastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
    drawFastVLine(x, y + r, h - 2 * r); // Left
    drawFastVLine(x + w - 1, y + r, h - 2 * r); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1);
    drawCircleHelper(x + w - r - 1, y + r, r, 2);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
    drawCircleHelper(x + r, y + h - r - 1, r, 8);
}

void Display::fillRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r) {
    if (r<2) {fillRectangle(x,y,w,h);return;}
    fillRectangle(x + r, y, w - 2 * r, h-1);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1);
}

void Display::drawTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2) {
    drawLine(x0, y0, x1, y1);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x0, y0);
}

void Display::fillTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swapWT(int16_t,y0, y1);
        swapWT(int16_t,x0, x1);
    }
    if (y1 > y2) {
        swapWT(int16_t,y2, y1);
        swapWT(int16_t,x2, x1);
    }
    if (y0 > y1) {
        swapWT(int16_t,y0, y1);
        swapWT(int16_t,x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a) a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a) a = x2;
        else if (x2 > b) b = x2;
        drawFastHLine(a, y0, b - a + 1);
        return;
    }

    int16_t
    dx01 = x1 - x0,
            dy01 = y1 - y0,
            dx02 = x2 - x0,
            dy02 = y2 - y0,
            dx12 = x2 - x1,
            dy12 = y2 - y1,
            sa = 0,
            sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) last = y1; // Include y1 scanline
    else last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b) swapWT(int16_t,a, b);
        drawFastHLine(a, y, b - a + 1);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        if (a > b) swapWT(int16_t,a, b);
        drawFastHLine(a, y, b - a + 1);
    }
}

void Display::setFont(const unsigned char * f) {
	font = f;
	fontWidth = *(font)+1;
	fontHeight = *(font + 1)+1;
}

void Display::drawMonoBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t index) {
    uint8_t w = *bitmap;
	uint8_t h = *(bitmap + 1);
	uint8_t xtra=0;
	if (w&0x7) xtra=1;
	bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
    #if POK_GAMEBUINO_SUPPORT > 0
    int8_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    for (i = 0; i < w; i++) {
        byteNum = i / 8;
        bitNum = i % 8;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            if (source & (0x80 >> bitNum)) {
                drawPixel(x + i, y + j);
            }
        }
    }
    #else
    /** not gamebuino */
    int8_t scrx,scry;
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    int8_t bitptr;
    for (scry = y; scry < y+h; scry+=1) {
            if ((x&1)==0) { /** EVEN pixel starting line**/
                for (scrx = x, bitptr=7; scrx < w+x; scrx+=2) {
                    uint8_t targetpixel = *scrptr;
                    if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF) | color<<4; // upper nibble
                    else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF) | bgcolor<<4; // upper nibble
                    bitptr--;
                    if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF0) | color; // lower nibble
                    else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF0) | bgcolor; // lower nibble
                    bitptr--;
                    if (bitptr<0) { bitptr = 7; bitmap++; }
                    *scrptr = targetpixel;
                    scrptr++;
                }
            } else { /** ODD pixel starting line **/
                for (scrx = x, bitptr=7; scrx < w+x; scrx+=2) {
                    uint8_t targetpixel = *scrptr;
                    // store higher nibble of source pixel in lower nibble of target
                    if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF0) | color; // lower nibble
                    else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF0) | bgcolor; // lower nibble
                    *scrptr = targetpixel; // store
                    bitptr--;scrptr++;targetpixel = *scrptr;
                    // store lower nibble of source pixel in higher nibble of target
                    if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF) | color<<4; // higher nibble
                    else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF) | bgcolor<<4; // higher nibble
                    *scrptr = targetpixel; // store
                    bitptr--; // do not increment scrptr here !
                }
            }
            if (bitptr!=7) bitmap++; // force skip to next line
            // increment the y jump in the scrptr
            scrptr = scrptr + ((width - w)>>1);
    }
    #endif // POK_GAMEBUINO_SUPPORT
}


void Display::drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t frame)
{
    int16_t w = *bitmap;
	int16_t h = *(bitmap + 1);
	uint8_t framew = *(bitmap+2);
    bitmap = bitmap + 3; //add an offset to the pointer to start after the width and height
    /** visibility check */
    if (y<-h || y>height) return; //invisible
    if (x<-framew || x>width) return;  //invisible
    /** 1 bpp mode */
    if (m_colordepth<2) {
    int16_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    for (i = 0; i < w; i++) {
        byteNum = i / 8;
        bitNum = i % 8;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            if (source & (0x80 >> bitNum)) {
                drawPixel(x + i, y + j);
            }
        }
    }

    return;
    }
    /** 2 bpp mode */
    if (m_colordepth<4) {
    int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
    for (i = 0; i < w; i++) {
        byteNum = i / 4;
        bitNum = (i % 4)<<1;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            uint8_t output = (source & (0xC0 >> bitNum));
            output >>= (6-bitNum);
            if (output != invisiblecolor) {
                setColor(output);
                drawPixel(x + i, y + j);
            }
        }
    }

    return;
    }
    /** 4bpp fast version */
	int16_t scrx,scry,xclip,xjump,scrxjump;
    xclip=xjump=scrxjump=0;
    bitmap += (framew*frame)>>1;
    /** y clipping */
    if (y<0) { h+=y; bitmap -= y*(w>>1); y=0;}
    else if (y+h>height) { h -=(y-height);}
    /** x clipping */
    xjump = (w-framew)>>1;
    if (x<0) { xclip=(x&1)<<1; framew+=x; xjump = ((-x)>>1); bitmap += xjump; x=0;}
    else if (x+framew>width) {
            xclip = (x&1)<<1;
            scrxjump = x&1;
            xjump=((x+framew-width)>>1)+scrxjump;
            framew = width-x;}

    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    /** ONLY 4-bit mode for time being **/
    for (scry = y; scry < y+h; scry+=1) {
            if (scry>=height) return;
            if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
                for (scrx = x; scrx < framew+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *bitmap;
                    if (xclip) {
                            sourcepixel <<=4;
                            sourcepixel |= ((*(bitmap+1))>>4);
                    }
                    uint8_t targetpixel = *scrptr;
                    if ((sourcepixel>>4) != invisiblecolor ) targetpixel = (targetpixel&0x0F) | (sourcepixel & 0xF0);
                    if ((sourcepixel&0x0F) != invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F);
                    *scrptr = targetpixel;
                    bitmap++;
                    scrptr++;
                }
                if (xclip){
                    if (framew&1) {
                        /**last pixel is odd pixel due to clipping & odd width*/
                        uint8_t sourcepixel = *bitmap;
                        if ((sourcepixel&0x0F) != invisiblecolor) {
                            sourcepixel <<=4;
                            uint8_t targetpixel = *scrptr;// & 0x0F;
                            targetpixel |= sourcepixel;
                            *scrptr = targetpixel;
                        }
                        //scrptr++;
                    }
                    bitmap++;
                    scrptr++;
                }
                bitmap += xjump; // needed if x<0 clipping occurs
            } else { /** ODD pixel starting line **/
                for (scrx = x; scrx < framew+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *bitmap;
                    uint8_t targetpixel = *scrptr;
                    // store higher nibble of source pixel in lower nibble of target
                    if((sourcepixel>>4)!=invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel >> 4 );
                    *scrptr = targetpixel;
                    scrptr++;
                    targetpixel = *scrptr;
                    // store lower nibble of source pixel in higher nibble of target
                    if((sourcepixel&0x0F)!=invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel << 4);
                    *scrptr = targetpixel;
                    bitmap++;
                }
                bitmap+=xjump;
            }
            // increment the y jump in the scrptr
            scrptr = scrptr + ((width - framew)>>1)+scrxjump;
    }
}


void Display::drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap)
{
    int16_t w = *bitmap;
	int16_t h = *(bitmap + 1);
    bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    drawBitmapData(x, y, w, h, bitmap);
}

void Display::drawBitmapData(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap) {
    /** visibility check */
    if (y<-h || y>height) return; //invisible
    if (x<-w || x>width) return;  //invisible
    /** 1 bpp mode */
    if (m_colordepth<2) {
    int16_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    for (i = 0; i < w; i++) {
        byteNum = i / 8;
        bitNum = i % 8;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            if (source & (0x80 >> bitNum)) {
                drawPixel(x + i, y + j);
            }
        }
    }

    return;
    }
    /** 2 bpp mode */
    if (m_colordepth<4) {
        if(clipH > 0) {

            // Clip
            int16_t x1 = max(x, clipX);
            int16_t x2 = min(x + w, clipX + clipW);
            int16_t bmupdateX = x1 - x;
            int16_t bmupdateX2 = x2 - x;
            int16_t y1 = max(y, clipY);
            int16_t y2 = min(y + h, clipY + clipH);
            int16_t bmupdateY = y1 - y;
            int16_t bmupdateY2 = y2 - y;

            int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
            for (i = bmupdateX; i < bmupdateX2; i++) {
                byteNum = i / 4;
                bitNum = (i % 4)<<1;
                for (j = bmupdateY; j < bmupdateY2; j++) {
                    uint8_t source = *(bitmap + (j * byteWidth) + byteNum);
                    uint8_t output = (source & (0xC0 >> bitNum));
                    output >>= (6-bitNum);
                    if (output != invisiblecolor) {
                        setColor(output);
                        drawPixel(x + i, y + j);
                    }
                }
            }
        }
        else {
            int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
            for (i = 0; i < w; i++) {
                byteNum = i / 4;
                bitNum = (i % 4)<<1;
                for (j = 0; j < h; j++) {
                    uint8_t source = *(bitmap + (j * byteWidth) + byteNum);
                    uint8_t output = (source & (0xC0 >> bitNum));
                    output >>= (6-bitNum);
                    if (output != invisiblecolor) {
                        setColor(output);
                        drawPixel(x + i, y + j);
                    }
                }
            }
        }
        return;
    }
    /** 4bpp fast version */
	int16_t scrx,scry,xclip,xjump,scrxjump;
    xclip=xjump=scrxjump=0;
    /** y clipping */
    if (y<0) { h+=y; bitmap -= y*(w>>1); y=0;}
    else if (y+h>height) { h -=(y-height);}
    /** x clipping */
    if (x<0) { xclip=(x&1)<<1; w+=x; xjump = ((-x)>>1); bitmap += xjump; x=0;}
    else if (x+w>width) {
            xclip = (x&1)<<1;
            scrxjump = x&1;
            xjump=((x+w-width)>>1)+scrxjump;
            w = width-x;}

    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    /** ONLY 4-bit mode for time being **/
    for (scry = y; scry < y+h; scry+=1) {
            if (scry>=height) return;
            if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
                for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *bitmap;
                    if (xclip) {
                            sourcepixel <<=4;
                            sourcepixel |= ((*(bitmap+1))>>4);
                    }
                    uint8_t targetpixel = *scrptr;
                    if ((sourcepixel>>4) != invisiblecolor ) targetpixel = (targetpixel&0x0F) | (sourcepixel & 0xF0);
                    if ((sourcepixel&0x0F) != invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F);
                    *scrptr = targetpixel;
                    bitmap++;
                    scrptr++;
                }
                if (xclip){
                    if (w&1) {
                        /**last pixel is odd pixel due to clipping & odd width*/
                        uint8_t sourcepixel = *bitmap;
                        if ((sourcepixel&0x0F) != invisiblecolor) {
                            sourcepixel <<=4;
                            uint8_t targetpixel = *scrptr & 0x0F;
                            targetpixel |= sourcepixel;
                            *scrptr = targetpixel;
                        }
                        //scrptr++;
                    }
                    bitmap++;
                    scrptr++;
                }
                bitmap += xjump; // needed if x<0 clipping occurs
            } else { /** ODD pixel starting line **/
                //for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *bitmap;
                    uint8_t targetpixel = *scrptr;
                    // store higher nibble of source pixel in lower nibble of target
                    if((sourcepixel>>4)!=invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel >> 4 );
                    *scrptr = targetpixel;
                    scrptr++;
                    targetpixel = *scrptr;
                    // store lower nibble of source pixel in higher nibble of target
                    if((sourcepixel&0x0F)!=invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel << 4);
                    *scrptr = targetpixel;
                    bitmap++;
                }
                if (xclip){
                    if (w&1) {
                        /**last pixel is odd pixel due to clipping & odd width*/
                        uint8_t sourcepixel = *bitmap;
                        sourcepixel >>=4; //top nibble of sourcebyte from bitmap...
                        if (sourcepixel != invisiblecolor) {
                            uint8_t targetpixel = *scrptr & 0xF0; //...put into the low nibble of the target
                            targetpixel |= sourcepixel;
                            *scrptr = targetpixel;
                        }
                        //scrptr++;
                    }
                }
                bitmap+=xjump;
            }
            // increment the y jump in the scrptr
            scrptr = scrptr + ((width - w)>>1)+scrxjump;
    }
}

void Display::drawRleBitmap(int16_t x, int16_t y, const uint8_t* rlebitmap)
{
    // ONLY can copy 4-bit bitmap to 4-bit screen mode for time being
    #if (POK_SCREENMODE != MODE_FAST_16COLOR)
    return;
    #endif

    int16_t w = *rlebitmap;
	int16_t h = *(rlebitmap + 1);
    rlebitmap = rlebitmap + 2; //add an offset to the pointer to start after the width and height

    // visibility check
    if (y<-h || y>height) return; //invisible
    if (x<-w || x>width) return;  //invisible

    // Clipping is not supported
    if ((x < 0) || (x+w > width) || (y < 0) || (y+h > height)) return;

    // Currently only support RLE bitmaps in 16 color mode.
    if (m_colordepth != 4)  //
        return;

    // Go through each line.
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    bool is_endofbitmap = false;
    for (int16_t scry = y; scry < y+h && !is_endofbitmap;) {

        // Process one line. Go through each pixel run and escape command in RLE data.
        for (int16_t scrx = x;;) {
            uint8_t rle_count = *rlebitmap++;

            if (rle_count == 0) {

               /* Escape or absolute mode */

                uint8_t rle_escape_or_runsize = *rlebitmap++;
                if ( rle_escape_or_runsize == RLE_ESC_EOL) {
                    // End of line.
                    break;
                }
                else if ( rle_escape_or_runsize == RLE_ESC_EOB) {
                    // End of bitmap.
                    is_endofbitmap = true;
                    break;
                }
                else if ( rle_escape_or_runsize == RLE_ESC_OFFSET) {
                    // Move position in target.
                    // TODO: not tested yet.
                    uint8_t xoffset = *rlebitmap++;
                    uint8_t yoffset = *rlebitmap++;
                    scrptr += (xoffset>1);
                    scrx += xoffset;
                    scrptr += yoffset*width;
                    scry += yoffset;
                 }
                else {

                    /* Absolute mode. Copy pixels from the source bitmap to the target screen. */

                    int16_t runsize = rle_escape_or_runsize;
                    uint8_t targetpixel = *scrptr;  // initial value
                    uint8_t sourcepixel = *rlebitmap;  // initial value
                    for( int16_t runx = 0; runx < runsize; ) {
                        if (scrx&0x1)  { // screen pixel is in the low nibble
                            if (runx&0x1) { // bitmap pixel is in the low nibble
                                if ((sourcepixel&0x0F) != invisiblecolor)
                                    targetpixel = (targetpixel&0xF0) | (sourcepixel&0x0F); // Copy low to low nibble.
                                rlebitmap++;
                            }
                            else // bitmap pixel is in the high nibble
                                if ((sourcepixel>>4) != invisiblecolor)
                                    targetpixel = (targetpixel&0xF0) | (sourcepixel>>4); // Copy high to low nibble.

                            // Copy the byte to the target.
                            *scrptr = targetpixel;
                            scrptr++;
                        }
                        else  { // screen pixel is in the high nibble
                            targetpixel = *scrptr;
                            sourcepixel = *rlebitmap;
                            if (runx&0x1) { // bitmap pixel is sourcepixel = *rlebitmapin the low nibble
                                if ((sourcepixel&0x0F) != invisiblecolor )
                                    targetpixel = (targetpixel&0x0F) | ((sourcepixel<<4)&0xF0); // Copy low to high nibble.
                                rlebitmap++;  // read the new source byte
                            }
                            else // bitmap pixel is in the high nibble
                                if ((sourcepixel>>4) != invisiblecolor )
                                    targetpixel = (targetpixel&0x0F) | (sourcepixel&0xF0); // Copy high to high nibble.
                        }
                        runx++;
                        scrx++;
                    }  // end for

                     // If this is odd target index, copy the byte to the target.
                    if (scrx&0x1) {
                        *scrptr = targetpixel;
                        scrptr++;
                    }

                    // In absolute mode the source size is always padded to the word boundary.
                    if (runsize%4) {
                        int16_t padpixcount = 4 - (runsize%4);
                        rlebitmap += padpixcount>>1;  // skip n padding bytes
                    }
                }
            }
            else {

                /* Encoded mode. Duplicate one pixel pair to the all required pixels on the target screen */

                int16_t runsize = rle_count;
                uint8_t clonepixelpair = *rlebitmap++;
                uint8_t targetpixel = *scrptr;  // initial value
                for( int16_t runx = 0;  runx < runsize; ) {
                    if (scrx&0x1)  { // screen pixel is in the low nibble
                        if (runx&0x1) { // bitmap pixel is in the low nibble
                            if ((clonepixelpair&0x0F) != invisiblecolor)
                                targetpixel = (targetpixel&0xF0) | (clonepixelpair&0x0F); // Copy low to low nibble.
                        }
                        else // bitmap pixel is in the high nibble
                            if ((clonepixelpair>>4) != invisiblecolor)
                                targetpixel = (targetpixel&0xF0) | (clonepixelpair>>4); // Copy high to low nibble.

                        // Copy the byte to the target.
                        *scrptr = targetpixel;
                        scrptr++;
                    }
                    else  { // screen pixel is in the high nibble
                        targetpixel = *scrptr;
                        if (runx&0x1) {// bitmap pixel is in the low nibble
                            if ((clonepixelpair&0x0F) != invisiblecolor )
                                targetpixel = (targetpixel&0x0F) | ((clonepixelpair<<4)&0xF0); // Copy low to high nibble.
                        }
                        else // bitmap pixel is in the high nibble
                            if ((clonepixelpair>>4) != invisiblecolor )
                                targetpixel = (targetpixel&0x0F) | (clonepixelpair&0xF0); // Copy high to high nibble.
                    }
                    runx++;
                    scrx++;

                }  // end for

                // If this is odd target index, copy the byte to the target.
                if (scrx&0x1) {
                    *scrptr = targetpixel;
                    scrptr++;
                 }
            } // end if
        }  // end while

        // Increment the target screen pointer and index.
        scrptr = scrptr + ((width - w)>>1);
        scry++;
    } // end for scry
}

void Display::drawBitmapXFlipped(int16_t x, int16_t y, const uint8_t* bitmap)
{
    int16_t w = *bitmap;
	int16_t h = *(bitmap + 1);
    bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    /** visibility check */
    if (y<-h || y>height) return; //invisible
    if (x<-w || x>width) return;  //invisible
    /** 1 bpp mode */
    if (m_colordepth<2) {
    int16_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    for (i = 0; i < w; i++) {
        byteNum = i / 8;
        bitNum = i % 8;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            if (source & (0x80 >> bitNum)) {
                drawPixel(x + w - i, y + j);
            }
        }
    }

    return;
    }
    /** 2 bpp mode */
    if (m_colordepth<4) {
    int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
    for (i = 0; i < w; i++) {
        byteNum = i / 4;
        bitNum = (i % 4)<<1;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            uint8_t output = (source & (0xC0 >> bitNum));
            output >>= (6-bitNum);
            if (output != invisiblecolor) {
                setColor(output);
                drawPixel(x + i, y + j);
            }
        }
    }

    return;
    }
    /** 4bpp fast version */
	int16_t scrx,scry,xclip,xjump,scrxjump;
    xclip=xjump=scrxjump=0;
    /** y clipping */
    if (y<0) { h+=y; bitmap -= y*(w>>1); y=0;}
    else if (y+h>height) { h -=(y-height);}
    /** x clipping */
    bitmap += ((w>>1)-1); //inverted!
    if (x<0) {
            xclip=(x&1)<<1;
            w+=x;
            xjump = ((-x)>>1);
            //bitmap += xjump; // do not clip left edge of source, as bitmap is inverted !
            x=0;
            }
    else if (x+w>width) {
            xclip = (x&1)<<1;
            scrxjump = x&1;
            xjump=((x+w-width)>>1)+scrxjump;
            w = width-x;}

    //uint8_t* scrptr = m_scrbuf + (y*(width>>1) + ((x+width)>>1));
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    /** ONLY 4-bit mode for time being **/
    for (scry = y; scry < y+h; scry+=1) {
    //    for (scry = y; scry < y+2; scry+=1) {
            if (scry>=height) return;
            if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
                //for (scrx = w+x-xclip-1; scrx >= x; scrx-=2) {
                for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *(bitmap);
                    if (xclip) {
                            sourcepixel <<=4;
                            sourcepixel |= ((*(bitmap-1))>>4);//inverted!
                    }
                    uint8_t targetpixel = *scrptr;
                    // NIBBLES ARE INVERTED BECAUSE PICTURE IS FLIPPED !!!
                    if ((sourcepixel>>4) != invisiblecolor ) targetpixel = (targetpixel&0xF0) | (sourcepixel>>4);
                    if ((sourcepixel&0x0F) != invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel<<4);
                    *scrptr = targetpixel;
                    bitmap--;
                    scrptr++;
                }
                bitmap += w; // w*2 >> 1 because inverted and because 2 pixels per byte!!
                if (xclip){
                    if (w&1) {
                        /**last pixel is odd pixel due to clipping & odd width*/
                        uint8_t sourcepixel = *bitmap;
                        if ((sourcepixel&0x0F) != invisiblecolor) {
                            sourcepixel <<=4;
                            uint8_t targetpixel = *scrptr;// & 0x0F;
                            targetpixel |= sourcepixel;
                            *scrptr = targetpixel;
                        }
                        //scrptr++;
                    }
                    bitmap++;
                    scrptr++;
                }
                bitmap += xjump; // needed if x<0 clipping occurs
            } else { /** ODD pixel starting line **/
                for (scrx = x; scrx < w+x-xclip; scrx+=2 ) {
                    uint8_t sourcepixel = *bitmap;
                    uint8_t targetpixel = *scrptr;
                    // inverted !!! store lower nibble of source pixel in lower nibble of target
                    if((sourcepixel&0x0F)!=invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F );
                    *scrptr = targetpixel;
                    scrptr++;
                    targetpixel = *scrptr;
                    // inverted ! store higher nibble of source pixel in higher nibble of target
                    if((sourcepixel>>4)!=invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel & 0xF0);
                    *scrptr = targetpixel;
                    bitmap--;
                }
                bitmap += w; // w*2 >> 1 because inverted and because 2 pixels per byte!!
                bitmap+=xjump;
            }
            // increment the y jump in the scrptr
            scrptr = scrptr + ((width - w)>>1)+scrxjump;
    }
}

void Display::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip) {
#if PROJ_GAMEBUINO == 0
    if (!flip) drawBitmap(x,y,bitmap);
    else drawBitmapXFlipped(x,y,bitmap);
#else
	if((rotation == NOROT) && (flip == NOFLIP)){
		drawBitmap(x,y,bitmap); //use the faster algorithm
		return;
	}
	uint8_t w = bitmap[0];
	uint8_t h = bitmap[1];
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    int8_t i, j, //coordinates in the raw bitmap
            k, l, //coordinates in the rotated/flipped bitmap
            byteNum, bitNum, byteWidth = (w + 7) >> 3;

    rotation %= 4;

    for (i = 0; i < w; i++) {
        byteNum = i / 8;
        bitNum = i % 8;
        for (j = 0; j < h; j++) {
            if (bitmap[j * byteWidth + byteNum] & (B10000000 >> bitNum)) {
                switch (rotation) {
                    case NOROT: //no rotation
                        k = i;
                        l = j;
                        break;
                    case ROTCCW: //90° counter-clockwise
                        k = j;
                        l = w - i - 1;
                        break;
                    case ROT180: //180°
                        k = w - i - 1;
                        l = h - j - 1;
                        break;
                    case ROTCW: //90° clockwise
                        k = h - j - 1;
                        l = i;
                        break;
                }
                if (flip) {
                    flip %= 4;
                    if (flip & B00000001) { //horizontal flip
                        k = w - k;
                    }
                    if (flip & B00000010) { //vertical flip
                        l = h - l;
                    }
                }
                k += x; //place the bitmap on the screen
                l += y;
                drawPixel(k, l);
            }
        }
    }
#endif //PROJ_GAMEBUINO

}

uint8_t* Display::getBuffer() {
    return m_scrbuf;
}

uint8_t Display::getBitmapPixel(const uint8_t* bitmap, uint16_t x, uint16_t y) {
    uint16_t w = *bitmap;
    uint8_t sourcebyte = bitmap[2+(y * ((w+7)>>3))+ (x>>3)];
    return sourcebyte & (0x80>>(x&7));
}

int Display::print_char(uint8_t x, uint8_t y, unsigned char c) {
	c -= font[2];
	if (m_mode) return directChar(x,y,c);
	return bufferChar(x,y,c);
}

void Display::drawChar(int8_t x, int8_t y, unsigned char c, uint8_t size) {
	print_char(x,y,c);
	return;
}


bool Display::isDirectPrintingEnabled() {
    return m_mode;
}

void Display::enableDirectPrinting(uint8_t m) {
    if (m) {
            m_mode=true;
            m_w = POK_LCD_W;
            m_h = POK_LCD_H;
    } else {
            m_mode=false;
            m_w = getWidth();
            m_h = getHeight();
    }
}

void Display::write(uint8_t c) {
	int charstep=0;
	if(font[3]) {
        // only caps in this font
        if (c>=97) c-=32;
	}
	switch(c) {
		case '\0':			//null
			break;
		case '\n':			//line feed
			cursorX = 0;
			inc_txtline();
			break;
		case 8:				//backspace
			cursorX -= font[0];
			charstep=print_char(cursorX,cursorY,' ');
			break;
		case 13:			//carriage return
			cursorX = 0;
			break;
		case 14:			//form feed new page(clear screen)
			//clear_screen();
			break;
		default:
			if (cursorX >= (m_w - font[0])) {
				cursorX = 0;
				if (textWrap) inc_txtline();
				else return; // stop outputting text
				charstep=print_char(cursorX,cursorY,c);
			}
			else
				charstep=print_char(cursorX,cursorY,c);
			if (c==' ' && adjustCharStep) charstep=(charstep>>1)+1;
			cursorX += charstep;
	}
}

void Display::inc_txtline() {
	if (cursorY > m_h - 2*font[1]) //= (height - (font[1]+1)))
		#if SCROLL_TEXT > 0
		scroll(font[1] + adjustLineStep);
		#else
		cursorY = 0;
		#endif
	else
		cursorY += font[1] + adjustLineStep;
}

/* default implementation: may be overridden */
void Display::write(const char *str)
{
  while (*str)
    write(*str++);
}

/* default implementation: may be overridden */
void Display::write(const uint8_t *buffer, uint8_t size)
{
  while (size--)
    write(*buffer++);
}

void Display::print(const char str[])
{
  write(str);
}

void Display::print(char c, int base)
{
  print((long) c, base);
}

void Display::print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void Display::print(int n, int base)
{
  print((long) n, base);
}

void Display::print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void Display::print(long n, int base)
{
  if (base == 0) {
    write(n);
  } else if (base == 10) {
    if (n < 0) {
      print('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void Display::print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void Display::print(double n, int digits)
{
  printFloat(n, digits);
}

void Display::println(void)
{
  print('\r');
  print('\n');
}

void Display::println(const char c[])
{
  print(c);
  println();
}

void Display::println(char c, int base)
{
  print(c, base);
  println();
}

void Display::println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void Display::println(int n, int base)
{
  print(n, base);
  println();
}

void Display::println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void Display::println(long n, int base)
{
  print(n, base);
  println();
}

void Display::println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void Display::println(double n, int digits)
{
  print(n, digits);
  println();
}

void Display::set_cursor(uint8_t x, uint8_t y) {
	cursorX = x;
	cursorY = y;
}

void Display::print(uint8_t x, uint8_t y, const char str[]) {
	cursorX = x;
	cursorY = y;
	write(str);

}
void Display::print(uint8_t x, uint8_t y, char c, int base) {
	cursorX = x;
	cursorY = y;
	print((long) c, base);
}
void Display::print(uint8_t x, uint8_t y, unsigned char b, int base) {
	cursorX = x;
	cursorY = y;
	print((unsigned long) b, base);
}
void Display::print(uint8_t x, uint8_t y, int n, int base) {
	cursorX = x;
	cursorY = y;
	print((long) n, base);
}
void Display::print(uint8_t x, uint8_t y, unsigned int n, int base) {
	cursorX = x;
	cursorY = y;
	print((unsigned long) n, base);
}
void Display::print(uint8_t x, uint8_t y, long n, int base) {
	cursorX = x;
	cursorY = y;
	print(n,base);
}
void Display::print(uint8_t x, uint8_t y, unsigned long n, int base) {
	cursorX = x;
	cursorY = y;
	print(n,base);
}
void Display::print(uint8_t x, uint8_t y, double n, int digits) {
	cursorX = x;
	cursorY = y;
	print(n,digits);
}

void Display::println(uint8_t x, uint8_t y, const char c[])
{
	cursorX = x;
	cursorY = y;
	print(c);
	println();
}

void Display::println(uint8_t x, uint8_t y, char c, int base)
{
	cursorX = x;
	cursorY = y;
	print(c, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, unsigned char b, int base)
{
	cursorX = x;
	cursorY = y;
	print(b, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, int n, int base)
{
	cursorX = x;
	cursorY = y;
	print(n, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, unsigned int n, int base)
{
	cursorX = x;
	cursorY = y;
	print(n, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, long n, int base)
{
	cursorX = x;
	cursorY = y;
	print(n, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, unsigned long n, int base)
{
	cursorX = x;
	cursorY = y;
	print(n, base);
	println();
}

void Display::println(uint8_t x, uint8_t y, double n, int digits)
{
	cursorX = x;
	cursorY = y;
	print(n, digits);
	println();
}

void Display::printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars.
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  }

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void Display::printFloat(double number, uint8_t digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint;
  }
}

void Display::draw4BitColumn(int16_t x, int16_t y, uint8_t h, uint8_t* bitmap)
{
    int8_t scry;
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));

    /** ONLY 4-bit mode for time being **/

            if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
                for (scry = y; scry < h+y; scry++) {
                    uint8_t sourcepixel = *bitmap;
                    uint8_t targetpixel = *scrptr;
                    targetpixel = (targetpixel&0x0F) | (sourcepixel << 4);
                    *scrptr = targetpixel;
                    bitmap++;
                    scrptr+=55;
                }
            } else { /** ODD pixel starting line **/
                for (scry = y; scry < h+y; scry++) {
                    uint8_t sourcepixel = *bitmap;
                    uint8_t targetpixel = *scrptr;
                    // store source pixel in lower nibble of target
                    targetpixel = (targetpixel & 0xF0) | (sourcepixel);
                    *scrptr = targetpixel;
                    scrptr+=55;
                    bitmap++;
                }
            }
}

/**
 * Setup or disable the sprite. Note that enabled sprites must always have subsequent indices, starting from the index zero.
 * You cannot have gaps in indices of enabled sprites.
 * The max number of sprites can be changed by a SPRITE_COUNT define, the default is 4.
 * Note: the sprites currently work only in the 220 x 176 x 2bpp mode.
 * @param index The sprite index. The lower index is drawn first, i.e. is on bottom.
 * @param bitmap A pointer to a 2bpp bitmap. A NULL value means that the sprite is disabled. The ownership is not transferred, so the caller must keep the bitmap alive.
 * @param palette4x16bit Four color palette of 16bit elements. The first color value is considered as transparent. The palette is copied to the sprite struct, so the caller do not have to keep it alive.
 * @param x The initial x
 * @param y The initial y
 * @param doResetDirtyRect (default=true) True, if the previous coordinates are reseted.
 */
void Display::setSpriteBitmap(uint8_t index, const uint8_t* bitmap,  const uint16_t* palette4x16bit, int16_t x, int16_t y, bool doResetDirtyRect ) {

    setSprite(index, &(bitmap[2]), palette4x16bit, x, y, bitmap[0], bitmap[1], doResetDirtyRect);
}

/**
 * Setup or disable the sprite. Note that enabled sprites must always have subsequent indices, starting from the index zero.
 * You cannot have gaps in indices of enabled sprites.
 * The max number of sprites can be changed by a SPRITE_COUNT define, the default is 4.
 * Note: the sprites currently work only in the 220 x 176 x 2bpp mode.
 * @param index The sprite index. The lower index is drawn first, i.e. is on bottom. Note that
 * @param data A pointer to a 2bpp pixel data of size w x h. A NULL value means that the sprite is disabled. The ownership is not transferred, so the caller must keep the data alive.
 * @param palette4x16bit Four color palette of 16bit elements. The first color value is considered as transparent. The palette is copied to the sprite struct, so the caller do not have to keep it alive.
 * @param x The initial x
 * @param y The initial y
 * @param w Width
 * @param h Height
 */
void Display::setSprite(uint8_t index, const uint8_t* data, const uint16_t* palette4x16bit, int16_t x, int16_t y, uint8_t w, uint8_t h, bool doResetDirtyRect ) {

    if(index >= SPRITE_COUNT) return;
    m_sprites[index].bitmapData = data;
    m_sprites[index].x = x;
    m_sprites[index].y = y;
    if (doResetDirtyRect) {
        m_sprites[index].oldx = x;
        m_sprites[index].oldy = y;
    }
    m_sprites[index].w = w;
    m_sprites[index].h = h;
    memcpy(m_sprites[index].palette, palette4x16bit, 4*2);
}

/**
 * Set the sprite position.
 * @param index The sprite index
 * @param x
 * @param y
 */
void Display::setSpritePos(uint8_t index, int16_t x, int16_t y) {

    if(index >= SPRITE_COUNT) return;
    m_sprites[index].x = x;
    m_sprites[index].y = y;
}

void Display::lcdRefresh(unsigned char* scr, bool useDirectDrawMode) {

#if POK_SCREENMODE == MODE_HI_4COLOR
    // If there is one or more sprites, use sprite enabled drawing.
    if (m_sprites[0].bitmapData != NULL)
        lcdRefreshMode1Spr(scr, 0, 0, LCDWIDTH, LCDHEIGHT, paletteptr, m_sprites, useDirectDrawMode);
    else if (!useDirectDrawMode)
        lcdRefreshMode1(m_scrbuf, 0, 0, LCDWIDTH, LCDHEIGHT, paletteptr);
#endif

    // For the screen modes that do not support sprites, return if the direct draw mode is used.
    if (useDirectDrawMode) return;

#if POK_SCREENMODE == MODE_GAMEBOY
    lcdRefreshModeGBC(scr, paletteptr);
#endif

#if POK_SCREENMODE == MODE_FAST_16COLOR
    lcdRefreshMode2(scr, paletteptr);
#endif

#if POK_SCREENMODE == MODE_GAMEBUINO_16COLOR
    lcdRefreshGB(scr, paletteptr);
#endif

#if POK_SCREENMODE == MODE_ARDUBOY_16COLOR
    lcdRefreshAB(scr, paletteptr);
#endif

}

void Display::setFrameBufferTo(uint8_t* sb) {
    m_scrbuf = sb;
};

void Display::setTileBufferTo(uint8_t* tb) {
    m_tilebuf = tb;
};

void Display::loadTileset(const uint8_t* ts) {
    m_tileset = (uint8_t*) ts;
};

void Display::setTile(uint16_t i, uint8_t t) {
    if (!m_tilebuf) return;
    m_tilebuf[i]=t;
};


/** Eof */



