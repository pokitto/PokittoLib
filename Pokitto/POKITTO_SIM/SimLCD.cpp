/**************************************************************************/
/*!
    @file     SimLCD.cpp
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

#include "SimLCD.h"
#include "PokittoSimulator.h"
#ifdef DISABLEAVRMIN
#include <algorithm>
using std::min;
using std::max;
#endif // DISABLEAVRMIN

#define CLR_WR
#define SET_WR toggle_data()

using namespace Pokitto;

#ifndef DISABLEAVRMIN
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#endif // DISABLEAVRMIN

void Pokitto::lcdInit() {
    simulator.initSDLGfx();
    lcdClear();
    simulator.refreshDisplay();
}

void Pokitto::lcdFillSurface(uint16_t c) {
    simulator.fillSDLSurface(c);
    simulator.refreshDisplay();
}

void Pokitto::lcdClear() {
    simulator.fillSDLSurface(0);
}

void Pokitto::lcdPixel(int16_t x, int16_t y, uint16_t c) {
    simulator.directSDLPixel(x,y,c);
}

void Pokitto::setWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	//ToDo Sim versions for these
	/*write_command(0x37); write_data(x1);
	write_command(0x36); write_data(x2);
	write_command(0x39); write_data(y1);
	write_command(0x38); write_data(y2);
	write_command(0x20); write_data(x1);
	write_command(0x21); write_data(y1);*/
}

void Pokitto::lcdTile(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t* gfx){
	//ToDo Sim versions for these
	/*int width=x1-x0;
	int height=y1-y0;
	if (x0 > POK_LCD_W) return;
	if (y0 > POK_LCD_H) return;
	if (x0 < 0) x0=0;
	if (y0 < 0) y0=0;

	setWindow(y0, x0, y1-1, x1-1);
    write_command(0x22);

    for (int x=0; x<=width*height-1;x++) {
        write_data(gfx[x]);
    }
	setWindow(0, 0, 175, 219);*/
}

void Pokitto::lcdRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {
    simulator.directSDLRectangle(x0,y0,x1,y1,c);
}

void Pokitto::setDRAMptr(uint16_t x, uint16_t y) {
    //simulator.dramptr = simulator.gfxbuf + (x*SIMH) + y;
    simulator.dramptr = simulator.gfxbuf + x + (y*SIMW); //tolandscape
}

inline void Pokitto::setup_data_16(uint16_t data){
    simulator.datalines = data;
}

void Pokitto::blitWord(uint16_t c) {
    setup_data_16(c);CLR_WR;SET_WR;
}

inline void Pokitto::toggle_data(){
    *(simulator.dramptr) = simulator.datalines;
    simulator.dramptr++;
}

/**************************************************************************/
/*!
  @brief  Pump data to the lcd, 16-bit bus, public function
*/
/**************************************************************************/
void Pokitto::pumpDRAMdata(uint16_t* data,uint16_t counter)
{
    while (counter--) {
        setup_data_16(*data++);
        CLR_WR;SET_WR;
    }
}

/**************************************************************************/
/*!
  @brief  Point to a (x,y) location in the LCD DRAM, public function
*/
/**************************************************************************/
void Pokitto::setDRAMpoint(uint8_t xptr, uint8_t yoffset)
{
    setDRAMptr(0,0);
    CLR_WR;SET_WR;
}

/**************************************************************************/
/*!
  @brief  Get the LCD ready for a buffered-mode refresh
*/
/**************************************************************************/
void Pokitto::lcdPrepareRefresh() {
    setDRAMptr(0,0);
}



