/**************************************************************************/
/*!
    @file     MicroPythonMain.cpp
    @author   Hannu Viitala

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2017, Hannu Viitala
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

#ifndef POK_SIM
#if USE_USB_SERIAL_PRINT
#include "USBSerial.h"
#endif
#ifdef USE_SEGGER_SERIAL_PRINT
#include "SEGGER_RTT.h"
#endif
#endif

#include "PythonBindings.h"

#ifdef TASUI
// Include for the TASUI API.
#include <tasui>
// Include for the Tileset.
#include <puits_UltimateUtopia4.h>
#include <ptui_StandardUITilesetDefinition.hpp>
#endif


#ifdef POK_SIM
extern "C" int PythonMain(int argc, char **argv);
#else
extern "C" int PyInSkyMain( unsigned int heapSize, char *heapMem );
#endif

int main () {

    int *tmp = new int;
    uintptr_t p = (uintptr_t)(void *)(&tmp) - (uintptr_t)(void *)(tmp);
    p -= 1024*3; // stack and stuff

    Pokitto::Core game;
    game.begin();
    game.display.persistence = 0;
    Pokitto::Display::setColorDepth(4);

    #ifdef TASUI
    // TAS UI
    using PUI=Pokitto::UI;
    // Select the tileset.
    PUI::setTilesetImage(puits::UltimateUtopia4::tileSet);
    // Show the Tilemap, the Sprites, then the UI.
    PUI::showTileMapSpritesUI();
    #endif

    #ifdef POK_SIM

   // Load the python script and start running it.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wwrite-strings" // The strings below will not be changed in the function called
    char* argv[] = {
        "",
        "..\\..\\..\\POKITTO_LIBS\\MicroPython\\src_py\\main.py"
    };
    #pragma GCC diagnostic pop

    PythonMain(2, argv);

    #else

    PyInSkyMain( p, new char[p] );

    #endif

    return 1;
}

