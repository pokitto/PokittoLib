/**************************************************************************/
/*!
    @file     PokittoSimCore.cpp
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


/** SimCore.cpp interfaces the purely Pokitto functions (Core::)
* with Simulator (Simulator::) functions
*/

#include "PokittoCore.h"
#include "PokittoSimulator.h"
#include <time.h>


/** Functions, that are equivalents of the hardware functions in PokittoHW*/

using namespace Pokitto;




void Core::quit() {
    run_state = false;
}

void Core::initRandom() {
    //TODO: hook random seed to battery level
    time_t t;
    time(&t);
    t = t&0xFF;
    srand((unsigned int) t);
}

void Core::initGPIO() {
    // Dummy function, will probably never "do" anything
}

void Core::initAudio() {
    #if SIM_SDL_AUDIO > 0
    simulator.initSDLAudio();
    #endif // SIM_SDL_AUDIO
}

void Core::pollButtons() {
    simulator.pollButtons();
}

uint8_t Core::aBtn() {
    return simulator.aBtn();
}

uint8_t Core::bBtn() {
    return simulator.bBtn();
}

uint8_t Core::cBtn() {
    return simulator.cBtn();
}

uint8_t Core::upBtn() {
    return simulator.upBtn();
}
uint8_t Core::downBtn() {
    return simulator.downBtn();
}

uint8_t Core::leftBtn() {
    return simulator.leftBtn();
}
uint8_t Core::rightBtn() {
    return simulator.rightBtn();
}

 uint8_t Core::ampIsOn() {
    return 1-simulator.amp; // enabled is LOW, so return the inverse !!
 }

 void Core::ampEnable(uint8_t v) {
    if (v>1) v=1; // limit against funny values
    simulator.amp=1-v; // Because v=true=enabled, but LOW is the enable condition
 }

/** Functions, that are specific to simulation */


