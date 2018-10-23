#include "Pokitto.h" // include Pokitto library
#include "spygfx.h"
#include "JoyHat.h"

Pokitto::Core mygame; //create Pokitto application instance
Pokitto::Display d; //faster way of writing mygame.display.

#define _DY 3
#define _DX 7
#define _DS 1
#define MAX_SCROLLSPEED 24

float scrollspeed=0,yspd,xspd;
float wx=0;
float carx=220/4*3;
float cary=176/2-16;
int carstate=0;

int main () {
    int x=0,y=20;
    mygame.begin(); // start the application
    d.load565Palette(spygfx_pal); //load the palette for the image
    d.setInvisibleColor(15);
    d.setColor(1,2); //fg color white, bg color asphalt grey

    /* the "while" loop runs as long as the program is running */
    while (mygame.isRunning()) {
        /* mygame.update() is processed whenever it is time to update the screen */
        if (mygame.update()) {
                //d.drawBitmap(0,0,spy_ss);
                yspd = 0; carstate=0;
                if (joyhat.JoyY()>80) yspd = _DY;
                if (joyhat.JoyY()<20) yspd = -_DY;
                if (yspd>0) carstate = 2;
                else if (yspd < 0) carstate = 1;
                cary += yspd;
                xspd = 0;
                if (joyhat.JoyX()>80) xspd = _DX;
                if (joyhat.JoyX()<20) xspd = -_DX;
                carx -= xspd;
                if (xspd > 0) scrollspeed += _DS;
                else if (xspd < 0 ) scrollspeed -= _DS;
                if (carx<220/3) carx = 220/3;
                if (carx>220/4*3) carx = 220/4*3;
                if (scrollspeed > MAX_SCROLLSPEED) scrollspeed = MAX_SCROLLSPEED;
                if (scrollspeed < 0 ) scrollspeed = 0;
                //top trees
                for (int i=-32; i<220; i+=32) {d.drawBitmap(i+wx,0,(const uint8_t*)bg_gfx+130*3); d.drawBitmap(i+wx+16,0,(const uint8_t*)bg_gfx+130*4);}
                //top road edge
                for (int i=-32; i<220; i+=16) {d.drawBitmap(i+wx,16,(const uint8_t*)bg_gfx+130*1);}
                //bottom road edge
                for (int i=-32; i<220; i+=16) {d.drawBitmap(i+wx,176-32,(const uint8_t*)bg_gfx);}
                //bottom trees
                for (int i=-32; i<220; i+=32) {d.drawBitmap(i+wx,160,(const uint8_t*)bg_gfx+130*3); d.drawBitmap(i+wx+16,160,(const uint8_t*)bg_gfx+130*4);}
                wx += scrollspeed;
                if (wx >= 32) wx=0;
                //draw car
                d.drawBitmap(carx,cary,(const uint8_t*)car+carstate*514);

        }
    }
    return 0; // this is "good programming manners". Program informs it ended without errors
}

