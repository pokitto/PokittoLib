#include "Pokitto.h"
#include "Animation.h"

#define X_L_EYE 6
#define Y_L_EYE 8
#define X_R_EYE 58
#define Y_R_EYE 8
#define X_CHEEKS 5
#define Y_CHEEKS 43
#define Y_PUPIL 18
#define X_LPUPIL 19
#define X_RPUPIL 66
#define DX_PUPIL 8
#define X_SMILE 40
#define Y_SMILE 57
#define X_OH 46
#define Y_OH 49
#define X_GRIN 19
#define Y_GRIN 56
#define X_FLAT 34
#define X_BLINKF 5
#define Y_BLINKF 6
#define X_BLINKH 2
#define Y_BLINKH 12
#define READLEVEL 4 // was 15
#define MIDPOINT 65
#define GRINLEVEL 230
#define GRINOUT 30000
#define TALKPROBABILITY 0x90 //was 90

Pokitto::Core gb;
#ifndef POK_SIM
DigitalOut enable(EXT4);
#else
uint8_t enable;
#endif

void popShowButton(char c) {
    gb.display.color=0;
    gb.display.fillRect(gb.display.width/2-10,gb.display.height/2-10,20,20);
    gb.display.color=1;
    gb.display.setCursor(gb.display.width/2-5,gb.display.height/2-5);
    gb.display.print(c);
}

