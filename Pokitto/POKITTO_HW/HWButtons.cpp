/**************************************************************************/
/*!
    @file     HWButtons.cpp
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

#include "HWButtons.h"
#include "PokittoCore.h"
#include "PokittoSound.h"
#include "PokittoDisplay.h"

Pokitto::Sound _s;
Pokitto::Display _bd;

using namespace mbed;

InterruptIn ABtn(POK_BTN_A_PIN);
InterruptIn BBtn(POK_BTN_B_PIN);
InterruptIn CBtn(POK_BTN_C_PIN);
InterruptIn UBtn(POK_BTN_UP_PIN);
InterruptIn DBtn(POK_BTN_DOWN_PIN);
InterruptIn LBtn(POK_BTN_LEFT_PIN);
InterruptIn RBtn(POK_BTN_RIGHT_PIN);

#define BS_IDLE 0
#define BS_DOWN 1
#define BS_UP 2

uint8_t Pokitto::heldStates[NUM_BTN];

void APressed() { Pokitto::heldStates[BTN_A] = 1; }
void AReleased() { Pokitto::heldStates[BTN_A] = 0; }
void BPressed() { Pokitto::heldStates[BTN_B] = 1; }
void BReleased() { Pokitto::heldStates[BTN_B] = 0; }
void CPressed() { Pokitto::heldStates[BTN_C] = 1;}
void CReleased() { Pokitto::heldStates[BTN_C] = 0; }
void UPressed() { Pokitto::heldStates[BTN_UP] = 1; }
void UReleased() { Pokitto::heldStates[BTN_UP] = 0; }
void DPressed() { Pokitto::heldStates[BTN_DOWN] = 1; }
void DReleased() { Pokitto::heldStates[BTN_DOWN] = 0; }
void RPressed() {
    /* Hardware volume control */
    if (Pokitto::heldStates[BTN_C]) _s.volumeUp();
    else Pokitto::heldStates[BTN_RIGHT] = 1;
    }
void RReleased() { Pokitto::heldStates[BTN_RIGHT] = 0; }
void LPressed() {
    /* Hardware volume control */
    if (Pokitto::heldStates[BTN_C]) _s.volumeDown();
    else Pokitto::heldStates[BTN_LEFT] = 1;
    }
void LReleased() { Pokitto::heldStates[BTN_LEFT] = 0; }

void Pokitto::initButtons() {  
  ABtn.fall(&AReleased);
  ABtn.rise(&APressed);
  BBtn.fall(&BReleased);
  BBtn.rise(&BPressed);
  CBtn.fall(&CReleased);
  CBtn.rise(&CPressed);
  UBtn.fall(&UReleased);
  UBtn.rise(&UPressed);
  DBtn.fall(&DReleased);
  DBtn.rise(&DPressed);
  LBtn.fall(&LReleased);
  LBtn.rise(&LPressed);
  RBtn.fall(&RReleased);
  RBtn.rise(&RPressed);
}

uint8_t Pokitto::Core::aBtn() {
    return Pokitto::heldStates[BTN_A];
}

uint8_t Pokitto::Core::bBtn() {
    return Pokitto::heldStates[BTN_B];
}

uint8_t Pokitto::Core::cBtn() {
    return Pokitto::heldStates[BTN_C];
}

uint8_t Pokitto::Core::upBtn() {
    return Pokitto::heldStates[BTN_UP];
}
uint8_t Pokitto::Core::downBtn() {
    return Pokitto::heldStates[BTN_DOWN];
}

uint8_t Pokitto::Core::leftBtn() {
    return Pokitto::heldStates[BTN_LEFT];
}
uint8_t Pokitto::Core::rightBtn() {
    return Pokitto::heldStates[BTN_RIGHT];
}

