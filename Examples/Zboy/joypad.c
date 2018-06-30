/*
 *  Joypad emulation of a GameBoy console
 *  This file is part of the zBoy project
 *  Copyright (C) Mateusz Viste 2010,2011,2012,2013,2014,2015
 *
 * The joypad is controlled via the register at $FF00:
 *  Bit 5 - P15 out
 *  Bit 4 - P14 out
 *  Bit 3 - P13 in
 *  Bit 2 - P12 in
 *  Bit 1 - P11 in
 *  Bit 0 - P10 in
 *
 * When one of the keys gets a transition from high to low, an interrupt is fired.
 * I got the list of SDL keycodes from http://sdl.beuc.net/sdl.wiki/SDLKey
 */

#define IFLAG_JOYSTICK 1
#define IFLAG_ERROR 2

struct KeyStateMap {  /* Used for internal monitoring of key states */
  int Up;
  int Down;
  int Right;
  int Left;
  int Select;
  int Start;
  int A;
  int B;
};

struct KeyStateMap KeyState;


int zboyScancodeToBackendScancode(char *zboyScanCode, int *iflag) {
  if (strcasecmp(zboyScanCode, "none") == 0) return(DRV_INPUT_NONE);
  /* KEYBOARD */
  if (strcasecmp(zboyScanCode, "kb.f1") == 0) return(DRV_INPUT_KEY_F1);
  if (strcasecmp(zboyScanCode, "kb.f2") == 0) return(DRV_INPUT_KEY_F2);
  if (strcasecmp(zboyScanCode, "kb.f3") == 0) return(DRV_INPUT_KEY_F3);
  if (strcasecmp(zboyScanCode, "kb.f4") == 0) return(DRV_INPUT_KEY_F4);
  if (strcasecmp(zboyScanCode, "kb.f5") == 0) return(DRV_INPUT_KEY_F5);
  if (strcasecmp(zboyScanCode, "kb.f6") == 0) return(DRV_INPUT_KEY_F6);
  if (strcasecmp(zboyScanCode, "kb.f7") == 0) return(DRV_INPUT_KEY_F7);
  if (strcasecmp(zboyScanCode, "kb.f8") == 0) return(DRV_INPUT_KEY_F8);
  if (strcasecmp(zboyScanCode, "kb.f9") == 0) return(DRV_INPUT_KEY_F9);
  if (strcasecmp(zboyScanCode, "kb.f10") == 0) return(DRV_INPUT_KEY_F10);
  if (strcasecmp(zboyScanCode, "kb.f11") == 0) return(DRV_INPUT_KEY_F11);
  if (strcasecmp(zboyScanCode, "kb.f12") == 0) return(DRV_INPUT_KEY_F12);
  if (strcasecmp(zboyScanCode, "kb.esc") == 0) return(DRV_INPUT_KEY_ESC);
  if (strcasecmp(zboyScanCode, "kb.tab") == 0) return(DRV_INPUT_KEY_TAB);
  if (strcasecmp(zboyScanCode, "kb.lctrl") == 0) return(DRV_INPUT_KEY_LCTRL);
  if (strcasecmp(zboyScanCode, "kb.rctrl") == 0) return(DRV_INPUT_KEY_RCTRL);
  if (strcasecmp(zboyScanCode, "kb.lalt") == 0) return(DRV_INPUT_KEY_LALT);
  if (strcasecmp(zboyScanCode, "kb.ralt") == 0) return(DRV_INPUT_KEY_RALT);
  if (strcasecmp(zboyScanCode, "kb.spc") == 0) return(DRV_INPUT_KEY_SPC);
  if (strcasecmp(zboyScanCode, "kb.ret") == 0) return(DRV_INPUT_KEY_RET);
  if (strcasecmp(zboyScanCode, "kb.bks") == 0) return(DRV_INPUT_KEY_BKSPC);
  if (strcasecmp(zboyScanCode, "kb.left") == 0) return(DRV_INPUT_KEY_LEFT);
  if (strcasecmp(zboyScanCode, "kb.right") == 0) return(DRV_INPUT_KEY_RIGHT);
  if (strcasecmp(zboyScanCode, "kb.up") == 0) return(DRV_INPUT_KEY_UP);
  if (strcasecmp(zboyScanCode, "kb.down") == 0) return(DRV_INPUT_KEY_DOWN);
  if (strcasecmp(zboyScanCode, "kb.rshift") == 0) return(DRV_INPUT_KEY_RSHIFT);
  if (strcasecmp(zboyScanCode, "kb.lshift") == 0) return(DRV_INPUT_KEY_LSHIFT);
  if (strcasecmp(zboyScanCode, "kb.lwin") == 0) return(DRV_INPUT_KEY_LWIN);
  if (strcasecmp(zboyScanCode, "kb.rwin") == 0) return(DRV_INPUT_KEY_RWIN);
  if (strcasecmp(zboyScanCode, "kb.menu") == 0) return(DRV_INPUT_KEY_MENU);
  if (strcasecmp(zboyScanCode, "kb.pgup") == 0) return(DRV_INPUT_KEY_PGUP);
  if (strcasecmp(zboyScanCode, "kb.pgdw") == 0) return(DRV_INPUT_KEY_PGDOWN);
  if (strcasecmp(zboyScanCode, "kb.home") == 0) return(DRV_INPUT_KEY_HOME);
  if (strcasecmp(zboyScanCode, "kb.end") == 0) return(DRV_INPUT_KEY_END);
  if (strcasecmp(zboyScanCode, "kb.ins") == 0) return(DRV_INPUT_KEY_INS);
  if (strcasecmp(zboyScanCode, "kb.del") == 0) return(DRV_INPUT_KEY_DEL);
  if (strcasecmp(zboyScanCode, "kb.a") == 0) return(DRV_INPUT_KEY_A);
  if (strcasecmp(zboyScanCode, "kb.b") == 0) return(DRV_INPUT_KEY_B);
  if (strcasecmp(zboyScanCode, "kb.c") == 0) return(DRV_INPUT_KEY_C);
  if (strcasecmp(zboyScanCode, "kb.d") == 0) return(DRV_INPUT_KEY_D);
  if (strcasecmp(zboyScanCode, "kb.e") == 0) return(DRV_INPUT_KEY_E);
  if (strcasecmp(zboyScanCode, "kb.f") == 0) return(DRV_INPUT_KEY_F);
  if (strcasecmp(zboyScanCode, "kb.g") == 0) return(DRV_INPUT_KEY_G);
  if (strcasecmp(zboyScanCode, "kb.h") == 0) return(DRV_INPUT_KEY_H);
  if (strcasecmp(zboyScanCode, "kb.i") == 0) return(DRV_INPUT_KEY_I);
  if (strcasecmp(zboyScanCode, "kb.j") == 0) return(DRV_INPUT_KEY_J);
  if (strcasecmp(zboyScanCode, "kb.k") == 0) return(DRV_INPUT_KEY_K);
  if (strcasecmp(zboyScanCode, "kb.l") == 0) return(DRV_INPUT_KEY_L);
  if (strcasecmp(zboyScanCode, "kb.m") == 0) return(DRV_INPUT_KEY_M);
  if (strcasecmp(zboyScanCode, "kb.n") == 0) return(DRV_INPUT_KEY_N);
  if (strcasecmp(zboyScanCode, "kb.o") == 0) return(DRV_INPUT_KEY_O);
  if (strcasecmp(zboyScanCode, "kb.p") == 0) return(DRV_INPUT_KEY_P);
  if (strcasecmp(zboyScanCode, "kb.q") == 0) return(DRV_INPUT_KEY_Q);
  if (strcasecmp(zboyScanCode, "kb.r") == 0) return(DRV_INPUT_KEY_R);
  if (strcasecmp(zboyScanCode, "kb.s") == 0) return(DRV_INPUT_KEY_S);
  if (strcasecmp(zboyScanCode, "kb.t") == 0) return(DRV_INPUT_KEY_T);
  if (strcasecmp(zboyScanCode, "kb.u") == 0) return(DRV_INPUT_KEY_U);
  if (strcasecmp(zboyScanCode, "kb.v") == 0) return(DRV_INPUT_KEY_V);
  if (strcasecmp(zboyScanCode, "kb.w") == 0) return(DRV_INPUT_KEY_W);
  if (strcasecmp(zboyScanCode, "kb.x") == 0) return(DRV_INPUT_KEY_X);
  if (strcasecmp(zboyScanCode, "kb.y") == 0) return(DRV_INPUT_KEY_Y);
  if (strcasecmp(zboyScanCode, "kb.z") == 0) return(DRV_INPUT_KEY_Z);
  if (strcasecmp(zboyScanCode, "kb.0") == 0) return(DRV_INPUT_KEY_0);
  if (strcasecmp(zboyScanCode, "kb.1") == 0) return(DRV_INPUT_KEY_1);
  if (strcasecmp(zboyScanCode, "kb.2") == 0) return(DRV_INPUT_KEY_2);
  if (strcasecmp(zboyScanCode, "kb.3") == 0) return(DRV_INPUT_KEY_3);
  if (strcasecmp(zboyScanCode, "kb.4") == 0) return(DRV_INPUT_KEY_4);
  if (strcasecmp(zboyScanCode, "kb.5") == 0) return(DRV_INPUT_KEY_5);
  if (strcasecmp(zboyScanCode, "kb.6") == 0) return(DRV_INPUT_KEY_6);
  if (strcasecmp(zboyScanCode, "kb.7") == 0) return(DRV_INPUT_KEY_7);
  if (strcasecmp(zboyScanCode, "kb.8") == 0) return(DRV_INPUT_KEY_8);
  if (strcasecmp(zboyScanCode, "kb.9") == 0) return(DRV_INPUT_KEY_9);
  /* JOYPAD - if it seems to match, set a flag */
  if (strcasecmp(zboyScanCode, "joy.a1pos") == 0) return(DRV_INPUT_JOYAX_1POS);
  if (strcasecmp(zboyScanCode, "joy.a1neg") == 0) return(DRV_INPUT_JOYAX_1NEG);
  if (strcasecmp(zboyScanCode, "joy.a2pos") == 0) return(DRV_INPUT_JOYAX_2POS);
  if (strcasecmp(zboyScanCode, "joy.a2neg") == 0) return(DRV_INPUT_JOYAX_2NEG);
  if (strcasecmp(zboyScanCode, "joy.a3pos") == 0) return(DRV_INPUT_JOYAX_3POS);
  if (strcasecmp(zboyScanCode, "joy.a3neg") == 0) return(DRV_INPUT_JOYAX_3NEG);
  if (strcasecmp(zboyScanCode, "joy.a4pos") == 0) return(DRV_INPUT_JOYAX_4POS);
  if (strcasecmp(zboyScanCode, "joy.a4neg") == 0) return(DRV_INPUT_JOYAX_4NEG);
  if (strcasecmp(zboyScanCode, "joy.b1") == 0) return(DRV_INPUT_JOY_1);
  if (strcasecmp(zboyScanCode, "joy.b2") == 0) return(DRV_INPUT_JOY_2);
  if (strcasecmp(zboyScanCode, "joy.b3") == 0) return(DRV_INPUT_JOY_3);
  if (strcasecmp(zboyScanCode, "joy.b4") == 0) return(DRV_INPUT_JOY_4);
  if (strcasecmp(zboyScanCode, "joy.b5") == 0) return(DRV_INPUT_JOY_5);
  if (strcasecmp(zboyScanCode, "joy.b6") == 0) return(DRV_INPUT_JOY_6);
  if (strcasecmp(zboyScanCode, "joy.b7") == 0) return(DRV_INPUT_JOY_7);
  if (strcasecmp(zboyScanCode, "joy.b8") == 0) return(DRV_INPUT_JOY_8);
  if (strcasecmp(zboyScanCode, "joy.b9") == 0) return(DRV_INPUT_JOY_9);
  if (strcasecmp(zboyScanCode, "joy.b10") == 0) return(DRV_INPUT_JOY_10);
  if (strcasecmp(zboyScanCode, "joy.b11") == 0) return(DRV_INPUT_JOY_11);
  if (strcasecmp(zboyScanCode, "joy.b12") == 0) return(DRV_INPUT_JOY_12);
  if (strcasecmp(zboyScanCode, "joy.b13") == 0) return(DRV_INPUT_JOY_13);
  if (strcasecmp(zboyScanCode, "joy.b14") == 0) return(DRV_INPUT_JOY_14);
  if (strcasecmp(zboyScanCode, "joy.b15") == 0) return(DRV_INPUT_JOY_15);
  if (strcasecmp(zboyScanCode, "joy.b16") == 0) return(DRV_INPUT_JOY_16);
  /* nothing matched -- this is an error condition */
  *iflag |= IFLAG_ERROR;
  return(DRV_INPUT_NONE);
}


