/**************************************************************************/
/*!
    @file     Macros_LCD.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, Jonne Valola
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

#ifndef LCDMACROS_H
#define LCDMACROS_H

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

#define CLR_CD  LPC_GPIO_PORT->CLR[LCD_CD_PORT] = 1 << LCD_CD_PIN; // RS = (0); // Clear pin
#define SET_CD LPC_GPIO_PORT->SET[LCD_CD_PORT] = 1 << LCD_CD_PIN; // RS = (1); // Set pin

#define CLR_WR { LPC_GPIO_PORT->CLR[LCD_WR_PORT] = 1 << LCD_WR_PIN; __asm("nop");}//WR = (0); // Clear pin
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
/**                          OTHER MACROS                                **/
/**************************************************************************/

#define write_command w_cmd_16
#define write_data w_data_16

#define AB_JUMP 1024 // jump one 1-bit Arduboy screen forward to get next color bit
#define GB_JUMP 504 // jump one 1-bit Gamebuino screen forward to get next color bit

#endif // LCDMACROS_H

