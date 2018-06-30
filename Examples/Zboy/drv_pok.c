#include <stdint.h>

#include "drv.h"   /* include self for control */
#include "icon.h"  /* icon32x32_bmp[] for titlebar icon */

int drv_init(int screenwidth, int screenheight, int joyid);

/* sets the window's title to a custom string */
void drv_setwintitle(char *title){
}


/* waits for ms miliseconds */
void drv_delay(int ms){
}

/* draws a pixel of 'color' color at location [x,y] in zBoy's memory buffer.
 * color is an index of the palette. the position [0,0] is located at the
 * upper left corner of the virtual screen */
void drv_putpixel(int x, int y, int color){
}

/* deinitialize the I/O subsystem. this is called just before zBoy quits */
int drv_close(void){
}

/* provides a directory for storing user files, guaranteed to be writeable.
 * *dir must point to a valid area that will be filled with a null-terminated
 * string containing the save directory. maxlen is the number of bytes
 * available in *dir */
void drv_getsavedir(char *dir, int maxlen){
}

