/**************************************************************************/
/*!
    @file     My_settings_example.h
    @author   XX

    @section HOW TO USE My_settings

   1. Copy-paste My_settings_example.h in your project folder (where your main.cpp is)
   2. Rename to My_settings.h
   3. Edit settings to suit your project
*/
/**************************************************************************/

#ifndef MY_SETTINGS_H
#define MY_SETTINGS_H

#define PROJ_HIRES 			 1     // 1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_ENABLE_SOUND 	 1     // 0 = all sound functions disabled
#define PROJ_GBSOUND 		 0	   // 1 = use Gamebuino-compatible sound API
#define PROJ_STREAMING_MUSIC 0	   // 1 = enable music streaming from SD		

#endif
