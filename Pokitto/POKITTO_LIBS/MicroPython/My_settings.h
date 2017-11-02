/**************************************************************************/
/*!
    @file     My_settings.h
    @author   XX

    @section HOW TO USE My_settings

   My_settings can be used to set project settings inside the mbed online IDE
*/
/**************************************************************************/

#ifndef MY_SETTINGS_H
#define MY_SETTINGS_H

#define PROJ_HIRES 0 //1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_STARTUPLOGO 1
#define PROJ_GAMEBUINO 0
#define PROJ_STREAMING_MUSIC 0
#define PROJ_ENABLE_SYNTH 0

// Python specific

#define PROJ_PYTHON_REPL 0
#define MICROPY_ENABLE_GC 1  // This just means micropython is in use

#ifdef POK_SIM
#define USE_USB_SERIAL_PRINT (0)
#else
#define USE_USB_SERIAL_PRINT (1)
#endif

#endif