#define DownIsPressed(); \
  if (KeyState.Down == 0) { \
    KeyState.Down = 1<<3; \
    INT(INT_JOYPAD); \
  }

#define DownIsReleased(); \
  KeyState.Down = 0;

#define UpIsPressed(); \
  if (KeyState.Up == 0) { \
    KeyState.Up = 1<<2; \
    INT(INT_JOYPAD); \
  }

#define UpIsReleased(); \
  KeyState.Up = 0;

#define LeftIsPressed(); \
  if (KeyState.Left == 0) { \
    KeyState.Left = 1<<1; \
    INT(INT_JOYPAD); \
  }

#define LeftIsReleased(); \
  KeyState.Left = 0;

#define RightIsPressed(); \
  if (KeyState.Right == 0) { \
    KeyState.Right = 1; \
    INT(INT_JOYPAD); \
  }

#define RightIsReleased(); \
  KeyState.Right = 0;

#define StartIsPressed(); \
  if (KeyState.Start == 0) { \
    KeyState.Start = 1<<3; \
    INT(INT_JOYPAD); \
  }

#define StartIsReleased(); \
  KeyState.Start = 0;

#define SelectIsPressed(); \
  if (KeyState.Select == 0) { \
    KeyState.Select = 1<<2; \
    INT(INT_JOYPAD); \
  }

