/* This is an example of how to set your own project settings

To use

1. make a copy of this file by pressing right mouse button on top of this file
in the mbed online compiler, then choose "Clone"

2. See that the copy is in the root of the project (where your main cpp files are), not inside the PokittoLib folder
Drag & drop to move if needed

3. rename the clone to "My_settings.h"

*/

#ifndef MY_SETTINGS_H
#define MY_SETTINGS_H

#define PROJ_HIRES              0       // 1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_ENABLE_SOUND       1       // 0 = all sound functions disabled
#define PROJ_STREAMING_MUSIC    1       // 1 = enable streaming music from SD card
#define PROJ_AUD_FREQ           11025
#define PROJ_STREAM_TO_DAC      1       // 1 use DAC for stream, 0 = use PWM for stream
#define PROJ_GBSOUND            0       // 1 = use Gamebuino-compatible sound interrupt (choose this or the one below)
#define PROJ_ENABLE_SYNTH       0       // 1 = use Rboy-compatible sound interrupt
#define PROJ_GAMEBUINO          0       // 1 if you are making a Gamebuino-based graphics mode (84x48)
#define PROJ_USE_PWM            0       // 0 = Use only DAC for output
#define DISABLEAVRMIN //disable AVR style min/max declarations

#endif
