/**************************************************************************/
/*!
    @file     PokittoClock.cpp
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

#include "PokittoCore.h"
#include "PokittoConsole.h"
#include "Pokitto_settings.h"
#include <SDL2/SDL.h>

using namespace Pokitto;

uint32_t Core::refreshtime;
uint64_t pt_count;
uint32_t* ptimer;


void Core::initClock() {
    #if POK_USE_CONSOLE > 0
    console.AddMessage(MSOURCE_TIMER,MSG_INIT_OK);
    #endif // POK_USE_CONSOLE
    pt_count=0;
}

unsigned int Core::getTime() {
    //pt_count += SDL_GetTicks()-pt_count;
    pt_count = SDL_GetTicks();
    return pt_count;
}

void Core::wait(uint16_t ms) {
    uint16_t until=getTime() + ms;
    while (getTime() < until) {
            simulator.refreshDisplay();
    }
}

