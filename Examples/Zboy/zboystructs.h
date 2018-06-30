
/* Provide struct declarations for complex datatypes used by zBoy */

#include <stdint.h>

#ifndef ZBOY_STRUCTS_SENTINEL
#define ZBOY_STRUCTS_SENTINEL

struct RomInformations {
  //char Title[256];
  //char NiceTitle[256];       /* Same as title, but with lower/upper chars, spaces etc. */
  // char Filename[1024];       /* The filename of the ROM (with full path) */
  // char ShortFilename[1024];  /* The filename of the ROM (without path) */
  // char ShortFilenameNoExt[1024];  /* The filename of the ROM (without path and without extension) */
  signed char ColorGB;       /* 0 = not color, 1 = color (but still GB compatible), 2 = GBC only (not GB compatible) */
  signed char SuperGB;       /* 0 = GameBoy, 1 = Super GameBoy */
  // char CartTypeString[256];  /* Descriptive string for the cart type */
  uint32_t CrcSum;
  signed char MbcModel;      /* 0 = ROM ONLY  1 = MBC1  2 = MBC2... -1 = unknown */
  signed char Rumble;        /* 0 = No rumble  1 = Rumble pak (found only on some MBC5 chips) */
  signed char TimerRTC;      /* 0 = No RTC  1 = RTC  (found only on some MBC3 chips) */
  signed char Battery;       /* 0 = no battery, 1 = battery included */
  signed char Destination;   /* 0 = Japan, 1 = Other */
  // char Licensee[256];
  int RomSize;
  int RamSize;
  const uint8_t *MemoryROM_PTR;
};

enum refreshscreenalgorithm {
  REFRESHSCREEN_NOSCALE = 0,
  REFRESHSCREEN_BASICSCALE = 1,
  REFRESHSCREEN_2X = 2,
  REFRESHSCREEN_3X = 3,
  REFRESHSCREEN_EAGLE = 4
};

struct zboyparamstype {
  uint8_t ShowFPS;
  uint8_t ShowRomInfos;
  uint8_t NoSpeedLimit;
  uint8_t HiScoresMem;
  uint8_t NoCpuIdle;
  uint8_t GraphicScaleFactor;
  uint8_t fpslimit;
  uint8_t colorize;
  uint8_t palette;       /* the palette to use */
  uint8_t joyid;
  uint16_t key_up;
  uint16_t key_down;
  uint16_t key_left;
  uint16_t key_right;
  uint16_t key_start;
  uint16_t key_select;
  uint16_t key_a;
  uint16_t key_b;
  uint16_t key_turboa;
  uint16_t key_turbob;
  uint16_t key_bckg;
  uint16_t key_sprt;
  uint16_t key_wind;
  uint16_t key_save;
  uint16_t key_load;
  uint16_t key_asht;
  uint16_t key_shot;
  uint16_t key_rset;
  uint16_t key_quit;
  // char windowtitle[256]; /* used to hold the window's title */
  char *custcolor;
  enum refreshscreenalgorithm scalingalgo;
};

#endif
