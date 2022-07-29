/**************************************************************************/
/*!
  @file     HWLCD.cpp
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

#include "HWLCD.h"
#include "Pokitto_settings.h"

#define avrmax(a,b) ((a)>(b)?(a):(b))
#define avrmin(a,b) ((a)<(b)?(a):(b))

#ifdef DISABLEAVRMIN
#include <algorithm>
using std::min;
using std::max;
#endif // DISABLEAVRMIN

using namespace Pokitto;

#if !defined(POK_EXT0_PWM_ENABLE) && POK_BOARDREV == 2
pwmout_t backlightpwm;
#endif

volatile uint32_t *LCD = reinterpret_cast< volatile uint32_t * >(0xA0002188);

/**************************************************************************/
/*!
  @brief  set up the 16-bit bus
*/
/**************************************************************************/

static inline void setup_data_16(uint16_t data)
{
    SET_MASK_P2;
    LPC_GPIO_PORT->MPIN[2] = (data<<3); // write bits to port
    CLR_MASK_P2;
}

/**************************************************************************/
/*!
  @brief  Write a command to the lcd, 16-bit bus
*/
/**************************************************************************/
void write_command_16(uint16_t data)
{
    CLR_CS; // select lcd
    CLR_CD; // clear CD = command
    SET_RD; // RD high, do not read
    setup_data_16(data); // function that inputs the data into the relevant bus lines
    CLR_WR_SLOW;  // WR low
    SET_WR;  // WR low, then high = write strobe
    SET_CS; // de-select lcd
}

/**************************************************************************/
/*!
  @brief  Write data to the lcd, 16-bit bus
*/
/**************************************************************************/
void write_data_16(uint16_t data)
{
    CLR_CS;
    SET_CD;
    SET_RD;
    setup_data_16(data);
    CLR_WR;
    SET_WR;
    SET_CS;
}

/**************************************************************************/
/*!
  @brief  Pump data to the lcd, 16-bit bus, public function
*/
/**************************************************************************/
void Pokitto::pumpDRAMdata(uint16_t* data,uint16_t counter)
{
    while (counter--) {
        CLR_CS;
        SET_CD;
        SET_RD;
        setup_data_16(*data++);
        CLR_WR;
        SET_WR;
        SET_CS;
    }
}


/**************************************************************************/
/*!
  @brief  Point to a (x,y) location in the LCD DRAM
*/
/**************************************************************************/
static inline void setDRAMptr(uint8_t xptr, uint8_t yoffset)
{
    write_command(0x20);  // Vertical DRAM Address
    write_data(yoffset);
    write_command(0x21);  // Horizontal DRAM Address
    write_data(xptr);  //
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
}

/**************************************************************************/
/*!
  @brief  Point to a (x,y) location in the LCD DRAM, public function
*/
/**************************************************************************/
void Pokitto::setDRAMpoint(uint8_t xptr, uint8_t yoffset)
{
    write_command(0x20);  // Vertical DRAM Address
    write_data(yoffset);
    write_command(0x21);  // Horizontal DRAM Address
    write_data(xptr);  //
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
}

/**************************************************************************/
/*!
  @brief  Get the LCD ready for a buffered-mode refresh
*/
/**************************************************************************/
void Pokitto::lcdPrepareRefresh() {
    #if POK_FLIP_SCREEN
    write_command(0x03); write_data(0x1008); // normal
    #else
    write_command(0x03); write_data(0x1038); // normal
    #endif
    write_command(0x20);  // Horizontal DRAM Address
    write_data(0);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(0);  // 0
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
    SET_MASK_P2;
}

void Pokitto::initBacklight() {
#ifndef POK_EXT0_PWM_ENABLE
#if POK_BOARDREV == 2
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<31);
    LPC_SYSCON->PRESETCTRL |=  (1 << (0 + 9));
    LPC_IOCON->PIO2_2 = (LPC_IOCON->PIO2_2 & ~(0x3FF)) | 0x3;     //set up pin for PWM use
    LPC_SCT0->CONFIG |= ((0x3 << 17) | 0x01);
    LPC_SCT0->CTRL |= (1 << 2) | (1 << 3);

    LPC_SCT0->OUT1_SET = (1 << 0); // event 0
    LPC_SCT0->OUT1_CLR = (1 << 1); // event 1
    LPC_SCT0->EV0_CTRL  = (1 << 12);
    LPC_SCT0->EV0_STATE = 0xFFFFFFFF;
    LPC_SCT0->EV1_CTRL  = (1 << 12) | (1 << 0);
    LPC_SCT0->EV1_STATE = 0xFFFFFFFF;
    LPC_SCT0->MATCHREL0 = 20000;

    setBacklight(POK_BACKLIGHT_INITIALVALUE);
