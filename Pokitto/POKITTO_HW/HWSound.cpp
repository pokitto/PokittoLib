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

#if (POK_ENABLE_SYNTH == 1)
#include "Synth.h"
#endif

#include "PokittoSound.h"

#include "timer_11u6x.h"
#include "clock_11u6x.h"
#include "HWLCD.h"
//#include "beat_11025.h"


Pokitto::Sound __shw;

using namespace Pokitto;

#ifndef POK_SIM
#if POK_USE_PWM
pwmout_t* obj = &audiopwm;
#endif
#endif

/** Sound Variables **/
#if (POK_STREAMING_MUSIC > 0)

#if POK_HIGH_RAM == HIGH_RAM_MUSIC
unsigned char *buffers[4] = {
    (unsigned char *) 0x20000000,
    (unsigned char *) 0x20000400,
    (unsigned char *) 0x20004000,
    (unsigned char *) 0x20004400
};
#else
    unsigned char buffers[4][BUFFER_SIZE];
#endif

    volatile int currentBuffer = 0, oldBuffer = 0;
    volatile int bufindex = 0, vol=1;
    volatile unsigned char * currentPtr;
    volatile unsigned char * endPtr;
    int8_t streamvol=3;
    uint32_t streamcounter=0;
    uint8_t streamstep=0;
#endif

#if POK_ENABLE_SOUND > 0

#if POK_USE_PWM
	pwmout_t Pokitto::audiopwm; // this way (instead of PwmOut class) pwm doesn't start screaming until it is initialized !
    //Ticker Pokitto::audio;
#endif

using namespace Pokitto;


/** stream output and status */
uint8_t Pokitto::streambyte,Pokitto::streamon;

uint8_t soundbuf[SBUFSIZE];
uint8_t Pokitto::HWvolume=0;
uint16_t soundbufindex;
uint8_t* soundbufptr;

bool volpotError=false; //test for broken MCP4018

uint16_t soundbyte;

sampletype Pokitto::snd[4]; // up to 4 sounds at once?


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

/*
 // 4-layer board rev 2.1 
 DigitalOut dac0(P1_28);
 DigitalOut dac1(P1_29);
 DigitalOut dac2(P1_30);
 DigitalOut dac3(P1_31);
 // has daniel made a mistake
 DigitalOut dac4(P2_20);
 DigitalOut dac5(P2_21);
 DigitalOut dac6(P2_22);
 DigitalOut dac7(P2_23);

 DigitalOut amp(P1_17);
 */

#endif // POK_BOARDREV
#endif // POK_USE_DAC

#if POK_BOARDREV == 2
//MCP4018 volpot(P0_5,P0_4);

/**
 * @brief	Handle interrupt from 32-bit timer 0
 * @return	Nothing
 */

uint32_t p=0;
uint8_t pixx=0, pixy=0;

extern "C" void TIMER32_0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
		//pokSoundBufferedIRQ();
		#if POK_GBSOUND > 0
    	/** GAMEBUINO SOUND **/
    	Pokitto::audio_IRQ();
    	#else
    	/** NOT GAMEBUINO SOUND **/
            #if POK_SOUND_BUFFERED
                pokSoundBufferedIRQ();
            #else
                pokSoundIRQ();
            #endif
		#endif
	}
}


void initHWvolumecontrol() {
    HWvolume=0;
   	volpotError=true;
    //if (volpot.put(HWvolume)) volpotError=true; //try if MCP4018 answers
    setHWvolume(discrete_vol_hw_levels[VOLUME_STARTUP>>5]);
}

