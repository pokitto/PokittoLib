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

#pragma once
#include <stdint.h>

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

#if !defined(PROJ_BUTTONS_POLLING)
    #define PROJ_BUTTONS_POLLING 1
#endif

#ifndef PROJ_FLIP_SCREEN
    #define POK_FLIP_SCREEN 0
#else
    #define POK_FLIP_SCREEN PROJ_FLIP_SCREEN
#endif


#ifndef PROJ_ENABLE_SOUND
    #define POK_ENABLE_SOUND 1
#else
    #define POK_ENABLE_SOUND PROJ_ENABLE_SOUND
#endif

#ifndef PROJ_SOUND_BUFFERED
    #define POK_SOUND_BUFFERED 0
#else
    #define POK_SOUND_BUFFERED PROJ_SOUND_BUFFERED
#endif

#ifndef PROJ_GBSOUND
    #if POK_ENABLE_SOUND > 0
        #define POK_GBSOUND 0
    #endif
#else
    #define POK_GBSOUND PROJ_GBSOUND
    //#define NUM_CHANNELS 2
#endif

#ifdef PROJ_ENABLE_SFX
	#ifndef PROJ_ENABLE_SOUND
		#define PROJ_ENABLE_SOUND 1
	#endif

	#ifndef PROJ_STREAMING_MUSIC
		#define PROJ_STREAMING_MUSIC 1
	#endif

	#ifndef PROJ_ENABLE_SD_MUSIC
		#define PROJ_DISABLE_SD_STREAMING 1
	#endif
#endif

#ifdef PROJ_ENABLE_SD_MUSIC
	#ifndef PROJ_ENABLE_SOUND
		#define PROJ_ENABLE_SOUND 1
	#endif

	#ifndef PROJ_STREAMING_MUSIC
		#define PROJ_STREAMING_MUSIC 1
	#endif
#endif

#ifndef PROJ_STREAMING_MUSIC
        #define POK_STREAMING_MUSIC 0 // Define true to stream music from SD
#else
    #define POK_STREAMING_MUSIC PROJ_STREAMING_MUSIC
#endif // PROJ_STREAMING_MUSIC

#ifndef PROJ_ENABLE_SYNTH
    #define POK_ENABLE_SYNTH 0
#else
    #define POK_ENABLE_SYNTH PROJ_ENABLE_SYNTH
#endif // PROJ_ENABLE_SYNTH

#define HIGH_RAM_OFF     0 // SRAM1/SRAM2 are at the default setting
#define HIGH_RAM_ON      1 // SRAM1/SRAM2 are enabled and free for use
#define HIGH_RAM_MUSIC   2 // SRAM1/SRAM2 are enabled and used by music

#ifndef PROJ_HIGH_RAM
    #define POK_HIGH_RAM HIGH_RAM_OFF
#else
    #define POK_HIGH_RAM PROJ_HIGH_RAM
#endif

/** CONSOLE **/
#define POK_USE_CONSOLE 0 //if debugging console is available or not
#define POK_CONSOLE_VISIBLE_AT_STARTUP 1 // whaddaya think ?
#define POK_CONSOLE_INTERVAL 1000 // interval in ms how often console is drawn
#if POK_USE_CONSOLE > 0 // this prevents trying to log messages if console is disabled
    #define POK_CONSOLE_LOG_BUTTONS 0 // if console logs keypresses
    #define POK_CONSOLE_LOG_COLLISIONS 1 // if console logs collisions
#endif // POK_USE_CONSOLE
#define CONSOLEBUFSIZE 20
#define POK_SHOW_VOLUME 0 // volumebar drawn after console if enabled
#define VOLUMEBAR_TIMEOUT 10 // frames before disappearing

/** SCREEN CONFIGURATION **/
#ifndef PROJ_PERSISTENCE
    #define PROJ_PERSISTENCE true
#endif
inline constexpr bool POK_PERSISTENCE = PROJ_PERSISTENCE;

#ifndef PROJ_CLEAR_SCREEN
    #define PROJ_CLEAR_SCREEN 0
#endif
inline constexpr uint32_t POK_CLEAR_SCREEN = PROJ_CLEAR_SCREEN;

