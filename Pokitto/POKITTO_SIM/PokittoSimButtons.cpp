/**************************************************************************/
/*!
    @file     PokittoSimButtons.cpp
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

#include "PokittoSimulator.h"
#include "Pokitto_settings.h"
#include "PokittoConsole.h"
#include "PokittoSound.h"
#include "PokittoCore.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace Pokitto;

Pokitto::Sound _s;

uint8_t Simulator::buttons_state;
uint8_t Simulator::buttons_held;
uint8_t Simulator::buttons_released; // from LSB up,down,left,right,a,b,c

void Simulator::pollButtons() {
    uint8_t buttons_state_old = buttons_state;
    //buttons_state = 0; // clear all
    #if POK_USE_CONSOLE > 0
    if (downBtn()&&cReleased()) console.Toggle();
    if (console.conscounter) console.conscounter--;
    #endif // POK_USE_CONSOLE

    SDL_PollEvent(&sdlEvent);
    #if USE_JOYSTICK > 0
    if (joystickPresent) {
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_A)) buttons_state |= (1<<ABIT);
    else buttons_state &= ~(1<<ABIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_B)) buttons_state |= (1<<BBIT);
    else buttons_state &= ~(1<<BBIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_BACK)) buttons_state |= (1<<CBIT);
    else buttons_state &= ~(1<<CBIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_UP)) buttons_state |= (1<<UPBIT);
    else buttons_state &= ~(1<<UPBIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) buttons_state |= (1<<DOWNBIT);
    else buttons_state &= ~(1<<DOWNBIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) buttons_state |= (1<<LEFTBIT);
    else buttons_state &= ~(1<<LEFTBIT);
    if (SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) buttons_state |= (1<<RIGHTBIT);
    else buttons_state &= ~(1<<RIGHTBIT);
    }
    #endif // USE_JOYSTICK

    if (sdlEvent.type == SDL_KEYDOWN) {
        switch (sdlEvent.key.keysym.sym) {
            case SDLK_LEFT:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!leftBtn()) pokConsoleAddMessage(MSOURCE_BTNL,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                if (cBtn()) {
                        _s.volumeDown();
                } else buttons_state |= (1<<LEFTBIT);
                break;
            case SDLK_RIGHT:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!rightBtn()) pokConsoleAddMessage(MSOURCE_BTNR,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                if (cBtn()) {
                        _s.volumeUp();
                } else buttons_state |= (1<<RIGHTBIT);
                break;
            case SDLK_UP:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!upBtn()) pokConsoleAddMessage(MSOURCE_BTNU,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state |= (1<<UPBIT); break;
            case SDLK_DOWN:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!downBtn()) pokConsoleAddMessage(MSOURCE_BTND,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state |= (1<<DOWNBIT); break;
            case SDLK_z:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!aBtn()) pokConsoleAddMessage(MSOURCE_BTNA,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state |= (1<<ABIT); break;
            case SDLK_x:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!bBtn()) pokConsoleAddMessage(MSOURCE_BTNB,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state |= (1<<BBIT); break;
            case SDLK_c:
                #if POK_CONSOLE_LOG_BUTTONS
                if (!cBtn()) pokConsoleAddMessage(MSOURCE_BTNC,MSG_DOWN);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state |= (1<<CBIT);
                break;
            case SDLK_ESCAPE:
                mutex=1; //stop screen refresh
                killSound();
                quit();
        }
    } else if (sdlEvent.type == SDL_KEYUP) {
            switch (sdlEvent.key.keysym.sym) {
            case SDLK_LEFT:
                #if POK_CONSOLE_LOG_BUTTONS
                if (leftBtn()) pokConsoleAddMessage(MSOURCE_BTNL,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<LEFTBIT); break;
            case SDLK_RIGHT:
                #if POK_CONSOLE_LOG_BUTTONS
                if (rightBtn()) pokConsoleAddMessage(MSOURCE_BTNR,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<RIGHTBIT); break;
            case SDLK_UP:
                #if POK_CONSOLE_LOG_BUTTONS
                if (upBtn()) pokConsoleAddMessage(MSOURCE_BTNU,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<UPBIT); break;
            case SDLK_DOWN:
                #if POK_CONSOLE_LOG_BUTTONS
                if (downBtn()) pokConsoleAddMessage(MSOURCE_BTND,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<DOWNBIT); break;
            case SDLK_z:
                #if POK_CONSOLE_LOG_BUTTONS
                if (aBtn())  pokConsoleAddMessage(MSOURCE_BTNA,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<ABIT); break;
            case SDLK_x:
                #if POK_CONSOLE_LOG_BUTTONS
                if (bBtn()) pokConsoleAddMessage(MSOURCE_BTNB,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<BBIT); break;
            case SDLK_c:
                #if POK_CONSOLE_LOG_BUTTONS
                if (cBtn()) pokConsoleAddMessage(MSOURCE_BTNC,MSG_UP);
                #endif // POK_CONSOLE_LOG_BUTTONS
                buttons_state &=~ (1<<CBIT); break;
            case SDLK_ESCAPE:
                mutex=1; //stop screen refresh
                killSound();
                quit();
            }
    } else if (sdlEvent.type == SDL_QUIT) {
        quit();
    }
    buttons_held = buttons_state & buttons_state_old; // only if both 1, end result is 1
    buttons_released = ~buttons_state & buttons_state_old; // if now zero, then 1 AND previous 1 = 1
}

uint8_t Simulator::upHeld() { if (buttons_held & (1<<UPBIT)) return 1; return 0;}
uint8_t Simulator::downHeld() { if (buttons_held & (1<<DOWNBIT)) return 1; return 0;}
uint8_t Simulator::leftHeld() { if (buttons_held & (1<<LEFTBIT)) return 1; return 0;}
uint8_t Simulator::rightHeld() { if (buttons_held & (1<<RIGHTBIT)) return 1; return 0;}
uint8_t Simulator::aHeld() { if (buttons_held & (1<<ABIT)) return 1; return 0;}
uint8_t Simulator::bHeld() { if (buttons_held & (1<<BBIT)) return 1; return 0;}
uint8_t Simulator::cHeld() { if (buttons_held & (1<<CBIT)) return 1; return 0;}

uint8_t Simulator::upReleased() { if (buttons_released & (1<<UPBIT)) return 1; return 0;}
uint8_t Simulator::downReleased() { if (buttons_released & (1<<DOWNBIT)) return 1; return 0;}
uint8_t Simulator::leftReleased() { if (buttons_released & (1<<LEFTBIT)) return 1; return 0;}
uint8_t Simulator::rightReleased() { if (buttons_released & (1<<RIGHTBIT)) return 1; return 0;}
uint8_t Simulator::aReleased() { if (buttons_released & (1<<ABIT)) return 1; return 0;}
uint8_t Simulator::bReleased() { if (buttons_released & (1<<BBIT)) return 1; return 0;}
uint8_t Simulator::cReleased() { if (buttons_released & (1<<CBIT)) return 1; return 0;}

uint8_t Simulator::leftBtn(void) { return buttons_state & (1<<LEFTBIT);}
uint8_t Simulator::rightBtn(void) { return buttons_state & (1<<RIGHTBIT);}
uint8_t Simulator::upBtn(void) { return buttons_state & (1<<UPBIT);}
uint8_t Simulator::downBtn(void) { return buttons_state & (1<<DOWNBIT);}
uint8_t Simulator::aBtn(void) { return buttons_state & (1<<ABIT);}
uint8_t Simulator::bBtn(void) { return buttons_state & (1<<BBIT);}
uint8_t Simulator::cBtn(void) { return buttons_state & (1<<CBIT);}
