#include "Pokitto.h"
#include "USBSerial.h"
#include "PythonBindings.h"
#include <cstdarg>

#define USE_USB_SERIAL_PRINT (0)
#define PROJ_PYTHON_REPL (0)

Pokitto::Core game;

extern "C" int main_upython(int argc, char **argv);

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

int main () {

    game.begin();

    //game.display.persistence = 0;
    game.display.persistence = 1;

    if (game.isRunning()) {

        #if PROJ_PYTHON_REPL

        main_upython(0,nullptr);

        #else // Run python script

        // Load the python script and start running it.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings" // The strings below will not be changed in the function called
        char* argv[] = {
            "frogitto.exe",
            "oscmain.py"
        };
        #pragma GCC diagnostic pop

        main_upython(2, argv);

        #endif
    }

    return 1;
}