#ifndef PROJ_FPS
    #define PROJ_FPS 60
#endif
inline constexpr uint32_t POK_FPS = PROJ_FPS;
inline constexpr uint32_t POK_FRAMEDURATION = 1000 / PROJ_FPS;

inline constexpr uint32_t R_MASK = 0xF800;
inline constexpr uint32_t G_MASK = 0x07E0;
inline constexpr uint32_t B_MASK = 0x001F;

inline constexpr uint32_t POK_LCD_W = 220;
inline constexpr uint32_t POK_LCD_H = 176;

#define MODE_NOBUFFER            0
#define MODE_HI_4COLOR           1
#define MODE_FAST_16COLOR        2
#define MODE13                   13
#define MODE15                   15
#define MIXMODE                  32
#define TASMODE                  42
#define TASMODELOW               43
#define MODE64                   64

/** SCREENMODE - USE THIS SELECTION FOR YOUR PROJECT **/
#ifndef PROJ_SCREENMODE
    #if defined(PROJ_HIRES) &&  PROJ_HIRES > 0
        #define PROJ_SCREENMODE MODE_HI_4COLOR
    #elif defined(PROJ_HICOLOR) && PROJ_HICOLOR > 0
        #define PROJ_SCREENMODE MODE13
    #elif defined(PROJ_TASMODE) && PROJ_TASMODE > 0
        #define PROJ_SCREENMODE TASMODE
    #elif defined(PROJ_TASMODELOW) && PROJ_TASMODELOW > 0
        #define PROJ_SCREENMODE TASMODELOW
    #elif defined(PROJ_MODE13) && PROJ_MODE13 > 0
        #define PROJ_SCREENMODE MODE13
    #elif defined(PROJ_MODE15) && PROJ_MODE15 > 0
        #define PROJ_SCREENMODE MODE15
    #elif defined(PROJ_MIXMODE) && PROJ_MIXMODE > 0
        #define PROJ_SCREENMODE MIXMODE
    #elif defined(PROJ_MODE64) && PROJ_MODE64 > 0
        #define PROJ_SCREENMODE MODE64
    #else
        #define PROJ_SCREENMODE MODE_FAST_16COLOR
    #endif
#endif // POK_TILEDMODE

#if PROJ_SCREENMODE == MODE_NOBUFFER
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_NOBUFFER
    #define PROJ_COLORDEPTH 8
#endif

#if PROJ_SCREENMODE == TASMODELOW
    #undef PROJ_SCREENMODE
    #define PROJ_SCREENMODE TASMODE
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_TASMODELOW
    #define PROJ_LCDWIDTH 110
    #define PROJ_LCDHEIGHT 88
#endif

#if PROJ_SCREENMODE == TASMODE
    #ifndef PROJ_SCREENBUFFERSIZE
        #define PROJ_SCREENBUFFERSIZE BUFSIZE_TASMODE
    #endif
    #define PROJ_COLORDEPTH 8
    #ifndef PROJ_MAX_SPRITES
        #define PROJ_MAX_SPRITES 100
    #endif
    #ifndef PROJ_TILE_W
        #define PROJ_TILE_W 16
    #endif // PROJ_TILEWIDTH
    #ifndef PROJ_TILE_H
        #define PROJ_TILE_H 16
    #endif // PROJ_TILEHEIGHT
    #if !defined(PROJ_LCDHEIGHT)
        #define PROJ_LCDWIDTH 220
        #define PROJ_LCDHEIGHT 176
    #endif
    #if !defined(PROJ_LINE_FILLERS)
        #define PROJ_LINE_FILLERS       \
                    TAS::BGTileFiller,  \
                    TAS::SpriteFiller,  \
                    TAS::NOPFiller,     \
                    TAS::NOPFiller
    #endif
    inline constexpr uint32_t POK_TILE_W = PROJ_TILE_W;
    inline constexpr uint32_t POK_TILE_H = PROJ_TILE_H;
#endif

#if PROJ_SCREENMODE == MODE_FAST_16COLOR
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_FAST_16COLOR
    #define PROJ_COLORDEPTH 4
    #define PROJ_LCDWIDTH 110
    #define PROJ_LCDHEIGHT 88
