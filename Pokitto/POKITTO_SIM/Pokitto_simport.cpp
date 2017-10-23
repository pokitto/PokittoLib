/**************************************************************************/
/*!
    @file     Pokitto_extport.cpp
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

#include "Pokitto.h"

void ext_write(uint32_t value) {
    #if POK_USE_EXT > 0
    /*
    if (value & 1) SET_EXT0 else CLR_EXT0;
    value >>= 1;
    if (value & 1) SET_EXT1 else CLR_EXT1;
    value >>= 1;
    if (value & 1) SET_EXT2 else CLR_EXT2;
    value >>= 1;
    if (value & 1) SET_EXT3 else CLR_EXT3;
    value >>= 1;
    if (value & 1) SET_EXT4 else CLR_EXT4;
    value >>= 1;
    if (value & 1) SET_EXT5 else CLR_EXT5;
    value >>= 1;
    if (value & 1) SET_EXT6 else CLR_EXT6;
    value >>= 1;
    if (value & 1) SET_EXT7 else CLR_EXT7;
    value >>= 1;
    if (value & 1) SET_EXT8 else CLR_EXT8;
    value >>= 1;
    if (value & 1) SET_EXT9 else CLR_EXT9;
    value >>= 1;
    if (value & 1) SET_EXT10 else CLR_EXT10;
    value >>= 1;
    if (value & 1) SET_EXT11 else CLR_EXT11;
    value >>= 1;
    if (value & 1) SET_EXT12 else CLR_EXT12;
    value >>= 1;
    if (value & 1) SET_EXT13 else CLR_EXT13;
    value >>= 1;
    if (value & 1) SET_EXT14 else CLR_EXT14;
    value >>= 1;
    if (value & 1) SET_EXT15 else CLR_EXT15;
    value >>= 1;
    if (value & 1) SET_EXT16 else CLR_EXT16;
    value >>= 1;
    if (value & 1) SET_EXT17 else CLR_EXT17;*/
    #endif
}

