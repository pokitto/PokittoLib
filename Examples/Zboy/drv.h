#include <stdint.h>

#ifndef DRV_H_SENTINEL
#define DRV_H_SENTINEL

/* convenience pseudo-functions */
#define drv_event_gettype(x) (x & 0x7F00)
#define drv_event_getval(x) (x & 0x00FF)

/* define input device types */
enum DRV_INPUT_DEVICE {
  DRV_INPUT_KEYBOARD = 0x1000,
  DRV_INPUT_JOYSTICK = 0x2000
};

/* define categories of input events */
enum DRV_INPUT_TYPE {
  DRV_INPUT_NONE      = 0x0000,
  DRV_INPUT_KEYDOWN   = 0x0100,
  DRV_INPUT_KEYUP     = 0x0200,
  DRV_INPUT_JOYDOWN   = 0x0300,
  DRV_INPUT_JOYUP     = 0x0400,
  DRV_INPUT_JOYAXDOWN = 0x0500,
  DRV_INPUT_JOYAXUP   = 0x0600,
  DRV_INPUT_QUIT      = 0x0700
};

/* define input events */
enum DRV_INPUT_KEY {
  DRV_INPUT_JOY_1,
  DRV_INPUT_JOY_2,
  DRV_INPUT_JOY_3,
  DRV_INPUT_JOY_4,
  DRV_INPUT_JOY_5,
  DRV_INPUT_JOY_6,
  DRV_INPUT_JOY_7,
  DRV_INPUT_JOY_8,
  DRV_INPUT_JOY_9,
  DRV_INPUT_JOY_10,
  DRV_INPUT_JOY_11,
  DRV_INPUT_JOY_12,
  DRV_INPUT_JOY_13,
  DRV_INPUT_JOY_14,
  DRV_INPUT_JOY_15,
  DRV_INPUT_JOY_16,
  DRV_INPUT_JOYAX_1POS,
  DRV_INPUT_JOYAX_1NEG,
  DRV_INPUT_JOYAX_2POS,
  DRV_INPUT_JOYAX_2NEG,
  DRV_INPUT_JOYAX_3POS,
  DRV_INPUT_JOYAX_3NEG,
  DRV_INPUT_JOYAX_4POS,
  DRV_INPUT_JOYAX_4NEG,
  DRV_INPUT_KEY_ESC,
  DRV_INPUT_KEY_RET,
  DRV_INPUT_KEY_TAB,
  DRV_INPUT_KEY_SPC,
  DRV_INPUT_KEY_BKSPC,
  DRV_INPUT_KEY_LCTRL,
  DRV_INPUT_KEY_RCTRL,
  DRV_INPUT_KEY_LSHIFT,
  DRV_INPUT_KEY_RSHIFT,
  DRV_INPUT_KEY_LALT,
  DRV_INPUT_KEY_RALT,
  DRV_INPUT_KEY_F1,
  DRV_INPUT_KEY_F2,
  DRV_INPUT_KEY_F3,
  DRV_INPUT_KEY_F4,
  DRV_INPUT_KEY_F5,
  DRV_INPUT_KEY_F6,
  DRV_INPUT_KEY_F7,
  DRV_INPUT_KEY_F8,
  DRV_INPUT_KEY_F9,
  DRV_INPUT_KEY_F10,
  DRV_INPUT_KEY_F11,
  DRV_INPUT_KEY_F12,
  DRV_INPUT_KEY_UP,
  DRV_INPUT_KEY_DOWN,
  DRV_INPUT_KEY_LEFT,
  DRV_INPUT_KEY_RIGHT,
  DRV_INPUT_KEY_PGUP,
  DRV_INPUT_KEY_PGDOWN,
  DRV_INPUT_KEY_HOME,
  DRV_INPUT_KEY_END,
  DRV_INPUT_KEY_INS,
  DRV_INPUT_KEY_DEL,
  DRV_INPUT_KEY_LWIN,
  DRV_INPUT_KEY_RWIN,
  DRV_INPUT_KEY_MENU,
  DRV_INPUT_KEY_0,
  DRV_INPUT_KEY_1,
  DRV_INPUT_KEY_2,
  DRV_INPUT_KEY_3,
  DRV_INPUT_KEY_4,
  DRV_INPUT_KEY_5,
  DRV_INPUT_KEY_6,
  DRV_INPUT_KEY_7,
  DRV_INPUT_KEY_8,
  DRV_INPUT_KEY_9,
  DRV_INPUT_KEY_A,
  DRV_INPUT_KEY_B,
  DRV_INPUT_KEY_C,
  DRV_INPUT_KEY_D,
  DRV_INPUT_KEY_E,
  DRV_INPUT_KEY_F,
  DRV_INPUT_KEY_G,
  DRV_INPUT_KEY_H,
  DRV_INPUT_KEY_I,
  DRV_INPUT_KEY_J,
  DRV_INPUT_KEY_K,
  DRV_INPUT_KEY_L,
  DRV_INPUT_KEY_M,
  DRV_INPUT_KEY_N,
  DRV_INPUT_KEY_O,
  DRV_INPUT_KEY_P,
  DRV_INPUT_KEY_Q,
  DRV_INPUT_KEY_R,
  DRV_INPUT_KEY_S,
  DRV_INPUT_KEY_T,
  DRV_INPUT_KEY_U,
  DRV_INPUT_KEY_V,
  DRV_INPUT_KEY_W,
  DRV_INPUT_KEY_X,
  DRV_INPUT_KEY_Y,
  DRV_INPUT_KEY_Z
};


/* initialization of the I/O subsystem. This is called by zBoy once, when the
 * emulator starts. screenwidth and screenheight must contain the size of
 * requested virtual screen, joyid is either the id of the joystick that have
 * to be polled (0..x) or -1 if no joystick support is required. */
int drv_init(int screenwidth, int screenheight, int joyid);

/* sets the window's title to a custom string */
void drv_setwintitle(char *title);

/* returns the next input event in queue */
int drv_keypoll(void);

/* loads a palette of colors into zBoy. *palette must be an array of at least
 * 256 color values written in 32bits each as RGB triplets */
int drv_loadpal(uint32_t *palette);

/* returns a monotonously increasing counter that increases by 1 every
 * milisecond */
unsigned long drv_getticks(void);

/* waits for ms miliseconds */
void drv_delay(int ms);

/* draws a pixel of 'color' color at location [x,y] in zBoy's memory buffer.
 * color is an index of the palette. the position [0,0] is located at the
 * upper left corner of the virtual screen */
void drv_putpixel(int x, int y, int color);

/* renders the screen from the memory buffer into the actual screen */
// void drv_refreshscreen(void);

/* deinitialize the I/O subsystem. this is called just before zBoy quits */
int drv_close(void);

/* provides a directory for storing user files, guaranteed to be writeable.
 * *dir must point to a valid area that will be filled with a null-terminated
 * string containing the save directory. maxlen is the number of bytes
 * available in *dir */
void drv_getsavedir(char *dir, int maxlen);

void drv_setscanline(uint32_t CurLY);

#endif
