/* Pokitto drawBitmap example - draws a Pokitto icon by @trelemar */

#include "Pokitto.h" // include Pokitto library
#include "monkey16.h"

Pokitto::Core mygame; //create Pokitto application instance

void drawBlurb(const char* text, int x, int y, uint8_t fc, uint8_t bgc) {
    for (int tx=-1;tx<2;tx++) {
        for (int ty=-1;ty<2;ty++) {
            mygame.display.setColor(bgc,15);
            mygame.display.setInvisibleColor(15);
            mygame.display.setCursor(x+tx,y+ty);
            mygame.display.print(text);
        }
    }
    mygame.display.setCursor(x,y);
    mygame.display.setColor(fc,bgc);
    mygame.display.setInvisibleColor(bgc);
    mygame.display.print(text);
}


int main () {
    int x=0,y=20;
    mygame.begin(); // start the application
    mygame.display.load565Palette(monkey16_pal); //load the palette for the image
    mygame.display.setColor(1,0); // set foreground and background colors from palette
    mygame.display.setFont(fontMonkey); // choose a lovely font
    mygame.display.setInvisibleColor(0);
    /* the "while" loop runs as long as the program is running */
    while (mygame.isRunning()) {
        /* mygame.update() is processed whenever it is time to update the screen */
        if (mygame.update()) {
            x-=2;
            if (x<-90) x=-90;
            else if (x>0) x = 0;
            mygame.display.drawBitmap(x,16,monkey161);
            mygame.display.drawBitmap(x+160,16,monkey162);
            if (x<-82) {
                    drawBlurb("The New Mode15!!",10,62,14,0);
            }
            if (x<-88) {
                    drawBlurb("Nice!",130,82,10,0);
            }

        }
    }
    return 0; // this is "good programming manners". Program informs it ended without errors
}

