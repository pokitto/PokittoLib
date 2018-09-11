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

#include "miniload.h"
#include <pinmap.h>

#ifndef MINILOADHIGH
pwmout_t blightpwm;
#else

#endif


unsigned int prevdata;

/**
*  SETUP BACKLIGHT
*/
void m_initBacklight() {
    #ifndef MINILOADHIGH
    pwmout_init(&blightpwm,P2_2);
    pwmout_period_us(&blightpwm,5);
    pwmout_write(&blightpwm, 0.3f);
    #else
    pin_function(P2_2,0);//set pin function back to 0
    LPC_GPIO_PORT->DIR[2] |= (1  << 2 );
    LPC_GPIO_PORT->SET[2] = 1 << 2; // full background light, smaller file size
    #endif

}


/**
*  SETUP GPIO & DATA
*/
static void setup_gpio()
{
    /** control lines **/
    LPC_GPIO_PORT->DIR[LCD_CD_PORT] |= (1  << LCD_CD_PIN );
    LPC_GPIO_PORT->DIR[LCD_WR_PORT] |= (1  << LCD_WR_PIN );
    LPC_GPIO_PORT->DIR[LCD_RD_PORT] |= (1  << LCD_RD_PIN );
    LPC_GPIO_PORT->DIR[LCD_RES_PORT] |= (1  << LCD_RES_PIN );
    /** data lines **/
    LPC_GPIO_PORT->DIR[2] |= (0xFFFF  << 3);  // P2_3...P2_18 as output

    pin_mode(P2_3,PullNone); // turn off pull-up
    pin_mode(P2_4,PullNone); // turn off pull-up
    pin_mode(P2_5,PullNone); // turn off pull-up
    pin_mode(P2_6,PullNone); // turn off pull-up

    pin_mode(P2_7,PullNone); // turn off pull-up
    pin_mode(P2_8,PullNone); // turn off pull-up
    pin_mode(P2_9,PullNone); // turn off pull-up
    pin_mode(P2_10,PullNone); // turn off pull-up

    pin_mode(P2_11,PullNone); // turn off pull-up
    pin_mode(P2_12,PullNone); // turn off pull-up
    pin_mode(P2_13,PullNone); // turn off pull-up
    pin_mode(P2_14,PullNone); // turn off pull-up

    pin_mode(P2_15,PullNone); // turn off pull-up
    pin_mode(P2_16,PullNone); // turn off pull-up
    pin_mode(P2_17,PullNone); // turn off pull-up
    pin_mode(P2_18,PullNone); // turn off pull-up
}


/**************************************************************************/
/*!
    @brief  set up the 16-bit bus
*/
/**************************************************************************/

static inline void setup_data(uint16_t data)
{
    uint32_t p2=0;

    if (data != prevdata) {

    prevdata=data;

    /** D0...D16 = P2_3 ... P2_18 **/
    p2 = data << 3;

    //__disable_irq();    // Disable Interrupts
    SET_MASK_P2;
    LPC_GPIO_PORT->MPIN[2] = p2; // write bits to port
    CLR_MASK_P2;
    //__enable_irq();     // Enable Interrupts
    }
}


/**************************************************************************/
/*!
    @brief  Write a command to the lcd, 16-bit bus
*/
/**************************************************************************/
inline void write_command(uint16_t data)
{
   CLR_CS; // select lcd
   CLR_CD; // clear CD = command
   SET_RD; // RD high, do not read
   setup_data(data); // function that inputs the data into the relevant bus lines
   CLR_WR;  // WR low
   SET_WR;  // WR low, then high = write strobe
   SET_CS; // de-select lcd
}

/**************************************************************************/
/*!
    @brief  Write data to the lcd, 16-bit bus
*/
/**************************************************************************/
inline void write_data(uint16_t data)
{
   CLR_CS;
   SET_CD;
   SET_RD;
   setup_data(data);
   CLR_WR;
   SET_WR;
   SET_CS;
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
    write_command(0x21);  // Horiszontal DRAM Address
    write_data(xptr);  //
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
}

void m_lcdClear() {
    uint32_t i;
    write_command(0x20);  // Horizontal DRAM Address
    write_data(0x0000);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(0);
    write_command(0x22); // write data to DRAM
    setup_data(0x0000);
    CLR_CS_SET_CD_RD_WR;
    for(i=0;i<220*176;i++)
    {
        CLR_WR;
        SET_WR;
    }
}


