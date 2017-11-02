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
#include "USBSerial.h"
#endif

#include "PythonBindings.h"

#ifdef POK_SIM
#define PYTHON_SOURCE_FILE_PATH "..\\..\\..\\POKITTO_LIBS\\MicroPython\\src_py\\"
#else
#define PYTHON_SOURCE_FILE_PATH
#endif

Pokitto::Core game;

extern "C" int main_upython(int argc, char **argv);

int main () {

    game.begin();

    game.display.persistence = 0;

    game.display.setFont(fontC64);

    if (game.isRunning()) {

        #if PROJ_PYTHON_REPL

        PythonMain(0, NULL);

        #else // Run python script

        // Load the python script and start running it.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings" // The strings below will not be changed in the function called
        char* argv[] = {
            "",
            PYTHON_SOURCE_FILE_PATH"example_main.py"
        };
        #pragma GCC diagnostic pop

        PythonMain(2, argv);

        #endif  // PROJ_PYTHON_REPL

   }

    return 1;
}

