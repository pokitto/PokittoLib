/**************************************************************************/
/*!
    @file     PokittoDisplay.h
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



/* THE SEGMENT BELOW PERTAINS TO CIRCLE DRAWING FUNCTIONS ONLY
*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).
Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
Copyright (c) 2013 Adafruit Industries.  All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef POKITTODISPLAY_H
#define POKITTODISPLAY_H

#include <stdint.h>
#include "Pokitto_settings.h"
#include "PokittoGlobs.h"
#include "PokittoFonts.h"
#include "PokittoPalettes.h"

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

/** The tables below are palettes, that resemble the Pico 8 palette*/

enum defcolors {
    C_BLACK,
    C_DARKBLUE,
    C_PURPLE,
    C_DARKGREEN,

    C_BROWN,
    C_DARKBROWN,
    C_LIGHTGRAY,
    C_WHITE,

    C_RED,
    C_ORANGE,
    C_YELLOW,
    C_GREEN,

    C_BLUE,
    C_DARKGRAY,
    C_PINK,
    C_PEACH
};

const uint16_t def565palette[16] = {
    //kind of like pico8 palette
    0x0000, 0x194a, 0x792a, 0x042a,
    0xaa86, 0x5aa9, 0xc618, 0xff9d,
    0xf809, 0xfd00, 0xff84, 0x072a,
    0x2d7f, 0x83b3, 0xfbb5, 0xfe75
};

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if PROJ_SCREENMODE == TASMODE
#include "TASMODE.h"
#endif

namespace Pokitto {

class Display {
public:
    static uint8_t m_colordepth;
    static uint8_t palOffset;
    static uint8_t width;
    static uint8_t height;
    #if PROJ_SCREENMODE != TASMODE
    static uint8_t screenbuffer[];
    #endif
    #if (PROJ_SCREENMODE == MIXMODE)
        static uint8_t subMode; // for mixed mode switching
        static uint8_t scanType[]; // for mixed screen mode
    #endif

    // PROPERTIES
    static void setColorDepth(uint8_t);
    static uint8_t getColorDepth();
    static uint8_t getBitsPerPixel();
    static uint16_t getWidth();
    static uint16_t getHeight();
    static uint32_t getNumberOfColors();

    // IMPORTANT PUBLIC STATE MEMBERS
    /** Selected font */
    static const unsigned char * font;
    /** Set if screen is cleared between updates or not*/
    static uint8_t persistence;
    /** Selected drawing color */
    static uint16_t color;
    /** Selected background color */
    static uint16_t bgcolor;
    /** Selected invisible color */
    static uint16_t invisiblecolor;
    /** Direct unbuffered color */
    static uint16_t directcolor;
    /** Direct unbuffered background color */
    static uint16_t directbgcolor;
    /** Direct text rotated */
    static bool directtextrotated;
    /** set color with a command */
    static void setColor(uint8_t);
    /** set color and bgcolor with a command */
    static void setColor(uint8_t,uint8_t);
    /** set invisiblecolor with a command */
    static void setInvisibleColor(uint16_t);
    /** get color */
    static uint8_t getColor();
    /** get background color */
    static uint8_t getBgColor();
    /** get invisible color */
    static uint16_t getInvisibleColor();
    /** Initialize display */
    static void begin();
    /** Clear display buffer */
    static void clear();
    /** Scroll by x lines */
    static void scroll(int16_t);
    /** Fill display buffer */
    static void fillScreen(uint16_t);
    /** Send display buffer to display hardware */
    static void update(bool useDirectMode=false);
    /** Forced update of LCD display memory with a given pixel buffer */
    static void lcdRefresh(const unsigned char*, bool useDirectMode=false);
    /** Clear LCD hardware memory */
    static void clearLCD();
    /** Fill LCD hardware memory */
    static void fillLCD(uint16_t);
    /** Show Pokitto logo at startup*/
    static void showLogo();
    /** Point to another screenbuffer instead of the default one */
    static void setFrameBufferTo(uint8_t*);

