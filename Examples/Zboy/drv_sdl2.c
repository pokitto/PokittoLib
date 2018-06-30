/*
 * zBoy's Input/Output driver
 *
 * Model: SDL2
 *
 * Copyright (C) Mateusz Viste 2015
 */

#include <stdio.h>  /* printf() */
#include <stdlib.h> /* calloc(), free() */
#include <string.h> /* strlen() */

#include <SDL2/SDL.h>


#include "drv.h"   /* include self for control */
#include "icon.h"  /* icon32x32_bmp[] for titlebar icon */


/* SDL-related variables */
SDL_Window *window;         /* virtual SDL screen area */
SDL_Renderer *renderer;     /* the SDL renderer */
SDL_Joystick *joystick;     /* joystick handler */
SDL_Texture *screentexture; /* a texture used for internal rendering */
uint32_t *screenbuf;        /* work pixel buffer */
uint32_t palette[256];
int screenwidth, screenheight;
int joysupport;

int eventqueue = -1;  /* a one-position buffer for putting events that have */
                      /* to be returned to zBoy at next call                */
int oldjoyaxstate[4][2] = {{0,0},{0,0},  /* used to detect changes in the   */
                           {0,0},{0,0}}; /* status of a joystick axis       */
int newjoyaxstate[4][2] = {{0,0},{0,0},  /* used to detect changes in the   */
                           {0,0},{0,0}}; /* status of a joystick axis       */


static void switchfullscreen(void) {
  static int fullscreenflag = 0;
  fullscreenflag ^= 1;
  if (fullscreenflag != 0) {
      SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      SDL_SetWindowFullscreen(window, 0);
  }
  SDL_Delay(50); /* wait for 50ms - the video thread needs some time to set things up */
}


/* sets the icon in the aplication's title bar */
static void seticon(void) {
  SDL_Surface *surface;
  SDL_RWops *rwop;
  rwop = SDL_RWFromMem(icon32x32_bmp, icon32x32_bmp_len);
  surface = SDL_LoadBMP_RW(rwop, 0);
  SDL_FreeRW(rwop);
  if (surface == NULL) return;
  SDL_SetWindowIcon(window, surface);
  SDL_FreeSurface(surface); /* once the icon is loaded, the surface is not needed anymore */
}


int drv_init(int w, int h, int joyid) {
  int sdlflags;
  /* Initialize the SDL library... */
  sdlflags = SDL_INIT_VIDEO;
  if (joyid >= 0) {
    sdlflags |= SDL_INIT_JOYSTICK;
    joysupport = 1;
  }
  if (SDL_Init(sdlflags) < 0) {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    return(1);
  }
  SDL_ShowCursor(SDL_DISABLE);                 /* Disable (hide) mouse cursor */
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE); /* ignore mouse events */
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");  /* this makes scaling nicer */
  if (joyid >= 0) {
    joystick = SDL_JoystickOpen(joyid);              /* ...and open the 1st system joystick */
  }

  screenwidth = w;
  screenheight = h;

  window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("Couldn't create window: %s\n", SDL_GetError());
    return(1);
  }
  SDL_SetWindowMinimumSize(window, 100, 100);
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    printf("Couldn't create renderer: %s\n", SDL_GetError());
    return(1);
  }
  /* instruct SDL2 to use the screen as a logical size output so the rendering will be nicely centered and ratio preserved */
  SDL_RenderSetLogicalSize(renderer, w, h);
  /* allocate a screen buffer */
  screenbuf = calloc(sizeof(uint32_t), w * h);
  if (screenbuf == NULL) {
    printf("Couldn't create screenbuf. Out of memory?\n");
    return(1);
  }
  screentexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
  if (screentexture == NULL) {
    printf("Couldn't create texture buffer: %s\n", SDL_GetError());
    return(1);
  }

  /* disable the screensaver, if enabled */
  SDL_DisableScreenSaver();

  /* load a title-bar icon */
  seticon();

  return(0);
}


void drv_setwintitle(char *title) {
  SDL_SetWindowTitle(window, title);
}


