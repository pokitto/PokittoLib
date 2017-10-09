/**************************************************************************/
/*!
    @file     Pokitto_settings.h
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


#ifndef POKITTO_SETTINGS_H
#define POKITTO_SETTINGS_H

#include "My_settings.h"

#ifdef PROJ_BOARDREV
 #define POK_BOARDREV PROJ_BOARDREV // which revision of Pokitto board
#else
 #define POK_BOARDREV 2 // default is boardrev 2 (the 4-layer board)
#endif

/** LOGO */
#ifdef PROJ_STARTUPLOGO
 #define POK_DISPLAYLOGO  PROJ_STARTUPLOGO// if enabled, show logo at start
#else
 #define POK_DISPLAYLOGO  1
#endif

#define POK_ENABLE_REFRESHWITHWAIT 0 // choose whether waiting in application refreshes display or not
#define POK_ENABLE_FPSCOUNTER 0 // turn off to save some cpu
#define POK_ENABLE_SD 1 // Define true to include SD library
#define POK_LOADER_COUNTDOWN 3 //how many seconds waiting for C press for loader

#ifndef PROJ_ENABLE_SOUND
    #define POK_ENABLE_SOUND 1
#else
    #define POK_ENABLE_SOUND PROJ_ENABLE_SOUND
#endif

#ifndef PROJ_GBSOUND
    #if POK_ENABLE_SOUND > 0
        #define POK_GBSOUND 0
    #endif
#else
    #define POK_GBSOUND PROJ_GBSOUND
#endif


#ifndef PROJ_STREAMING_MUSIC
    #if POK_ENABLE_SOUND > 0
        #define POK_STREAMING_MUSIC 1 // Define true to stream music from SD
    #endif
#else
    #define POK_STREAMING_MUSIC PROJ_STREAMING_MUSIC
#endif // PROJ_STREAMING_MUSIC

#ifndef PROJ_ENABLE_SYNTH
    #define POK_ENABLE_SYNTH 0
#else
    #define POK_ENABLE_SYNTH PROJ_ENABLE_SYNTH
#endif // PROJ_ENABLE_SYNTH


/** CONSOLE **/
#define POK_USE_CONSOLE 1 //if debugging console is available or not
#define POK_CONSOLE_VISIBLE_AT_STARTUP 1 // whaddaya think ?
#define POK_CONSOLE_INTERVAL 1000 // interval in ms how often console is drawn
#if POK_USE_CONSOLE > 0 // this prevents trying to log messages if console is disabled
    #define POK_CONSOLE_LOG_BUTTONS 0 // if console logs keypresses
    #define POK_CONSOLE_LOG_COLLISIONS 1 // if console logs collisions
#endif // POK_USE_CONSOLE
#define CONSOLEBUFSIZE 20
#define POK_SHOW_VOLUME 0 // volumebar drawn after console if enabled
#define VOLUMEBAR_TIMEOUT 10 // frames before disappearing