    // COLORS AND PALETTE
    /** set default palette */
    static void setDefaultPalette();
    /** master palette */
    static uint16_t palette[PALETTE_SIZE];
    /** runtime palette pointer */
    static uint16_t *paletteptr;
    /** convert RGB to 565 color value */
    static uint16_t RGBto565(uint8_t,uint8_t,uint8_t);
    /** linear interpolation between colors */
    static uint16_t interpolateColor(uint16_t, uint16_t, uint8_t);
    /** load an R,G,B triplet palette */
    static void loadRGBPalette(const unsigned char*);
    /** load a ready-made 565 palette */
    static void load565Palette(const uint16_t*);
    /** rotate palette by step */
    static void rotatePalette(int8_t);
    /** tween between two palettes **/
    static void tweenPalette(uint16_t*, const uint16_t*, const uint16_t*, uint8_t);

    // DIRECT DRAWING (NO BUFFERING)
    /** Direct pixel (not through display buffer) */
    static void directPixel(int16_t,int16_t,uint16_t);
    /** Direct tile 16bit (not through display buffer) */
    static void directTile(int16_t x, int16_t y, int16_t x2, int16_t y2, uint16_t* gfx);
    /** Direct rectangle (not through display buffer) */
    static void directRectangle(int16_t, int16_t,int16_t, int16_t, uint16_t);
    /** Set the cursor for printing to a certain screen position */
    static void setCursor(int16_t,int16_t);
    /** direct bitmap to screen (no buffering) */
    static void directBitmap(int16_t,int16_t,const uint8_t*, uint8_t,uint8_t);


    // DRAWING METHODS
    /** Draw pixel at various bit depths */
    static void drawPixel(int16_t,int16_t);
    /** Draw pixel with specific color index at various bit depths */
    static void drawPixel(int16_t x,int16_t y, uint8_t col);
    /** Draw pixel with specific color index at various bit depths with no bounds/transparency checks*/
    static void drawPixelRaw(int16_t x,int16_t y, uint8_t col);
    /** Placeholder that does nothing */
    static void drawPixelNOP(int16_t x,int16_t y, uint8_t col);

    /** Get pixel at various bit depths */
    static uint8_t getPixel(int16_t,int16_t);
    /** Draw line **/
    static void drawLine(int16_t,int16_t,int16_t,int16_t);
    /** Clip line with screen boundaries, returns 0 if whole line is out of bounds */
    static uint8_t clipLine(int16_t*, int16_t*, int16_t*, int16_t*);
    /** Draw a column real fast */
    static void drawColumn(int, int, int);
    /** Map a 1-bit column real fast */
    static void map1BitColumn(int16_t, int16_t, int16_t, const uint8_t*, uint16_t);
    /** Draw a row real fast */
    static void drawRow(int, int, int);
    /** Legacy drawColumn name, for compatibility - macros are not OK because of scope problems */
    static void drawFastVLine(int16_t, int16_t, int16_t);
    /** Legacy drawRow name, for compatibility - macros are not OK because of scope problems */
    static void drawFastHLine(int16_t, int16_t, int16_t);
    /** Draw rectangle (edges only) */
    static void drawRectangle(int,int,int,int);
    /** Fill rectangle */
    static void fillRectangle(int,int,int,int);
    /** GB compatibility fillRect */
    static void fillRect(int16_t x, int16_t y, int16_t w, int16_t h);
    /** GB compatibility drawRect */
    static void drawRect(int16_t x, int16_t y, int16_t w, int16_t h);