#define CHECKAXIS() \
  if (oldjoyaxstate[0][0] != newjoyaxstate[0][0]) { \
    oldjoyaxstate[0][0] = newjoyaxstate[0][0]; \
    if (newjoyaxstate[0][0] == 0) { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXUP | DRV_INPUT_JOYAX_1NEG); \
    } else { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXDOWN | DRV_INPUT_JOYAX_1NEG); \
    } \
  } \
  if (oldjoyaxstate[0][1] != newjoyaxstate[0][1]) { \
    oldjoyaxstate[0][1] = newjoyaxstate[0][1]; \
    if (newjoyaxstate[0][1] == 0) { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXUP | DRV_INPUT_JOYAX_1POS); \
    } else { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXDOWN | DRV_INPUT_JOYAX_1POS); \
    } \
  } \
  if (oldjoyaxstate[1][0] != newjoyaxstate[1][0]) { \
    oldjoyaxstate[1][0] = newjoyaxstate[1][0]; \
    if (newjoyaxstate[1][0] == 0) { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXUP | DRV_INPUT_JOYAX_2NEG); \
    } else { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXDOWN | DRV_INPUT_JOYAX_2NEG); \
    } \
  } \
  if (oldjoyaxstate[1][1] != newjoyaxstate[1][1]) { \
    oldjoyaxstate[1][1] = newjoyaxstate[1][1]; \
    if (newjoyaxstate[1][1] == 0) { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXUP | DRV_INPUT_JOYAX_2POS); \
    } else { \
      return(DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXDOWN | DRV_INPUT_JOYAX_2POS); \
    } \
  }