#define SelectIsReleased(); \
  KeyState.Select = 0;

#define ButtonAisPressed(); \
  if (KeyState.A == 0) { \
    KeyState.A = 1<<1; \
    INT(INT_JOYPAD); \
  }

#define ButtonAisReleased(); \
  KeyState.A = 0;

#define ButtonBisPressed(); \
  if (KeyState.B == 0) { \
    KeyState.B = 1; \
    INT(INT_JOYPAD); \
  }

#define ButtonBisReleased(); \
  KeyState.B = 0;

#define ButtonTurboAisPressed(); \
  turbo_a = 1; \
  turbo_a_nextswap = TotalCycles;

#define ButtonTurboAisReleased(); \
  turbo_a = 0; \
  KeyState.A = 0;

#define ButtonTurboBisPressed(); \
  turbo_b = 1; \
  turbo_b_nextswap = TotalCycles;

#define ButtonTurboBisReleased(); \
  turbo_b = 0; \
  KeyState.B = 0;

#define ButtonBgctrlIsReleased(); \
  if (HideBackgroundDisplay == 0) { \
      HideBackgroundDisplay = 1; \
      SetUserMsg("BACKGROUND OFF"); \
    } else { \
      HideBackgroundDisplay = 0; \
      SetUserMsg("BACKGROUND ON"); \
  }

