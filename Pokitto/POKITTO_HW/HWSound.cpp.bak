/**************************************************************************/
/*!
    @file     HWSound.cpp
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

#include "mbed.h"
#include "HWSound.h"
//#include "MCP4018.h"
#include "SoftwareI2C.h"
#include "Pokitto_settings.h"
#include "PokittoDisk.h"
#include "PokittoGlobs.h"
#include "Synth.h"



using namespace Pokitto;

/** Sound Variables **/
#if (POK_STREAMING_MUSIC > 0)
    unsigned char buffers[4][BUFFER_SIZE];
    volatile int currentBuffer = 0, oldBuffer = 0;
    volatile int bufindex = 0, vol=1;
    volatile unsigned char * currentPtr;
    volatile unsigned char * endPtr;
    int8_t streamvol=3;
    uint32_t streamcounter=0;
    uint8_t streamstep=0;
#endif

#if POK_ENABLE_SOUND > 0
	pwmout_t Pokitto::audiopwm; // this way (instead of PwmOut class) pwm doesn't start screaming until it is initialized !
    Ticker Pokitto::audio;

using namespace Pokitto;

/** stream output and status */
uint8_t Pokitto::streambyte,Pokitto::streamon;

uint8_t soundbuf[256], soundbufindex=0, Pokitto::HWvolume=0;
bool volpotError=false; //test for broken MCP4018

uint16_t soundbyte;


#if POK_USE_DAC > 0
#if POK_BOARDREV == 1
 /** 2-layer board rev 1.3 **/
 DigitalOut dac0(P1_6);
 DigitalOut dac1(P1_0);
 DigitalOut dac2(P1_16);
 DigitalOut dac3(P0_19);
 DigitalOut dac4(P0_17);
 DigitalOut dac5(P1_12);
 DigitalOut dac6(P1_15);
 DigitalOut dac7(P1_8);
#else
 /** 4-layer board rev 2.1 **/
 DigitalOut dac0(P1_28);
 DigitalOut dac1(P1_29);
 DigitalOut dac2(P1_30);
 DigitalOut dac3(P1_31);
 /* has daniel made a mistake ?*/
 DigitalOut dac4(P2_20);
 DigitalOut dac5(P2_21);
 DigitalOut dac6(P2_22);
 DigitalOut dac7(P2_23);

 DigitalOut amp(P1_17);

#endif // POK_BOARDREV
#endif // POK_USE_DAC

#if POK_BOARDREV == 2
//MCP4018 volpot(P0_5,P0_4);

void initHWvolumecontrol() {
    HWvolume=0;
   	volpotError=true;
    //if (volpot.put(HWvolume)) volpotError=true; //try if MCP4018 answers
    setHWvolume(VOLUME_STARTUP);
}

int Pokitto::setHWvolume(uint8_t v) {
    HWvolume = 0x7F&v;
    //if (!volpotError) return volpot.put(HWvolume); //use normal I2C
    /* fallback method for broken MCP4018 */
    SoftwareI2C swvolpot(P0_4, P0_5); //swapped SDA,SCL
    swvolpot.write(0x5e,HWvolume);
	return HWvolume;
}

uint8_t Pokitto::getHWvolume() {
    return HWvolume;
}

void Pokitto::changeHWvolume(int8_t v) {
    int temp  = HWvolume + v;
    if (temp < 0) temp = 0; //prevent volume "looparound" than can make a massive crack
    if (temp > 127) temp = 127;
    setHWvolume(temp);
}

 uint8_t Pokitto::ampIsOn() {
    return amp;
 }

 void Pokitto::ampEnable(uint8_t v) {
    if (v>1) v=1; // limit against funny values
    amp=v;
 }
#endif // POK_BOARDREV == 2

