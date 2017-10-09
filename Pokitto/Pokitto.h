/**************************************************************************/
/*!
    @file     Pokitto.h
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

#ifndef __POKITTO_H__
#define __POKITTO_H__

#include "Pokitto_settings.h"
#include "PokittoCore.h"

//typedef void (*funcptr)(PokittoApp&);

#ifndef POK_SIM
    #include "mbed.h"
    #include "HWLCD.h"
    #include "PokittoTimer.h"
    #include "Pokitto_extport.h"
    #include "PokittoEEPROM.h"
#else
    #include "PokittoSimulator.h"
#endif // if not POK_SIM


#if POK_ENABLE_SD > 0
    #ifndef POK_SIM
        #include "PokittoDisk.h"
    #else
        #include "FileIO.h"
    #endif // if not POK_SIM
    //#include "ImageFormat.h"
#endif // if SD ENABLED

#if POK_ENABLE_SOUND > 0
    #if POK_ENABLE_SYNTH > 0
    #include "Synth.h"
    #endif
    extern uint8_t soundbuf[];
    #ifndef POK_SIM
        #include "HWSound.h"
    #endif // if not POK_SIM
#endif // if POK_ENABLE_SOUND

/*extern void dac_write(uint8_t);
extern void ext_write(uint32_t);
extern uint16_t pokGetBattP1();
extern uint16_t pokGetBattP2();
extern void pokInitRandom();
extern void pokSoundIRQ();
*/

#endif //__POKITTO_H__