#define ButtonSprctrlIsReleased(); \
  if (HideSpritesDisplay == 0) { \
      HideSpritesDisplay = 1; \
      SetUserMsg("SPRITES OFF"); \
    } else { \
      HideSpritesDisplay = 0; \
      SetUserMsg("SPRITES ON"); \
  }

#define ButtonWinctrlIsReleased(); \
  if (HideWindowDisplay == 0) { \
      HideWindowDisplay = 1; \
      SetUserMsg("WINDOW OFF"); \
    } else { \
      HideWindowDisplay = 0; \
      SetUserMsg("WINDOW ON"); \
  }

#define ButtonScreenshotIsReleased(); \
  AskForScreenshot = 1;

#define ButtonAutoscreenshotIsReleased(); \
  if (AutoScreenshot == 0) { \
      AutoScreenshot = 1; \
      SetUserMsg("AUTOSCREENSHOT ON"); \
    } else { \
      AutoScreenshot = 0; \
      SetUserMsg("AUTOSCREENSHOT OFF"); \
  }

#define ButtonResetIsReleased(); \
  SetUserMsg("RESET..."); \
  if (zboyparams->HiScoresMem != 0) SaveHiScoresMem(); \
  ResetEmulator(); \
  if (zboyparams->HiScoresMem != 0) LoadHiScoresMem(); \

#define ButtonSaveIsReleased(); \
  SaveGame();

#define ButtonLoadIsReleased(); \
  LoadGame();

#define ButtonQuitIsReleased(); \
  QuitEmulator = 1;


