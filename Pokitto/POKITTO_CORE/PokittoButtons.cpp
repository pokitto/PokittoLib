/**************************************************************************/
/*!
    @file     PokittoButtons.cpp
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

using namespace Pokitto;

uint8_t Buttons::pins[NUM_BTN];
uint8_t Buttons::states[NUM_BTN];
uint8_t Buttons::buttons_state;
uint8_t Buttons::buttons_held;
uint8_t Buttons::buttons_released; // from LSB up,down,left,right,a,b,c
uint16_t Buttons::cHWLongPress = CHWLONGPRESSTIMEOUT;

void Buttons::update() {
    #if POK_USE_CONSOLE
    if (console.conscounter) return;
    #endif // POK_USE_CONSOLE
    #if PROJ_BUTTONS_POLLING == 1
    pollButtons();
    #endif
    #ifndef POK_SIM
        /** HARDWARE CODE **/
    for (uint8_t thisButton = 0; thisButton < NUM_BTN; thisButton++) {
        if (Pokitto::heldStates[thisButton]) { //if button pressed
            states[thisButton]++; //increase button hold time
            if (states[thisButton]==0xFF) states[thisButton]=2; // PREVENT WRAPAROUND!!!!
        } else {
            if (states[thisButton] == 0)//button idle
                continue;
            if (states[thisButton] == 0xFF)//if previously released
                states[thisButton] = 0; //set to idle
            else
                states[thisButton] = 0xFF; //button just released
        }
        }
    #else
        /** POK_SIM code **/
        simulator.pollButtons();
        for (uint8_t thisButton = 0; thisButton < NUM_BTN; thisButton++) {
        uint8_t temp=0;
        switch (thisButton) {
        case 0:
            temp = simulator.leftHeld(); break;
        case 1:
            temp = simulator.upHeld(); break;
        case 2:
            temp = simulator.rightHeld(); break;
        case 3:
            temp = simulator.downHeld(); break;
        case 4:
            temp = simulator.aHeld(); break;
        case 5:
            temp = simulator.bHeld(); break;
        case 6:
            temp = simulator.cHeld(); break;
        default:
            break;
        }

        if (temp == HIGH) { //if button pressed
            states[thisButton]++; //increase button hold time
            if (states[thisButton]==0xFF) states[thisButton] = 2; //prevent wraparound and retrigger
        } else {
            if (states[thisButton] == 0)//button idle
                continue;
            if (states[thisButton] == 0xFF)//if previously released
                states[thisButton] = 0; //set to idle
            else
                states[thisButton] = 0xFF; //button just released
        }
        }

        #endif // POK_SIM
}

/*
 * Returns true when 'button' is pressed.
 * The button has to be released for it to be triggered again.
 */
bool Buttons::pressed(uint8_t button) {
    if (states[button] == 1)
        return true;
    else
        return false;
}

/*
 * return true if 'button' is released
 */
bool Buttons::released(uint8_t button) {
    if (states[button] == 0xFF) {
        states[button] = 0; // prevent continuous released state!
        return true;
    }
    else
        return false;
}

/**
 * returns true ONCE when 'button' is held for 'time' frames
 * @param button The button's ID
 * @param time How much frames button must be held, between 1 and 254.
 * @return true when 'button' is held for 'time' frames
 */
bool Buttons::held(uint8_t button, uint8_t time){
    if(states[button] == (time+1))
        return true;
    else
        return false;
}

/**
 * returns true every 'period' frames when 'button' is held
 * @param button The button's ID
 * @param period How much frames button must be held, between 1 and 254.
 * @return true if the button is held for the given time
 */
bool Buttons::repeat(uint8_t button, uint8_t period) {
    if (period <= 1) {
        if ((states[button] != 0xFF) && (states[button]))
            return true;
    } else {
        if ((states[button] != 0xFF) && ((states[button] % period) == 1))
            return true;
    }
    return false;
}

/**
 *
 * @param button The button's ID
 * @return The number of frames during which the button has been held.
 */
uint8_t Buttons::timeHeld(uint8_t button){
    if(states[button] != 0xFF)
        return states[button];
    else
        return 0;

}

