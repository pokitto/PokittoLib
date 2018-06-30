/*
 * zBoy - A GameBoy emulator for Windows and Linux
 * Copyright (C) Mateusz Viste 2010, 2011, 2012, 2013, 2014, 2015
 * ----------------------------------------------------------------------
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 */

/* include some standard C headers */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit(), random() and randomize() */
#include <string.h>
#include <time.h>       /* for time-related functions (clock, time...) */
#include <stdint.h>     /* for uint8_t type */
#include <signal.h>

#include "drv.h"

#define pVer "0.60"
#define pDate "2010,2011,2012,2013,2014,2015"

/*  The options below make zBoy to be compiled in debug mode, and optionally trace CPU activity to a trace file */
/* #define DEBUGMODE */
/* #define DEBUGTRACEFILE */
/* #define MEMDUMP_AT_EXIT */

/* Produce a stand-alone exe file with an embedded ROM file */
/* #define EMBEDROM "embedrom/dracula.c" */
/* #define EMBEDROM "embedrom/pacman.c" */
/* #define EMBEDROM "embedrom/drmario.c" */

#ifndef DEBUGMODE   /* If not in debugmode, then disable PrintDebug */
  /* #define PrintDebug printf */
  /* #define PrintDebug(...) */
  void PrintDebug(char *s, ...) {
    s = s;  /* just to make gcc warnings shut up */
  }
  #else
  #define PrintDebug printf
#endif

#include "zboystructs.h"

struct RomInformations RomInfos;

// char CartTypes[256][256];
signed int AskForScreenshot = 0;
unsigned int LastScreenshotTime = 0;
uint8_t NetPlay = 0;
char *NetPlayPeer;
char *NetPlayBindaddr = NULL;
int QuitEmulator = 0;

unsigned int GraphicWidth, GraphicHeight;
// char UserMessage[1024];
unsigned int UserMessageFramesLeft = 0;

/* the zboyversion() function is used by many modules to display the app version */
char *zboyversion(void) {
  return(pVer);
}
/* the zboyversion() function is used by many modules to display the app date */
char *zboydate(void) {
  return(pDate);
}

#include "about.h"
#include "declares.h"         /* Some SUB/FUNCTION declarations */
#include "binary.h"           /* Provides basic support to binary notation (eg. bx01100111) */
#include "adjtiming.c"        /* AdjustTiming() */
#include "config.h"           /* zBoy_loadconfig() & zBoy_saveconfig() */
#include "numconv.c"          /* UbyteToByte() and DwordVal() */
#include "presskey.c"         /* PressAnyKey() */
#include "getsnapshotfilename.c"
#include "mmu.c"              /* Include Memory Management Unit emulation core */
#include "hiscores.c"         /* Provides SaveHiScoresMem() and LoadHiScoresMem() */
#include "cpu-ints.c"         /* Interrupts routines */
#include "video.c"            /* Video emulation core */
#include "loadpal.h"          /* LoadPalette() */
#include "colorize.c"         /* colorize() */
#include "joypad.c"           /* Joypad interface emulation */
#include "cpu-timer.c"        /* Include timer emulation */
#include "cpu-divider.c"      /* Include divider emulation */
#ifdef DEBUGMODE
  #include "debug.c"          /* Includes some debugging routines */
#endif
#include "cpu-z80.c"          /* Z80 CPU emulation core */
#include "battery.c"          /* Provides LoadBattRAM() and SaveBattRAM() */
#include "font.c"             /* Includes font data used by zBoy to display onscreen messages */
#include "wordwrap.h"         /* WordWrap(), required by PrintMsg() */
#include "printmsg.c"         /* PrintMsg(text AS STRING) */
#include "loadrom.h"          /* LoadROM(RomFile AS STRING) */
#include "serial.c"           /* Serial interface emulation */
#include "savepcx.c"          /* Screenshot routine SavePcxFile() */
#include "save.c"             /* SaveGame() and LoadGame() */
#include "reset.c"            /* ResetEmulator() */
#include "about.c"            /* printhelp() */


/* checks whether a joystick is needed in current input configuration
 * returns 0 if not, non-zero otherwise */
static int isjoystickneeded(struct zboyparamstype *p) {
  int res;
  res = p->key_bckg | p->key_sprt | p->key_wind | p->key_save | p->key_load |
        p->key_asht | p->key_shot | p->key_rset | p->key_quit | p->key_up |
        p->key_down | p->key_left | p->key_right | p->key_start |
        p->key_select | p->key_a | p->key_b | p->key_turboa | p->key_turbob;
  res &= DRV_INPUT_JOYSTICK;
  return(res);
}


