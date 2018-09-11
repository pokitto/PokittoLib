/**************************************************************************/
/*!
    @file     miniGFX.h
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

*/
/**************************************************************************/

#ifndef MINIGFX_H
#define MINIGFX_H

#include "miniprint.h"

enum pok_gfxdepth {
    UNBUFFERED,
    BPP1,
    BPP2,
    BPP3,
    BPP4,
    BPP5
};

enum pok_gfxscaling {
    NONE,
    GB_STRETCH,
    X2,
    X3,
    AB_STRETCH,
};

extern pok_gfxdepth pok_depth;
extern pok_gfxscaling pok_scale;
extern unsigned int pok_directcolor;
extern unsigned int palette[];
extern void pokDirectBitmap(int,int,const unsigned char*, unsigned char, unsigned char);
extern void pokDirectChar(int, int, const unsigned char*, unsigned int);
extern void pokDirectPixel(int,int,unsigned int);
extern void pokDirectRectangle(int,int, int, int, unsigned int);
extern void pokDirectRow(int,int, int, unsigned int);
extern void pokDirectColumn(int,int, int, unsigned int);

#define DC_BLACK    0x0000 //black
#define DC_WHITE    0xFFFF //white
#define DC_GREEN    0x07E0 //green
#define DC_CYAN     0x07FF //cyan
#define DC_MAGENTA  0xF81F //magenta
#define DC_YELLOW   0xFFE0 //yellow
#define DC_ORANGE   0xFC00 //orange
#define DC_RED      0xF800 //red
#define DC_DGREEN   0x0400 //dark green
#define DC_DCYAN    0x0410 //dark cyan
#define DC_DMAGENTA 0x8010 //dark magenta
#define DC_GRAY     0x7BEF //gray
#define DC_BLUE     0x001F //blue
#define DC_DBLUE    0x0010 //dark blue
#define DC_PINK     0xF810 //pink
#define DC_VIOLET   0x801F //violet

#endif // MINIGFX
