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

//#include "beat_11025.h"


/* Resets the timer terminal and prescale counts to 0 */
void Sound_TIMER_Reset()
{
	uint32_t reg;

	/* Disable timer, set terminal count to non-0 */
	reg = LPC_CT32B0->TCR;
	LPC_CT32B0->TCR = 0;
	LPC_CT32B0->TC = 1;

	/* Reset timer counter */
	LPC_CT32B0->TCR = (1 << 1);

	/* Wait for terminal count to clear */
	while (LPC_CT32B0->TC != 0) {}

	/* Restore timer state */
	LPC_CT32B0->TCR = reg;
}

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
	if ((LPC_CT32B0->IR & (1 << 1)) != 0) {
        LPC_CT32B0->IR = (1 << 1);
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
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 9);

    /* Timer rate is system clock rate */
	timerFreq = SystemCoreClock;

	/* Timer setup for match and interrupt at TICKRATE_HZ */
    Sound_TIMER_Reset();

	/* Enable both timers to generate interrupts when time matches */
    LPC_CT32B0->MCR |= (1 << ((1) * 3));

    /* Setup 32-bit timer's duration (32-bit match time) */
    LPC_CT32B0->MR1 = (timerFreq / POK_AUD_FREQ);

	/* Setup both timers to restart when match occurs */

    LPC_CT32B0->MCR |= (1 << ((1 * 3) + 1));

	/* Start both timers */
	LPC_CT32B0->TCR |= (1 << 0);

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

// *** INCLUDE THE COMMON IMPLEMENTATION FOR HWSound.cpp and SimSound.cpp.
#include "../POKITTO_CORE/SoundCommonImp.h"

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


