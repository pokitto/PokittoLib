#include "Pokitto.h"

Pokitto::Core mygame;

const uint8_t tileSet1[] = {
    6,6,6,6,6,6,6,6,
    6,7,7,7,7,7,5,6,
    6,7,6,6,6,6,5,6,
    6,7,6,7,5,6,5,6,
    6,7,6,5,5,6,5,6,
    6,7,6,6,6,6,5,6,
    6,5,5,5,5,5,5,6,
    6,6,6,6,6,6,6,6,
 };

const uint8_t tileSet2[] = {
    7, 7, 7, 7, 7, 7, 7, 3,
    7, 7, 7, 7, 7, 7, 3, 3,
    7, 7,11,11,11,11, 3, 3,
    7, 7,11,11,11,11, 3, 3,
    7, 7,11,11,11,11, 3, 3,
    7, 7,11,11,11,11, 3, 3,
    7, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
 };

const uint8_t tileW = 8;
const uint8_t tileH = 8;

// Second layer
const char tileBuffer2[8+1] = "POKITTO ";  // Must be 8 letters!

//
void DrawLayer1(uint8_t tilePosX, uint8_t tilePosY);
void DrawLayer2(uint32_t tile2PosX, uint32_t tile2PosY);

// Main
int main () {

    mygame.begin();

    mygame.display.loadRGBPalette(palettePico);
    mygame.display.persistence = 1;

    #ifdef POK_SIM
    mygame.setFrameRate(30);
    #endif // PROJ_SIM

    uint32_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int32_t y2Dur = 0;
    int32_t stepY2 = 1;
    int16_t paletteRotDur = 10;

    while (mygame.isRunning()) {

        if (mygame.update()) {

            if(paletteRotDur<=0) {
                mygame.display.rotatePalette(1);
                paletteRotDur = 100;
            }
            paletteRotDur--;

            DrawLayer1(x1, y1);
            DrawLayer2(x2, y2);

            x1--;y1--;

            y2Dur--;
            if(y2Dur <= 0) {
                stepY2 *= -1;
                y2Dur = 50;
            }
            x2++; y2 += stepY2;

        }
    }
}

// Draw the background tiles.

void DrawLayer1(uint8_t tilePosX, uint8_t tilePosY)
{
    uint8_t* scrptr = mygame.display.getBuffer();  // 4-bit screen buffer
    uint8_t tileY = tilePosY & 0x7;
    for( uint8_t y=0; y<88 ; y++,  tileY++ ) {

        uint8_t tileX = tilePosX & 0x7;  // Wrap around after 8 pixels.
        const uint8_t* rowPtr = tileSet1 + ((tileY & 0x7)*tileW);
        for( uint8_t x=0; x<55 ; x++, tileX+=2 ) {

            // Set two pixels at a time.
            const uint8_t p0 = *(rowPtr + (tileX & 0x7));
            const uint8_t p1 = *(rowPtr + ((tileX+1)& 0x7));
            *scrptr++ = (p0 << 4) | p1;
        }
    }
}

// Draw the foreground tiles, i.e. text as a tile map.

void DrawLayer2(uint32_t tile2PosX, uint32_t tile2PosY)
{
    uint8_t* scrptr = mygame.display.getBuffer(); // 4-bit screen buffer
    uint32_t finalY = tile2PosY;
    for( uint8_t y=0; y<88 ; y++,  finalY++ ) {

        uint32_t finalX = tile2PosX;
        const uint8_t* tileBitmapPtr = tileSet2 + ((finalY & 0x7)*tileW); // tile bitmap row pointer
        uint8_t* letterBitmapPtr = NULL;
        uint8_t prevLetterNum = 0xff;  // uninitialized

        for( uint8_t x=0; x<110 ; x++, finalX++ ) {

            // *** Get the tile number from the "map"

            // Get the letter bitmap. Each char bitmap pixel is a tile!
            const uint8_t letterNum = (finalX >> (3+3)) & 0x7;
            if(letterNum != prevLetterNum ) {

                //letterNum = letterNum>7 ? 7 : letterNum;
                letterBitmapPtr = (uint8_t*)fntC64UIGfx;  // the letter bitmap is our tile map!
                const uint8_t letter = *(tileBuffer2 + letterNum);
                const uint8_t letterindex =  letter - *(letterBitmapPtr + 2);
                uint8_t fontw=8;
                letterBitmapPtr += 4 + (letterindex * fontw) + letterindex + 1;
                prevLetterNum = letterNum;
            }

            // Get the pixel in a letter bitmap. Each pixel means a tile on screen.
            const uint8_t* tileMapPtr = letterBitmapPtr + ((finalX>>3) & 0x7);
            uint8_t isTile = ((*tileMapPtr)>>((finalY>>3) & 0x7)) & 1;

            // ** If there is a tile, get the offset inside the tile.

            if( isTile == 1 ) {

                // *** Draw one tile row fully.

                do {

                    // Draw pixel.
                    uint8_t color = *(tileBitmapPtr + (finalX & 0x7));
                    if(x & 1)
                        *scrptr = (*scrptr & 0xF0) | color;
                    else
                        *scrptr = (color << 4) | (*scrptr & 0x0F);

                    // Advance the screen pointer.
                    if(x & 1) scrptr++;

                    if((finalX & 0x7) == 0x7 || (x>=109))
                        break;

                    x++;
                    finalX++;

                } while(true);
            }
            else {

                // No tile. Advance scrptr and x and finalX.

                uint32_t newFinalX = (finalX | 0x7);
                uint8_t count = newFinalX - finalX ;
                uint8_t newX = x + count;
                if(newX > 110)
                    count = 109 - x;

                if(x & 1)
                    scrptr += (count+2)>>1;
                else
                    scrptr += (count+1) >> 1;

                // Note: does not matter if these go out of bounds because upper loop (x) checks the bounds.
                x = newX;
                finalX = newFinalX;
            }
        }
    }
}