void Pokitto::dac_write(uint8_t value) {
    #if POK_USE_DAC > 0
    #if POK_BOARDREV == 1 // was 1
    if (value & 1) SET_DAC0 else CLR_DAC0;
    value >>= 1;
    if (value & 1) SET_DAC1 else CLR_DAC1;
    value >>= 1;
    if (value & 1) SET_DAC2 else CLR_DAC2;
    value >>= 1;
    if (value & 1) SET_DAC3 else CLR_DAC3;
    value >>= 1;
    if (value & 1) SET_DAC4 else CLR_DAC4;
    value >>= 1;
    if (value & 1) SET_DAC5 else CLR_DAC5;
    value >>= 1;
    if (value & 1) SET_DAC6 else CLR_DAC6;
    value >>= 1;
    if (value & 1) SET_DAC7 else CLR_DAC7;
    #else
    //uint32_t val;
    //val = value<<28; //lower 4 bits go higher - because port mask is used, no AND is needed to clear bits
    //val += value<<(15-4); //higher 4 bits go lower. No need to shift by 15 because bits are in the higher nibble
    /* daniel has made a mistake with ports */
    //val = ((value&0x70)<<(28-4)); //higher 4 bits go higher - because port mask is used, no AND is needed to clear bits
    //val += value<<(15); //lower 4 bits go lower. No need to shift by 15 because bits are in the higher nibble
    //SET_MASK_DAC;
    //LPC_GPIO_PORT->MPIN[1] = val; // write bits to port
    //CLR_MASK_DAC;
    /* fixed here */
    /*val=value;
    SET_MASK_DAC_LO;
    LPC_GPIO_PORT->MPIN[1] = val<<28; // write lower 4 bits to port
    CLR_MASK_DAC_LO;
    SET_MASK_DAC_HI;
    LPC_GPIO_PORT->MPIN[2] = val<<(20-4); // write bits to port
    CLR_MASK_DAC_HI; */
    if (value & 1) SET_DAC0 else CLR_DAC0;
    value >>= 1;
    if (value & 1) SET_DAC1 else CLR_DAC1;
    value >>= 1;
    if (value & 1) SET_DAC2 else CLR_DAC2;
    value >>= 1;
    if (value & 1) SET_DAC3 else CLR_DAC3;
    value >>= 1;
    if (value & 1) SET_DAC4 else CLR_DAC4;
    value >>= 1;
    if (value & 1) SET_DAC5 else CLR_DAC5;
    value >>= 1;
    if (value & 1) SET_DAC6 else CLR_DAC6;
    value >>= 1;
    if (value & 1) SET_DAC7 else CLR_DAC7;
    //CLR_MASK_DAC;
    #endif // BOARDREV
    #endif
}

/** SOUND INIT **/
void Pokitto::soundInit() {

    pwmout_init(&audiopwm,POK_AUD_PIN);
    pwmout_period_us(&audiopwm,POK_AUD_PWM_US); //was 31us
    pwmout_write(&audiopwm,0.1f);

    #if POK_GBSOUND > 0
    /** GAMEBUINO SOUND **/
    audio.attach_us(&audio_IRQ, 1000000/(POK_AUD_FREQ>>0));
    #else
    /** NOT GAMEBUINO SOUND **/
    audio.attach_us(&pokSoundIRQ, 1000000/(POK_AUD_FREQ>>0));
    #endif // POK_GAMEBUINO_SUPPORT

    //emptySong();
    //emptyOscillators();
    //emptyBlocks();
    //emptyPatches();
    #ifdef TEST_SOUND
        testOsc();
    #endif // TEST_SOUND
    #if POK_BOARDREV == 2
        initHWvolumecontrol();
    #endif

}


uint8_t Pokitto::streamPaused() {
    return !streamon;
}

void Pokitto::pauseStream() {
    streamon=0;
}

void Pokitto::playStream() {
    streamon=1;
}


void pokPauseStream() {
    streamon=0;
}

void pokPlayStream() {
    streamon=1;
}

