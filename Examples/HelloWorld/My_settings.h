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

#define PROJ_HIRES 1            //1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_ENABLE_SOUND 1     // 0 = all sound functions disabled

// Sprite specific
#define SPRITE_COUNT 8

#define PROJ_USE_FPS_COUNTER 1  // Must define USE_USB_SERIAL_PRINT also
#define POK_SHOW_FPS_ON_DISPLAY

#define USE_USB_SERIAL_PRINT (0)

#endif
