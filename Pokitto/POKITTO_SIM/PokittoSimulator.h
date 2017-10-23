/**************************************************************************/
/*!
    @file     PokittoSimulator.h
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

#ifndef POKITTOSIM_H
#define POKITTOSIM_H

#include <stdint.h>
#include <SDL2/SDL.h>

#include "Pokitto_settings.h"
#include "SimLCD.h"

#define SIMW POK_LCD_W
#define SIMH POK_LCD_H
#define AB_JUMP 1024 // jump one 1-bit Arduboy screen forward to get next color bit
#define GB_JUMP 504 // jump one 1-bit Gamebuino screen forward to get next color bit

/** SETTINGS FOR SIMULATOR **/
#define LCD_OPAQUENESS 190
#define FLICKER_AMOUNT 10
#define SIM_SDL_AUDIO   1       // nonzero = initialize SDL audio
#define SIM_FULLSCREEN  0       // nonzero = run in fullscreen. WARNING ! Debug may hang !
#define SIM_SHOWDEVICE  0       // nonzero = show Pokitto device in simulation
#define SCREENCAPTURE   0      // nonzero = the nth frame will be written to disk
#define SOUNDCAPTURE    0       // nonzero = continuous capture of sound stream to disk
#define USE_JOYSTICK    0       // check for presence & use a game controller (analog stick is not supported yet)
#define MAKE_GIF        0       // nonzero = make a gif using ImageMagick Convert

/** INTERNAL SETTINGS, DO NOT CHANGE UNLESS YOU UNDERSTAND WHAT YOU'RE DOING **/
//#if SCREENCAPTURE > 1
//#define SFBUFSIZE       POK_AUD_FREQ/POK_FRAMEDURATION // sound capture buffer size (default: 10000)
//#else
//#define SFBUFSIZE       POK_AUD_FREQ/POK_FRAMEDURATION // sound capture buffer size (default: 10000)
//#endif // SCREENCAPTURE
#define SFBUFSIZE       1000
#define SAMPLE_RATE     POK_AUD_FREQ
#define NUMFRAMES       570               //1 ms before refresh

/** OPTIONS BASED ON PREVIOUS SETTINGS **/
#if SIM_FULLSCREEN > 0
    #define SCREENCAPTURE 0 // Screencapture will crash because writing full resolution is too much data at once
#endif // SIM_FULLSCREEN
#if USE_JOYSTICK > 0
    #include <SDL2/SDL_joystick.h>
#endif // USE_JOYSTICK


namespace Pokitto {

/** Sim class.
 *  The Sim class is a class that allows Pokitto programs to compile to a PC+SDL target.
 *  It simulates the hardware functions of the Pokitto.
 *  The intention is ,that there exists only 1 Sim instance in the application.
 */

class Simulator
{
public:
  /** Create Runtime instance */
  Simulator();
  /** Initialize graphics simulation */
  void initSDLGfx();
  /** Initialize SDL audio */
  int initSDLAudio();
  /** Quit simulation */
  static void quit();
  /** Indicate status of simulator */
  static int isRunning();
  /** Refresh the output (simulated LCD and device) to SDL window*/
  static void refreshDisplay();
  /** Poll for button events */
  static void pollButtons();
  /** Wait using SDL_Delay */
  static void waitSDL(uint16_t);
  /** Block concurrent display refreshes */
  static bool mutex;
  /** Clean up after simulator (screen capture and so */
  static void CleanUp();
  /** Stop permanently sound callback routine from running */
  static void killSound();
  /** Wait until milliseconds have passed */
  static void wait_ms(uint16_t);

  /** syncing sound capture and screen capture **/
  static uint32_t videotime;
  static uint32_t videoframesize;
  static uint32_t audiotime;
  static uint32_t audioframesize;

  void simSoundEnabled(int);
  int simSoundIsOn();

private:
  /** Runtime control */
  static int simRunning;
  static uint8_t cleaned; // used for cleaning up screen shots objects at shutdown


  /** SDL graphics objects */
  static SDL_Window *sdlSimWin;
  static SDL_Renderer *sdlBgRen;
  static SDL_Renderer *sdlRen;
  static SDL_Texture *sdlTex;
  static SDL_Rect SrcR;
  static SDL_Rect DestR;
  static SDL_Event sdlEvent;
  static SDL_Surface* Loading_Surf;
  static SDL_Texture* Background_Tx;
  static uint8_t lcdpixels[]; //used to create SDL texture of LCD screen

  /** other graphics variables */
public:
  static int ww,wh;
  static uint16_t gfxbuf[]; // simulated dram buffer of the LCD screen
  static uint16_t *dramptr; // points to current pixel in simulated DRAM
  static uint16_t datalines; // simulates the 16-bit datalines to the LCD
  static uint16_t prevdata; // if data does not change, do not adjust LCD bus lines
  static uint8_t lcd_shaking; // legacy variable from rboy (screen offset shake)

  /** screen capture related */
private:
  static uint16_t sc_count; // counter to decide at which intervals a screen capture is taken
  static uint16_t framenumber; // framenumber is the number of the frame
  static uint16_t audframenumber; // framenumber is the number of the frame

  /** button input related */
  static uint8_t buttons_state;
  static uint8_t buttons_held;
  static uint8_t buttons_released; // from LSB up,down,left,right,a,b,c
public:
  static uint8_t leftBtn();
  static uint8_t rightBtn();
  static uint8_t upBtn();
  static uint8_t downBtn();
  static uint8_t aBtn();
  static uint8_t bBtn();
  static uint8_t cBtn();
  static uint8_t leftHeld();
  static uint8_t rightHeld();
  static uint8_t upHeld();
  static uint8_t downHeld();
  static uint8_t aHeld();
  static uint8_t bHeld();
  static uint8_t cHeld();
  static uint8_t leftReleased();
  static uint8_t rightReleased();
  static uint8_t upReleased();
  static uint8_t downReleased();
  static uint8_t aReleased();
  static uint8_t bReleased();
  static uint8_t cReleased();

  /** Joystick related */
private:
  static SDL_GameController* gGameController;
public:
  static bool joystickPresent;

  /** Audio related */
public:
  static uint8_t amp;


  /** Graphics related */
public:
  void directSDLPixel(int16_t, int16_t, uint16_t);
  void directSDLRectangle(int16_t, int16_t,int16_t, int16_t, uint16_t);
  void fillSDLSurface(uint16_t);
public:


};

}

// this is the simulator instance used by the core
extern Pokitto::Simulator simulator;

#endif // POKITTOSIM_H