void pokSoundIRQ() {
    uint8_t output=0;
    #ifndef POK_SIM
    //pwmout_t* obj = &audiopwm;
    #endif
    #if POK_STREAMING_MUSIC > 0
        #if POK_STREAMFREQ_HALVE
        streamstep = 1-streamstep;
        #else
        streamstep = 1;
        #endif // POK_STREAMFREQ_HALVE
        streamstep &= streamon; // streamon is used to toggle SD music streaming on and off
        if (streamstep) {
            output = (*currentPtr++);
            if(streamvol && streamon) {
                output >>= 3-streamvol;
                streambyte = output;
            } else {
                streambyte = 0; // duty cycle
                output = 0;
            }
            if (currentPtr >= endPtr)
            {
            currentBuffer++;
            if (currentBuffer==4) currentBuffer=0;
            currentPtr = buffers[currentBuffer];
            endPtr = currentPtr + BUFFER_SIZE;
            }
        }
    #endif // POK_STREAMING_MUSIC

    /** DO ADDITIONAL SOUND PROCESSING (NOT STREAM) OF SOUND HERE **/

    #if POK_ENABLE_SYNTH
        /** if song is being played from sd **/
        if (playing) {
                notetick++;
                updatePlaybackSD(playerpos&7);
        }
        /** oscillators update **/
        osc1.count += osc1.cinc + (osc1.pitchbend >> 4); // counts to 65535 and overflows to zero WAS 8 !
        osc2.count += osc2.cinc + (osc2.pitchbend >> 4); // counts to 65535 and overflows to zero
        osc3.count += osc3.cinc + (osc3.pitchbend >> 4); // counts to 65535 and overflows to zero
        Marr[tick](); // call mixing function
        --tick;

        /** mixing oscillator output **/

        uint16_t op = (uint16_t) ((osc1.output)*(osc1.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc2.output)*(osc2.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint16_t) ((osc3.output)*(osc3.vol>>8))>>9;// >> 2 osc1.vol Marr;
        output = (uint8_t) op;

    #endif // POK_ENABLE_SYNTH

    #ifndef POK_SIM
    /** HARDWARE **/
        #if POK_ENABLE_SOUND > 0
            #if POK_STREAMING_MUSIC > 0
                /** sound is enabled, streaming is enabled */
                #if POK_STREAM_TO_DAC > 0
                    /** stream goes to DAC */
                    #if POK_USE_DAC > 0
                    if (streamstep) dac_write((uint8_t)streambyte); // duty cycle
                    #endif // POK_USE_DAC
                #else
                    /** stream goes to PWM */
                    if (streamstep) {
                            pwmout_write(&audiopwm,(float)streambyte/(float)255);
                            //uint32_t t_on = (uint32_t)(((obj->pwm->MATCHREL0)*streambyte)>>8); //cut out float
                            //obj->pwm->MATCHREL1 = t_on;
                            //dac_write((uint8_t)streambyte); // duty cycle
                    }
                #endif // POK_STREAM_TO_DAC
            #endif // POK_STREAMING_MUSIC
            #if POK_STREAM_TO_DAC > 0
            /** synth goes to PWM */
            pwmout_write(&audiopwm,(float)output/(float)255);
            //uint32_t t_on = (uint32_t)(((obj->pwm->MATCHREL0)*output)>>8); //cut out float
            //obj->pwm->MATCHREL1 = t_on;
            #else
            dac_write((uint8_t)output);
            #endif // decide where synth is output
            soundbyte = (output+streambyte)>>1;
            soundbuf[soundbufindex++]=soundbyte;
        #endif //POK_ENABLE_SOUND
    #endif // HARDWARE
}


void Pokitto::updateSDAudioStream() {
    if (streamPaused()) return;

    #if POK_STREAMING_MUSIC > 0
    if (oldBuffer != currentBuffer) {
        if (currentBuffer==0) fileReadBytes(&buffers[3][0],BUFFER_SIZE);
        else if (currentBuffer==1) fileReadBytes(&buffers[0][0],BUFFER_SIZE);
        else if (currentBuffer==2) fileReadBytes(&buffers[1][0],BUFFER_SIZE);
        else fileReadBytes(&buffers[2][0],BUFFER_SIZE);
        oldBuffer = currentBuffer;
        streamcounter += BUFFER_SIZE;
    } else return;

    #ifndef POK_SIM
    if ( streamcounter > fs.fsize - (BUFFER_SIZE*6)) {
    #else
    if ( streamcounter > getFileLength() - (BUFFER_SIZE*6)) {
    #endif
        streamcounter=0;
        #if POK_STREAM_LOOP
        fileRewind();
        #else
        pokPauseStream();
        #endif
    }
    #endif
}


#endif // POK_ENABLE_SOUND


