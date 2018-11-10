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

int notenumber=46;
uint8_t volume = 255;//127;
uint8_t wavetype = 0;
uint32_t changed = 1;
char notestr[12];
uint8_t sbindx=0,sbx=0,prevy=0;
bool forcerefresh=true;
uint8_t loop=0, echo=0,adsr=0;
uint16_t attack=0, decay=0, sustain=0, release=0;
int16_t maxbend=0, bendrate=0;
uint8_t arpmode = 0, overdrive=0, kick=0;


#define M_WAVE      0
#define M_NOTE      1
#define M_VOL       2
#define M_LOOP      3
#define M_ECHO      4
#define M_ADSR      5
#define M_ATTACK    6
#define M_DECAY     7
#define M_SUSTAIN   8
#define M_RELEASE   9
#define M_MAXBEND   10
#define M_BENDRATE  11
#define M_ARPMODE   12
#define M_OVERDRIVE 13
#define M_KICK      14
#define M_SYSTEMVOL 15

int menupos=0;

void highlighttxt() {
    disp.color = 1;
    disp.setFont(fontKoubit);
}

void greentxt() {
    disp.color = 2;
    disp.setFont(fontKoubit);
}

void normaltxt() {
    disp.color = 3;
    disp.setFont(fontKoubit);
}