int zboymain(int argc, char **argv) {
  struct zboyparamstype zboyparams;
  int UsedCycles;
  RomInfos.MemoryROM_PTR = MemoryROM;

  /* preload a default zBoy configuration */
  zboy_loaddefaultconfig(&zboyparams);

  /* (try to) load parameters from the zBoy configuration file */
  zboy_loadconfig(&zboyparams);

  zboyparams.GraphicScaleFactor = 1;    /* use 1x scaling */
  zboyparams.scalingalgo = REFRESHSCREEN_NOSCALE; /* No scaling at all */ 

  /* check the configuration, and fix it if needed */
  zboy_fixconfig(&zboyparams);

  GraphicWidth = 160 * zboyparams.GraphicScaleFactor;
  GraphicHeight = 144 * zboyparams.GraphicScaleFactor;

  /* init the I/O subsystem and set a window title */
  if (drv_init(GraphicWidth, GraphicHeight, isjoystickneeded(&zboyparams) != 0 ? zboyparams.joyid : -1) != 0) {
    return(0);
  }

  // InitScreenOldBuffer();   /* Init the screen buff used for LCD change detection (will force the complete draw of the 1st frame) */

  /* load the palette into the I/O subsystem now, in case we need to display an error message */
  // drv_loadpal(ScreenPalette32);

  /*
  if (NetPlay != 0) {
    if (net_open(NetPlayBindaddr, NetPlayPeer) != 0) {
      printf("Error: Failed to init networking!\n");
    }
  }
  * /
  
  switch (RomInfos.MbcModel) {  /* Check the MBC chip type, and select the right Read/Write functions * /
    case 0:   /* ROM ONLY * /
      MemoryWriteSpecial = &MBC0_MemoryWrite;
      MemoryReadSpecial = &MBC0_MemoryRead;
      break;
      /*
    case 1:   /* MBC1 * /
      MemoryWriteSpecial = &MBC1_MemoryWrite;
      MemoryReadSpecial = &MBC1_MemoryRead;
      break;
    case 2:   /* MBC2 * /
      MemoryWriteSpecial = &MBC2_MemoryWrite;
      MemoryReadSpecial = &MBC2_MemoryRead;
      break;
    case 3:   /* MBC3 * /
      MemoryWriteSpecial = &MBC3_MemoryWrite;
      MemoryReadSpecial = &MBC3_MemoryRead;
      break;
    case 5:   /* MBC5 * /
      MemoryWriteSpecial = &MBC5_MemoryWrite;
      MemoryReadSpecial = &MBC5_MemoryRead;
      break;
      * /
    default:  /* Any other (not supported) memory controller * /
      PrintMsg("ERROR: THE MEMORY CONTROLLER USED BY THIS ROM IS NOT SUPPORTED (YET) BY ZBOY. SORRY.", 0);
      RefreshScreen(0, 159, 0, 143, &zboyparams);
      PressAnyKey();
      return(0);
      break;
  }

  /*InitRAM() */    /* Init RAM areas to random values (as in a real GameBoy) */
  if ((RomInfos.Battery != 0) && (RomInfos.RamSize > 0)) LoadBattRAM();  /* check for battery-saved RAM */

  ResetEmulator(); /* Fire up the Zilog80 monster, and init all required variables */

  // if (zboyparams.HiScoresMem != 0) LoadHiScoresMem();   /* Check out if need to look after hiscores */

  /* starting emulation... */
  
  while (QuitEmulator == 0) {

    int partial = 0;
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    UsedCycles = partial;
    
    TotalCycles += partial;    /* Increment the global cycles counter */
    uTimer( UsedCycles );           /* Update uTimer */
    incDivider( UsedCycles );       /* Increment the DIV register */

    CheckInterrupts();  /* Check if any interrupt has been requested since last time */
    
    partial = 0;
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    partial += CpuExec();
    UsedCycles += partial;

    TotalCycles += partial;    /* Increment the global cycles counter */
    uTimer( partial );           /* Update uTimer */
    incDivider( partial );       /* Increment the DIV register */

    
    VideoSysUpdate(UsedCycles, &zboyparams);   /* Update Video subsystem */
    CheckJoypad(UsedCycles , &zboyparams);  /* Update the Joypad register */

    // CheckSerialLink(UsedCycles);  /* Check serial link (not supported yet) */
    CheckInterrupts();  /* Check if any interrupt has been requested since last time */
    
  }

  if ((RomInfos.Battery != 0) && (RomInfos.RamSize > 0)) SaveBattRAM();  /* save the battery-saved RAM, if any */
  if (zboyparams.HiScoresMem != 0) SaveHiScoresMem();
  zboy_saveconfig(&zboyparams);

  if (NetPlay != 0) net_close();

  /* shutdown the I/O subsystem */
  drv_close();

  return(0);
}