int drv_keypoll(void) {
  SDL_Event event;

  /* first check if anything changed in the joystick state since last time */
  if (joysupport != 0) {
    CHECKAXIS();
  }

  for (;;) {
    if (SDL_PollEvent(&event) == 0) return(DRV_INPUT_NONE);
    /* check special events */
    if (event.type == SDL_QUIT) return(DRV_INPUT_QUIT);
    /* keyboard */
    if ((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP)) {
      int res;
      if (event.type == SDL_KEYDOWN) {
        res = DRV_INPUT_KEYBOARD | DRV_INPUT_KEYDOWN;
      } else {
        res = DRV_INPUT_KEYBOARD | DRV_INPUT_KEYUP;
      }
      switch (event.key.keysym.sym) {
        case SDLK_F1:        return(res | DRV_INPUT_KEY_F1);
        case SDLK_F2:        return(res | DRV_INPUT_KEY_F2);
        case SDLK_F3:        return(res | DRV_INPUT_KEY_F3);
        case SDLK_F4:        return(res | DRV_INPUT_KEY_F4);
        case SDLK_F5:        return(res | DRV_INPUT_KEY_F5);
        case SDLK_F6:        return(res | DRV_INPUT_KEY_F6);
        case SDLK_F7:        return(res | DRV_INPUT_KEY_F7);
        case SDLK_F8:        return(res | DRV_INPUT_KEY_F8);
        case SDLK_F9:        return(res | DRV_INPUT_KEY_F9);
        case SDLK_F10:       return(res | DRV_INPUT_KEY_F10);
        case SDLK_F11:       return(res | DRV_INPUT_KEY_F11);
        case SDLK_F12:       return(res | DRV_INPUT_KEY_F12);
        case SDLK_ESCAPE:    return(res | DRV_INPUT_KEY_ESC);
        case SDLK_TAB:       return(res | DRV_INPUT_KEY_TAB);
        case SDLK_LCTRL:     return(res | DRV_INPUT_KEY_LCTRL);
        case SDLK_LALT:      return(res | DRV_INPUT_KEY_LALT);
        case SDLK_RCTRL:     return(res | DRV_INPUT_KEY_RCTRL);
        case SDLK_RALT:      return(res | DRV_INPUT_KEY_RALT);
        case SDLK_SPACE:     return(res | DRV_INPUT_KEY_SPC);
        case SDLK_RETURN:
          /* if that's ALT+ENTER, then switch fullscreen */
          if (SDL_GetModState() & KMOD_ALT) {
            if (res == (DRV_INPUT_KEYBOARD | DRV_INPUT_KEYDOWN)) switchfullscreen();
            return(DRV_INPUT_NONE);
          } else {
            return(res | DRV_INPUT_KEY_RET);
          }
          break;
        case SDLK_BACKSPACE: return(res | DRV_INPUT_KEY_BKSPC);
        case SDLK_LEFT:      return(res | DRV_INPUT_KEY_LEFT);
        case SDLK_RIGHT:     return(res | DRV_INPUT_KEY_RIGHT);
        case SDLK_UP:        return(res | DRV_INPUT_KEY_UP);
        case SDLK_DOWN:      return(res | DRV_INPUT_KEY_DOWN);
        case SDLK_RSHIFT:    return(res | DRV_INPUT_KEY_RSHIFT);
        case SDLK_LSHIFT:    return(res | DRV_INPUT_KEY_LSHIFT);
        case SDLK_RGUI:      return(res | DRV_INPUT_KEY_RWIN);
        case SDLK_LGUI:      return(res | DRV_INPUT_KEY_LWIN);
        case SDLK_MENU:      return(res | DRV_INPUT_KEY_MENU);
        case SDLK_PAGEUP:    return(res | DRV_INPUT_KEY_PGUP);
        case SDLK_PAGEDOWN:  return(res | DRV_INPUT_KEY_PGDOWN);
        case SDLK_HOME:      return(res | DRV_INPUT_KEY_HOME);
        case SDLK_END:       return(res | DRV_INPUT_KEY_END);
        case SDLK_INSERT:    return(res | DRV_INPUT_KEY_INS);
        case SDLK_DELETE:    return(res | DRV_INPUT_KEY_DEL);
        case SDLK_a:         return(res | DRV_INPUT_KEY_A);
        case SDLK_b:         return(res | DRV_INPUT_KEY_B);
        case SDLK_c:         return(res | DRV_INPUT_KEY_C);
        case SDLK_d:         return(res | DRV_INPUT_KEY_D);
        case SDLK_e:         return(res | DRV_INPUT_KEY_E);
        case SDLK_f:         return(res | DRV_INPUT_KEY_F);
        case SDLK_g:         return(res | DRV_INPUT_KEY_G);
        case SDLK_h:         return(res | DRV_INPUT_KEY_H);
        case SDLK_i:         return(res | DRV_INPUT_KEY_I);
        case SDLK_j:         return(res | DRV_INPUT_KEY_J);
        case SDLK_k:         return(res | DRV_INPUT_KEY_K);
        case SDLK_l:         return(res | DRV_INPUT_KEY_L);
        case SDLK_m:         return(res | DRV_INPUT_KEY_M);
        case SDLK_n:         return(res | DRV_INPUT_KEY_N);
        case SDLK_o:         return(res | DRV_INPUT_KEY_O);
        case SDLK_p:         return(res | DRV_INPUT_KEY_P);
        case SDLK_q:         return(res | DRV_INPUT_KEY_Q);
        case SDLK_r:         return(res | DRV_INPUT_KEY_R);
        case SDLK_s:         return(res | DRV_INPUT_KEY_S);
        case SDLK_t:         return(res | DRV_INPUT_KEY_T);
        case SDLK_u:         return(res | DRV_INPUT_KEY_U);
        case SDLK_v:         return(res | DRV_INPUT_KEY_V);
        case SDLK_w:         return(res | DRV_INPUT_KEY_W);
        case SDLK_x:         return(res | DRV_INPUT_KEY_X);
        case SDLK_y:         return(res | DRV_INPUT_KEY_Y);
        case SDLK_z:         return(res | DRV_INPUT_KEY_Z);
        case SDLK_0:         return(res | DRV_INPUT_KEY_0);
        case SDLK_1:         return(res | DRV_INPUT_KEY_1);
        case SDLK_2:         return(res | DRV_INPUT_KEY_2);
        case SDLK_3:         return(res | DRV_INPUT_KEY_3);
        case SDLK_4:         return(res | DRV_INPUT_KEY_4);
        case SDLK_5:         return(res | DRV_INPUT_KEY_5);
        case SDLK_6:         return(res | DRV_INPUT_KEY_6);
        case SDLK_7:         return(res | DRV_INPUT_KEY_7);
        case SDLK_8:         return(res | DRV_INPUT_KEY_8);
        case SDLK_9:         return(res | DRV_INPUT_KEY_9);
      }
    } else if (event.type == SDL_JOYAXISMOTION) { /* joystick axis */
      int curax;
      if ((event.jaxis.axis != 0) && (event.jaxis.axis != 1)) continue; /* skip events related to axis over 0-1 */
      if (event.jaxis.value > 3200) {
        curax = 1;
      } else if (event.jaxis.value < -3200) {
        curax = -1;
      } else {
        curax = 0;
      }
      if (curax > 0) {
        newjoyaxstate[event.jaxis.axis][0] = 0;
        newjoyaxstate[event.jaxis.axis][1] = 1;
      } else if (curax < 0) {
        newjoyaxstate[event.jaxis.axis][0] = 1;
        newjoyaxstate[event.jaxis.axis][1] = 0;
      } else {
        newjoyaxstate[event.jaxis.axis][0] = 0;
        newjoyaxstate[event.jaxis.axis][1] = 0;
      }
      /* check axis states and return the result */
      if (joysupport != 0) {
        CHECKAXIS();
      }
    } else if ((event.type == SDL_JOYBUTTONDOWN) || (event.type == SDL_JOYBUTTONUP)) { /* joystick button */
      int res;
      if (event.type == SDL_JOYBUTTONDOWN) {
        res = DRV_INPUT_JOYSTICK | DRV_INPUT_JOYDOWN;
      } else {
        res = DRV_INPUT_JOYSTICK | DRV_INPUT_JOYUP;
      }
      switch (event.jbutton.button) {
        case 0: return(res | DRV_INPUT_JOY_1);
        case 1: return(res | DRV_INPUT_JOY_2);
        case 2: return(res | DRV_INPUT_JOY_3);
        case 3: return(res | DRV_INPUT_JOY_4);
        case 4: return(res | DRV_INPUT_JOY_5);
        case 5: return(res | DRV_INPUT_JOY_6);
        case 6: return(res | DRV_INPUT_JOY_7);
        case 7: return(res | DRV_INPUT_JOY_8);
        case 8: return(res | DRV_INPUT_JOY_9);
        case 9: return(res | DRV_INPUT_JOY_10);
        case 10: return(res | DRV_INPUT_JOY_11);
        case 11: return(res | DRV_INPUT_JOY_12);
        case 12: return(res | DRV_INPUT_JOY_13);
        case 13: return(res | DRV_INPUT_JOY_14);
        case 14: return(res | DRV_INPUT_JOY_15);
        case 15: return(res | DRV_INPUT_JOY_16);
      }
    }
  } /* for (;;) */
}


