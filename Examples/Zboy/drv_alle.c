/*
 * zBoy's Input/Output driver
 *
 * Model: Allegro v4.2.3 on DOS with DJGPP 2.04
 *
 * Copyright (C) Mateusz Viste 2015
 */

#define ALLEGRO_HAVE_STDINT_H  /* this makes sure Allegro won't try to redefine stuff from stdint.h */

#include <allegro.h>

#include "setupdos/dosconf.c"
#include "drv.h"   /* include self for control */


#define MAXJOYBUTS 8
#define MAXJOYAXIS 4

/* this helps making the executable a little bit smaller, since Allegro  */
/* will include only graphic code related to 8bit depth operations       */
BEGIN_COLOR_DEPTH_LIST
  COLOR_DEPTH_8
END_COLOR_DEPTH_LIST


static unsigned long timercounter = 0;
static int allegro_inited = 0;
static int screenoffsetx = 0;
static int screenoffsety = 0;
static int joybutscount = 0;
static int joyaxiscount = 0;
static int joyid = -1;
BITMAP *dbuf = NULL;
char *keyold = NULL;

static void inctimer(void) {
  timercounter++;
}


int drv_init(int w, int h, int jid) {
  int screenw, screenh;
  int videomodel, soundmodel, joystmodel;
  char cfgfile[256];

  if ((w <= 320) && (h <= 200)) {
    screenw = 320;
    screenh = 200;
  } else if ((w <= 320) && (h <= 240)) {
    screenw = 320;
    screenh = 240;
  } else if ((w <= 640) && (h <= 480)) {
    screenw = 640;
    screenh = 480;
  } else if ((w <= 800) && (h <= 600)) {
    screenw = 800;
    screenh = 600;
  } else if ((w <= 1024) && (h <= 768)) {
    screenw = 1024;
    screenh = 768;
  } else if ((w <= 1280) && (h <= 1024)) {
    screenw = 1280;
    screenh = 1024;
  } else {
    return(-1);
  }

  screenoffsetx = (screenw - w) / 2;
  screenoffsety = (screenh - h) / 2;

  /* read allegro's configuration */
  drv_getsavedir(cfgfile, 256);
  strcat(cfgfile, "setup.cfg");
  load_allegro_config(cfgfile, &videomodel, &soundmodel, &joystmodel);

  if (allegro_inited == 0) {
    allegro_inited = 1;
    allegro_init();
  }

  if (jid >= 0) {
    install_joystick(joystmodel);
    joyid = jid;
  }

  set_color_depth(8);
  if (set_gfx_mode(videomodel, screenw, screenh, 0, 0) != 0) {
    return(-1);
  }

  /* blank the screen */
  clear_to_color(screen, 254);

  /* allocate a double buffer bitmap */
  dbuf = create_bitmap(w, h);

  /* install a timer that will increment my internal counter once per ms */
  install_timer();
  install_int(inctimer, 1);

  /* init the Allegro keyboard subsystem */
  install_keyboard();

  /* make a copy of all keys statuses for keypress change detection */
  keyold = calloc(1, KEY_MAX);

  /* remember how many axis and buttons we have on the joystick */
  joyaxiscount = joy[joyid].stick[0].num_axis;
  joybutscount = joy[joyid].num_buttons;
  if (joyaxiscount > MAXJOYAXIS) joyaxiscount = MAXJOYAXIS;
  if (joybutscount > MAXJOYBUTS) joybutscount = MAXJOYBUTS;

  return(0);
}


void drv_setwintitle(char *title) {
  /* this is a no-op, since obviously DOS doesn't have a titlebar anywhere */
  title = title;
}


