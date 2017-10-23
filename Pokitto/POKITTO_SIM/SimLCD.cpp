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

#define CLR_WR
#define SET_WR toggle_data()

using namespace Pokitto;



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

void Pokitto::lcdRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {
    simulator.directSDLRectangle(x0,y0,x1,y1,c);
}

inline void Pokitto::setDRAMptr(uint16_t x, uint16_t y) {
    simulator.dramptr = simulator.gfxbuf + (x*SIMH) + y;
}

inline void Pokitto::setup_data_16(uint16_t data){
    //if (data != 0xFFFF && data !=0) {
    //    simulator.aBtn();
    //}
    simulator.datalines = data;
}

void Pokitto::blitWord(uint16_t c) {
    setup_data_16(c);CLR_WR;SET_WR;
}

inline void Pokitto::toggle_data(){
    *(simulator.dramptr) = simulator.datalines;
    simulator.dramptr++;
}

void Pokitto::lcdRefreshGB(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,data,xptr;
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

void Pokitto::lcdRefreshMode1(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[4][176]; // read 4 half-nibbles = 4 pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;
setDRAMptr(xptr,yoffset);


for(x=0;x<220;x+=4)
  {
    d = scrbuf+(x>>2);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(y=0;y<176;y++)
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

    d+=220/4; // jump to read byte directly below in screenbuffer
    }
    s=0;
    /** draw scanlines **/
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
    for (s=0;s<176;) {
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[2][s++]);CLR_WR;SET_WR;
    }
    for (s=0;s<176;) {
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
        setup_data_16(scanline[3][s++]);CLR_WR;SET_WR;
    }
  }
    simulator.refreshDisplay();
}

void Pokitto::lcdRefreshMode2(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y,xptr;
uint16_t scanline[2][88]; // read two nibbles = pixels at a time
uint8_t *d, yoffset=0;

xptr = 0;
setDRAMptr(xptr,yoffset);


for(x=0;x<110;x+=2)
  {
    d = scrbuf+(x>>1);// point to beginning of line in data
    /** find colours in one scanline **/
    uint8_t s=0;
    for(y=0;y<88;y++)
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
    d+=110/2; // jump to read byte directly below in screenbuffer
    }
    s=0;
    /** draw scanlines **/
    /** leftmost scanline twice**/

    for (s=0;s<88;) {
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
    }

    for (s=0;s<88;) {
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[0][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
    }
    /** rightmost scanline twice**/
    //setDRAMptr(xptr++,yoffset);
    for (s=0;s<88;) {
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
    }

    for (s=0;s<88;) {
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
        setup_data_16(scanline[1][s++]);CLR_WR;SET_WR;CLR_WR;SET_WR;
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
uint16_t x,y,data,xptr;
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
uint16_t x,y,data,xptr;
uint16_t scanline[176];
uint8_t yoffset=0, tilebyte, tileindex, tilex=0, tiley=0,xcount;

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