/** PROJECT LIBRARY TYPE **/
// Tiled mode can NOT be buffered mode (fast mode, arduboy mode, gamebuino mode etc)
#if PROJ_TILEDMODE > 0
    #define POK_TILEDMODE 1
    #ifdef PROJ_TILEWIDTH
        #define POK_TILE_W PROJ_TILEWIDTH
    #else
        #define POK_TILE_W 11
    #endif // PROJ_TILEWIDTH
    #if POK_TILE_W == 11
        #define POK_TILES_X 20
        #define LCDWIDTH 220
    #elif POK_TILE_W == 12
        #define POK_TILES_X 18
        #define LCDWIDTH 216
    #elif POK_TILE_W == 8
        #define POK_TILES_X 27
        #define LCDWIDTH 216
    #elif POK_TILE_W == 32
        #define POK_TILES_X 6
        #define LCDWIDTH 220
    #elif POK_TILE_W == 10
        #define POK_TILES_X 22
        #define LCDWIDTH 220
    #elif POK_TILE_W == 14
        #define POK_TILES_X 15
        #define LCDWIDTH 210
    #endif
    #ifdef PROJ_TILEHEIGHT
        #define POK_TILE_H PROJ_TILEHEIGHT
    #else
        #define POK_TILE_H 11
    #endif // PROJ_TILEHEIGHT
    #if POK_TILE_H == 11
        #define POK_TILES_Y 16
        #define LCDHEIGHT 176
    #elif POK_TILE_H == 12
        #define POK_TILES_Y 14
        #define LCDHEIGHT 168
    #elif POK_TILE_H == 8
        #define POK_TILES_Y 22
        #define LCDHEIGHT 176
    #elif POK_TILE_H == 32
        #define POK_TILES_Y 5
        #define LCDHEIGHT 176
    #elif POK_TILE_H == 10
        #define POK_TILES_Y 17
        #define LCDHEIGHT 170
    #elif POK_TILE_H == 14
        #define POK_TILES_Y 12
        #define LCDHEIGHT 168
    #endif
#else
#if PROJ_GAMEBUINO > 0
    #define POK_GAMEBUINO_SUPPORT PROJ_GAMEBUINO // Define true to support Gamebuino library calls
    #define PROJ_SCREENMODE MODE_GAMEBUINO_16COLOR
    #define POK_STRETCH 1
    #define PICOPALETTE 0
    #define POK_COLORDEPTH 4
#else
    #if PROJ_ARDUBOY > 0
        #define POK_ARDUBOY_SUPPORT PROJ_ARDUBOY // Define true to support Arduboy library calls
        #define PROJ_SCREENMODE MODE_ARDUBOY_16COLOR
        #define POK_COLORDEPTH 1
        #define POK_STRETCH 1
        #define POK_FPS 20
        #define PICOPALETTE 0
    #else
        #if PROJ_RBOY > 0
            #define PROJ_SCREENMODE MODE_GAMEBUINO_16COLOR
            #define POK_COLORDEPTH 1
            #define POK_STRETCH 0
            #define POK_FPS 40
            #define PICOPALETTE 0
        #else
            #if PROJ_GAMEBOY > 0
            #define PROJ_SCREENMODE MODE_GAMEBOY
            #define POK_COLORDEPTH 2
            #define POK_STRETCH 0
            #define POK_FPS 6
            #define PICOPALETTE 0
            #else
                #define POK_GAMEBUINO_SUPPORT 0
                #define POK_GAMEBOY_SUPPORT 0
                #define POK_ARDUBOY_SUPPORT 0
                #define PICOPALETTE 0
                #define POK_COLORDEPTH 4
            #endif // PROJ_GAMEBOY
        #endif // PROJ_RBOY
    #endif // PROJ_ARDUBOY
#endif // PROJ_GAMEBUINO
#endif // PROJ_TILEDMODE

/** SCREEN MODES TABLE -- DO NOT CHANGE THESE **/

#define POK_LCD_W 220 //<- do not change !!
#define POK_LCD_H 176 //<- do not change !!

#define MODE_NOBUFFER               0   //Size: 0
#define BUFSIZE_NOBUFFER            0
#define MODE_HI_4COLOR              1   //Size: 9680
#define BUFSIZE_HI_4                9680
#define MODE_FAST_16COLOR           2   //Size: 4840
#define BUFSIZE_FAST_16             4840
#define MODE_HI_16COLOR             3
#define BUFSIZE_HI_16               19360
#define MODE_GAMEBUINO_16COLOR      4   //Size: 2016
#define BUFSIZE_GAMEBUINO_16        2016
#define MODE_ARDUBOY_16COLOR        5   //Size: 4096
#define BUFSIZE_ARDUBOY_16          4096
#define MODE_HI_MONOCHROME          6   //Size: 4840
#define BUFSIZE_HI_MONO             4840
#define MODE_HI_GRAYSCALE           7   //Size: 9680
#define BUFSIZE_HI_GS               9680
#define MODE_GAMEBOY                8
#define BUFSIZE_GAMEBOY             5760
#define MODE_UZEBOX                 9
#define MODE_TVOUT                  10
#define MODE_LAMENES                11
#define BUFSIZE_LAMENES             7680
#define MODE_256_COLOR              12
#define BUFSIZE_MODE_12              4176 // 72 x 58
// Tiled modes
#define MODE_TILED_1BIT             1001
#define MODE_TILED_8BIT             1002



