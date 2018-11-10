#ifndef _SFXEDITOR_H
#define _SFXEDITOR_H

#include "PokittoCookie.h"
#include "Synth.h"

class sfxcookie : public Pokitto::Cookie {
public:
    OSC inst;
    int notenumber;
    uint32_t firstsave;
    sfxcookie() {
        firstsave=0; //a "checking" number
        notenumber=46;
        inst.vol = 255;//127;
        inst.on=1;
        inst.wave=0;
        inst.loop=0;
        inst.echo=0;
        inst.adsr=1;
        inst.attack=0;
        inst.decay=0;
        inst.sustain=0;
        inst.release=0;
        inst.maxbend=0;
        inst.bendrate=0;
        inst.arpmode = 0;
        inst.overdrive=0;
        inst.kick=0;
    }
};

extern sfxcookie sfx;

#endif // _SFXEDITOR_H
