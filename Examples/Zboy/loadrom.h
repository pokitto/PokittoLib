/* LoadRom include file */

#ifndef ZBOY_LOADROM_SENTINEL
  #define ZBOY_LOADROM_SENTINEL
  int LoadRom(struct RomInformations *RomInfos, char *msg); /* Returns 0 on success, non-zero on failure */
  char *LoadRomErrStr(int errcode);
#endif