void Pokitto::lcdRefreshGB(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[48];
uint8_t *d, yoffset=0;
//lcdClear();
#if POK_STRETCH
xptr = 8;
yoffset = 0x10;
#else
xptr = 26; //was 26
#endif

for(x=0;x<84;x++)
  {
    //write_command(0x20);  // Horizontal DRAM Address
    //write_data(0x10);  // 0
    //write_command(0x21);  // Vertical DRAM Address
    //write_data(xptr++);
    //write_command(0x22); // write data to DRAM
    //CLR_CS_SET_CD_RD_WR;
    setDRAMptr(xptr++,yoffset);

        d = scrbuf + x;// point to beginning of line in data

        /** find colours in one scanline **/
        uint8_t s=0;
        for(y=0;y<6;y++)
            {
            uint8_t t = *d;
            #if POK_COLORDEPTH > 1
            uint8_t t2 = *(d+504);
            #endif // POK_COLORDEPTH
            #if POK_COLORDEPTH > 2
            uint8_t t3 = *(d+504+504);
            #endif // POK_COLORDEPTH
            #if POK_COLORDEPTH > 3
            uint8_t t4 = *(d+504+504+504);
            #endif // POK_COLORDEPTH
            uint8_t paletteindex = 0;

            /** bit 1 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x1);
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x1)) | ((t2 & 0x01)<<1);
            #elif POK_COLORDEPTH == 3
            paletteindex = (t & 0x1) | ((t2 & 0x1)<<1) | ((t3 & 0x1)<<2);
            #elif POK_COLORDEPTH == 4
            paletteindex = (t & 0x1) | ((t2 & 0x1)<<1) | ((t3 & 0x1)<<2) | ((t4 & 0x1)<<3);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 2 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x2)>>1;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x02));
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x2)) | ((t3 & 0x2)<<1);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x2)) | ((t3 & 0x2)<<1) | ((t4 & 0x2)<<2);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 3 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x4)>>2;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 4)>>2) | ((t2 & 0x04)>>1);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x4)>>2) | ((t2 & 0x4)>>1) | (t3 & 0x4);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x4)>>2) | ((t2 & 0x4)>>1) | (t3 & 0x4) | ((t4 & 0x4)<<1);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 4 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x8)>>3;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x08)>>2);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x8)>>2) | ((t3 & 0x8)>>1);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x8)>>2) | ((t3 & 0x8)>>1) | (t4 & 0x8);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 5 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x10)>>4;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3) | ((t3 & 0x10)>>2);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3) | ((t3 & 0x10)>>2) | ((t4 & 0x10)>>1);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 6 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x20)>>5;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4) | ((t3 & 0x20)>>3);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4) | ((t3 & 0x20)>>3) | ((t4 & 0x20)>>2);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 7 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x40)>>6;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5) | ((t3 & 0x40)>>4) ;
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5) | ((t3 & 0x40)>>4) | ((t4 & 0x40)>>3);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 8 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x80)>>7;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6) | ((t3 & 0x80)>>5);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6) | ((t3 & 0x80)>>5) | ((t4 & 0x80)>>4);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            d+=84; // jump to byte directly below
            }

        s=0;

        /** draw scanlines **/
        for (s=0;s<48;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
        }

        setDRAMptr(xptr++,yoffset);

        for (s=0;s<48;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
        }

        #if POK_STRETCH
        //if (x>16 && x<68)
        if (x&2)// && x&2)
        {
            //write_command(0x20);  // Horizontal DRAM Address
            //write_data(0x10);  // 0
            //write_command(0x21);  // Vertical DRAM Address
            //write_data(xptr++);
            //write_command(0x22); // write data to DRAM
            //CLR_CS_SET_CD_RD_WR;
            setDRAMptr(xptr++,yoffset);

            for (s=0;s<48;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;
        }
        }
        #endif
    }
    simulator.refreshDisplay();
}

/***
 * Update the screen buffer of 220x176 pixels, 4 colors to LCD.
 *
 * The update rect is used for drawing only part of the screen buffer to LCD. Because of speed optimizations, the
 * x, y, and width of the update rect must be dividable by 4 pixels, and the height must be dividable by 8 pixels.
 * Note: The update rect is currently used for 220x176, 4 colors, screen mode only.
 * @param scrbuf The screen buffer.
 * @param updRectX The update rect.
 * @param updRectY The update rect.
 * @param updRectW The update rect.
 * @param updRectH The update rect.
 * @param paletteptr The screen palette.
*/
void Pokitto::lcdRefreshMode1(const unsigned char* scrbuf, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH, uint16_t* paletteptr) {

    uint16_t x,y;
    uint16_t scanline[220];
    uint8_t *d, yoffset=0;

    // If not the full screen is updated, check the validity of the update rect.
    if ( updRectX != 0 || updRectY != 0 ||updRectW != LCDWIDTH ||updRectH != LCDHEIGHT ) {
        uint8_t org_screenx = updRectX;
        updRectX &= 0xfc; // Make the value dividable by 4.
        updRectW += org_screenx - updRectX;
        updRectW = (updRectW + 3) & 0xfc; // Make the value dividable by 4, round up.

        uint8_t org_screeny = updRectY;
        updRectY &= 0xfc; // Make the value dividable by 4.
        updRectH += org_screeny - updRectY;
        updRectH = (updRectH + 7) & 0xf8; // Make the value dividable by 8 (because of loop unroll optimization), round up.
    }


    #ifdef PROJ_SHOW_FPS_COUNTER
    setDRAMptr(0, 8);
    #else
    setDRAMptr(0, 0);
    #endif

    for (y=updRectY; y<updRectY+updRectH; y++) {
        d = (uint8_t*)scrbuf+y*(LCDWIDTH/4);// point to beginning of line in data

        /** find colours in one scanline **/

        d += (updRectX/4);
        int s =0;
        for (x=updRectX; x<updRectX+updRectW; x+=4) {
            uint8_t tdata = *d;
            uint8_t t4 = tdata & 0x03; tdata >>= 2;// lowest half-nibble
            uint8_t t3 = tdata & 0x03; tdata >>= 2;// second lowest half-nibble
            uint8_t t2 = tdata & 0x03; tdata >>= 2;// second highest half-nibble
            uint8_t t = tdata & 0x03;// highest half-nibble

            /** put nibble values in the scanlines **/
            scanline[s++] = paletteptr[t];
            scanline[s++] = paletteptr[t2];
            scanline[s++] = paletteptr[t3];
            scanline[s++] = paletteptr[t4];

            d++; // jump to next byte
        }

        #ifdef PROJ_SHOW_FPS_COUNTER
        if (y>=8 ) {
        #else
        {

        #endif

        setDRAMptr(updRectX, y);
        for (uint8_t s=updRectX; s<updRectX+updRectW;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;
        }
        }
    }
    #ifdef POK_SIM
    simulator.refreshDisplay();
    #endif
}

// Copy sprite pixels to the scanline
#define SPRITE_2BPP_INNER_LOOP(n)\
\
    /* If the sprite is enabled and contained in this vertical scanline, copy 4 pixels. */\
    if (sprScanlineAddr[(n)] &&\
        y >= sprites[(n)].y && y < sprites[(n)].y + sprites[(n)].h ) {\
\
        int16_t sprx = sprites[(n)].x;\
        uint16_t s_data16b = 0;  /* sprite data, 2 bytes */\
\
        /* Get pixel block, 4 or 8 pixels horizontally. Use the predefined bitshift mode. */\
        /* Note:it is cheapest to compare to 0 first. */\
        if (sprScanlineBitshiftMode[(n)] == BITSHIFT_MODE_MIDDLE_BYTE) {\
            s_data16b = *(sprScanlineAddr[(n)]);\
            uint16_t leftByte = *(sprScanlineAddr[(n)]-1);\
            s_data16b = (leftByte << 8) | s_data16b;\
        }\
        else if (sprScanlineBitshiftMode[(n)] == BITSHIFT_MODE_FIRST_BYTE) {\
            s_data16b = *(sprScanlineAddr[(n)]);\
        }\
        else { /* BITSHIFT_MODE_LAST_BYTE */\
            uint16_t leftByte = *(sprScanlineAddr[(n)]-1);\
            s_data16b = (leftByte << 8) | s_data16b;\
        }\
\
        /* Shift sprite pixels according to sprite x. After shifting we have only 4 pixels. */\
        uint8_t shiftRight = (sprx&0x3) << 1;\
        s_data16b = (s_data16b >> shiftRight);\
\
        /* Get individual pixels */\
        uint8_t s_t4 = s_data16b & 0x03; s_data16b >>= 2; /* lowest half-nibble */\
        uint8_t s_t3 = s_data16b & 0x03; s_data16b >>= 2; /* second lowest half-nibble */\
        uint8_t s_t2 = s_data16b & 0x03; s_data16b >>= 2; /* second highest half-nibble */\
        uint8_t s_t1 = s_data16b & 0x03;                  /* highest half-nibble */\
\
        /* Store pixels as 16-bit colors from the palette */\
        if (s_t4 != transparentColor) p4 = sprites[(n)].palette[s_t4];\
        if (s_t3 != transparentColor) p3 = sprites[(n)].palette[s_t3];\
        if (s_t2 != transparentColor) p2 = sprites[(n)].palette[s_t2];\
        if (s_t1 != transparentColor) p = sprites[(n)].palette[s_t1];\
\
        /* Advance scanline address */\
        sprScanlineAddr[(n)] += (sprites[(n)].w >> 2);\
    }

// Loop unrolling macros
#define UNROLLED_LOOP_1() SPRITE_2BPP_INNER_LOOP(0)
#define UNROLLED_LOOP_2() UNROLLED_LOOP_1() SPRITE_2BPP_INNER_LOOP(1)
#define UNROLLED_LOOP_3() UNROLLED_LOOP_2() SPRITE_2BPP_INNER_LOOP(2)
#define UNROLLED_LOOP_4() UNROLLED_LOOP_3() SPRITE_2BPP_INNER_LOOP(3)
#define UNROLLED_LOOP_5() UNROLLED_LOOP_4() SPRITE_2BPP_INNER_LOOP(4)
#define UNROLLED_LOOP_6() UNROLLED_LOOP_5() SPRITE_2BPP_INNER_LOOP(5)
#define UNROLLED_LOOP_7() UNROLLED_LOOP_6() SPRITE_2BPP_INNER_LOOP(6)
#define UNROLLED_LOOP_8() UNROLLED_LOOP_7() SPRITE_2BPP_INNER_LOOP(7)
#define UNROLLED_LOOP_9() UNROLLED_LOOP_8() SPRITE_2BPP_INNER_LOOP(8)
#define UNROLLED_LOOP_10() UNROLLED_LOOP_9() SPRITE_2BPP_INNER_LOOP(9)
#define UNROLLED_LOOP_11() UNROLLED_LOOP_10() SPRITE_2BPP_INNER_LOOP(10)
#define UNROLLED_LOOP_12() UNROLLED_LOOP_11() SPRITE_2BPP_INNER_LOOP(11)
#define UNROLLED_LOOP_13() UNROLLED_LOOP_12() SPRITE_2BPP_INNER_LOOP(12)
#define UNROLLED_LOOP_14() UNROLLED_LOOP_13() SPRITE_2BPP_INNER_LOOP(13)
#define UNROLLED_LOOP_15() UNROLLED_LOOP_14() SPRITE_2BPP_INNER_LOOP(14)
#define UNROLLED_LOOP_16() UNROLLED_LOOP_15() SPRITE_2BPP_INNER_LOOP(15)
#define UNROLLED_LOOP_N_(n) UNROLLED_LOOP_##n()
#define UNROLLED_LOOP_N(n) UNROLLED_LOOP_N_(n)

/***
 * Update the screen buffer of 220x176 pixels, 4 colors and free size 4 color sprites to LCD.
 *
 * The update rect is used for drawing only part of the screen buffer to LCD. Because of speed optimizations, the
 * x, y, and width of the update rect must be dividable by 4 pixels, and the height must be dividable by 8 pixels.
 * Note: The update rect is currently used for 220x176, 4 colors, screen mode only.
 * If drawSpritesOnly=true, only sprites are fully updated to LCD. However, the dirty rect of the screen buffer is
 * drawn behind the sprite current and previous location.
 * Note: Sprite is enabled if sprite.bitmapData is not NULL. Also all enabled sprites must be at the beginning of
 * the sprites array. No gaps are allowed in the array.
 * @param scrbuf The screen buffer.
 * @param updRectX The update rect.
 * @param updRectY The update rect.
 * @param updRectW The update rect.
 * @param updRectH The update rect.
 * @param paletteptr The screen palette.
 * @param sprites The sprite array.
 * @param drawSpritesOnly True, if only sprites are drawn. False, if both sprites and the screen buffer are drawn.
*/
void Pokitto::lcdRefreshMode1Spr(
    const unsigned char* scrbuf, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH, uint16_t* paletteptr,
    SpriteInfo* sprites, bool drawSpritesOnly) {

    // In direct mode draw only sprites and their dirty rects. Return now if there are no sprites
    if (drawSpritesOnly && (sprites == NULL || sprites[0].bitmapData == NULL))
        return;

    uint16_t x,y;
    uint16_t scanline[220]; // read 4 half-nibbles (= 4 pixels) at a time
    const uint8_t transparentColor = 0;  // fixed palette index 0 for transparency

    // If not the full screen is updated, check the validity of the update rect.
    if ( updRectX != 0 || updRectY != 0 ||updRectW != LCDWIDTH ||updRectH != LCDHEIGHT ) {
        uint8_t org_screenx = updRectX;
        updRectX &= 0xfc; // Make the value dividable by 4.
        updRectW += org_screenx - updRectX;
        updRectW = (updRectW + 3) & 0xfc; // Make the value dividable by 4, round up.

        uint8_t org_screeny = updRectY;
        updRectY &= 0xfc; // Make the value dividable by 4.
        updRectH += org_screeny - updRectY;
        updRectH = (updRectH + 7) & 0xf8; // Make the value dividable by 8 (because of loop unroll optimization), round up.
    }

    // Calculate the current  amount of sprites
    // Note: Sprites must be taken into use from index 0 upwards, because the first sprite with bitmapData==NULL is considered as the last sprite
    uint8_t spriteCount = 0;
    if (sprites != NULL)
        for (;sprites[spriteCount].bitmapData != NULL && spriteCount < SPRITE_COUNT; spriteCount++);

    // If drawing the screen buffer, set the start pos to LCD commands only here.
    #ifdef PROJ_SHOW_FPS_COUNTER
    if (!drawSpritesOnly) setDRAMptr(8, 0);
    #else
    if (!drawSpritesOnly) setDRAMptr(0, 0);
    #endif

    for (y=0; y<LCDHEIGHT; y++) {

        uint8_t *screenBufScanlineAddr = (uint8_t*) scrbuf + (y*LCDWIDTH/4);// point to beginning of line in data

        /*Prepare scanline start address for sprites that are visible in this vertical scanline. Sprite width cannot exceed the screen width*/
        uint8_t *sprScanlineAddr[SPRITE_COUNT];  // Sprite start address for the scanline
        uint8_t sprScanlineBitshiftMode[SPRITE_COUNT];  // Sprite bitshift mode for the scanline
        const uint8_t BITSHIFT_MODE_MIDDLE_BYTE = 0;
        const uint8_t BITSHIFT_MODE_FIRST_BYTE = 1;
        const uint8_t BITSHIFT_MODE_LAST_BYTE = 2;
        uint8_t scanlineMinX = 255; // Init to uninitialized value. Do not draw by default.
        uint8_t scanlineMaxX = 0; // Init to uninitialized value. Do not draw by default.

        //*** CALCULATE DIRTY RECTS AND RESOLVE WHICH SPRITES BELONG TO THIS SCANLINE GROUP ***

        if (sprites != NULL) {

            // Check all the sprites for this scanline. That is used for handling the given update rect
            // Note that the last round is when (sprindex == spriteCount). That is used to add the screen buffer
            // update rect to the dirty rect.
            for (int sprindex = 0; sprindex <= spriteCount; sprindex++) {

                int16_t sprx, spry, sprOldX, sprOldY;
                uint8_t sprw, sprh;
                bool isCurrentSpriteOutOfScreen = false;
                bool isOldSpriteOutOfScreen = false;

                if (sprindex < spriteCount) {

                    sprx = sprites[sprindex].x;
                    spry = sprites[sprindex].y;
                    sprw = sprites[sprindex].w;
                    sprh = sprites[sprindex].h;
                    sprOldX = sprites[sprindex].oldx;
                    sprOldY = sprites[sprindex].oldy;
               }

                // Handle the screen buffer update rect after all sprites
                else if(!drawSpritesOnly){

                    sprx = updRectX;
                    spry = updRectY;
                    sprw = updRectW;
                    sprh = updRectH;
                    sprOldX = updRectX;
                    sprOldY = updRectY;
                    isCurrentSpriteOutOfScreen = false;
                    isOldSpriteOutOfScreen = false;
                }

                // Check for out-of-screen
                if (sprx >= LCDWIDTH || spry >= LCDHEIGHT)
                    isCurrentSpriteOutOfScreen = true;
                if (sprOldX >= LCDWIDTH || sprOldY >= LCDHEIGHT)
                    isOldSpriteOutOfScreen = true;

                // Skip if current and old sprites are out-of-screen
                if (isCurrentSpriteOutOfScreen && isOldSpriteOutOfScreen)
                    continue;

                // Detect the dirty rect x-span by combining the previous and current sprite position.
                int16_t sprDirtyXMin = min(sprx, sprOldX);
                int16_t sprDirtyXMax = max(sprx, sprOldX);
                if (isCurrentSpriteOutOfScreen)
                    sprDirtyXMax = sprOldX;
                if (isOldSpriteOutOfScreen)
                    sprDirtyXMax = sprx;

                // Is current x inside the sprite combined dirty rect ?
                int16_t sprDirtyXMaxEnd = sprDirtyXMax + sprw - 1 + 4; // Add 4 pixels to dirty rect width (needed?)
                if (sprDirtyXMin <= x+3 && x <= sprDirtyXMaxEnd) {

                    // *** COMBINE DIRTY RECTS FOR THIS SCANLINE GROUP ***

                    // Dirty rect
                    int16_t sprDirtyXMin = min(sprx, sprOldX);
                    sprDirtyXMin = max(sprDirtyXMin, (int16_t)0);
                    int16_t sprDirtyXMax = max(sprx, sprOldX);
                    if (isCurrentSpriteOutOfScreen)
                        sprDirtyXMax = sprOldX;
                    if (isOldSpriteOutOfScreen)
                        sprDirtyXMax = sprx;
                    int16_t sprDirtyXMaxEnd = sprDirtyXMax + sprw - 1;
                    sprDirtyXMaxEnd = min(sprDirtyXMaxEnd, (int16_t)(LCDWIDTH - 1));  // Should use LCDHEIGHT instead of screenH? Same with other screen* ?

                    // Get the scanline min and max y values for drawing
                    if (sprDirtyXMin < scanlineMinX)
                        scanlineMinX = sprDirtyXMin;
                    if (sprDirtyXMaxEnd > scanlineMaxX)
                        scanlineMaxX = sprDirtyXMaxEnd;

                   // *** PREPARE SPRITE FOR DRAWING ***

                   // Check if the sprite should be active for this vertical scanline group.
                    if (sprindex < spriteCount &&  // not for update rect
                        !isCurrentSpriteOutOfScreen && //out-of-screen
                        sprx <= x+3 && x < sprx + sprw) { // note: cover group of 4 pixels of the scanline (x+3)

                        // Find the byte number in the sprite data
                        int16_t byteNum = ((x+3) - sprx)>>2;

                        // Get the start addres of the spite data in this scanline.
                        sprScanlineAddr[sprindex] = const_cast<uint8_t*>(sprites[sprindex].bitmapData + byteNum);

                        // If the sprite goes over the top, it must be clipped from the top.
                        if(spry < 0)
                            sprScanlineAddr[sprindex] += (-spry) * (sprw >> 2);

                        // Select the bitshift mode for the blit algorithm
                        if (byteNum == 0)
                            sprScanlineBitshiftMode[sprindex] = BITSHIFT_MODE_FIRST_BYTE;
                        else if (byteNum >= (sprw >> 2))
                            sprScanlineBitshiftMode[sprindex] = BITSHIFT_MODE_LAST_BYTE;
                        else
                            sprScanlineBitshiftMode[sprindex] = BITSHIFT_MODE_MIDDLE_BYTE;
                    }
                    else
                        sprScanlineAddr[sprindex] = NULL;  // Deactive sprite for this scanline
                }
                else
                    sprScanlineAddr[sprindex] = NULL;  // Deactive sprite for this scanline
            }
        }

        // *** ADJUST THE SCANLINE GROUP HEIGHT ***

        // The width must dividable by 8. That is needed because later we copy 8 pixels at a time to the LCD.
        if (scanlineMaxX - scanlineMinX + 1 > 0) {
            uint8_t scanlineW = scanlineMaxX - scanlineMinX + 1;
            uint8_t addW = 8 - (scanlineW & 0x7);

            // if height is not dividable by 8, make it be.
            if (addW != 0) {
                if (scanlineMinX > addW )
                    scanlineMinX -= addW;
                else if( scanlineMaxX + addW < updRectX+updRectW)
                    scanlineMaxX += addW;
                else {
                    // Draw full height scanline
                    scanlineMinX = updRectX;
                    scanlineMaxX = updRectX+updRectW-1;
                }
            }
        }

        // *** COMBINE THE SCANLINE GROUP OF THE SCREEN BUFFER AND ALL SPRITES ***

        screenBufScanlineAddr += scanlineMinX;
        int s =0;
        for (x=scanlineMinX; x<=scanlineMaxX; x+=4)
        {
            // get the screen buffer data first
            uint8_t tdata = *screenBufScanlineAddr;
            uint8_t t4 = tdata & 0x03; tdata >>= 2;// lowest half-nibble
            uint8_t t3 = tdata & 0x03; tdata >>= 2;// second lowest half-nibble
            uint8_t t2 = tdata & 0x03; tdata >>= 2;// second highest half-nibble
            uint8_t t = tdata & 0x03;// highest half-nibble

            // Convert to 16-bit colors in palette
            uint16_t p = paletteptr[t];
            uint16_t p2 = paletteptr[t2];
            uint16_t p3 = paletteptr[t3];
            uint16_t p4 = paletteptr[t4];

            #if 0
            // Dirty rect visual test
            p = COLOR_BLUE >> (Core::frameCount % 5);
            p2 = COLOR_BLUE >> (Core::frameCount % 5);
            p3 = COLOR_BLUE >> (Core::frameCount % 5);
            p4 = COLOR_BLUE >> (Core::frameCount % 5);
            #endif

            // Add active sprite pixels
            if (sprites != NULL) {

                // Use loop unrolling for speed optimization
                UNROLLED_LOOP_N(SPRITE_COUNT)
            }

            // put the result nibble values in the scanline
            scanline[s++] = p;
            scanline[s++] = p2;
            scanline[s++] = p3;
            scanline[s++] = p4;

            screenBufScanlineAddr++; // jump to read next byte in screenbuffer
        }

        // *** DRAW THE SCANLINE GROUP TO LCD

#ifdef PROJ_SHOW_FPS_COUNTER
        if (x>=8 && scanlineMaxY - scanlineMinY +1 > 0) {
#else
        if (scanlineMaxX - scanlineMinX +1 > 0) {
#endif
            setDRAMptr(scanlineMinX, y);
            for (uint8_t t=scanlineMinX;t<=scanlineMaxX;) {
                setup_data_16(scanline[t++]);CLR_WR;SET_WR;
            }
        }
    }

    // Update old x and y for the sprites
    if (sprites != NULL) {
        for (int sprindex = 0; sprindex < spriteCount; sprindex++) {
            sprites[sprindex].oldx = sprites[sprindex].x;
            sprites[sprindex].oldy = sprites[sprindex].y;
        }
    }

    #ifdef POK_SIM
    simulator.refreshDisplay();
    #endif
}

void Pokitto::lcdRefreshMode2(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[110]; // one horizontal line on "lcd", each pixel drawn twice
uint8_t *d, yoffset=0;

xptr = 0;


for(y=0;y<88;y++)
  {
    d = scrbuf+(y*55); // point to beginning of line in source data

    /** find colours in one scanline **/
    uint8_t s=0;
    for(x=0;x<110;x+=2) // 55 loops as 2 pixels are packed into one byte
    {
    uint8_t t = *d >> 4; // higher nibble of source byte
    uint8_t t2 = *d & 0xF; // lower nibble of source byte
    /** higher nibble = left pixel in pixel pair **/
    scanline[s++] = paletteptr[t]; // pixel n as RGB565 color
    scanline[s++] = paletteptr[t2]; // pixel n+1 as RGB565 color
    d++; // next source byte
    }
    /** draw scanlines **/

    #ifdef PROJ_SHOW_FPS_COUNTER
    if (x<4) continue;
    setDRAMptr(x<<1, 0);
    #endif

    /** draw scanline twice**/
    setDRAMptr(xptr,y*2); // point to beginning of dram in lcd
    for (s=0;s<110;) {
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR; // pixel n twice to lcd
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR; // pixel n+1 twice to lcd
    }
    for (s=0;s<110;) {
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR; // pixel n twice to lcd
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR; // pixel n+1 twice to lcd
    }

  }
    simulator.refreshDisplay();
}

void Pokitto::lcdRefreshMode3(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[2][176]; // read two nibbles = pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;
setDRAMptr(xptr,yoffset);


for(x=0;x<220;x+=2)
  {
    d = scrbuf+(x>>1);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(y=0;y<176;y++)
    {
    uint8_t t = *d >> 4; // higher nibble
    uint8_t t2 = *d & 0xF; // lower nibble
    /** higher nibble = left pixel in pixel pair **/
    scanline[0][s] = paletteptr[t];
    scanline[1][s++] = paletteptr[t2];
    /** testing only **/
    //scanline[0][s] = 0xFFFF*(s&1);
    //scanline[1][s] = 0xFFFF*(!(s&1));
    //s++;
    /** until here **/
    d+=220/2; // jump to read byte directly below in screenbuffer
    }
    s=0;
    /** draw scanlines **/
    /** leftmost scanline**/

    #ifdef PROJ_SHOW_FPS_COUNTER
    if (x<8) continue;
    setDRAMptr(x, 0);
    #endif

    for (s=0;s<176;) {
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
    }

    /** rightmost scanline**/
    //setDRAMptr(xptr++,yoffset);
    for (s=0;s<176;) {
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
    }

  }
    simulator.refreshDisplay();
}


void Pokitto::lcdRefreshAB(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[64];
uint8_t *d, yoffset=0;
//lcdClear();
#if POK_STRETCH
xptr = 14;
yoffset = 24;
#else
xptr = 0; //was 26
#endif

for(x=0;x<128;x++)
  {
    //write_command(0x20);  // Horizontal DRAM Address
    //write_data(0x10);  // 0
    //write_command(0x21);  // Vertical DRAM Address
    //write_data(xptr++);
    //write_command(0x22); // write data to DRAM
    //CLR_CS_SET_CD_RD_WR;
    setDRAMptr(xptr++,yoffset);

        d = scrbuf + x;// point to beginning of line in data

        /** find colours in one scanline **/
        uint8_t s=0;
        for(y=0;y<8;y++)
            {
            uint8_t t = *d;
            #if POK_COLORDEPTH > 1
            uint8_t t2 = *(d+AB_JUMP);
            #endif // POK_COLORDEPTH
            #if POK_COLORDEPTH > 2
            uint8_t t3 = *(d+AB_JUMP+AB_JUMP);
            #endif // POK_COLORDEPTH
            #if POK_COLORDEPTH > 3
            uint8_t t4 = *(d+AB_JUMP+AB_JUMP+AB_JUMP);
            #endif // POK_COLORDEPTH
            uint8_t paletteindex = 0;

            /** bit 1 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x1);
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x1)) | ((t2 & 0x01)<<1);
            #elif POK_COLORDEPTH == 3
            paletteindex = (t & 0x1) | ((t2 & 0x1)<<1) | ((t3 & 0x1)<<2);
            #elif POK_COLORDEPTH == 4
            paletteindex = (t & 0x1) | ((t2 & 0x1)<<1) | ((t3 & 0x1)<<2) | ((t4 & 0x1)<<3);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 2 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x2)>>1;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x02));
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x2)) | ((t3 & 0x2)<<1);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x2)>>1) | ((t2 & 0x2)) | ((t3 & 0x2)<<1) | ((t4 & 0x2)<<2);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 3 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x4)>>2;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 4)>>2) | ((t2 & 0x04)>>1);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x4)>>2) | ((t2 & 0x4)>>1) | (t3 & 0x4);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x4)>>2) | ((t2 & 0x4)>>1) | (t3 & 0x4) | ((t4 & 0x4)<<1);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 4 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x8)>>3;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x08)>>2);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x8)>>2) | ((t3 & 0x8)>>1);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x8)>>3) | ((t2 & 0x8)>>2) | ((t3 & 0x8)>>1) | (t4 & 0x8);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 5 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x10)>>4;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3) | ((t3 & 0x10)>>2);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x10)>>4) | ((t2 & 0x10)>>3) | ((t3 & 0x10)>>2) | ((t4 & 0x10)>>1);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 6 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x20)>>5;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4) | ((t3 & 0x20)>>3);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x20)>>5) | ((t2 & 0x20)>>4) | ((t3 & 0x20)>>3) | ((t4 & 0x20)>>2);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 7 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x40)>>6;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5) | ((t3 & 0x40)>>4) ;
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x40)>>6) | ((t2 & 0x40)>>5) | ((t3 & 0x40)>>4) | ((t4 & 0x40)>>3);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            /** bit 8 **/
            #if POK_COLORDEPTH == 1
            paletteindex = (t & 0x80)>>7;
            #elif POK_COLORDEPTH == 2
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6);
            #elif POK_COLORDEPTH == 3
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6) | ((t3 & 0x80)>>5);
            #elif POK_COLORDEPTH == 4
            paletteindex = ((t & 0x80)>>7) | ((t2 & 0x80)>>6) | ((t3 & 0x80)>>5) | ((t4 & 0x80)>>4);
            #endif
            scanline[s++] = paletteptr[paletteindex];

            d+=128; // jump to byte directly below
            }

        s=0;

        /** draw scanlines **/
        for (s=0;s<64;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        }

        #if POK_STRETCH
        if (x&1) {
        setDRAMptr(xptr++,yoffset);

        for (s=0;s<64;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        }
        }
        #endif
    }
    simulator.refreshDisplay();
}


void Pokitto::lcdRefreshT1(uint8_t* tilebuf, uint8_t* tilecolorbuf, uint8_t* tileset, uint16_t* paletteptr) {
uint16_t xptr;
uint16_t scanline[176];
uint8_t yoffset=0, tilebyte, tileindex, tilex=0, tiley=0, xcount;

if (!tileset) return;

#if LCDWIDTH < POK_LCD_W
xptr = (POK_LCD_W-LCDWIDTH)/2;
#else
xptr = 0;
#endif
#if LCDHEIGHT < POK_LCD_H
yoffset = (POK_LCD_H-LCDHEIGHT)/2;
#else
yoffset = 0;
#endif

#ifdef POK_TILEDMODE
for(x=0, xcount=0 ;x<LCDWIDTH;x++,xcount++)  // loop through vertical columns
  {
    setDRAMptr(xptr++,yoffset); //point to VRAM

        /** find colours in one scanline **/
        uint8_t s=0, tiley=0;
        //tileindex = tilebuf[tilex*POK_TILES_Y];
        if (xcount==POK_TILE_W) {
            tilex++;
            xcount=0;
        }

        for(y=0;y<LCDHEIGHT;)
        {
            uint8_t tileval = tilebuf[tilex+tiley*POK_TILES_X]; //get tile number
            uint16_t index = tileval*POK_TILE_W+xcount;
            uint8_t tilebyte = tileset[index]; //get bitmap data
            for (uint8_t ycount=0, bitcount=0; ycount<POK_TILE_H; ycount++, y++, bitcount++) {
                if (bitcount==8) {
                    bitcount=0;
                    index += 176; //jump to byte below in the tileset bitmap
                    tilebyte = tileset[index]; //get bitmap data
                }
                //tilebyte = tile[(tileindex>>4)+*POK_TILE_W]; //tilemaps are 16x16
                //uint8_t paletteindex = ((tilebyte>>(bitcount&0x7)) & 0x1);
                if (!tileval) scanline[s++] = COLOR_MAGENTA*((tilebyte>>bitcount)&0x1);//paletteptr[paletteindex];
                else scanline[s++] = paletteptr[((tilebyte>>bitcount)&0x1)*tileval];//paletteptr[paletteindex];
            }
            tiley++; //move to next tile
        }
        s=0;

        /** draw scanlines **/
        for (s=0;s<LCDHEIGHT;) {
            setup_data_16(scanline[s++]);CLR_WR;SET_WR;
        }
    }
    #endif // POK_TILEDMODE
    simulator.refreshDisplay();
}


void Pokitto::lcdRefreshModeLamenes(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[2][120]; // read two nibbles = pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;

for(x=0;x<128;x+=2)
  {
    d = scrbuf+(x>>1);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(y=0;y<120;y++)
    {
    uint8_t t = *d >> 4; // higher nibble
    uint8_t t2 = *d & 0xF; // lower nibble
    /** higher nibble = left pixel in pixel pair **/
    scanline[0][s] = paletteptr[t];
    scanline[1][s++] = paletteptr[t2];

    d+=128/2; // jump to read byte directly below in screenbuffer
    }
    s=0;
    /** draw scanlines **/
    /** leftmost scanline**/

    for (s=0;s<120;) {
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
    }


    /** rightmost scanline **/
    setDRAMptr(xptr++,yoffset);
    for (s=0;s<120;) {
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
    }

     setDRAMptr(xptr++,yoffset);
  }
    simulator.refreshDisplay();
}

void Pokitto::lcdRefreshModeGBC(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[4][144]; // read 4 half-nibbles = 4 pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;
setDRAMptr(xptr,yoffset);


for(x=0;x<160;x+=4)
  {
    d = scrbuf+(x>>2);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(y=0;y<144;y++)
    {
    uint8_t tdata = *d;

    uint8_t t4 = tdata & 0x03; tdata >>= 2;// lowest half-nibble
    uint8_t t3 = tdata & 0x03; tdata >>= 2;// second lowest half-nibble
    uint8_t t2 = tdata & 0x03; tdata >>= 2;// second highest half-nibble
    uint8_t t = tdata & 0x03;// highest half-nibble

    /** put nibble values in the scanlines **/

    scanline[0][s] = paletteptr[t];
    scanline[1][s] = paletteptr[t2];
    scanline[2][s] = paletteptr[t3];
    scanline[3][s++] = paletteptr[t4];

     d+=160/4; // jump to read byte directly below in screenbuffer
    }

    s=0;
    /** draw scanlines **/
    for (s=0;s<144;) {
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;
    }
    setDRAMptr(++xptr,yoffset);
    for (s=0;s<144;) {
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;
    }
    setDRAMptr(++xptr,yoffset);
    for (s=0;s<144;) {
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
    }
    setDRAMptr(++xptr,yoffset);
    for (s=0;s<144;) {
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
    }
    setDRAMptr(++xptr,yoffset);
  }
    simulator.refreshDisplay();
}


void Pokitto::lcdRefreshMode13(const unsigned char* scrbuf, uint16_t* paletteptr, uint8_t offset){
uint16_t x,y;
uint16_t scanline[110];
uint8_t *d;

setDRAMptr(0,0);

for(y=0;y<88;y++)
 {
    d = (uint8_t*)scrbuf+y*110;// point to beginning of line in data
    uint8_t s=0;
    for(x=0;x<110;x++)
    {
        uint8_t t = *d;
        scanline[s++] = paletteptr[(t+offset)&255];
        d++; // jump to next source byte
    }
    s=0;
    // draw scanline twice
    for (s=0;s<110;) {
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
    }
    for (s=0;s<110;) {
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
    }
 }
}


void Pokitto::lcdRefreshMode15(const unsigned char* scrbuf, uint16_t* paletteptr){
uint16_t x,y,xptr;
uint16_t scanline[220]; // read two nibbles = pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;
setDRAMptr(xptr,yoffset);

for(y=0;y<176;y++)
  {
    d = (uint8_t*)scrbuf+(y*110);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(x=0;x<220;x+=2)
    {
    uint8_t t = *d >> 4; // higher nibble
    uint8_t t2 = *d & 0xF; // lower nibble
    /** higher nibble = left pixel in pixel pair **/
    scanline[s++] = paletteptr[t];
    scanline[s++] = paletteptr[t2];
    /** testing only **/
    //scanline[0][s] = 0xFFFF*(s&1);
    //scanline[1][s] = 0xFFFF*(!(s&1));
    //s++;
    /** until here **/
    d++; // jump to read next source byte
    }
    s=0;
    /** draw scanlines **/

    #ifdef PROJ_SHOW_FPS_COUNTER
    if (y<4) continue;
    setDRAMptr(0, y);
    #endif

    for (s=0;s<220;) {
        setup_data_16(scanline[s++]);CLR_WR;SET_WR;
    }
  }
    simulator.refreshDisplay();
}


void pixelCopy(uint8_t* dest, const uint8_t *src, uint32_t count, uint32_t recolor) {
    while (count--) {
        if (*src !=0 ) *dest = *src + recolor;
        src++; dest++; //to next pixel
    }
}

void pixelCopyMirror(uint8_t* dest, const uint8_t *src, uint32_t count, uint32_t recolor) {
    src += count - 1;
    while (count--) {
        if (*src !=0 ) *dest = *src + recolor;
        src--; dest++; //to next pixel
    }
}

void pixelCopySolid(uint8_t* dest, const uint8_t *src, uint32_t count, uint32_t recolor) {
    while (count--) {
        *dest = *src + recolor;
        src++; dest++; //to next pixel
    }
}

void flushLine(const uint16_t *palette, const uint8_t *line){
  for(int i=0; i<220; ++i){
    Pokitto::blitWord(palette[line[i]]);
  }
}

void flushLine2X(const uint16_t *palette, const uint8_t *line){

  // Blit lowres screen 2x zoomed
  for(int i=0; i<110; ++i){
    Pokitto::blitWord(palette[line[i]]);
    Pokitto::blitWord(palette[line[i]]);
  }
}

void Pokitto::lcdRefreshMixMode(const uint8_t * screenBuffer, const uint16_t * palettePointer, const uint8_t * scanType)
{
uint16_t scanline[220];

setDRAMptr(0,0);



    // point to beginning of line in data
    const uint8_t * d = screenBuffer;
    for(uint32_t y = 0; y < 176; ++y)
    {
        // find colours in one scanline
        uint8_t scanTypeIndex = y >> 1;
        uint8_t lineIndex = 0;
        switch(scanType[scanTypeIndex])
        {
            case 0:
            {
				// Point to the beginning of the line in data
				d = &screenBuffer[110 * scanTypeIndex];
	            for(int x=0; x<110; x++){
					int color = static_cast<uint32_t>(palettePointer[*d++]);
					setup_data_16(color);CLR_WR;SET_WR;
					setup_data_16(color);CLR_WR;SET_WR;
	            }
	            break;
            }

            case 1:
            {
	            for(int x=0; x<55; x++){
    				const uint8_t value = *d++;
                    uint32_t color1 = palettePointer[((value >> 4) & 0x0F)+256];
                    uint32_t color2 = palettePointer[((value >> 0) & 0x0F)+256];
					setup_data_16(color1);CLR_WR;SET_WR;
					setup_data_16(color1);CLR_WR;SET_WR;
					setup_data_16(color2);CLR_WR;SET_WR;
					setup_data_16(color2);CLR_WR;SET_WR;
                }
    			break;
            }

            case 2:
            {
	            for(int x=0; x<55; x++){
    				const uint8_t value = *d++;
    				uint32_t color1 = palettePointer[((value >> 6) & 0x03)+272];
	    			uint32_t color2 = palettePointer[((value >> 4) & 0x03)+272];
		    		uint32_t color3 = palettePointer[((value >> 2) & 0x03)+272];
			    	uint32_t color4 = palettePointer[((value >> 0) & 0x03)+272];
					setup_data_16(color1);CLR_WR;SET_WR;
					setup_data_16(color2);CLR_WR;SET_WR;
					setup_data_16(color3);CLR_WR;SET_WR;
					setup_data_16(color4);CLR_WR;SET_WR;
                }
    			break;
            }

        }
    }
}