int main()
{
    #ifndef POK_SIM
    enable=0;
    #endif // POK_SIM
    game.begin();
    disp.persistence = true;
    disp.bgcolor=0;
    disp.palette[3] = COLOR_GRAY_128;
    disp.palette[2] = COLOR_GREEN;

    uint8_t sbindx=0;
    int oldvol=0;
    int linecenter = (disp.height/3); // center oscilloscope line vertically
    linecenter *=2;
    linecenter += 0;
    snd.ampEnable(1);
    snd.loadSampleToOsc(1,(uint8_t*)beat11k,sizeof(beat11k));

    while (game.isRunning()) {

        #ifdef POK_SIM
            #define DEBOU 2048*64
        #else
            #define DEBOU 256
        #endif


        if (changed == 0 || forcerefresh) {
            if(btn.upBtn()) { (int)menupos--; changed = DEBOU; }
            if(btn.downBtn()) { (int)menupos++; changed = DEBOU; }
            int mult=1;
            if(btn.aBtn()) { mult=10; }
            if(btn.bBtn()) { mult=100; }

            if (menupos<0) menupos=0;
            if (menupos==M_ATTACK && !adsr) menupos=M_MAXBEND;
            if (menupos>10) menupos=0;

            switch (menupos) {
            case M_NOTE:
                if(btn.leftBtn() && (notenumber >= mult)) { notenumber -=mult; changed = DEBOU; }
                if(btn.rightBtn() &&  (notenumber < 88-mult)) { notenumber +=mult; changed = DEBOU; }
                break;
            case M_WAVE:
                if(btn.leftBtn() && (wavetype > 0)) { wavetype--; changed = DEBOU*4; }
                if(btn.rightBtn() &&  (wavetype < MAX_WAVETYPES)) { wavetype++; changed = DEBOU*4; }
                break;
            case M_VOL:
                if(btn.leftBtn() && (volume >= mult)) { volume-=mult; changed = DEBOU; }
                if(btn.rightBtn() &&  (volume < 255-mult)) { volume+=mult; changed = DEBOU; }
                break;
            case M_LOOP:
                if(btn.leftBtn() && (loop > 0)) { loop--; changed = DEBOU; }
                if(btn.rightBtn() &&  (loop < 1)) { loop++; changed = DEBOU; }
                break;
            case M_ECHO:
                if(btn.leftBtn() && (echo > 0)) { echo--; changed = DEBOU; }
                if(btn.rightBtn() &&  (echo < 1)) { echo++; changed = DEBOU; }
                break;
            case M_ADSR:
                if(btn.leftBtn() && (adsr> 0)) { adsr--; changed = DEBOU; }
                if(btn.rightBtn() &&  (adsr < 1)) { adsr++; changed = DEBOU; }
                break;
            case M_ATTACK:
                if(btn.leftBtn() && (attack >= mult)) { attack-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (attack < 255-mult)) { attack+=mult; changed = DEBOU; }
                break;
            case M_DECAY:
                if(btn.leftBtn() && (decay >= mult)) { decay-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (decay < 255-mult)) { decay+=mult; changed = DEBOU; }
                break;
            case M_SUSTAIN:
                if(btn.leftBtn() && (sustain >= mult)) { sustain-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sustain < 255 - mult)) { sustain+=mult; changed = DEBOU; }
                break;
            case M_RELEASE:
                if(btn.leftBtn() && (release >= mult)) { release-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (release < 255 - mult)) { release+=mult; changed = DEBOU; }
                break;
            }


            if (snd.getVolume() != oldvol) changed=DEBOU;



            if (changed) {
                    disp.clear();
                    disp.color=0;//owavetype+1;
                    disp.fillRectangle(0,0,219,132);
                    disp.bgcolor=0;
                    highlighttxt();
                    disp.println("            Pokitto Badass SFX Editor\n");
                    // Wave type
                    greentxt();
                    disp.print("Wave: ");
                    normaltxt(); if (menupos==M_WAVE) highlighttxt();
                    disp.print((int)wavetype);
                    switch (wavetype) {
                    case 0:
                        disp.println(" Off");break;
                    case 1:
                        disp.println(" Square");break;
                    case 2:
                        disp.println(" Saw");break;
                    case 3:
                        disp.println(" Triangle");break;
                    case 4:
                        disp.println(" Noise");break;
                    case 5:
                        disp.println(" Square Noise");break;
                    case 6:
                        disp.println(" Sample");break;
                    }
                    // Note
                    greentxt();
                    disp.print("Note: ");
                    normaltxt(); if (menupos==M_NOTE) highlighttxt();
                    getNoteString(notestr,notenumber);
                    disp.println(notestr);

                    // Volume
                    greentxt();
                    disp.print("Volume: ");
                    normaltxt(); if (menupos==M_VOL) highlighttxt();
                    disp.println((int)volume);

                    // Loop
                    greentxt();
                    disp.print("Loop: ");
                    normaltxt(); if (menupos==M_LOOP) highlighttxt();
                    disp.println((int)loop);

                    // Echo
                    greentxt();
                    disp.print("Echo: ");
                    normaltxt(); if (menupos==M_ECHO) highlighttxt();
                    disp.println((int)echo);

                    // ADSR
                    greentxt();
                    disp.print("ADSR: ");
                    normaltxt(); if (menupos==M_ADSR) highlighttxt();
                    disp.println((int)adsr);

                    if (adsr) {
                        // Attack
                        greentxt();
                        disp.print("  Attack: ");
                        normaltxt(); if (menupos==M_ATTACK) highlighttxt();
                        disp.println(attack);
                        // Decay
                        greentxt();
                        disp.print("  Decay: ");
                        normaltxt(); if (menupos==M_DECAY) highlighttxt();
                        disp.println(decay);
                        // Sustain
                        greentxt();
                        disp.print("  Sustain: ");
                        normaltxt(); if (menupos==M_SUSTAIN) highlighttxt();
                        disp.println(sustain);
                        // Release
                        greentxt();
                        disp.print("  Release: ");
                        normaltxt(); if (menupos==M_RELEASE) highlighttxt();
                        disp.println(release);
                        // draw curve
                        disp.color=2;
                        disp.drawRect(84,63,180-84,92-63);
                    }

                    // Master volume
                    greentxt();
                    disp.print("System vol: ");
                    normaltxt(); if (menupos==M_SYSTEMVOL) highlighttxt();
                    #ifndef POK_SIM
                    disp.print((int)Pokitto::getHWvolume());
                    disp.print(", ");
                    #endif // POK_SIM
                    disp.println((int)snd.getVolume());

                    // Play the tone
                    //snd.playTone(1,notenumber,volume,wavetype,arpmode);
                    setOSC(&osc1,1, wavetype, loop, echo, adsr,
                    notenumber, volume,
                    attack, decay, sustain, release,
                    maxbend, bendrate,
                    arpmode, overdrive, kick);
                    oldvol = (int) snd.getVolume();
                    forcerefresh=false;
            }
        } else changed--;
        disp.color=0;
        disp.drawColumn(sbx,132,disp.height);
        disp.color=1;
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