void m_initLCD() {
   setup_gpio();
   m_initBacklight();

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

    m_lcdClear();
}

/**************************************************************************/
/*!
    @brief  directPixel(int16_t, int16_t, uint8_t) set a pixel directly (no buffering)
*/
/**************************************************************************/

void pokDirectPixel(int x, int y, unsigned int color) {
	if ((x < 0) || (x >= POK_LCD_W) || (y < 0) || (y >= POK_LCD_H))
	return;
	write_command(0x20);  // Horizontal DRAM Address
    write_data(y);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(x);
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
    setup_data(color);
    CLR_WR;SET_WR;
}

/**************************************************************************/
/*!
    @brief  pokDirectRectangle(uint8_t, uint8_t, uint8_t,uint8_t, uint16_t) fill rectangle directly (no buffering)
*/
/**************************************************************************/

void pokDirectRectangle(int x0,int y0, int x1, int y1, unsigned int color) {
	int temp;
	if (x0>x1) {temp=x0;x0=x1;x1=temp;}
	if (y0>y1) {temp=y0;y0=y1;y1=temp;}
	if (x0 > POK_LCD_W) return;
	if (y0 > POK_LCD_H) return;
    if (x1 > POK_LCD_W) x1=POK_LCD_W;
	if (y1 > POK_LCD_H) y1=POK_LCD_W;
	if (x0 < 0) x0=0;
	if (y0 < 0) y0=0;

	int x,y;
    for (x=x0; x<=x1;x++) {
        write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
        write_data(y0);
        write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
        write_data(x);
        write_command(0x22); // write data to DRAM

        CLR_CS_SET_CD_RD_WR; // go to vram write mode


        for (y=y0; y<y1;y++) {
                setup_data(color); // setup the data (flat color = no change between pixels)
                CLR_WR;SET_WR; //toggle writeline, pokitto screen writes a column up to down
        }
    }
}

/**************************************************************************/
/*!
    @brief  pokDirectRow(uint8_t, uint8_t, uint8_t, uint16_t) fill rectangle directly (no buffering)
*/
/**************************************************************************/

void pokDirectRow(int x0,int y0, int w, unsigned int color) {
	int temp, x1;
	x1 = x0 + w;
	if (x0>x1) {temp=x0;x0=x1;x1=temp;}

	if (x0 > POK_LCD_W) return;
	if (y0 > POK_LCD_H) return;
    if (x1 > POK_LCD_W) x1=POK_LCD_W;
	if (x0 < 0) x0=0;
	if (y0 < 0) y0=0;

    for (int x=x0; x<=x1;x++) {
        write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
        write_data(y0);
        write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
        write_data(x);
        write_command(0x22); // write data to DRAM

        CLR_CS_SET_CD_RD_WR; // go to vram write mode

        setup_data(color); // setup the data (flat color = no change between pixels)
        CLR_WR;SET_WR; //toggle writeline, pokitto screen writes a column up to down

    }
}

/**************************************************************************/
/*!
    @brief  pokDirectColumn(int, int, int, uint16_t)
*/
/**************************************************************************/

void pokDirectColumn(int x0,int y0, int h, unsigned int color) {
	int temp,y1;
	y1 = y0 + h;
	if (y0>y1) {temp=y0;y0=y1;y1=temp;}
	if (x0 > POK_LCD_W) return;
	if (y0 > POK_LCD_H) return;
	if (y1 > POK_LCD_H) y1=POK_LCD_W;
	if (x0 < 0) x0=0;
	if (y0 < 0) y0=0;

    write_command(0x20);  // Horizontal DRAM Address (=y on pokitto screen)
    write_data(y0);
    write_command(0x21);  // Vertical DRAM Address (=x on pokitto screen)
    write_data(x0);
    write_command(0x22); // write data to DRAM

    CLR_CS_SET_CD_RD_WR; // go to vram write mode


    for (int y=y0; y<y1;y++) {
             setup_data(color); // setup the data (flat color = no change between pixels)
             CLR_WR;SET_WR; //toggle writeline, pokitto screen writes a column up to down
        }
}
