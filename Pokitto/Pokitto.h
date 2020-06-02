/**************************************************************************/
/*!
    @file     Pokitto.h
    @author   Jonne Valola
    \brief
    Pokitto.h is an "umbrella" header that allows building PokittoLib to both hardware and to PokittoSim (Windows & other PC targets)
    The real PokittoLib classes are laid out in PokittoCore.h
    
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

/** 
If POK_SIM is defined (it is in the project settings of the PokittoSimulator PC target project)
the functions are taken from PokittoSimulator. Otherwise use hardware versions. 
*/
#ifndef POK_SIM
    #include "mbed.h"
    #include "HWLCD.h"
    #include "PokittoTimer.h"
    #include "Pokitto_extport.h"
    #include "PokittoEEPROM.h"
#else
    #include "PokittoSimulator.h"
#endif 

/** 
File handling is something that is completely different in a PC target and a Pokitto hardware target.
If POK_ENABLE_SD is defined as 1, an SD card is simulated in the simulator (FileIO.h) or enabled in hardware (PokittoDisk.h)
*/
#if POK_ENABLE_SD > 0
    #ifndef POK_SIM
        #include "PokittoDisk.h"
    #else
        #include "FileIO.h"
    #endif 
    //#include "ImageFormat.h" // ToDo - there was some issue here. ImageFormat was used in Hanski's RLE image packing
#endif 

/** 
If POK_ENABLE_SOUND is defined as 1 the sound interrupt and sound buffers are included. 
Usually defined in My_settings.h for your project.
If POK_ENABLE_SYNTH is defined as 1 a very specific sound synth is added to the project. 
Not many use it, because ther is a lack of tracker and other support.
*/

#if POK_ENABLE_SOUND > 0
    #if POK_ENABLE_SYNTH > 0
    #include "Synth.h"
    #endif
    extern uint8_t soundbuf[];
    extern uint16_t soundbufindex;
    extern uint8_t* soundbufptr;
    #ifndef POK_SIM
        #include "HWSound.h"
    #endif 
#endif 

#endif //__POKITTO_H__

