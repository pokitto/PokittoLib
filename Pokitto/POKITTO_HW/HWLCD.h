/**************************************************************************/
/*!
    @file     HWLCD.h
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

#ifndef __HWLCD_H__
#define __HWLCD_H__

#include "mbed.h"
#include "gpio_api.h"
#include "pinmap.h"

#define write_command write_command_16
#define write_data write_data_16

namespace Pokitto {

struct SpriteInfo {
    const uint8_t* bitmapData;
    int16_t x;
    int16_t y;
    int16_t oldx;
    int16_t oldy;
    uint8_t w;
    uint8_t h;
    uint16_t palette[4];
};

extern void initBacklight();
extern void setBacklight(float);
extern void lcdFillSurface(uint16_t);
extern void lcdPixel(int16_t x, int16_t y, uint16_t c);
extern void setWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
extern void lcdTile(int16_t x0, int16_t y0, int16_t width, int16_t height, uint16_t* gfx);
extern void lcdRectangle(int16_t x, int16_t y,int16_t x2, int16_t y2, uint16_t color);
extern void lcdInit();
extern void lcdSleep();
extern void lcdWakeUp();
extern void lcdRefresh(uint8_t *, uint16_t*);
extern void lcdRefreshAB(uint8_t *, uint16_t*);
extern void lcdRefreshGB(uint8_t *, uint16_t*);
extern void lcdRefreshMode1(uint8_t* scrbuf, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH, uint16_t* paletteptr);
extern void lcdRefreshMode1Spr(uint8_t * scrbuf, uint8_t screenx, uint8_t screeny, uint8_t screenw, uint8_t screenh, uint16_t* paletteptr, Pokitto::SpriteInfo* sprites, bool drawSpritesOnly);
extern void lcdRefreshMode2(uint8_t *, uint16_t*);
extern void lcdRefreshMode3(uint8_t *, uint16_t*);
extern void lcdRefreshModeGBC(uint8_t *, uint16_t*);
extern void lcdRefreshMode13(uint8_t *, uint16_t*, uint8_t);

/** Update LCD from 1-bit tile mode */
extern void lcdRefreshT1(uint8_t*, uint8_t*, uint8_t*, uint16_t*);
extern void lcdClear();
extern void lcdFill(uint16_t);
/** Blit one word of data*/
extern void blitWord(uint16_t);

/**************************************************************************/
/**                          PINS AND PORTS                              **/
/**************************************************************************/

#if POK_BOARDREV == 1
/** 2-layer board version 1.3 **/
 #define LCD_CD_PORT           0
 #define LCD_CD_PIN            2
 #define LCD_WR_PORT           1
 #define LCD_WR_PIN            23
 #define LCD_RD_PORT           1
 #define LCD_RD_PIN            24
 #define LCD_RES_PORT          1
 #define LCD_RES_PIN           28
#else
/** 4-layer board version 2.1 **/
 #define LCD_CD_PORT           0
 #define LCD_CD_PIN            2
 #define LCD_WR_PORT           1
 #define LCD_WR_PIN            12
 #define LCD_RD_PORT           1
 #define LCD_RD_PIN            24
 #define LCD_RES_PORT          1
 #define LCD_RES_PIN           0
#endif

/**************************************************************************/
/**                          LCD CONTROL MACROS                          **/
/**************************************************************************/

#define CLR_RESET LPC_GPIO_PORT->CLR[LCD_RES_PORT] = 1 << LCD_RES_PIN; //RST = (0); // Clear pin
#define SET_RESET LPC_GPIO_PORT->SET[LCD_RES_PORT] = 1 << LCD_RES_PIN; // RST = (1); // Set pin

#define CLR_CD { LPC_GPIO_PORT->CLR[LCD_CD_PORT] = 1 << LCD_CD_PIN; } // RS = (0); // Clear pin
#define SET_CD { LPC_GPIO_PORT->SET[LCD_CD_PORT] = 1 << LCD_CD_PIN; }// RS = (1); // Set pin

#define CLR_WR { LPC_GPIO_PORT->CLR[LCD_WR_PORT] = 1 << LCD_WR_PIN; __asm("nop");__asm("nop");}//WR = (0); // Clear pin
#define SET_WR LPC_GPIO_PORT->SET[LCD_WR_PORT] = 1 << LCD_WR_PIN; //WR = (1); // Set pin

#define CLR_RD LPC_GPIO_PORT->CLR[LCD_RD_PORT] = 1 << LCD_RD_PIN; //RD = (0); // Clear pin
#define SET_RD LPC_GPIO_PORT->SET[LCD_RD_PORT] = 1 << LCD_RD_PIN; //RD = (1); // Set pin

#define SET_CS  //CS tied to ground
#define CLR_CS

#define CLR_CS_CD_SET_RD_WR {CLR_CD; SET_RD; SET_WR;}
#define CLR_CS_SET_CD_RD_WR {SET_CD; SET_RD; SET_WR;}
#define SET_CD_RD_WR {SET_CD; SET_RD; SET_WR;}
#define SET_WR_CS SET_WR;

#define SET_MASK_P2 LPC_GPIO_PORT->MASK[2] = ~(0x7FFF8); //mask P2_3 ...P2_18
#define CLR_MASK_P2 LPC_GPIO_PORT->MASK[2] = 0; // all on



/**************************************************************************/
/**                          SETUP GPIO & DATA                           **/
/**************************************************************************/

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




#define HI_BYTE(d) (LPC_GPIO->MPIN[1]= (d<<13)) //((d>>8)<<21))
#define LO_BYTE(d) (LPC_GPIO->MPIN[1]= (d<<21)) //because of mask makes no difference

// Macros to set data bus direction to input/output
#define LCD_GPIO2DATA_SETINPUT  GPIO_GPIO2DIR &= ~LCD_DATA_MASK
#define LCD_GPIO2DATA_SETOUTPUT GPIO_GPIO2DIR |= LCD_DATA_MASK


// Basic Color definitions
#define	COLOR_BLACK                         (uint16_t)(0x0000)
#define	COLOR_BLUE                          (uint16_t)(0x001F)
#define	COLOR_RED                           (uint16_t)(0xF800)
#define	COLOR_GREEN                         (uint16_t)(0x07E0)
#define COLOR_CYAN                          (uint16_t)(0x07FF)
#define COLOR_MAGENTA                       (uint16_t)(0xF81F)
#define COLOR_YELLOW                        (uint16_t)(0xFFE0)
#define COLOR_WHITE                         (uint16_t)(0xFFFF)

// Grayscale Values
#define COLOR_GRAY_15                       (uint16_t)(0x0861)    //  15  15  15
#define COLOR_GRAY_30                       (uint16_t)(0x18E3)    //  30  30  30
#define COLOR_GRAY_50                       (uint16_t)(0x3186)    //  50  50  50
#define COLOR_GRAY_80                       (uint16_t)(0x528A)    //  80  80  80
#define COLOR_GRAY_128                      (uint16_t)(0x8410)    // 128 128 128
#define COLOR_GRAY_200                      (uint16_t)(0xCE59)    // 200 200 200
#define COLOR_GRAY_225                      (uint16_t)(0xE71C)    // 225 225 225


} // namespace pokitto
#endif // __HWLCD_H_

