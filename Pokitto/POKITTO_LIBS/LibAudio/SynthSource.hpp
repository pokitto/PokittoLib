#pragma once

#include "Synth.h"

namespace Audio {

    class SynthSource {

        static u8 update(){
            if (playing) {
                notetick++;
                updatePlayback();
            }
            /** oscillators update **/
            osc1.count += osc1.cinc + (osc1.pitchbend); // counts to 65535 and overflows to zero WAS 8 !
            osc2.count += osc2.cinc + (osc2.pitchbend); // counts to 65535 and overflows to zero
            osc3.count += osc3.cinc + (osc3.pitchbend); // counts to 65535 and overflows to zero
#if POK_ALT_MIXING > 0 // heaviest cpu load, recalculate envelopes on each cycle
            Marr[3]();
            Marr[2]();
            Marr[1]();
            if (tick==0) Marr[0]();
#else
            Marr[tick](); // call mixing function
#endif // ALT_MIXING
            --tick;

            /** mixing oscillator output **/

            int32_t mix_out = (((int32_t)(osc1.output) - 0x8000) * osc1.vol) >> 16;
            mix_out += (((int32_t)(osc2.output) - 0x8000) * osc2.vol) >> 16;
            mix_out += (((int32_t)(osc3.output) - 0x8000) * osc3.vol) >> 16;
            mix_out = (mix_out < -0x8000) ? -0x8000 : (mix_out > 0x7fff) ? 0x7fff : mix_out;
            return 0x80 + (mix_out >> 8);
        }

        static void copy(u8 *buffer, void *ptr){
            for(u32 i = 0; i < 512; ++i){
                buffer[i] = update();
            }
        }

        static void mix(u8 *buffer, void *ptr){
            for(u32 i = 0; i < 512; ++i){
                buffer[i] = Audio::mix(buffer[i], update());
            }
        }

    public:
        template<u32 channel = 0>
        static void playOSC(){
            Audio::connect(
                channel,
                nullptr,
                channel == 0 ? copy : mix);
        }

    };

    template<u32 channel = 0>
    inline void play(u32 oscId, const OSC& osc){
        SynthSource::playOSC<channel>();
        OSC *oscillators[] = {&osc1, &osc2, &osc3, &osc3};
        *oscillators[oscId&0x3] = osc;
    }

    struct Note {
        constexpr Note(u32 notenumber=25) : _notenumber(notenumber){}

        template<u32 channel = 0>
        void play(u32 oscId = ~0U) const {
            if(oscId == ~0U) oscId = _oscId;
            Audio::play<channel>(oscId, *this);
        }

        uint8_t _oscId = 0;
        constexpr Note& osc(uint8_t oscId){ _oscId = oscId; return *this; }

        uint8_t _wave = 1;
        constexpr Note& wave(uint8_t v){ _wave = v; return *this; }

        bool _loop = 0;
        constexpr Note& loop(bool v){ _loop = v; return *this; }

        bool _echo = 0;
        constexpr Note& echo(bool v){ _echo = v; return *this; }

        bool _adsr = 0;
        constexpr Note& adsr(bool v){ _adsr = v; return *this; }

        uint8_t _notenumber = 25;
        constexpr Note& noteNumber(uint8_t v){ _notenumber = v; return *this; }

        uint16_t _volume = 127;
        constexpr Note& volume(uint16_t v){ _volume = v; return *this; }

        uint16_t _attack = 0;
        constexpr Note& attack(uint16_t v){
            _adsr = 1;
            _attack = v;
            return *this;
        }

        uint16_t _decay = 0;
        constexpr Note& decay(uint16_t v){
            _adsr = 1;
            _decay = v;
            return *this;
        }

        uint16_t _sustain = 0;
        constexpr Note& sustain(uint16_t v){
            _adsr = 1;
            _sustain = v;
            return *this;
        }

        uint16_t _release = 0;
        constexpr Note& release(uint16_t v){
            _adsr = 1;
            _release = v;
            return *this;
        }

        int16_t _maxbend = 0;
        constexpr Note& maxbend(int16_t v){ _maxbend = v; return *this; }

        int16_t _bendrate = 0;
        constexpr Note& bendrate(int16_t v){ _bendrate = v; return *this; }

        uint8_t _arpmode = 0;
        constexpr Note& arpmode(uint8_t v){ _arpmode = v; return *this; }

        uint8_t _overdrive = 0;
        constexpr Note& overdrive(uint8_t v){ _overdrive = v; return *this; }

        uint8_t _kick = 0;
        constexpr Note& kick(uint8_t v){ _kick = v; return *this; }

        uint32_t _duration = ~0;
        constexpr Note& duration(uint32_t v){ _duration = v; return *this; }

        operator OSC () const {
            OSC osc;
            setOSC(&osc, 1,
                   _wave,  _loop,  _echo,  _adsr,
                   _notenumber,  _volume,
                   _attack,  _decay,  _sustain,  _release,
                   _maxbend,  _bendrate,  _arpmode,  _overdrive,  _kick
                );
            osc.duration = _duration;
            return osc;
        }
    };
}
