/**************************************************************************/
/*!
    @file     HWSOUND.h
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
#ifndef HWSOUND_H
#define HWSOUND_H

#include "mbed.h"
#include "Pokitto_settings.h"

#define SPEAKER 3
#define BUFFER_SIZE 512*4 //*8 //*8 // 512 // was 512 (works really well with crabator) was 256

#if POK_BOARDREV == 1
 /** 2-layer board rev 1.3 **/
 #define DAC0_PORT   1
 #define DAC0_PIN    6
 #define DAC1_PORT   1
 #define DAC1_PIN    0
 #define DAC2_PORT   1
 #define DAC2_PIN    16
 #define DAC3_PORT   0
 #define DAC3_PIN    19
 #define DAC4_PORT   0
 #define DAC4_PIN    17
 #define DAC5_PORT   1
 #define DAC5_PIN    12
 #define DAC6_PORT   1
 #define DAC6_PIN    15
 #define DAC7_PORT   1
 #define DAC7_PIN    8
#else
 /** 4-layer board rev 2.1 **/
 #define DAC0_PORT   1
 #define DAC0_PIN    28
 #define DAC1_PORT   1
 #define DAC1_PIN    29
 #define DAC2_PORT   1
 #define DAC2_PIN    30
 #define DAC3_PORT   1
 #define DAC3_PIN    31
/** has daniel made a mistake with dac ? **/
 #define DAC4_PORT   2
 #define DAC4_PIN    20
 #define DAC5_PORT   2
 #define DAC5_PIN    21
 #define DAC6_PORT   2
 #define DAC6_PIN    22
 #define DAC7_PORT   2
 #define DAC7_PIN    23

 /** MASK FOR FASTER DAC **/
 #define SET_MASK_DAC LPC_GPIO_PORT->MASK[1] = ~(0xF0078000); //mask P1_15...P1_18 and P1_28...P1_31
 #define CLR_MASK_DAC LPC_GPIO_PORT->MASK[1] = 0; // all on

 /* fixing wrong pins from daniel*/
 #define SET_MASK_DAC_LO LPC_GPIO_PORT->MASK[1] = ~(0xF0000000); //mask P1_28...P1_31
 #define CLR_MASK_DAC_LO LPC_GPIO_PORT->MASK[1] = 0; // all on
 #define SET_MASK_DAC_HI LPC_GPIO_PORT->MASK[2] = ~(0xF00000); //mask P2_20...P2_23
 #define CLR_MASK_DAC_HI LPC_GPIO_PORT->MASK[2] = 0; // all on

#endif

#define CLR_DAC0 LPC_GPIO_PORT->CLR[DAC0_PORT] = 1UL << DAC0_PIN;
#define SET_DAC0 LPC_GPIO_PORT->SET[DAC0_PORT] = 1UL << DAC0_PIN;
#define CLR_DAC1 LPC_GPIO_PORT->CLR[DAC1_PORT] = 1UL << DAC1_PIN;
#define SET_DAC1 LPC_GPIO_PORT->SET[DAC1_PORT] = 1UL << DAC1_PIN;
#define CLR_DAC2 LPC_GPIO_PORT->CLR[DAC2_PORT] = 1UL << DAC2_PIN;
#define SET_DAC2 LPC_GPIO_PORT->SET[DAC2_PORT] = 1UL << DAC2_PIN;
#define CLR_DAC3 LPC_GPIO_PORT->CLR[DAC3_PORT] = 1UL << DAC3_PIN;
#define SET_DAC3 LPC_GPIO_PORT->SET[DAC3_PORT] = 1UL << DAC3_PIN;
#define CLR_DAC4 LPC_GPIO_PORT->CLR[DAC4_PORT] = 1UL << DAC4_PIN;
#define SET_DAC4 LPC_GPIO_PORT->SET[DAC4_PORT] = 1UL << DAC4_PIN;
#define CLR_DAC5 LPC_GPIO_PORT->CLR[DAC5_PORT] = 1UL << DAC5_PIN;
#define SET_DAC5 LPC_GPIO_PORT->SET[DAC5_PORT] = 1UL << DAC5_PIN;
#define CLR_DAC6 LPC_GPIO_PORT->CLR[DAC6_PORT] = 1UL << DAC6_PIN;
#define SET_DAC6 LPC_GPIO_PORT->SET[DAC6_PORT] = 1UL << DAC6_PIN;
#define CLR_DAC7 LPC_GPIO_PORT->CLR[DAC7_PORT] = 1UL << DAC7_PIN;
#define SET_DAC7 LPC_GPIO_PORT->SET[DAC7_PORT] = 1UL << DAC7_PIN;

/** the output holder **/
extern uint16_t soundbyte;

namespace Pokitto {


/** stream output and status */
extern uint8_t streambyte, streamon, HWvolume;

extern float pwm2; //virtual pwm output

extern void soundInit();
extern void dac_write(uint8_t value);
extern uint8_t ampIsOn();
extern void ampEnable(uint8_t v);
extern void audio_IRQ();
extern void updateSDAudioStream();
extern uint8_t streamPaused();
extern void pauseStream();
extern void playStream();
extern int setHWvolume(uint8_t);
extern uint8_t getHWvolume();
extern void changeHWvolume(int8_t);

extern pwmout_t audiopwm;
extern uint8_t pokAmpIsOn();
extern void pokAmpEnable(uint8_t);

extern Ticker audio;

extern void update_SDAudioStream();
}


extern void pokSoundIRQ();

#if POK_STREAMING_MUSIC > 0
    extern unsigned char buffers[][BUFFER_SIZE];
    extern volatile int currentBuffer, oldBuffer;
    extern volatile int bufindex, vol;
    extern volatile unsigned char * currentPtr;
    extern volatile unsigned char * endPtr;
    extern int8_t streamvol;
    extern uint32_t streamcounter;
    extern uint8_t streamstep;
#endif

#endif //HWSOUND_H


