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

uint32_t pt_count=0;
uint32_t* ptimer;

extern "C" {
void SysTick_Handler(void) {
    //interrupt happens when systick has counted down to zero
    #if PROJ_GAMEBOY > 0
    uint32_t a;
    a = *ptimer;
    *ptimer = a + 1;
    #endif
    pt_count+=10;                        // increment counter
}}

using namespace Pokitto;

uint32_t Core::refreshtime;

void Core::initClock() {
        // to get 1000 interrupts per second the reload value should be 48000
    #if PROJ_GAMEBOY > 0
    ptimer = &pt_count;
    SysTick->LOAD = 480000-1;
    #else
    SysTick->LOAD = 480000-1;
    #endif
    SysTick->VAL  = 0;
    SysTick->CTRL  = 4 | 2 | 1; //CLKSOURCE=CPU clock | TICKINT | ENABLE
    pt_count = 0;
}

uint32_t Core::getTime() {
    return pt_count;
}

void Core::wait(uint16_t ms) {
    wait_ms(ms);
}