inline void CheckJoypad(int cycles, struct zboyparamstype *zboyparams) {
  static uint64_t JoyCheckCounter = 0;
  // static int turbo_a = 0, turbo_b = 0;
  int JoyNewReg;
  // static uint64_t turbo_a_nextswap = 0, turbo_b_nextswap = 0;
  int event;

  JoyCheckCounter += cycles;

  if (JoyCheckCounter > 80000) {  /* Check joypad state every ~20 ms */
    JoyCheckCounter = 0;
    /* Now poll the event queue... */
    while ((event = drv_keypoll()) != DRV_INPUT_NONE) {
        switch (drv_event_gettype(event)) {
            case DRV_INPUT_KEYBOARD | DRV_INPUT_KEYDOWN:
            case DRV_INPUT_JOYSTICK | DRV_INPUT_JOYDOWN:
            case DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXDOWN:
                if (drv_event_getval(event) == zboyparams->key_start) {
                    StartIsPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_select) {
                    SelectIsPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_b) {
                    ButtonBisPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_a) {
                    ButtonAisPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_up) {
                    UpIsPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_down) {
                    DownIsPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_left) {
                    LeftIsPressed();
                  } else if (drv_event_getval(event) == zboyparams->key_right) {
                    RightIsPressed();
                }
                break;
            case DRV_INPUT_KEYBOARD | DRV_INPUT_KEYUP:
            case DRV_INPUT_JOYSTICK | DRV_INPUT_JOYUP:
            case DRV_INPUT_JOYSTICK | DRV_INPUT_JOYAXUP:
                if (drv_event_getval(event) == zboyparams->key_start) { /* ENTER */
                    StartIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_select) { /* TAB */
                    SelectIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_b) { /* ALT */
                    ButtonBisReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_a) { /* LCTRL */
                    ButtonAisReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_up) { /* UP */
                    UpIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_down) { /* DOWN */
                    DownIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_left) { /* LEFT */
                    LeftIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_right) { /* RIGHT */
                    RightIsReleased();
		    /*
                  } else if (drv_event_getval(event) == zboyparams->key_bckg) { /* F1 * /
                    ButtonBgctrlIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_sprt) { /* F2 * /
                    ButtonSprctrlIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_wind) { /* F3 * /
                    ButtonWinctrlIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_save) { /* F5 * /
		  // ButtonSaveIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_load) { /* F7 * /
		  // ButtonLoadIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_asht) { /* F8 * /
                    ButtonAutoscreenshotIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_shot) { /* F9 * /
                    ButtonScreenshotIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_rset) { /* F10 * /
                    ButtonResetIsReleased();
                  } else if (drv_event_getval(event) == zboyparams->key_quit) { /* ESC * /
                    ButtonQuitIsReleased();
		    /* */
                }
                break;
            case DRV_INPUT_QUIT:
                QuitEmulator = 1;
                break;
	}
    }
  }

  JoyRegA = 0xF ^ (KeyState.Down | KeyState.Up | KeyState.Left | KeyState.Right);
  // if (KeyState.Down != 0) JoyNewReg &= bx11110111; /* if DOWN -> Set P13 */
  // if (KeyState.Up != 0) JoyNewReg &= bx11111011; /* if UP -> Set P12 */
  // if (KeyState.Left != 0) JoyNewReg &= bx11111101; /* if LEFT -> Set P11 */
  // if (KeyState.Right != 0) JoyNewReg &= bx11111110; /* if RIGHT -> Set P10 */

  JoyRegB = 0xF ^ (KeyState.Start | KeyState.Select | KeyState.A | KeyState.B);
  //  if (KeyState.Start != 0) JoyNewReg &= bx11110111; /* if ENTER -> Set P13 */
  //  if (KeyState.Select != 0) JoyNewReg &= bx11111011; /* if TAB -> Set P12 */
  //  if (KeyState.A != 0) JoyNewReg &= bx11111101; /* if CTRL -> Set P11 */
  //  if (KeyState.B != 0) JoyNewReg &= bx11111110; /* if ALT -> Set P10 */

  // if (JoyOldReg != JoyNewReg) JoyOldReg = JoyNewReg; /* If a key has been pressed/released update the JoyOldReg variable */
  IOWrite( IoRegisters[0xFF00] );
  
}
