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
    0,0x194a,0x792a,0x42a,
    0xaa86,0x5aa9,0xc618,0xff9d,
    0xf809,0xfd00,0xff84,0x72a,
    0x2d7f,0x83b3,0xfbb5,0xfe75
};

#define PALETTE_SIZE 256
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


namespace Pokitto {

class Display {
public:
    Display();

    // PROPERTIES
private:
    static uint8_t* canvas;
    static uint8_t bpp;
    static uint8_t m_colordepth;
public:
    static uint8_t palOffset;
    static uint8_t width;
    static uint8_t height;
    static uint8_t screenbuffer[];

    // PROPERTIES
    static void setColorDepth(uint8_t);
    static uint8_t getColorDepth();
    static uint8_t getBitsPerPixel();
    static uint16_t getWidth();
    static uint16_t getHeight();
    static uint8_t getNumberOfColors();

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
    /** clip rect on screen**/
    static int16_t clipX;
    static int16_t clipY;
    static int16_t clipW;
    static int16_t clipH;
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
    /** set clip rect on screen**/
    static void setClipRect(int16_t x, int16_t y, int16_t w, int16_t h);

    /** Initialize display */
    static void begin();
    /** Clear display buffer */
    static void clear();
    /** Scroll by x lines */
    static void scroll(int16_t);
    /** Fill display buffer */
    static void fillScreen(uint16_t);
    /** Send display buffer to display hardware */
    static void update(bool useDirectMode=false, uint8_t updRectX=0, uint8_t updRectY=0, uint8_t updRectW=LCDWIDTH, uint8_t updRectH=LCDHEIGHT);
    /** Forced update of LCD display memory with a given pixel buffer */
    static void lcdRefresh(unsigned char*, bool useDirectMode=false);
    /** Clear LCD hardware memory */
    static void clearLCD();
    /** Fill LCD hardware memory */
    static void fillLCD(uint16_t);
    /** Show Pokitto logo at startup*/
    static void showLogo();
    /** Point to another screenbuffer instead of the default one */
    static void setFrameBufferTo(uint8_t*);

    // COLORS AND PALETTE
public:
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
    /** Get pixel at various bit depths */
    static uint8_t getPixel(int16_t,int16_t);
    /** Draw line **/
    static void drawLine(int16_t,int16_t,int16_t,int16_t);
    /** Clip line with screen boundaries, returns 0 if whole line is out of bounds */
    static uint8_t clipLine(int16_t*, int16_t*, int16_t*, int16_t*);
    /** Draw a column real fast */
    static void drawColumn(int16_t, int16_t, int16_t);
    /** Map a 1-bit column real fast */
    static void map1BitColumn(int16_t, int16_t, int16_t, const uint8_t*, uint16_t);
    /** Draw a row real fast */
    static void drawRow(int16_t, int16_t, int16_t);
    /** Legacy drawColumn name, for compatibility - macros are not OK because of scope problems */
    static void drawFastVLine(int16_t, int16_t, int16_t);
    /** Legacy drawRow name, for compatibility - macros are not OK because of scope problems */
    static void drawFastHLine(int16_t, int16_t, int16_t);
    /** Draw rectangle (edges only) */
    static void drawRectangle(int16_t,int16_t,int16_t,int16_t);
    /** Fill rectangle */
    static void fillRectangle(int16_t,int16_t,int16_t,int16_t);
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
    /** Draw monochromatic bitmap. Used in font rendering */
    static void drawMonoBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t index);
    /** Draw bitmap data*/
    static void drawBitmapData(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap);
    /** Draw bitmap */
    static void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw RLE bitmap */
    static void drawRleBitmap(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw animated bitmap frame */
    static void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t frame);
    /** Draw bitmap flipped on x-axis*/
    static void drawBitmapXFlipped(int16_t x, int16_t y, const uint8_t* bitmap);
    /** Draw bitmap with options */
    static void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);
    /** Get pointer to the screen buffer - GB compatibility */
    static uint8_t* getBuffer();
    /** Get pixel in a monochromatic bitmap - GB compatibility */
    static uint8_t getBitmapPixel(const uint8_t*, uint16_t, uint16_t);
    /** Optimized functions for drawing bit columns - used in raytracing */
    static void draw4BitColumn(int16_t x, int16_t y, uint8_t h, uint8_t* bitmap);

    // SPRITES
    /* Setup or disable the sprite */
    static void setSpriteBitmap(uint8_t index, const uint8_t* bitmap, const uint16_t* palette4x16bit, int16_t x, int16_t y, bool doResetDirtyRect=true );
    /* Setup or disable the sprite */
    static void setSprite(uint8_t index, const uint8_t* data, const uint16_t* palette4x16bit, int16_t x, int16_t y, uint8_t w, uint8_t h, bool doResetDirtyRect=true );
    /* Set the sprite position */
    static void setSpritePos(uint8_t index, int16_t x, int16_t y);

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
	static bool fixedWidthFont;

    static void inc_txtline();
    static void printNumber(unsigned long, uint8_t);
    static void printFloat(double, uint8_t);

    /** Tiled mode functions **/

    static void loadTileset(const uint8_t*);

    static void setTileBufferTo(uint8_t*);
    static void clearTileBuffer();
    static void shiftTileBuffer(int8_t,int8_t);

    static void setTile(uint16_t,uint8_t);
    static uint8_t getTile(uint16_t);
    static uint8_t getTile(uint8_t,uint8_t);



private:
    static uint8_t m_mode;
    static uint16_t m_w,m_h; // store these for faster access when switching printing modes
    /** Pointer to screen buffer */
    static uint8_t* m_scrbuf;
    /** Pointer to tileset */
    static uint8_t* m_tileset;
    /** Pointer to tilebuffer */
    static uint8_t* m_tilebuf;
    /** Pointer to tilecolorbuffer */
    static uint8_t* m_tilecolorbuf;
    /** Sprites */
    static SpriteInfo m_sprites[SPRITE_COUNT];  // Does not own sprite bitmaps
};

}

#endif // POKITTODISPLAY_H




