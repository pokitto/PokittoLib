/* Pokitto drawBitmap example - draws a Pokitto icon by @trelemar */

#include "Pokitto.h" // include Pokitto library
#include "pokitto_icon.h" // include the Pokitto icon graphics file

Pokitto::Core mygame; //create Pokitto application instance

int main () {
    mygame.begin(); // start the application
    mygame.display.load565Palette(pokitto_icon_pal); //load the palette for the image
    mygame.display.bgcolor=0;
    /* the "while" loop runs as long as the program is running */
    while (mygame.isRunning()) {
        /* mygame.update() is processed whenever it is time to update the screen */
        if (mygame.update()) {
            mygame.display.drawBitmap(0,0,pokitto_icon); // draw the pokitto_icon graphic
            }
        }

    return 0; // this is "good programming manners". Program informs it ended without errors
}

