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
    static void playOSC(){
        SynthSource::playOSC<channel>();
    }
}