/** SCREENMODE - USE THIS SELECTION FOR YOUR PROJECT **/

#if POK_TILEDMODE > 0
    #ifndef PROJ_TILEBITDEPTH
        #define PROJ_TILEBITDEPTH 8 //default tiling mode is 256 color mode!
    #endif // PROJ_TILEBITDEPTH
    #if PROJ_TILEBITDEPTH == 1
        #define POK_SCREENMODE MODE_TILED_1BIT
        #define POK_COLORDEPTH 1
    #else
        #define POK_SCREENMODE MODE_TILED_8BIT
        #define POK_COLORDEPTH 8
    #endif // PROJ_TILEBITDEPTH
#else
#ifndef PROJ_SCREENMODE
    #undef POK_COLORDEPTH
    #ifdef PROJ_HIRES
        #if PROJ_HIRES > 0
            #define POK_SCREENMODE MODE_HI_4COLOR
            #undef POK_COLORDEPTH
            #define POK_COLORDEPTH 2
        #elif PROJ_HICOLOR > 0
            #define POK_SCREENMODE MODE_256_COLOR
            #undef POK_COLORDEPTH
            #define POK_COLORDEPTH 8
        #else
            #define POK_SCREENMODE MODE_FAST_16COLOR
            #undef POK_COLORDEPTH
            #define POK_COLORDEPTH 4
        #endif // PROJ_HIRES
    #else
        #define POK_SCREENMODE MODE_FAST_16COLOR
        #define POK_COLORDEPTH 4
    #endif // PROJ_HIRES
#else
    #define POK_SCREENMODE PROJ_SCREENMODE
#endif
#endif // POK_TILEDMODE

/* DEFINE SCREENMODE AS THE MAXIMUM SCREEN SIZE NEEDED BY YOUR APP ... SEE SIZES LISTED ABOVE */

/** AUTOMATIC COLOR DEPTH SETTING - DO NOT CHANGE **/
#ifndef POK_COLORDEPTH
    #define POK_COLORDEPTH 4 // 1...5 is valid
#endif // POK_COLORDEPTH

/** AUTOMATIC SCREEN BUFFER SIZE CALCULATION - DO NOT CHANGE **/
#if POK_SCREENMODE == 0
    #define POK_SCREENBUFFERSIZE 0
    #define LCDWIDTH POK_LCD_W
    #define LCDHEIGHT POK_LCD_H
    #define POK_BITFRAME 0
#elif POK_SCREENMODE == MODE_HI_MONOCHROME
    #define POK_SCREENBUFFERSIZE POK_LCD_W*POK_LCD_H*POK_COLORDEPTH/8
    #define LCDWIDTH POK_LCD_W
    #define LCDHEIGHT POK_LCD_H
    #define POK_BITFRAME 4840
#elif POK_SCREENMODE == MODE_HI_16COLOR
    #define POK_SCREENBUFFERSIZE POK_LCD_W*POK_LCD_H/2
    #define LCDWIDTH 220
    #define LCDHEIGHT 176
    #define POK_BITFRAME 4840
#elif POK_SCREENMODE == MODE_HI_4COLOR || POK_SCREENMODE == MODE_HI_GRAYSCALE
    #define POK_SCREENBUFFERSIZE POK_LCD_W*POK_LCD_H*POK_COLORDEPTH/4
    #define LCDWIDTH POK_LCD_W
    #define LCDHEIGHT POK_LCD_H
    #define POK_BITFRAME 4840
