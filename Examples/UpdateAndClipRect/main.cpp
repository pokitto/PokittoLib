#include "Pokitto.h"
#include "gfxdata.h"

Pokitto::Core mygame;

int main () {

    mygame.begin();

    mygame.display.persistence = 1; // Do not clear the background!

    mygame.display.load565Palette( pokitto_pal );

    mygame.display.clear();

    mygame.setFrameRate(100);  // No limits!

    bool doDrawBitmap = true;
    while (mygame.isRunning()) {

        while (! mygame.update());

        // Draw one column at a time. Two scans: one for even and another for odd columns.
        uint16_t xx = 0;
        for( uint16_t x=0; x < 220*2-4; x+=2 ) {

            if(x<220)
                xx = x;  // An even column.
            else
                xx = x - 220 + 1; // An odd column.

            if(doDrawBitmap) {

                // Draw bitmap

                // Set clip rect to draw only one column of the bitmap to the screen buffer.
                mygame.display.setClipRect(xx, 0, 1, 176);
                mygame.display.drawBitmap(0, 0, pokitto_bmp);
            }
            else {

                // Draw background color

                // Draw one column with the background color.
                mygame.display.setColor(0);
                mygame.display.fillRectangle(xx, 0, 1, 176);
            }

            // Use the update rect to update only one column to the LCD.
            while (! mygame.update(false, xx, 0, 1, 176));

        }
        doDrawBitmap = !doDrawBitmap;
    }

    return 0;
}