int drv_keypoll(void) {
  static int joyold[MAXJOYBUTS];
  static int joyoldaxis[MAXJOYAXIS]; /* static vals are inited to 0 */
  int x;
  const int joybuttons[] = {
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_1,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_2,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_3,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_4,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_5,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_6,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_7,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_8,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_9,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_10,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_11,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_12,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_13,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_14,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_15,
    DRV_INPUT_JOYSTICK | DRV_INPUT_JOY_16
  };
  const int joyaxis[][3] = { /* axis/edge/enum */
    {0, 0, DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAX_1NEG},
    {0, 1, DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAX_1POS},
    {1, 0, DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAX_2NEG},
    {1, 1, DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAX_2POS}
  };
  const int keypairs[][2] = {
    {KEY_F1, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F1},
    {KEY_F2, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F2},
    {KEY_F3, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F3},
    {KEY_F4, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F4},
    {KEY_F5, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F5},
    {KEY_F6, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F6},
    {KEY_F7, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F7},
    {KEY_F8, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F8},
    {KEY_F9, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F9},
    {KEY_F10, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F10},
    {KEY_F11, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F11},
    {KEY_F12, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F12},
    {KEY_ESC, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_ESC},
    {KEY_TAB, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_TAB},
    {KEY_LCONTROL, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LCTRL},
    {KEY_ALT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LALT},
    {KEY_RCONTROL, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RCTRL},
    {KEY_ALTGR, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RALT},
    {KEY_SPACE, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_SPC},
    {KEY_ENTER, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RET},
    {KEY_BACKSPACE, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_BKSPC},
    {KEY_LEFT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LEFT},
    {KEY_RIGHT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RIGHT},
    {KEY_UP, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_UP},
    {KEY_DOWN, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_DOWN},
    {KEY_RSHIFT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RSHIFT},
    {KEY_LSHIFT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LSHIFT},
    {KEY_LWIN, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LWIN},
    {KEY_RWIN, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RWIN},
    {KEY_MENU, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_MENU},
    {KEY_PGUP, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_PGUP},
    {KEY_PGDN, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_PGDOWN},
    {KEY_HOME, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_HOME},
    {KEY_END, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_END},
    {KEY_INSERT, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_INS},
    {KEY_DEL, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_DEL},
    {KEY_A, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_A},
    {KEY_B, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_B},
    {KEY_C, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_C},
    {KEY_D, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_D},
    {KEY_E, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_E},
    {KEY_F, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_F},
    {KEY_G, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_G},
    {KEY_H, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_H},
    {KEY_I, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_I},
    {KEY_J, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_J},
    {KEY_K, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_K},
    {KEY_L, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_L},
    {KEY_M, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_M},
    {KEY_N, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_N},
    {KEY_O, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_O},
    {KEY_P, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_P},
    {KEY_Q, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_Q},
    {KEY_R, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_R},
    {KEY_S, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_S},
    {KEY_T, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_T},
    {KEY_U, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_U},
    {KEY_V, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_V},
    {KEY_W, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_W},
    {KEY_X, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_X},
    {KEY_Y, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_Y},
    {KEY_Z, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_Z},
    {KEY_0, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_0},
    {KEY_1, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_1},
    {KEY_2, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_2},
    {KEY_3, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_3},
    {KEY_4, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_4},
    {KEY_5, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_5},
    {KEY_6, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_6},
    {KEY_7, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_7},
    {KEY_8, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_8},
    {KEY_9, DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_9},
    {-1, -1} /* this must be set to -1,-1 because it is the list terminator */
  };

  poll_keyboard();  /* Allegro requires this to update its internal key states */
  poll_joystick();  /* same goes for the joystick */

  /* poll all keys defined in keypairs[] */
  for (x = 0; keypairs[x][0] >= 0; x++) {
    if (key[keypairs[x][0]] != keyold[keypairs[x][0]]) { /* detect a change */
      keyold[keypairs[x][0]] = key[keypairs[x][0]];
      if (keyold[keypairs[x][0]] == 0) return(DRV_INPUT_KEYUP | keypairs[x][1]);
      return(DRV_INPUT_KEYDOWN | keypairs[x][1]);
    }
  }
  /* poll the joystick buttons */
  for (x = 0; x < joybutscount; x++) {
    if (joy[joyid].button[x].b != joyold[x]) {
      joyold[x] = joy[joyid].button[x].b;
      if (joyold[x] == 0) {
        return(DRV_INPUT_JOYUP | joybuttons[x]);
      } else {
        return(DRV_INPUT_JOYDOWN | joybuttons[x]);
      }
    }
  }
  /* poll the joystick axis */
  /* http://alleg.sourceforge.net/stabledocs/en/alleg007.html#joy */
  for (x = 0; x < joyaxiscount; x++) {
    int s;
    if (joyaxis[x][1] == 0) {
      s = joy[joyid].stick[0].axis[joyaxis[x][0]].d1; /* set if joystick is moved left */
    } else {
      s = joy[joyid].stick[0].axis[joyaxis[x][0]].d2; /* set if joystick is moved right */
    }
    if (joyoldaxis[x] != s) {
      joyoldaxis[x] = s;
      if (s == 0) {
        return(DRV_INPUT_JOYAXUP | joyaxis[x][2]);
      } else {
        return(DRV_INPUT_JOYAXDOWN | joyaxis[x][2]);
      }
    }
  }
  /* else nothing changed */
  return(DRV_INPUT_NONE);
}


int drv_loadpal(uint32_t *newpalette) {
  PALETTE mypal;
  int x, r, g, b;
  /* prepare a palette structure, as Allegro expects it */
  for (x = 0; x < 256; x++) {
      r = (newpalette[x] >> 16) & 0xff;
      g = (newpalette[x] >> 8) & 0xff;
      b = newpalette[x] & 0xff;
      mypal[x].r = r / 4; /* our palettes are sets of RGB triplets, where   */
      mypal[x].g = g / 4; /* each intensity is in a range of 0..255, while  */
      mypal[x].b = b / 4; /* Allegro expects values in the a range of 0..63 */
  }
  /* call Allegro's set_palette() to send palette to actual hardware */
  set_palette(mypal);
  return(0);
}


/* get time elapsed (in ms) since an undefined point in time */
unsigned long drv_getticks(void) {
  return(timercounter);
}


void drv_delay(int ms) {
  rest(ms);
}


void drv_putpixel(int x, int y, int color) {
  _putpixel(dbuf, x, y, color); /* _putpixel() is a faster 8bit version of putpixel() */
}


void drv_refreshscreen(void) {
  /* blit the double buffer into actual screen */
  blit(dbuf, screen, 0, 0, screenoffsetx, screenoffsety, dbuf->w, dbuf->h);
}


void drv_getsavedir(char *savedir, int maxlen) {
  if (allegro_inited == 0) {
    allegro_init();
    allegro_inited = 1;
  }
  savedir[0] = 0; /* just in case allegro would fail somehow */
  get_executable_name(savedir, maxlen);
  replace_filename(savedir, savedir, "", maxlen);
}


int drv_close(void) {
  free(keyold);
  keyold = NULL;
  destroy_bitmap(dbuf);
  dbuf = NULL;
  /* clear the keyboard buffer (who knows what accumulated there) */
  clear_keybuf();
  /* ask allegro to cleanup iteself */
  allegro_exit();
  return(0);
}