void Buttons::pollButtons() {
    #ifdef POK_SIM
    simulator.pollButtons();
    #else
    uint8_t buttons_state_old = buttons_state;
    buttons_state = 0; // clear all
    if (upBtn()) buttons_state |= (1<<UPBIT);
    if (downBtn()) buttons_state |= (1<<DOWNBIT);
    if (leftBtn()) buttons_state |= (1<<LEFTBIT);
    if (rightBtn()) buttons_state |= (1<<RIGHTBIT);
    if (aBtn()) buttons_state |= (1<<ABIT);
    if (bBtn()) buttons_state |= (1<<BBIT);
    if (cBtn()) buttons_state |= (1<<CBIT);
    buttons_held = buttons_state & buttons_state_old; // only if both 1, end result is 1
    buttons_released = ~buttons_state & buttons_state_old; // if now zero, then 1 AND previous 1 = 1
    #endif // POK_SIM
}

uint8_t Buttons::aBtn() {
    #ifdef POK_SIM
    return simulator.aBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_A];
    #else
    return Pokitto::Core::aBtn();
    #endif
    #endif // POK_SIM
}


uint8_t Buttons::bBtn() {
    #ifdef POK_SIM
    return simulator.bBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_B];
    #else
    return Pokitto::Core::bBtn();
    #endif
    #endif // POK_SIM
}

uint8_t Buttons::cBtn() {
    uint8_t c;
    #ifdef POK_SIM
    c = simulator.cBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    c = Pokitto::heldStates[BTN_C];
    #else
    return Pokitto::Core::cBtn();
    #endif
    #endif // POK_SIM
    return c;
}

uint8_t Buttons::leftBtn() {
    #ifdef POK_SIM
    return simulator.leftBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_LEFT];
    #else
    return Pokitto::Core::leftBtn();
    #endif
    #endif // POK_SIM
}

uint8_t Buttons::rightBtn() {
    #ifdef POK_SIM
    return simulator.rightBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_RIGHT];
    #else
    return Pokitto::Core::rightBtn();
    #endif
    #endif // POK_SIM
}

uint8_t Buttons::upBtn() {
    #ifdef POK_SIM
    return simulator.upBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_UP];
    #else
    return Pokitto::Core::upBtn();
    #endif
    #endif // POK_SIM
}

uint8_t Buttons::downBtn() {
    #ifdef POK_SIM
    return simulator.downBtn();
    #else
    #if PROJ_BUTTONS_POLLING == 0
    return Pokitto::heldStates[BTN_DOWN];
    #else
    return Pokitto::Core::downBtn();
    #endif
    #endif // POK_SIM
}

#ifndef POK_SIM
/* This functionality was missing from HW and is therefore added here. Simulator has this code in SimButtons.cpp 
The state handling is a bit different between hardware and SDL used in the sim.
*/

uint8_t Buttons::upHeld() { return (uint8_t) Buttons::held(BTN_UP);}
uint8_t Buttons::downHeld() { return (uint8_t) Buttons::held(BTN_DOWN);}
uint8_t Buttons::leftHeld() { return (uint8_t) Buttons::held(BTN_LEFT);}
uint8_t Buttons::rightHeld() { return (uint8_t) Buttons::held(BTN_RIGHT);}
uint8_t Buttons::aHeld() { return (uint8_t) Buttons::held(BTN_A);}
uint8_t Buttons::bHeld() { return (uint8_t) Buttons::held(BTN_B);}
uint8_t Buttons::cHeld() { return (uint8_t) Buttons::held(BTN_C);}

uint8_t Buttons::upReleased() { return (uint8_t) Buttons::released(BTN_UP);}
uint8_t Buttons::downReleased() { return (uint8_t) Buttons::released(BTN_DOWN);}
uint8_t Buttons::leftReleased() { return (uint8_t) Buttons::released(BTN_LEFT);}
uint8_t Buttons::rightReleased() { return (uint8_t) Buttons::released(BTN_RIGHT);}
uint8_t Buttons::aReleased() { return (uint8_t) Buttons::released(BTN_A);}
uint8_t Buttons::bReleased() { return (uint8_t) Buttons::released(BTN_B);}
uint8_t Buttons::cReleased() { return (uint8_t) Buttons::released(BTN_C);}

#endif // POK_SIM






//** EOF **//
