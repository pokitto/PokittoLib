
#include "Pokitto.h"
#include "SDFileSystem.h"
#include "wave_player.h"

SDFileSystem sd(/*MOSI*/P0_9, /*MISO*/P0_8, /*SCK*/P0_6, /*CS*/P0_7, /*Mountpoint*/"sd");

Pokitto::Core game;

wave_player waver;
static FILE *fp = NULL;

int main() {

    //uint8_t bitstream[88];
    fp = fopen("/sd/play11.wav", "rb");

    if(fp == NULL){
        game.display.println("Couldn't open file");
    }
    waver.set_verbosity(0);
    waver.play2(fp);
    game.begin();
    game.display.loadRGBPalette(paletteRainbow);
    game.display.palette[0]=0;
    uint8_t ob=0, cou=1;
    while (game.isRunning())
    {
        waver.update(fp);
        if (game.update()) {
                //game.display.setCursor(0,0);
                //game.display.setColor(0);
                //game.display.drawCircle(game.display.width/2,game.display.height/2,ob);
                for (int i=0;i<110;i++) {
                        game.display.drawPixel(i,game.display.height/2+((int16_t)soundbuf[soundbufindex+i]-127)/2,soundbuf[soundbufindex+i]>>3);
                }
                ob = soundbuf[soundbufindex]>>2;
                game.display.setColor(ob/2);
                game.display.drawCircle(game.display.width/2,game.display.height/2,ob);
        //if (!cou++) {game.display.rotatePalette(1);}
        }

    }
    fseek(fp, 0, SEEK_SET);  // set file poiter to beginning
    wait(3.0);
    fclose(fp);
}