#endif

#if PROJ_SCREENMODE == MODE13
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_MODE13
    #define PROJ_COLORDEPTH 8
    #define PROJ_LCDWIDTH 110
    #define PROJ_LCDHEIGHT 88
#endif

#if PROJ_SCREENMODE == MODE15
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_MODE15
    #define PROJ_COLORDEPTH 4
#endif

#if PROJ_SCREENMODE == MIXMODE
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_MIXMODE
    #define PROJ_COLORDEPTH 8
    #define PROJ_LCDWIDTH 110
    #define PROJ_LCDHEIGHT 88
#endif

#if PROJ_SCREENMODE == MODE64
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_MODE64
    #define PROJ_COLORDEPTH 8
    #define PROJ_LCDWIDTH 110
    #define PROJ_LCDHEIGHT 176
#endif

#if PROJ_SCREENMODE == MODE_HI_4COLOR || !defined(PROJ_SCREENBUFFERSIZE)
    #define PROJ_SCREENBUFFERSIZE BUFSIZE_HI_4COLOR
    #define PROJ_COLORDEPTH 2
#endif

#ifndef PROJ_LCDWIDTH
    #define PROJ_LCDWIDTH POK_LCD_W
#endif
inline constexpr uint32_t LCDWIDTH = PROJ_LCDWIDTH;

#ifndef PROJ_LCDHEIGHT
    #define PROJ_LCDHEIGHT POK_LCD_H
#endif
inline constexpr uint32_t LCDHEIGHT = PROJ_LCDHEIGHT;

/** AUTOMATIC COLOR DEPTH SETTING - DO NOT CHANGE **/
#ifndef PROJ_COLORDEPTH
    #define PROJ_COLORDEPTH 4
#endif // PROJ_COLORDEPTH
inline constexpr uint32_t POK_COLORDEPTH = PROJ_COLORDEPTH;

inline constexpr uint32_t BUFSIZE_NOBUFFER = 0;
inline constexpr uint32_t BUFSIZE_TASMODE = 220;
inline constexpr uint32_t BUFSIZE_TASMODELOW = 110;
inline constexpr uint32_t BUFSIZE_HI_4COLOR = ((POK_LCD_H+1)*POK_LCD_W)*POK_COLORDEPTH/8;
inline constexpr uint32_t BUFSIZE_FAST_16COLOR = (((POK_LCD_H/2)+1)*POK_LCD_W/2)*POK_COLORDEPTH/8;
inline constexpr uint32_t BUFSIZE_MODE13 = 110*88;
inline constexpr uint32_t BUFSIZE_MODE15 = 0x4BA0;
inline constexpr uint32_t BUFSIZE_MODE64 = 220*88;
inline constexpr uint32_t BUFSIZE_MIXMODE = 110*88;

#ifndef PROJ_SCREENBUFFERSIZE
    #define PROJ_SCREENBUFFERSIZE 0
#endif
inline constexpr uint32_t POK_SCREENBUFFERSIZE = PROJ_SCREENBUFFERSIZE;

inline constexpr uint32_t XCENTER = LCDWIDTH / 2;
inline constexpr uint32_t YCENTER = LCDHEIGHT / 2;

#ifndef PROJ_PALETTE_SIZE
    #define PROJ_PALETTE_SIZE 1<<PROJ_COLORDEPTH
#endif
#if (PROJ_SCREENMODE == MIXMODE)
	#define PROJ_PALETTE_SIZE 276;
#endif

inline constexpr uint32_t PALETTE_SIZE = PROJ_PALETTE_SIZE;

/** SCROLL TEXT VS. WRAP AROUND WHEN PRINTING **/
#if PROJ_NO_AUTO_SCROLL
#define SCROLL_TEXT 0
#else
#define SCROLL_TEXT 1
#endif

/** AUDIO **/

#define POK_ALT_MIXING 1 // NEW! alternative more accurate mixing, uses more CPU

