/*
   LoadPalette()  -  Loads a color palette from an external file.

   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010, 2011
*/

#ifndef ZBOY_LOADPAL_SENTINEL
  #define ZBOY_LOADPAL_SENTINEL
  int LoadPalette(char *palfile, uint32_t *palette);
  int LoadPaletteFromFile(char *palfile, uint32_t *palette);
  void CheckForColorizedPalette(uint32_t romcrc, uint32_t *palette);
#endif