#endif
#else
    pin_function(P2_2,0);//set pin function back to 0
    LPC_GPIO_PORT->DIR[2] |= (1  << 2 );
    LPC_GPIO_PORT->SET[2] = 1 << 2; // full background light, smaller file size
#endif

}


void Pokitto::setBacklight(uint8_t value) {
#ifndef POK_EXT0_PWM_ENABLE
    if (value>100)
        value = 100;

    LPC_SCT0->MATCHREL1 = (LPC_SCT0->MATCHREL0 * value)/100; 
    LPC_SCT0->CTRL &= ~(1 << 2);
#endif
}

void Pokitto::lcdInit() {
    initBacklight();

    SET_RESET;
    wait_ms(10);
    CLR_RESET;
    wait_ms(10);
    SET_RESET;
    wait_ms(10);
    //************* Start Initial Sequence **********//
    write_command(0x01); // driver output control, this also affects direction
    write_data(0x11C); // originally: 0x11C 100011100 SS,NL4,NL3,NL2
    // NL4...0 is the number of scan lines to drive the screen !!!
    // so 11100 is 1c = 220 lines, correct
    // test 1: 0x1C 11100 SS=0,NL4,NL3,NL2 -> no effect
    // test 2: 0x31C 1100011100 GS=1,SS=1,NL4,NL3,NL2 -> no effect
    // test 3: 0x51C 10100011100 SM=1,GS=0,SS=1,NL4,NL3,NL2 -> no effect
    // test 4: 0x71C SM=1,GS=1,SS=1,NL4,NL3,NL2
    // test 5: 0x
    // seems to have no effect... is this perhaps only for RGB mode ?

    write_command(0x02); // LCD driving control
    write_data(0x0100); // INV = 1

    write_command(0x03); // Entry mode... lets try if this affects the direction
    write_data(0x1030); // originally 0x1030 1000000110000 BGR,ID1,ID0
                        // test 1: 0x1038 1000000111000 BGR,ID1,ID0,AM=1 ->drawing DRAM horizontally
                        // test 4: am=1, id0=0, id1=0, 1000000001000,0x1008 -> same as above, but flipped on long
                        // test 2: am=0, id0=0, 1000000100000, 0x1020 -> flipped on long axis
                        // test 3: am=0, id1=0, 1000000010000, 0x1010 -> picture flowed over back to screen


    write_command(0x08); // Display control 2
    write_data(0x0808); // 100000001000 FP2,BP2

    write_command(0x0C); // RGB display interface
    write_data(0x0000); // all off

    write_command(0x0F); // Frame marker position
    write_data(0x0001); // OSC_EN

    write_command(0x20);  // Horizontal DRAM Address
    write_data(0x0000);  // 0

    write_command(0x21);  // Vertical DRAM Address
    write_data(0x0000); // 0

    //*************Power On sequence ****************//
    write_command(0x10);
    write_data(0x0000);

    write_command(0x11);
    write_data(0x1000);
    wait_ms(10);
//------------------------ Set GRAM area --------------------------------//
    write_command(0x30); // Gate scan position
    write_data(0x0000); // if GS=0, 00h=G1, else 00h=G220

    write_command(0x31); // Vertical scroll control
    write_data(0x00DB); // scroll start line 11011011 = 219

    write_command(0x32); // Vertical scroll control
    write_data(0x0000); // scroll end line 0

    write_command(0x33); // Vertical scroll control
    write_data(0x0000); // 0=vertical scroll disabled

    write_command(0x34); // Partial screen driving control
    write_data(0x00DB); // db = full screen (end)

    write_command(0x35); // partial screen
    write_data(0x0000); // 0 = start

    write_command(0x36); // Horizontal and vertical RAM position
    write_data(0x00AF); //end address 175

    write_command(0x37);
    write_data(0x0000); // start address 0

    write_command(0x38);
    write_data(0x00DB); //end address 219

    write_command(0x39); // start address 0
    write_data(0x0000);
    wait_ms(10);
    write_command(0xff); // start gamma register control
    write_data(0x0003);

// ----------- Adjust the Gamma  Curve ----------//
    write_command(0x50);
    write_data(0x0203);

    write_command(0x051);
    write_data(0x0A09);

    write_command(0x52);
    write_data(0x0005);

    write_command(0x53);
    write_data(0x1021);

    write_command(0x54);
    write_data(0x0602);

    write_command(0x55);
    write_data(0x0003);

    write_command(0x56);
    write_data(0x0703);

    write_command(0x57);
    write_data(0x0507);

    write_command(0x58);
    write_data(0x1021);

    write_command(0x59);
    write_data(0x0703);

    write_command(0xB0);
    write_data(0x2501);

    write_command(0xFF);
    write_data(0x0000);

    write_command(0x07);
    write_data(0x1017);
    wait_ms(200);
    write_command(0x22);

    lcdClear();
}