int Pokitto::setHWvolume(uint8_t v) {
    HWvolume=v;
    if (HWvolume>127) HWvolume=127;
    //HWvolume = 0x7F&v;
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
    return (LPC_GPIO_PORT->PIN[1]& (1 << 17));
 }

 void Pokitto::ampEnable(uint8_t v) {
    if(v)
        LPC_GPIO_PORT->SET[1] = (1 << 17);
    else
        LPC_GPIO_PORT->CLR[1] = (1 << 17);
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

    //val = value<<28; //lower 4 bits go higher - because port mask is used, no AND is needed to clear bits
    //val += value<<(15-4); //higher 4 bits go lower. No need to shift by 15 because bits are in the higher nibble
    /* daniel has made a mistake with ports */
    //val = ((value&0x70)<<(28-4)); //higher 4 bits go higher - because port mask is used, no AND is needed to clear bits
    //val += value<<(15); //lower 4 bits go lower. No need to shift by 15 because bits are in the higher nibble
    //SET_MASK_DAC;
    //LPC_GPIO_PORT->MPIN[1] = val; // write bits to port
    //CLR_MASK_DAC;
    /* fixed here */
    #define MASKED_DAC 0
    #if MASKED_DAC
    uint32_t val;
    val=value;
    SET_MASK_DAC_LO;
    LPC_GPIO_PORT->MPIN[1] = val<<28; // write lower 4 bits to port
    CLR_MASK_DAC_LO;
    SET_MASK_DAC_HI;
    LPC_GPIO_PORT->MPIN[2] = val<<(20-4); // write bits to port
    CLR_MASK_DAC_HI;
    #else
/*
    CLR_MASK_P2;
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
    SET_MASK_P2;
*/
    //from minilib by FManga
    volatile unsigned char *P1 = (volatile unsigned char *) 0xA0000020;
    volatile unsigned char *P2 = (volatile unsigned char *) 0xA0000040;
    P1[28] = value&1; value >>= 1;
    P1[29] = value&1; value >>= 1;
    P1[30] = value&1; value >>= 1;
    P1[31] = value&1; value >>= 1;
    P2[20] = value&1; value >>= 1;
    P2[21] = value&1; value >>= 1;
    P2[22] = value&1; value >>= 1;
    P2[23] = value;
    #endif //MASKED_DAC
    //CLR_MASK_DAC;
    #endif // BOARDREV
    #endif
}

void Pokitto::soundInit() {
    soundInit(false);
}

/** SOUND INIT **/
void Pokitto::soundInit(uint8_t reinit) {
    #ifdef XPERIMENTAL
    mbed::DigitalOut expr4(EXT4);
    expr4=0;
    #endif
    #if POK_ENABLE_SOUND > 0
    uint32_t timerFreq;
    #if POK_USE_PWM
    if (!reinit) {
    pwmout_init(&audiopwm,POK_AUD_PIN);
    pwmout_period_us(&audiopwm,POK_AUD_PWM_US); //was 31us
    pwmout_write(&audiopwm,0.1f);
    }
    #endif

    //#if POK_GBSOUND > 0
    /** GAMEBUINO SOUND **/
    //audio.attach_us(&audio_IRQ, 1000000/(POK_AUD_FREQ>>0));
    //#else
    /** NOT GAMEBUINO SOUND **/
    //audio.attach_us(&pokSoundBufferedIRQ, 1000000/(POK_AUD_FREQ>>0));
     /* Initialize 32-bit timer 0 clock */
	Chip_TIMER_Init(LPC_TIMER32_0);

    /* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER32_0);

	/* Enable both timers to generate interrupts when time matches */
	Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);

    /* Setup 32-bit timer's duration (32-bit match time) */
	Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / POK_AUD_FREQ));

	/* Setup both timers to restart when match occurs */
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);

	/* Start both timers */
	Chip_TIMER_Enable(LPC_TIMER32_0);

	/* Clear both timers of any pending interrupts */
    #define TIMER_32_0_IRQn 18
	NVIC_ClearPendingIRQ((IRQn_Type)TIMER_32_0_IRQn);

    /* Redirect IRQ vector - Jonne*/
    NVIC_SetVector((IRQn_Type)TIMER_32_0_IRQn, (uint32_t)&TIMER32_0_IRQHandler);

	/* Enable both timer interrupts */
	NVIC_EnableIRQ((IRQn_Type)TIMER_32_0_IRQn);
    //#endif // POK_GAMEBUINO_SUPPORT

    //emptySong();
    //emptyOscillators();
    //emptyBlocks();
    //emptyPatches();
    #ifdef TEST_SOUND
        testOsc();
    #endif // TEST_SOUND
    #if POK_BOARDREV == 2
        //initHWvolumecontrol();
    #endif

    #if POK_ENABLE_SYNTH
        emptyOscillators();
    #endif

    #if POK_USE_DAC && POK_BOARDREV == 2
    // configure dac as output
    volatile unsigned int *PIO1 = (volatile unsigned int *) 0x40044060;
    volatile unsigned int *PIO2 = (volatile unsigned int *) 0x400440F0;
    volatile unsigned int *DIR1 = (volatile unsigned int *) 0xA0002004;
    volatile unsigned int *DIR2 = (volatile unsigned int *) 0xA0002008;
    PIO1[28] = PIO1[29] = PIO1[30] = PIO1[31] = 1<<7;
    PIO2[20] = PIO2[21] = PIO2[22] = PIO2[23] = 1<<7;
    *DIR1 |= (1<<28) | (1<<29) | (1<<30) | (1<<31);
    *DIR2 |= (1<<20) | (1<<21) | (1<<22) | (1<<23);

    // init amp
    LPC_GPIO_PORT->DIR[1] |= (1 << 17);
    LPC_GPIO_PORT->SET[1] = (1 << 17);
    #endif


    #endif // POK_ENABLE_SOUND

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