#elif POK_SCREENMODE == MODE_FAST_16COLOR
    #define POK_SCREENBUFFERSIZE (POK_LCD_W/2)*(POK_LCD_H/2)*POK_COLORDEPTH/8
    #define XCENTER POK_LCD_W/4
    #define YCENTER POK_LCD_H/4
    #define LCDWIDTH 110
    #define LCDHEIGHT 88
    #define POK_BITFRAME 1210
#elif POK_SCREENMODE == MODE_256_COLOR
    #define POK_SCREENBUFFERSIZE 72*58
    #define XCENTER 36
    #define YCENTER 29
    #define LCDWIDTH 72
    #define LCDHEIGHT 58
    #define POK_BITFRAME 72*58
#elif POK_SCREENMODE == MODE_GAMEBUINO_16COLOR
    #define POK_SCREENBUFFERSIZE (84/2)*(48/2)*POK_COLORDEPTH/8
    #define LCDWIDTH 84
    #define LCDHEIGHT 48
    #define POK_BITFRAME 504
#elif POK_SCREENMODE == MODE_ARDUBOY_16COLOR
    #define POK_SCREENBUFFERSIZE (128/2)*(64/2)*POK_COLORDEPTH/8
    #define LCDWIDTH 128
    #define LCDHEIGHT 64
    #define POK_BITFRAME 1024
#elif POK_SCREENMODE == MODE_LAMENES
    #define POK_SCREENBUFFERSIZE (128)*(120)*POK_COLORDEPTH/8
    #define LCDWIDTH 128
    #define LCDHEIGHT 120
    #define POK_BITFRAME 1210
#elif POK_SCREENMODE == MODE_GAMEBOY
    #define POK_SCREENBUFFERSIZE (160)*(144)/4
    #define LCDWIDTH 160
    #define LCDHEIGHT 144
    #define POK_BITFRAME 2880
#else
    #define POK_SCREENBUFFERSIZE 0
#endif // POK_SCREENMODE

#ifndef POK_STRETCH
    #define POK_STRETCH 1 // Stretch Gamebuino display
#endif
#ifndef POK_FPS
    #define POK_FPS 20
#endif
#define POK_FRAMEDURATION 1000/POK_FPS

/** SCROLL TEXT VS. WRAP AROUND WHEN PRINTING **/
#define SCROLL_TEXT 1

/** AUDIO **/
#define POK_AUD_PIN P2_19
#define POK_AUD_PWM_US 31
#define POK_AUD_FREQ 11025 //16000 //14285 //24000 // 14285 // 57143 // 8000 //11025// audio update frequency in Hz
#define POK_CINC_MULTIPLIER 2 // multiplies synth cycle table
#define POK_STREAMFREQ_HALVE  0  // if true, stream update freq is half audio freq
#define POK_STREAM_LOOP 1 //master switch

#define POK_USE_DAC 1 // is DAC in use in this project
#define POK_USE_EXT 0 // if extension port is in use or not
#define POK_STREAM_TO_DAC 1  // 1 = stream from SD to DAC, synthesizer to PWM,  0 = opposite


#define POK_BACKLIGHT_PIN P2_2
#define POK_BACKLIGHT_INITIALVALUE 0.3f

#define POK_BATTERY_PIN1 P0_22 // read battery level through these pins
#define POK_BATTERY_PIN2 P0_23

#define POK_BTN_A_PIN   P1_9
#define POK_BTN_B_PIN   P1_4
#define POK_BTN_C_PIN   P1_10
#define POK_BTN_UP_PIN   P1_13
#define POK_BTN_DOWN_PIN   P1_3
#define POK_BTN_LEFT_PIN   P1_25
#define POK_BTN_RIGHT_PIN   P1_7

#define UPBIT    0
#define DOWNBIT  1
#define LEFTBIT  2
#define RIGHTBIT 3
#define ABIT     4
#define BBIT     5
#define CBIT     6

#endif // POKITTO_SETTINGS_H