void Pokitto::lcdSleep(void){
    write_command(0xFF);
    write_data(0x0000);

    write_command(0x07);
    write_data(0x0000);
    wait_ms(50);
    write_command(0x10);// Enter Standby mode
    write_data(0x0003);
    wait_ms(200);

}

void Pokitto::lcdWakeUp (void){

    wait_ms(200);
    write_command(0xFF);
    write_data(0x0000);

    write_command(0x10);// Exit Sleep/ Standby mode
    write_data(0x0000);
    wait_ms(50);
    write_command(0x07);
    write_data(0x0117);
    wait_ms(200);
}

void Pokitto::lcdFillSurface(uint16_t c) {
    uint32_t i;
    write_command(0x20);  // Horizontal DRAM Address
    write_data(0x0000);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(0);
    write_command(0x22); // write data to DRAM
    setup_data_16(c);
    CLR_CS_SET_CD_RD_WR;
    for(i=0;i<220*176;i++)
    {
        CLR_WR;
        SET_WR;
    }
}

void Pokitto::lcdClear() {
    uint32_t i;
    write_command(0x20);  // Horizontal DRAM Address
    write_data(0x0000);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(0);
    write_command(0x22); // write data to DRAM
    setup_data_16(0x0000);
    CLR_CS_SET_CD_RD_WR;
    for(i=0;i<220*176;i++)
    {
        CLR_WR;
        SET_WR;
    }
}

void Pokitto::lcdPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= POK_LCD_W) || (y < 0) || (y >= POK_LCD_H))
	return;
    write_command(0x20);  // Horizontal DRAM Address
    write_data(y);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(x);
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
    setup_data_16(color);
    CLR_WR;SET_WR;
}

void Pokitto::lcdLine(int16_t x0, int16_t x1, int16_t y, uint16_t color) {
    for (int16_t x=x0; x<=x1;x++) {
        write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
        write_data(y);
        write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
        write_data(x);
        write_command(0x22); // write data to DRAM

        CLR_CS_SET_CD_RD_WR; // go to vram write mode

        setup_data_16(color); // setup the color data
        CLR_WR;SET_WR; //toggle writeline, pokitto screen writes a column up to down
    }
}

void Pokitto::lcdBlitLine(int16_t x0, int16_t x1, int16_t y, uint8_t* srcLine) {
    for (int16_t x=x0; x<=x1;x++) {
        write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
        write_data(y);
        write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
        write_data(x);
        write_command(0x22); // write data to DRAM

        CLR_CS_SET_CD_RD_WR; // go to vram write mode

        uint16_t color = (((uint16_t)srcLine[2*x+1])<<8)+ (uint16_t)srcLine[2*x];
        setup_data_16(color); // setup the color data
        CLR_WR;SET_WR; //toggle writeline, pokitto screen writes a column up to down
    }
}

void Pokitto::setWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    write_command(0x37); write_data(x1);
    write_command(0x36); write_data(x2);
    write_command(0x39); write_data(y1);
    write_command(0x38); write_data(y2);
    write_command(0x20); write_data(x1);
    write_command(0x21); write_data(y1);
}

void Pokitto::lcdTile(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t* gfx){
    int width=x1-x0;
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
    setWindow(0, 0, 175, 219);
}


void Pokitto::lcdRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t temp;
    if (x0>x1) {temp=x0;x0=x1;x1=temp;}
    if (y0>y1) {temp=y0;y0=y1;y1=temp;}
    if (x0 > POK_LCD_W) return;
    if (y0 > POK_LCD_H) return;
    if (x1 > POK_LCD_W) x1=POK_LCD_W;
    if (y1 > POK_LCD_H) y1=POK_LCD_H;
    if (x0 < 0) x0=0;
    if (y0 < 0) y0=0;

    int16_t x,y;
    for (x=x0; x<=x1;x++) {
        write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
        write_data(y0);
        write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
        write_data(x);
        write_command(0x22); // write data to DRAM

        CLR_CS_SET_CD_RD_WR; // go to vram write mode


        for (y=y0; y<y1;y++) {
            setup_data_16(color); // setup the data (flat color = no change between pixels)
            CLR_WR;SET_WR; //CLR_WR;SET_WR;//toggle writeline, pokitto screen writes a column up to down
        }
    }
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
void Pokitto::lcdRefreshMode1(const uint8_t *scrbuf, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH, const uint16_t *paletteptr) {
    volatile uint32_t palette[32];
    for( uint32_t i=0; i<16; ++i ){
        palette[(i<<1)+1] = static_cast<uint32_t>(paletteptr[i&3 ]) << 3;
        palette[(i<<1)  ] = static_cast<uint32_t>(paletteptr[i>>2]) << 3;
    }
    
    if(POK_PERSISTENCE > 0){ 
	updateMode1(palette, scrbuf);
    }else{
        updateMode1Clear(palette, scrbuf, POK_CLEAR_SCREEN);
    }
}

