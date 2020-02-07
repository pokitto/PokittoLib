/**************************************************************************/
/*!
    @file     Sound.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

// *** THE COMMON IMPLEMENTATION FOR HWSound.cpp and SimSound.cpp. THIS FILE IS INCLUDED FROM BOTH OF THEM.

// The audio interrupt handler.
inline void pokSoundIRQ() {
    #if POK_ENABLE_SOUND > 0
    uint32_t output=127;
    uint32_t op;
    #ifndef POK_SIM
        #if POK_USE_PWM
        pwmout_t* obj = &audiopwm;
        #endif
    #endif

    #if POK_STREAMING_MUSIC > 0
        // streaming enabled
        #if POK_STREAMFREQ_HALVE
        streamstep = 1-streamstep;
        #else
        streamstep = 1;
        #endif // POK_STREAMFREQ_HALVE

        // SFX sample
        uint8_t sfxSample = 127;
        // If flash-based sample exists, get the sample
        if( Pokitto::Sound::sfxDataPtr != Pokitto::Sound::sfxEndPtr ){
                if( Pokitto::Sound::sfxIs4bitSamples ) {
                    // 4 bit sample
                    if(Pokitto::Sound::sfxBytePos++ == 0) {
                        sfxSample = (*Pokitto::Sound::sfxDataPtr) & 0xf0;  // 4-bit sample is in the high nibble
                    }
                    else {
                        sfxSample = (*Pokitto::Sound::sfxDataPtr++) << 4;  // 4-bit sample is in the low nibble
                        Pokitto::Sound::sfxBytePos = 0;
                    }
                }
                else {
                    // 8 bit sample
                    sfxSample = (*Pokitto::Sound::sfxDataPtr++);  // 8-bit sample
                }
        }

        // SD stream sample
        streamstep &= streamon; // streamon is used to toggle SD music streaming on and off
        if (streamstep) {
            // stream is ON and if it is half-stepped, thats also cool
            output = (*currentPtr++);

            #ifndef PROJ_DISABLE_SD_STREAMING
            if (currentPtr >= endPtr) {
                currentBuffer++;
                if (currentBuffer==4) currentBuffer=0;
                currentPtr = buffers[currentBuffer];
                endPtr = currentPtr + BUFFER_SIZE;
            }
            #endif
        }

        // Mix the samples
        #ifdef PROJ_DISABLE_SD_STREAMING
            // Only SFX
            output = int32_t(sfxSample); // just the sample
        #else
            // SFX and SD
            int32_t s;
            if (streamon) {
                // SD stream active
                s = (int32_t(output) + int32_t(sfxSample)) - 128;
            } else {
                s = int32_t(sfxSample); // just the sample
            }
            if( s < 0 ) s = 0;
            else if( s > 255 ) s = 255;
            output = s;
        #endif

        // sound adjust
        if(streamvol) {
            output >>= 3-streamvol;
            streambyte = output;
            } else {
                streambyte = 0; // duty cycle
                output = 0;
        }
    #endif // POK_STREAMING_MUSIC

    /** DO ADDITIONAL SOUND PROCESSING (NOT STREAM) OF SOUND HERE **/

    #if POK_ENABLE_SYNTH
        /** if song is being played from sd **/
        if (playing) {
                notetick++;
                updatePlayback();
        }
        /** oscillators update **/
        osc1.count += osc1.cinc + (osc1.pitchbend); // counts to 65535 and overflows to zero WAS 8 !
        osc2.count += osc2.cinc + (osc2.pitchbend); // counts to 65535 and overflows to zero
        osc3.count += osc3.cinc + (osc3.pitchbend); // counts to 65535 and overflows to zero
        #if POK_ALT_MIXING > 0 // heaviest cpu load, recalculate envelopes on each cycle
        uint32_t o = 0;
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
        output = 0x80 + (mix_out >> 8);

    #endif // POK_ENABLE_SYNTH

    #ifndef POK_SIM
    /** HARDWARE **/
        #if POK_ENABLE_SOUND > 0
            #if POK_STREAMING_MUSIC > 0
                /** sound is enabled, streaming is enabled */
                #if POK_STREAM_TO_DAC > 0
                    /** stream goes to DAC */
                    #if POK_USE_DAC > 0
                    uint32_t sbyte = streambyte;
                    sbyte *= discrete_vol_multipliers[discrete_vol];
                    sbyte >>= 8;
                    dac_write((uint8_t)sbyte); // duty cycle
                    #endif // POK_USE_DAC
                #else
                    /** stream goes to PWM */
                    if (streamstep) {
                            //pwmout_write(&audiopwm,(float)streambyte/(float)255);
                            #if POK_USE_PWM
                            uint32_t sbyte = streambyte;
                            sbyte *= discrete_vol_multipliers[discrete_vol];
                            sbyte >>= 8;
                            uint32_t t_on = (uint32_t)((((obj->pwm->MATCHREL0)*sbyte)>>8)); //cut out float
                            obj->pwm->MATCHREL1 = t_on;
                            #endif
                            //dac_write((uint8_t)streambyte); // duty cycle
                    }
                #endif // POK_STREAM_TO_DAC
            #endif // POK_STREAMING_MUSIC
            #if POK_STREAM_TO_DAC > 0
                /** synth goes to PWM */
                //pwmout_write(&audiopwm,(float)output/(float)255);
                #if POK_USE_PWM
                    op = output;
                    op *= discrete_vol_multipliers[discrete_vol];
                    op >>= 8;
                    uint32_t t_on = (uint32_t)((((obj->pwm->MATCHREL0)*op)>>8)); //cut out float
                    obj->pwm->MATCHREL1 = t_on;
                #endif
            #else // ! POK_STREAM_TO_DAC
                op = 0x80 + ((((int32_t)(output) - 0x80) * discrete_vol_multipliers[discrete_vol]) >> 8);
                dac_write((uint8_t)op); // SYNTH to DAC
            #endif
            soundbyte = (output+streambyte)>>1;
            soundbuf[soundbufindex++]=soundbyte;
            if (soundbufindex==256) soundbufindex=0;
        #endif //POK_ENABLE_SOUND
    #endif // HARDWARE

    #ifdef POK_SIM
    /** SIMULATOR **/
        #if POK_STREAMING_MUSIC
            //if (streamstep) {
                uint32_t o = output + streambyte;
                output = o/2;
            //}
        #endif // STREAMING
        soundbyte = output;
        soundbuf[soundbufindex++]=soundbyte/3;
        if (soundbufindex==SBUFSIZE) soundbufindex=0;
    #endif // POK_SIM
#endif //POK_ENABLE_SOUND
}
