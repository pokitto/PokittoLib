#include "Pokitto.h"
#include "Synth.h"
#include "beat11k.h"
#include "SFXeditor.h"

Pokitto::Core game;
Pokitto::Display disp;
Pokitto::Sound snd;
Pokitto::Buttons btn;
#ifndef POK_SIM
DigitalOut enable(EXT4);
#endif // POK_SIM

sfxcookie sfx;

uint32_t changed = 1;
char notestr[12];
uint8_t sbindx=0,sbx=0,prevy=0, save=0;
bool forcerefresh=true;


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
#define M_NORMALIZE 14
#define M_SAVE      15

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
    sfx.begin("BSFXedit",sfx);
    if (sfx.firstsave==0 || sfx.inst.wave > MAX_WAVETYPES) sfx.init();
    game.begin();
    disp.persistence = true;
    disp.bgcolor=0;
    disp.palette[3] = COLOR_GRAY_128;
    disp.palette[2] = COLOR_GREEN;

    uint8_t sbindx=0;
    int oldvol=0;
    int linecenter = (disp.height/4); // center oscilloscope line vertically
    linecenter *=3;
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
            if (menupos==M_ATTACK && !sfx.inst.adsr) menupos=M_MAXBEND;
            if (menupos==M_RELEASE && !sfx.inst.adsr) menupos=M_ADSR;
            if (menupos>15) menupos=0;

            switch (menupos) {
            case M_NOTE:
                if(btn.leftBtn() && (sfx.notenumber >= mult)) { sfx.notenumber -=mult; changed = DEBOU; }
                if(btn.rightBtn() &&  (sfx.notenumber < 88-mult)) { sfx.notenumber +=mult; changed = DEBOU; }
                break;
            case M_WAVE:
                if(btn.leftBtn() && (sfx.inst.wave > 0)) { sfx.inst.wave--; changed = DEBOU*2; }
                if(btn.rightBtn() &&  (sfx.inst.wave < MAX_WAVETYPES)) { sfx.inst.wave++; changed = DEBOU*2; }
                break;
            case M_VOL:
                if(btn.leftBtn() && (sfx.inst.vol >= mult)) { sfx.inst.vol-=mult; changed = DEBOU; }
                if(btn.rightBtn() &&  (sfx.inst.vol < 255-mult)) { sfx.inst.vol+=mult; changed = DEBOU; }
                break;
            case M_LOOP:
                if(btn.leftBtn() && (sfx.inst.loop > 0)) { sfx.inst.loop--; changed = DEBOU; }
                if(btn.rightBtn() &&  (sfx.inst.loop < 1)) { sfx.inst.loop++; changed = DEBOU; }
                break;
            case M_ECHO:
                if(btn.leftBtn() && (sfx.inst.echo > 0)) { sfx.inst.echo--; changed = DEBOU; }
                if(btn.rightBtn() &&  (sfx.inst.echo < 1)) { sfx.inst.echo++; changed = DEBOU; }
                break;
            case M_ADSR:
                if(btn.leftBtn() && (sfx.inst.adsr> 0)) { sfx.inst.adsr--; changed = DEBOU; }
                if(btn.rightBtn() &&  (sfx.inst.adsr < 1)) { sfx.inst.adsr++; changed = DEBOU; }
                break;
            case M_ATTACK:
                if(btn.leftBtn() && (sfx.inst.attack >= mult)) { sfx.inst.attack-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.attack < 255-mult)) { sfx.inst.attack+=mult; changed = DEBOU; }
                break;
            case M_DECAY:
                if(btn.leftBtn() && (sfx.inst.decay >= mult)) { sfx.inst.decay-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.decay < 255-mult)) { sfx.inst.decay+=mult; changed = DEBOU; }
                break;
            case M_SUSTAIN:
                if(btn.leftBtn() && (sfx.inst.sustain >= mult)) { sfx.inst.sustain-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.sustain < 255 - mult)) { sfx.inst.sustain+=mult; changed = DEBOU; }
                break;
            case M_RELEASE:
                if(btn.leftBtn() && (sfx.inst.release >= mult)) { sfx.inst.release-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.release < 255 - mult)) { sfx.inst.release+=mult; changed = DEBOU; }
                break;
            case M_MAXBEND:
                if(btn.leftBtn() && (sfx.inst.maxbend >= -2000+mult)) { sfx.inst.maxbend-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.maxbend < 2000 - mult)) { sfx.inst.maxbend+=mult; changed = DEBOU; }
                break;
            case M_BENDRATE:
                if(btn.leftBtn() && (sfx.inst.bendrate >= -3000 + mult)) { sfx.inst.bendrate-=mult; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.bendrate < 3000 - mult)) { sfx.inst.bendrate+=mult; changed = DEBOU; }
                break;
            case M_ARPMODE:
                if(btn.leftBtn() && (sfx.inst.arpmode > 0)) { sfx.inst.arpmode--; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.arpmode < MAX_ARPMODE)) { sfx.inst.arpmode++; changed = DEBOU; }
                break;
            case M_OVERDRIVE:
                if(btn.leftBtn() && (sfx.inst.overdrive > 0)) { sfx.inst.overdrive--; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.overdrive < 1)) { sfx.inst.overdrive++; changed = DEBOU; }
                break;
            case M_NORMALIZE:
                if(btn.leftBtn() && (sfx.inst.kick > 0)) { sfx.inst.kick--; changed = DEBOU; }
                if(btn.rightBtn() && (sfx.inst.kick < 1)) { sfx.inst.kick++; changed = DEBOU; }
                break;
            case M_SAVE:
                if(btn.leftBtn() || btn.rightBtn()) { save=1; changed = DEBOU*20; }
                break;
            }


            if (snd.getVolume() != oldvol) changed=DEBOU;



            if (changed) {
                    disp.clear();
                    disp.color=0;//osfx.inst.wave+1;
                    disp.fillRectangle(0,0,219,150);
                    disp.bgcolor=0;
                    highlighttxt();
                    disp.println("            Pokitto Badass SFX Editor\n");
                    // Wave type
                    greentxt();
                    disp.print("Wave: ");
                    normaltxt(); if (menupos==M_WAVE) highlighttxt();
                    disp.print((int)sfx.inst.wave);
                    switch (sfx.inst.wave) {
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
                    getNoteString(notestr,sfx.notenumber);
                    disp.println(notestr);

                    // Volume
                    greentxt();
                    disp.print("Volume: ");
                    normaltxt(); if (menupos==M_VOL) highlighttxt();
                    disp.println((int)sfx.inst.vol);

                    // Loop
                    greentxt();
                    disp.print("Loop: ");
                    normaltxt(); if (menupos==M_LOOP) highlighttxt();
                    disp.println((int)sfx.inst.loop);

                    // Echo
                    greentxt();
                    disp.print("Echo: ");
                    normaltxt(); if (menupos==M_ECHO) highlighttxt();
                    disp.println((int)sfx.inst.echo);

                    // ADSR
                    greentxt();
                    disp.print("ADSR: ");
                    normaltxt(); if (menupos==M_ADSR) highlighttxt();
                    disp.println((int)sfx.inst.adsr);

                    if (sfx.inst.adsr) {
                        // Attack
                        greentxt();
                        disp.print("  Attack: ");
                        normaltxt(); if (menupos==M_ATTACK) highlighttxt();
                        disp.println(sfx.inst.attack);
                        // Decay
                        greentxt();
                        disp.print("  Decay: ");
                        normaltxt(); if (menupos==M_DECAY) highlighttxt();
                        disp.println(sfx.inst.decay);
                        // Sustain
                        greentxt();
                        disp.print("  Sustain: ");
                        normaltxt(); if (menupos==M_SUSTAIN) highlighttxt();
                        disp.println(sfx.inst.sustain);
                        // Release
                        greentxt();
                        disp.print("  Release: ");
                        normaltxt(); if (menupos==M_RELEASE) highlighttxt();
                        disp.println(sfx.inst.release);
                        // draw curve
                        disp.color=2;
                        #define BSX     85
                        #define BSY     65
                        #define BOXW    128
                        #define BOXH    27
                        #define BEX     BSX+BOXW
                        #define BEY     BSY+BOXH
                        disp.drawRect(BSX-1,BSY-1,BOXW+2,BOXH+2);
                        disp.color=1;
                        int attacky = BEY-(float)BOXH*(float)sfx.inst.vol/(float)255+1;
                        int attackx = 0;
                        if (sfx.inst.attack) attackx = 32 - (float)sfx.inst.attack/(float)255*32;
                        attackx += BSX;
                        disp.drawLine(BSX,BEY,attackx,attacky-1);
                        int sustainy = BEY;
                        if (sfx.inst.sustain) sustainy = BEY-(float)28*(float)sfx.inst.sustain/(float)255+1;
                        if (!sfx.inst.decay) sustainy = BEY; //sfx.inst.sustain is not active if there is no sfx.inst.decay
                        int decayx = attackx+1;
                        if (sfx.inst.decay) decayx += 32-(float)sfx.inst.decay/(float)sfx.inst.vol*32;
                        disp.drawLine(attackx+1,attacky-1,decayx,sustainy);
                        int releasey = sustainy;
                        int releasex = BEX;
                        float dx = BEX-decayx;
                        float dy;
                        if (sfx.inst.sustain) dy = (float)sfx.inst.release/(float)sfx.inst.sustain;
                        if (sfx.inst.release) {
                               releasey = BEY;
                                releasex = BEX-dx*dy;
                                if (releasex<decayx) releasex = decayx;
                        }
                        disp.drawLine(decayx,sustainy,releasex,releasey);
                    }

                    // Maxbend
                    greentxt();
                    disp.print("Bend to: ");
                    normaltxt(); if (menupos==M_MAXBEND) highlighttxt();
                    disp.println(sfx.inst.maxbend);

                    // Bendrate
                    greentxt();
                    disp.print("Bend rate: ");
                    normaltxt(); if (menupos==M_BENDRATE) highlighttxt();
                    disp.println(sfx.inst.bendrate);

                    // Arpeggio
                    greentxt();
                    disp.print("Arp mode: ");
                    normaltxt(); if (menupos==M_ARPMODE) highlighttxt();
                    disp.println((int)sfx.inst.arpmode);

                    // Overdrive
                    greentxt();
                    disp.print("Overdrive: ");
                    normaltxt(); if (menupos==M_OVERDRIVE) highlighttxt();
                    disp.println((int)sfx.inst.overdrive);

                    // Normalize
                    greentxt();
                    disp.print("Normalize: ");
                    normaltxt(); if (menupos==M_NORMALIZE) highlighttxt();
                    disp.println((int)sfx.inst.kick);

                    // Normalize
                    greentxt();
                    disp.print("Save(toggle): ");
                    normaltxt(); if (menupos==M_SAVE) highlighttxt();
                    disp.println((int)save);

                    // The actual command
                    #define SCX 120
                    #define SCY 16
                    disp.setCursor(SCX,SCY);
                    disp.color=3;
                    disp.setFont(fontTiny);
                    disp.print("To make this sound:");
                    disp.setCursor(SCX,SCY+8);
                    disp.color=2;
                    disp.print("setOSC(");
                    disp.color=1;
                    disp.print("&osc1"); disp.color=2;
                    disp.print(","); disp.color=1;
                    disp.print("1"); disp.color=2;
                    disp.print(","); disp.color=1;
                    disp.print(int(sfx.inst.wave));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.loop));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.echo));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.adsr));disp.color=2;disp.print(",");disp.color=1;
                    disp.setCursor(SCX,SCY+16);
                    disp.print(int(sfx.notenumber));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.vol));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.attack));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.decay));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.sustain));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.release));disp.color=2;disp.print(",");disp.color=1;
                    disp.setCursor(SCX,SCY+24);
                    disp.print(int(sfx.inst.maxbend));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.bendrate));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.arpmode));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.overdrive));disp.color=2;disp.print(",");disp.color=1;
                    disp.print(int(sfx.inst.kick));disp.color=2;disp.print(");");disp.color=1;
                    disp.setCursor(SCX,SCY+96);
                    disp.color=3;
                    disp.print("To go faster:");
                    disp.setCursor(SCX,SCY+96+8);
                    disp.print("+A btn = x10");
                    disp.setCursor(SCX,SCY+96+16);
                    disp.print("+B btn = x100");
                    // Master sfx.inst.vol
                    //greentxt();
                    //disp.print("System vol: ");
                    //normaltxt(); if (menupos==M_SYSTEMVOL) highlighttxt();
                    #ifndef POK_SIM
                    //disp.print((int)Pokitto::getHWsfx.inst.vol());
                    //disp.print(", ");
                    #endif // POK_SIM
                    //disp.println((int)snd.getVolume());

                    setOSC(&osc1,1, sfx.inst.wave, sfx.inst.loop, sfx.inst.echo, sfx.inst.adsr,
                    sfx.notenumber, sfx.inst.vol,
                    sfx.inst.attack, sfx.inst.decay, sfx.inst.sustain, sfx.inst.release,
                    sfx.inst.maxbend, sfx.inst.bendrate,
                    sfx.inst.arpmode, sfx.inst.overdrive, sfx.inst.kick);
                    oldvol = (int) snd.getVolume();
                    forcerefresh=false;
            // save cookie if asked to
            if (save) {
            sfx.firstsave=12345678; //not first save anymore
            sfx.saveCookie();
            save=0;
            }
            }
        } else changed--;

        disp.color=0;
        disp.drawColumn(sbx,152-8,disp.height);
        disp.color=1;
        uint8_t tindex = sbindx*2;
        int16_t y = linecenter + (((int)128-(soundbuf[tindex]))>>2);
        if (y<152-8) y = 152-8;
        disp.drawLine(sbx-1,prevy,sbx,y);
        game.update();
        disp.color=0;
        prevy=y;
        sbx++;sbindx++;
        if (sbx>disp.width-1) {sbx=0;}
    }
    return 0; // good manners!
}