int main () {
    enable=0;
    pokInitSD();
    gb.sound.playMusicStream("intro44.snd");
    int16_t x,y,x_lpupil,x_rpupil,y_pupil, squint=1;
    uint8_t mouth=0,eyes=1,blink=0,blinkcount=0, eyecount=0;
    uint16_t grincounter=0, grinout=GRINOUT;
    bool blockmouth=false;
    bool talking=false;
    uint8_t sbtemp=0;

    x_lpupil = X_LPUPIL; x_rpupil = X_RPUPIL; y_pupil=Y_PUPIL;
    gb.begin();
    gb.display.load565Palette(m_anim_pal);
    gb.display.bgcolor=10;
    gb.display.clear();
    gb.display.setColorDepth(4);
    gb.sound.ampEnable(true);
    gb.sound.playMusicStream();
    while (gb.isRunning()) {

        #ifdef POK_SIM]
        sbtemp = (uint8_t)(soundbyte * 3);
        #else
        sbtemp = (uint8_t)soundbyte;
        #endif // POK_SIM
        if (sbtemp > MIDPOINT + READLEVEL|| sbtemp < MIDPOINT-READLEVEL) talking = true;
        //if (sbtemp > MIDPOINT + READLEVEL) talking = true;
        else talking=false;
        if (gb.sound.getMusicStreamElapsedSec()<6) talking=false;
        if (gb.sound.getMusicStreamElapsedMilliSec()>10270 && gb.sound.getMusicStreamElapsedMilliSec() < 11303) talking=false;
         if (gb.sound.getMusicStreamElapsedMilliSec()>15800 && gb.sound.getMusicStreamElapsedMilliSec() < 17400) talking=false;
        //talking = false;
        //if (gb.buttons.leftBtn()) talking=true;
        //if (gb.buttons.aBtn()) squint=false;
        //else squint = true;
        //if (gb.buttons.bBtn()) squint=2;
        //if (gb.buttons.downBtn()) blockmouth=true;
        //else blockmouth=false;

        if (gb.buttons.aBtn()) popShowButton('A');
        if (gb.buttons.bBtn()) popShowButton('B');
        if (gb.buttons.cBtn()) popShowButton('C');
        if (gb.buttons.upBtn()) popShowButton('U');
        if (gb.buttons.downBtn()) popShowButton('D');
        if (gb.buttons.leftBtn()) popShowButton('L');
        if (gb.buttons.rightBtn()) popShowButton('R');

        if (!talking) grincounter++;
        else grincounter = 0;

        if (grincounter>GRINOUT) grincounter=0;

        if (gb.update()) {
        x=X_L_EYE; y=Y_L_EYE;
            gb.display.drawBitmap(x,y,m_lefteye);
            x=X_R_EYE; y=Y_R_EYE;
            gb.display.drawBitmap(x,y,m_righteye);

        uint8_t t;
        //if (Pokitto::streamPaused()) eyes=2;
        switch (eyes) {
        case 0:
            x_lpupil=X_LPUPIL; x_rpupil=X_RPUPIL;
            t=98;
            break;
        case 1:
            x_lpupil=X_LPUPIL-DX_PUPIL-2; x_rpupil=X_RPUPIL-DX_PUPIL;
            t=70;
            break;
        case 2:
            x_lpupil=X_LPUPIL+DX_PUPIL; x_rpupil=X_RPUPIL+DX_PUPIL+2;
            t=70;
             break;
        }
        if (random(100)>t) eyes = random(2);

        switch (blink) {
            case 0:
                if (blinkcount) blinkcount--;
                else {
                        blink = random(2);
                        blinkcount = 8/(blink+1);
                        if (blink==1) blinkcount *=3;
                }
                break;
            case 2:
                if (blinkcount) {
                    x=X_BLINKF; y=Y_BLINKF;
                    gb.display.invisiblecolor=255;
                    gb.display.drawBitmap(x,y,m_lids_closed);
                    blinkcount--;
                } else {
                    blink = 0;
                    blinkcount = random(0x20);
                }
                break;
            case 1:
                if (blinkcount) {
                    x=X_BLINKH; y=Y_BLINKH;
                    gb.display.invisiblecolor=255;
                    gb.display.color=10;
                    gb.display.fillRect(0,0,110,12);
                    gb.display.drawBitmap(x,y,m_lids_halfeyes);
                    blinkcount--;
                } else {
                    blink=0;
                    blinkcount = random(0x20);
                }
        }
        if (blink !=2 && random(100)<98) {
        gb.display.invisiblecolor=1;
        if (squint==1) {
            gb.display.drawBitmap(x_lpupil,y_pupil,m_eyepupil);
            gb.display.drawBitmap(x_rpupil,y_pupil,m_eyepupil);
            gb.display.invisiblecolor=10;
            //if (random(100)>98) eyes =0;
        }else if (squint ==0) {
            gb.display.drawBitmap(x_lpupil,y_pupil,m_squint);
            gb.display.drawBitmap(x_rpupil,y_pupil,m_squint);
            gb.display.invisiblecolor=10;
            //if (random(100)>97) eyes =1;
        } else if (squint ==2) {
            gb.display.drawBitmap(x_lpupil-3,y_pupil-3,m_heart);
            gb.display.drawBitmap(x_rpupil-3,y_pupil-3,m_heart);
            gb.display.invisiblecolor=10;
        }
        }

        //if (Pokitto::streamPaused()) mouth=5;
        x=X_CHEEKS; y=Y_CHEEKS;
        gb.display.invisiblecolor=10;
        if (mouth != 5) gb.display.drawBitmap(x,y,m_cheeks);

        uint8_t om=mouth;

        if (talking) {
        if (random(0xfff)>TALKPROBABILITY) {
                uint8_t oldmouth = mouth;
                mouth = random(3)+1;
                if (oldmouth=mouth) mouth++;
                if (mouth==5) mouth = 1;
        }
        } else {
            if (grincounter>GRINLEVEL) mouth = 5;
            else mouth = 5;
        }

        if (blockmouth) mouth = om;
        //if (Pokitto::streamPaused()) mouth=5;
        switch (mouth) {
            case 0:
                x=X_SMILE; y=Y_SMILE;
                gb.display.drawBitmap(x,y,m_smile);
                break;
            case 1:
                x=X_SMILE; y=Y_SMILE;
                gb.display.drawBitmap(x,y,m_smile_open);
                break;
            case 2:
                x=X_SMILE; y=Y_SMILE;
                gb.display.drawBitmap(x,y,m_tongue);
                break;
            case 3:
                x=X_OH; y=Y_OH;
                gb.display.drawBitmap(x,y,m_oh);
                break;
            case 4:
                x=X_FLAT; y=Y_SMILE;
                gb.display.drawBitmap(x,y,m_flat);
                break;
            case 5:
                x=X_GRIN; y=Y_GRIN;
                gb.display.drawBitmap(x,y,m_grin);
                break;
            default:
                break;
        }
        //gb.display.color = 0;
        //gb.display.setCursor(0,0);
        //gb.display.print((int)sbtemp);
        //if (gb.buttons.pressed(BTN_A)) gb.display.rotatePalette(1);

        }
    }
}


