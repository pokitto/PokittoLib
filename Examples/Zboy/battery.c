/*
  ----------------------------------------
   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010-2015
  ----------------------------------------

  Support for battery-saved RAM. This file
  provides two routines:

  SaveBattRam()  - Saves the battery-saved RAM
  LoadBattRAM()  - Loads the battery-saved RAM

  The battery-saved RAM is stored in the
  MemoryBankedRAM(0 TO 2097152) table. Battery
  support can be checked in RomInfos.Battery
  (0=No battery ; 1=Battery), and RAM size can
  be checked via RomInfos.RamSize (in bytes).

  zBoy is saving the content of battery-saved
  RAM in a specific (platform-dependant) location.
*/


void LoadBattRAM(void) {
  /*
  int x;
  FILE *BattFileHandler;
  char SaveDir[1024], savebattfile[1024];
  drv_getsavedir(SaveDir, 1024);
  snprintf(savebattfile, 1022, "%s%08X.ram", SaveDir, (unsigned int)RomInfos.CrcSum);
  BattFileHandler = fopen(savebattfile, "rb");
  if (BattFileHandler != NULL) {
    for (x = 0; x < RomInfos.RamSize; x++) {
      MemoryBankedRAM[0xA000 + x] = fgetc(BattFileHandler);
    }
    fclose(BattFileHandler);
  }
  */
}


void SaveBattRAM(void) {
  /*
  int x;
  FILE *BattFileHandler;
  char SaveDir[1024], savebattfile[1024];
  drv_getsavedir(SaveDir, 1024);
  snprintf(savebattfile, 1022, "%s%08X.ram", SaveDir, (unsigned int)RomInfos.CrcSum);
  BattFileHandler = fopen(savebattfile, "wb");
  if (BattFileHandler != NULL) {
    for (x = 0; x < RomInfos.RamSize; x++) {
      fputc(MemoryBankedRAM[0xA000 + x], BattFileHandler);
    }
    fclose(BattFileHandler);
  }
  */
}