    // Functions lifted from Adafruit GFX library (see PokittoDisplay.h for license //
    /** Draw circle */
    static void drawCircle(int16_t x0, int16_t y0, int16_t r);
    /** Draw circle helper */
    static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername);
    /** Fill circle */
    static void fillCircle(int16_t x0, int16_t y0, int16_t r);
    /** Fill circle helper*/
    static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername, int16_t delta);
    /** draw triangle */
    static void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    /** Fill triangle*/
    static void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    /** Draw rounded rectangle */
    static void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius);
    /** Fill rounded rectangle */
    static void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius);

    // BITMAPS !
    #if PROJ_SCREENMODE == TASMODE
    static inline TAS::LineFiller lineFillers[] = {
        PROJ_LINE_FILLERS
    };

    static void setTASRowMask(uint32_t mask){
        TASMask = ~mask;
    }
    /** Draw tile in Tiles-and-Sprites mode. X&Y are in tile-space, not pixels. */
    static void drawTile(uint32_t x, uint32_t y, const uint8_t *data);
    /** Draw a solid-color tile in Tiles-and-Sprites mode. X&Y are in tile-space, not pixels. */
    static void drawColorTile(uint32_t x, uint32_t y, uint8_t color);
    /** Draw sprite in Tiles-And-Sprites mode */
    static void drawSprite(int x, int y, const uint8_t *data, bool flipped=0, bool mirrored=0, uint8_t recolor=0);
    static void drawSpriteBitmap(int x, int y, int width, int height, const uint8_t *data, bool flipped=0, bool mirrored=0, uint8_t recolor=0);
    static void shiftTilemap(int x, int y);
    #endif

    /** Draw monochromatic bitmap. Used in font rendering */
    static void drawMonoBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t index);
    /** Draw bitmap data*/
    static void drawBitmapData(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap);
    /** Draw bitmap */
    static void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw RLE bitmap */
    static void drawRleBitmap(int16_t x, int16_t y, const uint8_t* bitmap);

    /// \brief
    /// Draws a single frame of a multi-frame bitmap
    /// \param x The x coordinate to draw the bitmap frame at
    /// \param y The y coordinate to draw the bitmap frame at
    /// \param bitmap The multi-frame bitmap whose frame is to be drawn
    /// \param frameIndex The index of the frame to be drawn
    /// \details
    /// A multi-frame bitmap is expected to be in a particular format.
    /// The 0th byte of the bitmap should be the width of the bitmap's frames.
    /// The 1st byte of the bitmap should be the height of the bitmap's frames.
    /// The remaining bytes should consist of the frames of the multi-frame bitmap,
    /// stored one after another without any kind of separator or terminator.
    ///
    /// Example bitmap:
    /// \code{.cpp}
    /// #pragma once
    /// 
    /// #include <cstdint>
    /// 
    /// // An example bitmap, in 4bpp mode
    /// const std::uint8_t exampleBitmap[] =
    /// {
    /// 	// Width, Height
    /// 	8, 8,
    /// 	// Frame 0
    /// 	0x11, 0x11, 0x11, 0x11,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x10, 0x00, 0x00, 0x01,
    /// 	0x11, 0x11, 0x11, 0x11,
    /// 	// Frame 1
    /// 	0x22, 0x22, 0x22, 0x22,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x20, 0x00, 0x00, 0x02,
    /// 	0x22, 0x22, 0x22, 0x22,
    /// };
    /// \endcode
    static void drawBitmap(int16_t x, int16_t y, const uint8_t * bitmap, uint8_t frameIndex);

    /** Draw bitmap data flipped on x-axis*/
    static void drawBitmapDataXFlipped(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap);
    /** Draw bitmap data flipped on y-axis*/
    static void drawBitmapDataYFlipped(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap);
    /** Draw bitmap flipped on x-axis*/
    static void drawBitmapXFlipped(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw bitmap flipped on y-axis*/
    static void drawBitmapYFlipped(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw bitmap with options */
    static void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);
    /** Get pointer to the screen buffer - GB compatibility */
    static uint8_t* getBuffer();
    /** Optimized functions for drawing bit columns - used in raytracing */
    static void draw4BitColumn(int16_t x, int16_t y, uint8_t h, uint8_t* bitmap);
    /** Get pixel in a monochromatic bitmap - GB compatibility */
    static uint8_t getBitmapPixel(const uint8_t*, uint16_t, uint16_t);

    // PRINTING
    /** direct character to screen (no buffering) */
    static int directChar(int16_t, int16_t, uint16_t);
    /** character to screenbuffer */
    static int bufferChar(int16_t, int16_t, uint16_t);
    /** set the active font */
    static void setFont(const unsigned char * f);
    /** font dimensions */
    static uint8_t fontWidth, fontHeight;
    /** text wrapping */
    static bool textWrap;
    /** GB compatibility drawChar */
    static void drawChar(int8_t x, int8_t y, unsigned char c, uint8_t size);

    static void enableDirectPrinting(uint8_t m);
    static bool isDirectPrintingEnabled();
    static int print_char(uint8_t x, uint8_t y, unsigned char c);
    static void set_cursor(uint8_t, uint8_t);
    static void write(uint8_t);
    static void write(const char *str);
    static void write(const uint8_t *buffer, uint8_t size);
    static void print(const char[]);
    static void print(char, int base = 0);
    static void print(unsigned char, int base = 0);
    static void print(int, int base = 10);
    static void print(unsigned int, int base = 10);
    static void print(long, int base = 10);
    static void print(unsigned long, int base = 10);
    static void print(double, int base = 2);
    static void print(uint8_t, uint8_t, const char[]);
    static void print(uint8_t, uint8_t, char, int = 0);
    static void print(uint8_t, uint8_t, unsigned char, int = 0);
    static void print(uint8_t, uint8_t, int, int = 10);
    static void print(uint8_t, uint8_t, unsigned int, int = 10);
    static void print(uint8_t, uint8_t, long, int = 10);
    static void print(uint8_t, uint8_t, unsigned long, int = 10);
    static void print(uint8_t, uint8_t, double, int = 2);
    static void println(uint8_t, uint8_t, const char[]);
    static void println(uint8_t, uint8_t, char, int = 0);
    static void println(uint8_t, uint8_t, unsigned char, int = 0);
    static void println(uint8_t, uint8_t, int, int = 10);
    static void println(uint8_t, uint8_t, unsigned int, int = 10);
    static void println(uint8_t, uint8_t, long, int = 10);
    static void println(uint8_t, uint8_t, unsigned long, int = 10);
    static void println(uint8_t, uint8_t, double, int = 2);
    static void println(uint8_t, uint8_t);
    static void println(const char[]);
    static void println(char, int = 0);
    static void println(unsigned char, int = 0);
    static void println(int, int = 10);
    static void println(unsigned int, int = 10);
    static void println(long, int = 10 );
    static void println(unsigned long, int = 10);
    static void println(double, int = 2);
    static void println(void);

    static int16_t cursorX,cursorY;
    static uint8_t fontSize;
    static int8_t adjustCharStep, adjustLineStep;
    static bool fixedWidthFont, flipFontVertical;

    static void inc_txtline();
    static void printNumber(unsigned long, uint8_t);
    static void printFloat(double, uint8_t);

    /** external small printf, source in PokittoPrintf.cpp **/
    static int printf(const char *format, ...);

#if PROJ_SCREENMODE == TASMODE
    static inline uint32_t TASMask = 0; // enable all rows
#endif

private:
    static void printFPS();
    static uint8_t m_mode;
    static uint8_t m_w,m_h; // store these for faster access when switching printing modes
    /** Pointer to screen buffer */
    static uint8_t* m_scrbuf;

    static void drawBitmapData2BPP(int x, int y, int w, int h, const uint8_t* bitmap);
    static void drawBitmapData4BPP(int x, int y, int w, int h, const uint8_t* bitmap);
    static void drawBitmapData8BPP(int x, int y, int w, int h, const uint8_t* bitmap);
    static void drawBitmapDataXFlipped2BPP(int x, int y, int w, int h, const uint8_t* bitmap);
    static void drawBitmapDataXFlipped4BPP(int x, int y, int w, int h, const uint8_t* bitmap);
    static void drawBitmapDataXFlipped8BPP(int x, int y, int w, int h, const uint8_t* bitmap);
};

}

#endif // POKITTODISPLAY_H




