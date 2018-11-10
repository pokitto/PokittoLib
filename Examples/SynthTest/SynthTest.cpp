#include "Pokitto.h"
#include "Synth.h"
#include "beat11k.h"

Pokitto::Core game;
Pokitto::Display disp;
Pokitto::Sound snd;
Pokitto::Buttons btn;
#ifndef POK_SIM
DigitalOut enable(EXT4);
#endif // POK_SIM

int tonefreq=46;
uint8_t amplitude = 255;//127;
uint8_t wavetype = 6, arpmode=3;
uint32_t changed = 1;
char notestr[6];
uint8_t sbindx=0,sbx=0,prevy=0;


int main()
{
    #ifndef POK_SIM
    enable=0;
    #endif // POK_SIM
    game.begin();
    //snd.setVolume(snd.getMaxVol()); // volume to max level depending on headset/speaker safety level
    disp.persistence = true;
    disp.color=wavetype+1;
    disp.bgcolor=0;
    uint8_t sbindx=0;
    int oldvol=0;
    int linecenter = (disp.height/4); // center oscilloscope line vertically
    linecenter *=2;
    linecenter += 0;
    snd.ampEnable(1);
    //snd.loadSampleToOsc(1,(uint8_t*)b1943,sizeof(b1943));
    snd.loadSampleToOsc(1,(uint8_t*)beat11k,sizeof(beat11k));
    snd.loadSampleToOsc(2,(uint8_t*)beat11k,sizeof(beat11k));
    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
    // snd.playTone(1,100,255,0);
    // want to have 2 oscillators 1 octave apart ?
    snd.playTone(2,tonefreq+12,amplitude,wavetype,arpmode);

    disp.clear();
    disp.print("Frq:");
    getNoteString(notestr,tonefreq);
    disp.println(notestr);
    disp.print("Vol:");
    disp.println((int)amplitude);
    disp.print("Wav:");
    disp.print((int)wavetype);
    switch (wavetype) {
    case 0:
        disp.println(" Off");break;
    case 1:
        disp.println(" Square");break;
    case 2:
        disp.println(" Saw");break;
    case 3:
        disp.println(" Triang");break;
    case 4:
        disp.println(" Noise");break;
    case 5:
        disp.println(" SqNois");break;
    case 6:
        disp.println(" Sample");break;
    }
    disp.print("Vol:");
    #ifndef POK_SIM
    disp.print((int)Pokitto::getHWvolume());
    disp.print(", ");
    #endif // POK_SIM
    disp.println((int)snd.getVolume());
    disp.lcdRefresh(disp.screenbuffer);



    while (game.isRunning()) {

        #ifdef POK_SIM
            #define DEBOU 2048*32
        #else
            #define DEBOU 256
        #endif


        if (changed == 0) {
            if(btn.upBtn()) { amplitude++; changed = DEBOU; }
            if(btn.downBtn()) { amplitude--; changed = DEBOU; }
            if(btn.leftBtn() && (tonefreq > 0)) { tonefreq --; changed = DEBOU; }
            if(btn.rightBtn() &&  (tonefreq < 88)) { tonefreq ++; changed = DEBOU; }
            if(btn.released(BTN_A) && wavetype > 0 ) { wavetype--; changed = DEBOU*10;}
            if(btn.released(BTN_B) && wavetype < MAX_WAVETYPES ) { wavetype++; changed = DEBOU*10;}
            if (snd.getVolume() != oldvol) changed=DEBOU;

            if (changed) {
                    disp.clear();
                    disp.color=0;//owavetype+1;
                    disp.fillRectangle(0,0,109,34);
                    disp.color=wavetype+1;
                    disp.bgcolor=0;
                    disp.print("Frq:");
                    getNoteString(notestr,tonefreq);
                    disp.println(notestr);
                    disp.print("Vol:");
                    disp.println((int)amplitude);
                    disp.print("Wav:");
                    disp.print((int)wavetype);
                    switch (wavetype) {
                    case 0:
                        disp.println(" Off");break;
                    case 1:
                        disp.println(" Square");break;
                    case 2:
                        disp.println(" Saw");break;
                    case 3:
                        disp.println(" Triang");break;
                    case 4:
                        disp.println(" Noise");break;
                    case 5:
                        disp.println(" SqNois");break;
                    case 6:
                        disp.println(" Sample");break;
                    }
                    disp.print("Vol:");
                    #ifndef POK_SIM
                    disp.print((int)Pokitto::getHWvolume());
                    disp.print(", ");
                    #endif // POK_SIM
                    disp.println((int)snd.getVolume());
                    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
                    oldvol = (int) snd.getVolume();
                    // want to have 2 oscillators 1 octave apart ?
                    snd.playTone(2,tonefreq+12,amplitude,wavetype,arpmode);
            }
        } else changed--;
        disp.color=0;
        disp.drawColumn(sbx,38,disp.height);
        disp.color=wavetype+1;
        uint8_t tindex = sbindx*2;
        int16_t y = linecenter + (((int)128-(soundbuf[tindex]))>>2);
        disp.drawLine(sbx-1,prevy,sbx,y);
        game.update();
        disp.color=0;
        prevy=y;
        sbx++;sbindx++;
        if (sbx>disp.width-1) {sbx=0;}
    }
    return 0; // good manners!
}
