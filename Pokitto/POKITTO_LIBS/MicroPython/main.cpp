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

#if !POKITTO_USE_WIN_SIMULATOR
#include "USBSerial.h"
#endif

#include "PythonBindings.h"
#include <cstdarg>

#define PROJ_PYTHON_REPL (0)

#if POKITTO_USE_WIN_SIMULATOR
#define USE_USB_SERIAL_PRINT (0)
#define PYTHON_SOURCE_FILE_PATH "..\\..\\..\\POKITTO_LIBS\\MicroPython\\src_py\\"
#else
#define USE_USB_SERIAL_PRINT (1)
#define PYTHON_SOURCE_FILE_PATH
#endif

Pokitto::Core game;

extern "C" int main_upython(int argc, char **argv);

#if !POKITTO_USE_WIN_SIMULATOR

#if USE_USB_SERIAL_PRINT

USBSerial pc;

extern "C" int pc_putc(int c) {
    return pc.putc(c);
}

extern "C" int pc_getc() {
    return pc.getc();
}

extern "C" void pc_puts(const char* strWithNull) {

    pc_putsn(strWithNull, strlen(strWithNull));
}
extern "C" void pc_putsn(const char* str, int len) {

    int i = 0;
    while(i<len)
        pc.putc((int)str[i++]);
}

extern "C" int pc_printf(const char* format, ...) {

    std::va_list arg;
    va_start(arg, format);
    int ret = pc.printf( format, arg);
    va_end(arg);
    return ret;
}

#else !USE_USB_SERIAL_PRINT

extern "C" int pc_putc(int c) { return 0;}
extern "C" int pc_getc() {return 0;}
extern "C" void pc_puts(const char* strWithNull) {}
extern "C" void pc_putsn(const char* str, int len) {}
extern "C" int pc_printf(const char* format, ...) {return 0;}

#endif

#endif // POKITTO_USE_WIN_SIMULATOR

int main () {

    game.begin();

    game.display.persistence = 0;

    game.display.setFont(fontC64);

    if (game.isRunning()) {

        #if POKITTO_USE_WIN_SIMULATOR

        #if PROJ_PYTHON_REPL

        PythonMain(0, nullptr);

        #else // Run python script

        // Load the python script and start running it.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings" // The strings below will not be changed in the function called
        char* argv[] = {
            "pokitto_sim.exe",
            PYTHON_SOURCE_FILE_PATH"marsattack_main.py"
        };
        #pragma GCC diagnostic pop

        PythonMain(2, argv);

        #endif  // PROJ_PYTHON_REPL

        #else // ! POKITTO_USE_WIN_SIMULATOR

        #if PROJ_PYTHON_REPL

        PythonMain(0, nullptr);

        #else // Run python script

        // Load the python script and start running it.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings" // The strings below will not be changed in the function called
        char* argv[] = {
            "frogitto.exe",
            "marsattack_main.py"
        };
        #pragma GCC diagnostic pop

        PythonMain(2, argv);

        #endif // PROJ_PYTHON_REPL

        #endif // POKITTO_USE_WIN_SIMULATOR

   }

    return 1;
}

