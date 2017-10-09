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

#include "HWLCD.h" //HWLCD.h" #include "HWLCD.h"
#include "Pokitto_settings.h"

#define AB_JUMP 1024 // jump one 1-bit Arduboy screen forward to get next color bit
#define GB_JUMP 504 // jump one 1-bit Gamebuino screen forward to get next color bit

using namespace Pokitto;

uint16_t prevdata=0; // if data does not change, do not adjust LCD bus lines

#if POK_BOARDREV == 2
    pwmout_t backlightpwm;
#endif


/**************************************************************************/
/*!
    @brief  set up the 16-bit bus
*/
/**************************************************************************/

static inline void setup_data_16(uint16_t data)
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
inline void write_command_16(uint16_t data)
{
   CLR_CS; // select lcd
   CLR_CD; // clear CD = command
   SET_RD; // RD high, do not read
   setup_data_16(data); // function that inputs the data into the relevant bus lines
   CLR_WR;  // WR low
   SET_WR;  // WR low, then high = write strobe
   SET_CS; // de-select lcd
}

/**************************************************************************/
/*!
    @brief  Write data to the lcd, 16-bit bus
*/
/**************************************************************************/
inline void write_data_16(uint16_t data)
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

void Pokitto::initBacklight() {
    #if POK_BOARDREV == 2
    pwmout_init(&backlightpwm,POK_BACKLIGHT_PIN);
    pwmout_period_us(&backlightpwm,5);
    pwmout_write(&backlightpwm,POK_BACKLIGHT_INITIALVALUE);
    #endif
}

void Pokitto::setBacklight(float value) {
    if (value>0.999f) value = 0.999f;
    pwmout_write(&backlightpwm,value);
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

void Pokitto::lcdRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    	int16_t temp;
	if (x0>x1) {temp=x0;x0=x1;x1=temp;}
	if (y0>y1) {temp=y0;y0=y1;y1=temp;}
	if (x0 > POK_LCD_W) return;
	if (y0 > POK_LCD_H) return;
    if (x1 > POK_LCD_W) x1=POK_LCD_W;
	if (y1 > POK_LCD_H) y1=POK_LCD_W;
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
}

void Pokitto::lcdRefreshMode2(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y;
uint16_t scanline[2][88]; // read two nibbles = pixels at a time
uint8_t *d;

write_command(0x20);  // Horizontal DRAM Address
write_data(0);  // 0
write_command(0x21);  // Vertical DRAM Address
write_data(0);
write_command(0x22); // write data to DRAM
CLR_CS_SET_CD_RD_WR;

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
}

void Pokitto::lcdRefreshMode3(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y;
uint16_t scanline[2][176]; // read two nibbles = pixels at a time
uint8_t *d;

write_command(0x20);  // Horizontal DRAM Address
write_data(0);  // 0
write_command(0x21);  // Vertical DRAM Address
write_data(0);
write_command(0x22); // write data to DRAM
CLR_CS_SET_CD_RD_WR;

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
}

void Pokitto::lcdRefreshGB(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y;
uint16_t scanline[48];
uint8_t * d;

#if POK_STRETCH
//uint16_t xptr = 8;
#else
//xptr = 26;
#endif

write_command(0x20);  // Horizontal DRAM Address
write_data(0);  // 0
write_command(0x21);  // Vertical DRAM Address
write_data(0);
write_command(0x22); // write data to DRAM
CLR_CS_SET_CD_RD_WR;

/** draw border **/
    for (int s=0;s<5*176;) {
            setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;s++;
    }

for(x=0;x<84;x++)
  {

        d = scrbuf + x;// point to beginning of line in data

        /** find colours in one scanline **/
        uint8_t s=0;
        for(y=0;y<6;y++)
            {
            uint8_t t = *d;
            #if POK_COLORDEPTH > 1
            uint8_t t2 = *(d+504);
            #endif
            #if POK_COLORDEPTH > 2
            uint8_t t3 = *(d+504+504);
            #endif
            #if POK_COLORDEPTH > 3
            uint8_t t4 = *(d+504+504+504);
            #endif
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


        /*write_command(0x20);  // Horizontal DRAM Address
        write_data(0x10);  // 0
        write_command(0x21);  // Vertical DRAM Address
        write_data(xptr++);
        write_command(0x22); // write data to DRAM
        CLR_CS_SET_CD_RD_WR;*/
        /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;

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
        /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;


        /*write_command(0x20);  // Horizontal DRAM Address
        write_data(0x10);  // 0
        write_command(0x21);  // Vertical DRAM Address
        write_data(xptr++);
        write_command(0x22); // write data to DRAM
        CLR_CS_SET_CD_RD_WR;*/
        /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;

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

        /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;


        #if POK_STRETCH
        //if (x>16 && x<68)
        if (x&2)// && x&2)
        {
            /*write_command(0x20);  // Horizontal DRAM Address
            write_data(0x10);  // 0
            write_command(0x21);  // Vertical DRAM Address
            write_data(xptr++);
            write_command(0x22); // write data to DRAM
            CLR_CS_SET_CD_RD_WR;*/
            /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;


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

            /** draw border **/
        setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;        CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;CLR_WR;SET_WR;

        }
        #endif
    }
    /** draw border **/
    for (int s=0;s<5*176;) {
            setup_data_16(COLOR_BLACK);CLR_WR;SET_WR;s++;
    }
}


void Pokitto::lcdRefreshAB(uint8_t * scrbuf, uint16_t* paletteptr) {
uint16_t x,y;
uint16_t scanline[64];
uint8_t *d;
//lcdClear();
#if POK_STRETCH
uint16_t xptr = 14;
uint8_t yoffset = 24;
#else
xptr = 0; //was 26
#endif

for(x=0;x<128;x++)
  {
    write_command(0x20);  // Horizontal DRAM Address
    write_data(yoffset);  // 0
    write_command(0x21);  // Vertical DRAM Address
    write_data(xptr++);
    write_command(0x22); // write data to DRAM
    CLR_CS_SET_CD_RD_WR;
    //setDRAMptr(xptr++,yoffset);

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
        write_command(0x20);  // Horizontal DRAM Address
        write_data(yoffset);  // 0
        write_command(0x21);  // Vertical DRAM Address
        write_data(xptr++);
        write_command(0x22); // write data to DRAM
        CLR_CS_SET_CD_RD_WR;
        //setDRAMptr(xptr++,yoffset);

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
}


void Pokitto::lcdRefreshT1(uint8_t* tilebuf, uint8_t* tilecolorbuf, uint8_t* tileset, uint16_t* paletteptr) {
#ifdef POK_TILEMODE
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
    #endif
}

void Pokitto::blitWord(uint16_t c) {
    setup_data_16(c);CLR_WR;SET_WR;
}

