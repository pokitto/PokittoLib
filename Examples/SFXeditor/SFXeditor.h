#ifndef _SFXEDITOR_H
#define _SFXEDITOR_H

#include "PokittoCookie.h"
#include "Synth.h"

class sfxcookie : public Pokitto::Cookie {
public:
    OSC inst;
    int notenumber;
    uint32_t firstsave;

    void init() {
        firstsave=0; //a "checking" number
        notenumber=53;
        inst.vol = 111;//127;
        inst.on=1;
        inst.wave=2;
        inst.loop=1;
        inst.echo=1;
        inst.adsr=1;
        inst.attack=150;
        inst.decay=34;
        inst.sustain=230;
        inst.release=3;
        inst.maxbend=-1600;
        inst.bendrate=-13;
        inst.arpmode = 6;
        inst.overdrive=1;
        inst.kick=0;
    }

    sfxcookie() {
        init();
    }

};

extern sfxcookie sfx;

#endif // _SFXEDITOR_H