int drv_loadpal(uint32_t *newpalette) {
  int x;
  for (x = 0; x < 256; x++) {
    palette[x] = 0xff; /* alpha value */
    palette[x] <<= 24; /* move the alpha bits to their place */
    palette[x] |= newpalette[x]; /* fill in the RGB triplets */
  }
  return(0);
}


/* get time elapsed (in ms) since an undefined point in time */
unsigned long drv_getticks(void) {
  return(SDL_GetTicks());
}


void drv_delay(int ms) {
  SDL_Delay(ms);
}


void drv_putpixel(int x, int y, int color) {
  screenbuf[y * screenwidth + x] = palette[color];
}


void drv_refreshscreen(void) {
  SDL_UpdateTexture(screentexture, NULL, screenbuf, screenwidth * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, screentexture, NULL, NULL);
  SDL_RenderPresent(renderer);
}


void drv_getsavedir(char *savedir, int maxlen) {
  char *prefpath;
  if (maxlen > 0) savedir[0] = 0;
  maxlen -= 1; /* to be sure we will always have enough room for the extra NULL terminator */
  if (maxlen < 1) return;
  prefpath = SDL_GetPrefPath("Mateusz Viste", "zBoy");
  if (prefpath == NULL) return;
  if (strlen(prefpath) > (unsigned) maxlen) return;
  strcpy(savedir, prefpath);
  SDL_free(prefpath);
}


int drv_close(void) {
  free(screenbuf);
  SDL_DestroyTexture(screentexture);
  SDL_DestroyRenderer(renderer);
  SDL_JoystickClose(joystick);
  SDL_Quit(); /* remember to shutdown SDL before quitting. */
  return(0);
}
