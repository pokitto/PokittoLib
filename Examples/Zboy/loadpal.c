/*
   LoadPalette()  -  Loads a color palette from an external file.

   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010, 2011, 2012, 2013, 2014, 2015
*/

#include <stdio.h>  /* FILE */
#include <stdlib.h> /* strtol() */
#include <string.h> /* strcasecmp() */
#include <stdint.h>

int LoadPalette(char *palfile, uint32_t *ScreenPalette32) {
  ScreenPalette32[254] = 0x000000l; /* zBoy text foreground */
  ScreenPalette32[255] = 0xFFFFFFl; /* zBoy text background */
  if (strcasecmp(palfile, "gbmicro") == 0) {
    ScreenPalette32[0] = 0xFFFFFFl; /* background */
    ScreenPalette32[1] = 0xAAAAAAl;
    ScreenPalette32[2] = 0x555555l;
    ScreenPalette32[3] = 0x000000l;
  } else if (strcasecmp(palfile, "peasoup") == 0) {
    ScreenPalette32[0] = 0xD5EA94l;
    ScreenPalette32[1] = 0xACC641l;
    ScreenPalette32[2] = 0x527D39l;
    ScreenPalette32[3] = 0x204431l;
  } else {
    return(1);
  }
  /* synch colors for window, OBJ0 and OBJ1 */
  ScreenPalette32[32] = ScreenPalette32[0];
  ScreenPalette32[33] = ScreenPalette32[1];
  ScreenPalette32[34] = ScreenPalette32[2];
  ScreenPalette32[35] = ScreenPalette32[3];
  ScreenPalette32[64] = ScreenPalette32[0];
  ScreenPalette32[65] = ScreenPalette32[1];
  ScreenPalette32[66] = ScreenPalette32[2];
  ScreenPalette32[67] = ScreenPalette32[3];
  ScreenPalette32[128] = ScreenPalette32[0];
  ScreenPalette32[129] = ScreenPalette32[1];
  ScreenPalette32[130] = ScreenPalette32[2];
  ScreenPalette32[131] = ScreenPalette32[3];
  return(0);
}


static void readnextline(FILE *fd, char *line, int maxchars) {
  int bytebuff, linelen = 0;
  for (;;) {
    bytebuff = fgetc(fd);
    if (bytebuff < 0) break;
    if (bytebuff == '\n') break;
    if (bytebuff == '\r') continue;
    if (linelen < maxchars) {
      line[linelen++] = bytebuff;
    }
  }
  line[linelen] = 0;
}


int LoadPaletteFromFile(char *palfile, uint32_t *ScreenPalette32) {
  int x;
  int idx[16] = {0,1,2,3,32,33,34,35,64,65,66,67,128,129,130,131};
  char rgbline[8];
  long rgbval;
  FILE *fd;
  fd = fopen(palfile, "rb");
  if (fd == NULL) return(-1);
  for (x = 0; x < 16; x++) {
    readnextline(fd, rgbline, 6);
    rgbval = strtol(rgbline, NULL, 16);
    ScreenPalette32[idx[x]] = rgbval;
  }
  fclose(fd);
  return(0);
}