void Pokitto::lcdRefreshMode2(const uint8_t* scrbuf, const uint16_t* paletteptr ) {
    if(POK_PERSISTENCE == 0){
        updateMode2Clear(paletteptr, scrbuf, POK_CLEAR_SCREEN);
    }else{
        updateMode2(paletteptr, scrbuf);
    }
}

void Pokitto::lcdRefreshMode13(const uint8_t* scrbuf, const uint16_t* paletteptr, uint8_t offset){
    if(POK_PERSISTENCE == 0){
        updateMode13Clear(paletteptr, scrbuf, offset, POK_CLEAR_SCREEN);
    }else{
        updateMode13(paletteptr, scrbuf, offset);
    }
}

void Pokitto::lcdRefreshMode64( const uint8_t* scrbuf, const uint16_t* paletteptr ){
    if(POK_PERSISTENCE == 0){
        updateMode64Clear(paletteptr, scrbuf, POK_CLEAR_SCREEN);
    }else{
        updateMode64(paletteptr, scrbuf);
    }
}

void Pokitto::lcdRefreshMode15(const uint8_t* scrbuf, const uint16_t* paletteptr){
    uint32_t palette[16];
    for( uint32_t i=0; i<16; ++i )
        palette[i] = uint32_t(paletteptr[i]) << 3;

    if(POK_PERSISTENCE == 0){
        updateMode15Clear(palette, scrbuf, POK_CLEAR_SCREEN);
    }else{
        updateMode15(palette, scrbuf);
    }
}

void Pokitto::lcdRefreshMixMode(const uint8_t * screenBuffer, const uint16_t * palettePointer, const uint8_t * scanType)
{
    uint32_t scanline[220];

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
            // point to beginning of line in data
            d = &screenBuffer[110 * scanTypeIndex];
            for(uint8_t x = 0; x < 110; ++x) // 110 = 220/2
            {
                uint32_t color = static_cast<uint32_t>(palettePointer[*d]) << 3;
                ++d;
                scanline[lineIndex] = color;
                ++lineIndex;
                scanline[lineIndex] = color;
                ++lineIndex;
            }
            break;
        }
        case 1:
        {
            for(uint8_t x = 0; x < 55; ++x) // 55 = 220/4
            {
                uint8_t t = *d;
                ++d;
                uint32_t color1 = static_cast<uint32_t>(palettePointer[256 + (t >> 4)]) << 3;
                scanline[lineIndex] = color1;
                ++lineIndex;
                scanline[lineIndex] = color1;
                ++lineIndex;
                uint32_t color2 = static_cast<uint32_t>(palettePointer[256 + (t & 0xF)]) << 3;
                scanline[lineIndex] = color2;
                ++lineIndex;
                scanline[lineIndex] = color2;
                ++lineIndex;
            }
            break;
        }
        case 2:
        {
            for(uint8_t x = 0; x < 55; ++x) // 55 = 220/4
            {
                uint8_t t = *d;
                ++d;
                scanline[lineIndex] = static_cast<uint32_t>(palettePointer[272 + ((t >> 6) & 0x03)]) << 3;
                ++lineIndex;
                scanline[lineIndex] = static_cast<uint32_t>(palettePointer[272 + ((t >> 4) & 0x03)]) << 3;
                ++lineIndex;
                scanline[lineIndex] = static_cast<uint32_t>(palettePointer[272 + ((t >> 2) & 0x03)]) << 3;
                ++lineIndex;
                scanline[lineIndex] = static_cast<uint32_t>(palettePointer[272 + ((t >> 0) & 0x03)]) << 3;
                ++lineIndex;
            }
            break;
        }
        }

        uint32_t color = scanline[0];
#define WRITE_SCANLINE *LCD = color; TGL_WR_OP(color = scanline[++i]);

        for (uint8_t i = 0; i < 220;)
        {
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE  WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE  WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE  WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE	WRITE_SCANLINE
            WRITE_SCANLINE  WRITE_SCANLINE	WRITE_SCANLINE
		}

#undef WRITE_SCANLINE
    }

    CLR_MASK_P2;
}

void Pokitto::blitWord(uint16_t c) {
    setup_data_16(c);CLR_WR;SET_WR;
}


