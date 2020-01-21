/**************************************************************************/
/*!
    @file     PokittoClock.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, Jonne Valola
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

#include "PokittoCore.h"
#include "pinmap.h"
#include "HWLCD.h"
#include "Pokitto_settings.h"
#include "wait_api.h"

volatile uint32_t pt_count=0;
uint32_t* ptimer;

extern "C" {
void SysTick_Handler(void) {
    //interrupt happens when systick has counted down to zero
    #if PROJ_GAMEBOY > 0
        uint32_t a;
        a = *ptimer;
        *ptimer = a + 1;
    #endif
    pt_count+=100;                 // increment counter
}
}

using namespace Pokitto;

uint32_t Core::refreshtime;

#ifdef _OSCT
    #define SYSTEM_CLOCK ((48000000*3)/_OSCT)
#else
    #define SYSTEM_CLOCK 48000000
#endif


void Core::initClock() {
    // Reload value of 24 bit down counter. 4800000-1 gives 100 ms interval on a 48 MHz system
    #if PROJ_GAMEBOY > 0
        ptimer = &pt_count;
        SysTick->LOAD = 4800000-1;
    #else
        SysTick->LOAD = (SYSTEM_CLOCK/10)-1;
    #endif
    SysTick->VAL  = 0;
    SysTick->CTRL  = 4 | 2 | 1; //CLKSOURCE=CPU clock | TICKINT | ENABLE
    pt_count = 0;
}

uint32_t Core::getTime() {
  // Convert SysTick counter value to milliseconds since last interrupt:
  // systick_ms = (4800000-1-SysTick->VAL)/48000
#ifdef _OSCT
    uint32_t systick_ms = ((SYSTEM_CLOCK/10)-1-SysTick->VAL)/(SYSTEM_CLOCK/1000);
#else
    uint32_t systick_ms = ((((SYSTEM_CLOCK/10)-1-SysTick->VAL)>>9)*699)>>16; // div by 48000 => mul by 699>>25
#endif

    return pt_count + systick_ms;
}

uint32_t Core::getTime_us() {
  // Convert SysTick counter value to microseconds since last interrupt:
  // systick_us = (4800000-1-SysTick->VAL)/48

#ifdef _OSCT
    uint32_t systick_us = ((SYSTEM_CLOCK/10)-1-SysTick->VAL)/(SYSTEM_CLOCK/1000000);
#else
    uint32_t systick_us = ((((SYSTEM_CLOCK/10)-1-SysTick->VAL)>>5)*21845)>>15; // div by 48 => mul by 21845>>20
#endif

    return pt_count*1000 + systick_us;
}

void Core::wait(uint16_t ms) {
    wait_ms(ms);
}

