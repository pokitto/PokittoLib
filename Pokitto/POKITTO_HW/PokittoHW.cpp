/**************************************************************************/
/*!
    @file     PokittoHW.cpp
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
#include "loader.h"


using namespace Pokitto;


void Core::quit() {
    run_state = false;
}

void Core::initRandom() {
    //TODO: hook random seed to battery level
}

void Core::initGPIO() {
    /** control lines **/
    LPC_GPIO_PORT->DIR[LCD_CD_PORT] |= (1  << LCD_CD_PIN );
    LPC_GPIO_PORT->DIR[LCD_WR_PORT] |= (1  << LCD_WR_PIN );
    LPC_GPIO_PORT->DIR[LCD_RD_PORT] |= (1  << LCD_RD_PIN );
    LPC_GPIO_PORT->DIR[LCD_RES_PORT] |= (1  << LCD_RES_PIN );
    /** data lines **/
    LPC_GPIO_PORT->DIR[2] |= (0xFFFF  << 3);  // P2_3...P2_18 as output

    pin_mode(P2_3,PullNone); // turn off pull-up
    pin_mode(P2_4,PullNone); // turn off pull-up
    pin_mode(P2_5,PullNone); // turn off pull-up
    pin_mode(P2_6,PullNone); // turn off pull-up

    pin_mode(P2_7,PullNone); // turn off pull-up
    pin_mode(P2_8,PullNone); // turn off pull-up
    pin_mode(P2_9,PullNone); // turn off pull-up
    pin_mode(P2_10,PullNone); // turn off pull-up

    pin_mode(P2_11,PullNone); // turn off pull-up
    pin_mode(P2_12,PullNone); // turn off pull-up
    pin_mode(P2_13,PullNone); // turn off pull-up
    pin_mode(P2_14,PullNone); // turn off pull-up

    pin_mode(P2_15,PullNone); // turn off pull-up
    pin_mode(P2_16,PullNone); // turn off pull-up
    pin_mode(P2_17,PullNone); // turn off pull-up
    pin_mode(P2_18,PullNone); // turn off pull-up
}