#define POK_AUD_PIN P2_19
#define POK_AUD_PWM_US 15 //31 //Default value 31
#ifndef PROJ_AUD_FREQ
    #define POK_AUD_FREQ 22050 //Valid values: 8000, 11025, 16000, 22050 // audio update frequency in Hz
#else
    #define POK_AUD_FREQ PROJ_AUD_FREQ
#endif

#if PROJ_AUD_TRACKER > 0
    #define POK_AUD_TRACKER 1
#endif

#define POK_USE_EXT 0 // if extension port is in use or not

#define POK_STREAMFREQ_HALVE  0  // if true, stream update freq is half audio freq
#ifdef PROJ_STREAM_LOOP
#define POK_STREAM_LOOP PROJ_STREAM_LOOP //master switch
#else
#define POK_STREAM_LOOP 1 //loop by default
#endif

#ifndef PROJ_USE_DAC
    #define POK_USE_DAC 1 // is DAC in use in this project
#else
    #define POK_USE_DAC PROJ_USE_DAC
#endif
#ifndef PROJ_USE_PWM
    #define POK_USE_PWM 1 // is PWM for audio used in this project
#else
    #define POK_USE_PWM PROJ_USE_PWM
#endif

#ifndef PROJ_STREAM_TO_DAC
    #define POK_STREAM_TO_DAC 1  // 1 = stream from SD to DAC, synthesizer to PWM,  0 = opposite
#else
    #define POK_STREAM_TO_DAC PROJ_STREAM_TO_DAC
#endif


#define POK_BACKLIGHT_PIN P2_2
#define POK_BACKLIGHT_INITIALVALUE 30 // 30%

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

/** ALLOW PWMOUT TO EXT0 (SOLVE CONFLICT WITH BACKLIGHT **/

#define POK_EXT0_PWM_ENABLE 1

/** LOADER UPDATE MECHANISM **/
#define POK_ENABLE_LOADER_UPDATES 1 //1=check for new loader versions on SD and update if new found

#ifndef SPRITE_COUNT
#define SPRITE_COUNT 4  // The default max sprite count
#endif

/** SYSTEM SETTINGS ADDRESSES IN EEPROM **/
#define EESETTINGS_FILENAME         3980 // 0xF8C 20bytes last filename requested
#define EESETTINGS_VOL              4000 // 0xFA0 Volume
#define EESETTINGS_DEFAULTVOL       4001 // 0xFA1 Default volume
#define EESETTINGS_LOADERWAIT       4002 //	0xFA2 Loader wait in sec
#define EESETTINGS_VOLWAIT          4003 // 0xFA3 Volume screen wait in sec
#define EESETTINGS_TIMEFORMAT       4004 // 0xFA4 Time format (0=24 hrs, 1 = 12 hrs)
#define EESETTINGS_LASTHOURSSET     4005 // 0xFA5 Last time set in hours
#define EESETTINGS_LASTMINUTESSET   4006 // 0xFA6 Last time set in minutes
#define EESETTINGS_DATEFORMAT       4007 // 0xFA7 Date format (0=D/M/Y, 1 = M/D/Y)
#define EESETTINGS_LASTDAYSET       4008 // 0xFA8 Last Day set
#define EESETTINGS_LASTMONTHSET     4009 // 0xFA9 Last Month set
#define EESETTINGS_LASTYEARSET      4010 // 0xFAA Last Year set (counting from 2000)
#define EESETTINGS_RTCALARMMODE     4011 // 0xFAB RTC alarm mode (0=disabled, 1=enabled, 3 = enabled with sound)
#define EESETTINGS_RESERVED         4012 // 0xFAC 4bytes reserved (additional sleep configuration)
#define EESETTINGS_WAKEUPTIME       4016 // 0xFB0 Wake-up time as 32bit value for 1Hz RTC clock
#define EESETTINGS_SKIPINTRO	    4020 // 0xFB4 Show Logo (0-Yes, 1-No, 2-Yes then switch to 0, 3-No, then switch to 1)

/** USB SERIAL PORT **/

#ifndef PROJ_VENDOR_ID
#define POK_VENDOR_ID 0x04D8
#define POK_PRODUCT_ID 0x000A
#endif