uint8_t mixSound()
{
    int temp = 0;
    signed int ss[4];
    for(int s=0; s<4; s++){
        snd[s].soundPoint++;

        int currentPos = (snd[s].soundPoint*snd[s].speed)>>8;

        if( currentPos >= snd[s].currentSoundSize){
            if(snd[s].repeat){
                snd[s].soundPoint = snd[s].repeat;
            }else{
                snd[s].playSample=0;
                snd[s].soundPoint=0;
            }
        }

        ss[s] = snd[s].currentSound[currentPos] -128;
        ss[s] *= snd[s].volume;
        ss[s] = ss[s]>>8;
        ss[s] *= snd[s].playSample; // will be 1 or 0, if not playing, will silence

     }
    temp = (ss[0] + ss[1] + ss[2] + ss[3])/4;
    return temp +128;
}


inline void pokSoundBufferedIRQ() {

#if POK_AUD_TRACKER
           uint32_t output = mixSound();
#else
           uint32_t output = soundbuf[soundbufindex+=Pokitto::streamon];
#endif
           if (soundbufindex==SBUFSIZE) soundbufindex=0;
           //if (p==sizeof(beat_11025_raw)) p=0;
           //soundbuf[soundbufindex++] = output;
           //uint32_t t_on = (uint32_t)(((obj->pwm->MATCHREL0)*output)>>8); //cut out float
           //obj->pwm->MATCHREL1 = t_on;
           output *= discrete_vol_multipliers[discrete_vol];
           output >>= 8;
           dac_write(output);

           //setDRAMpoint(pixx, pixy);
}

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

        op = (uint32_t) ((osc1.output)*(osc1.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint32_t) ((osc2.output)*(osc2.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op += (uint32_t) ((osc3.output)*(osc3.vol>>8))>>9;// >> 2 osc1.vol Marr;
        op *= discrete_vol_multipliers[discrete_vol];
        op >>= 8;
        output = op & 0xFF;

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
                op = output;
                op *= discrete_vol_multipliers[discrete_vol];
                op >>= 8;
                dac_write((uint8_t)op); // SYNTH to DAC
            #endif
            soundbyte = (output+streambyte)>>1;
            soundbuf[soundbufindex++]=soundbyte;
            if (soundbufindex==256) soundbufindex=0;
        #endif //POK_ENABLE_SOUND
    #endif // HARDWARE
#endif //POK_ENABLE_SOUND
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


