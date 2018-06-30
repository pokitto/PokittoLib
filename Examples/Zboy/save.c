/*
  Save & Load game state
  This file is part of the zBoy project
  Copyright (C) Mateusz Viste 2010, 2011, 2012, 2013, 2014, 2015

 Here is the list of things I need to save (or load):
  Register.A
  Register.B
  Register.C
  Register.D
  Register.E
  Register.F
  Register.H
  Register.L
  Register.PC
  Register.SP
  InterruptState
  HaltBug
  HaltState
  TimerC
  TotalCycles
  NextTimingCheck
  EmulationStartTimer
  JoyOldReg
  JoyNewReg
  JoyNextCheck
  KeybState
  VideoClkCounterMode
  VideoClkCounterVBlank
  MemoryInternalRAM()
  MemoryInternalHiRAM()
  MemoryBankedRAM()  ' Check RAM size (if any) via RomInfos.RamSize
  MemoryROM()        ' Check ROM size via RomInfos.RomSize
  VideoRAM()
  SpriteOAM()
  IoRegisters()
  MemoryMAP()
  CurRamBank
  CurRomBank
  MbcModel
  BootRomEnabledFlag = 0
  DividerCycleCounter
*/


void LoadGame(void) {
  FILE *FileHandler;
  int x;
  unsigned int CrcSumFromFile = 0;
  char savefile[1024];
  char fileheader[20];
  char SaveDir[1024];
  drv_getsavedir(SaveDir, 1024);
  snprintf(savefile, 1022, "%s%08X.sav", SaveDir, (unsigned int)RomInfos.CrcSum);
  FileHandler = fopen(savefile, "rb");
  if (FileHandler == NULL) {
      SetUserMsg("NOTHING TO LOAD");
    } else {
      for (x = 0; x < 16; x++) fileheader[x] = fgetc(FileHandler);
      fileheader[16] = 0; /* string terminator */
      CrcSumFromFile = (fgetc(FileHandler) << 24);
      CrcSumFromFile |= (fgetc(FileHandler) << 16);
      CrcSumFromFile |= (fgetc(FileHandler) << 8);
      CrcSumFromFile |= fgetc(FileHandler);
      if ((CrcSumFromFile != RomInfos.CrcSum) || (strcmp(fileheader, "zBoy v0.50 SAVE.") != 0)) {  /* Problem! */
          SetUserMsg("LOADING GAME STATE FAILED!");
        } else {
          /* Load CPU Registers */
          Register.A = fgetc(FileHandler);
          Register.B = fgetc(FileHandler);
          Register.C = fgetc(FileHandler);
          Register.D = fgetc(FileHandler);
          Register.E = fgetc(FileHandler);
          Register.F = fgetc(FileHandler);
          Register.H = fgetc(FileHandler);
          Register.L = fgetc(FileHandler);
          Register.PC = (fgetc(FileHandler) << 8);
          Register.PC |= fgetc(FileHandler);
          Register.SP = (fgetc(FileHandler) << 8);
          Register.SP |= fgetc(FileHandler);
          /* Load the IME */
          InterruptsState = fgetc(FileHandler);
          /* Load internal counters and flags */
          HaltBug = fgetc(FileHandler);
          HaltState = fgetc(FileHandler);
          TimerC = (fgetc(FileHandler) << 8);
          TimerC |= fgetc(FileHandler);
          TotalCycles = ((uint64_t) fgetc(FileHandler) << 56);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 48);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 40);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 32);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 24);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 16);
          TotalCycles |= ((uint64_t) fgetc(FileHandler) << 8);
          TotalCycles |= (uint64_t) fgetc(FileHandler);

          JoyOldReg = fgetc(FileHandler);
          VideoClkCounterMode = (fgetc(FileHandler) << 8);
          VideoClkCounterMode |= fgetc(FileHandler);
          VideoClkCounterVBlank = (fgetc(FileHandler) << 8);
          VideoClkCounterVBlank |= fgetc(FileHandler);

          CurLY = fgetc(FileHandler);
          /*LastLY = fgetc(FileHandler); */
          fgetc(FileHandler); /* Ignore this byte */
          LastLYdraw = fgetc(FileHandler);
          for (x = 0xC000; x < 0xE000; x++) MemoryInternalRAM[x] = fgetc(FileHandler);
          for (x = 0xFF80; x <= 0xFFFF; x++) MemoryInternalHiRAM[x] = fgetc(FileHandler);
          for (x = 0xA000; x < RomInfos.RamSize; x++) MemoryBankedRAM[x] = fgetc(FileHandler);  /* Check RAM size (if any) via RomInfos.RamSize */
          for (x = 0x8000; x < 0xA000; x++) VideoRAM[x] = fgetc(FileHandler);
          for (x = 0xFE00; x < 0xFEA0; x++) SpriteOAM[x] = fgetc(FileHandler);
          for (x = 0xFF00; x < 0xFF4C; x++) IoRegisters[x] = fgetc(FileHandler);
          // for (x = 0; x <= 0xFFFF; x++) MemoryMAP[x] = fgetc(FileHandler);
          CurRamBank = fgetc(FileHandler);
          CurRomBank = fgetc(FileHandler);
          Mbc1Model = fgetc(FileHandler);
          /*Mbc1RamRomSelect = fgetc(FileHandler); */
          fgetc(FileHandler); /* Ignore this byte */
          /*BootRomEnabledFlag = fgetc(FileHandler);*/
          fgetc(FileHandler); /* Ignore this byte */
          DividerCycleCounter = fgetc(FileHandler);
          SetUserMsg("GAME LOADED");
      }
      fclose(FileHandler);
  }
}


void SaveGame(void) {
  FILE *FileHandler;
  signed int x;
  char savefile[1024];
  char SaveDir[1024];
  drv_getsavedir(SaveDir, 1024);
  snprintf(savefile, 1022, "%s%08X.sav", SaveDir, (unsigned int)RomInfos.CrcSum);
  FileHandler = fopen(savefile, "wb");
  if (FileHandler == NULL) {
      SetUserMsg("FILE ERROR");
    } else {
      /* Write header */
      fprintf(FileHandler, "zBoy v0.50 SAVE.");
      /* Write ROM's CRC32 checksum */
      fputc((RomInfos.CrcSum >> 24) & 0xFF, FileHandler);
      fputc((RomInfos.CrcSum >> 16) & 0xFF, FileHandler);
      fputc((RomInfos.CrcSum >> 8) & 0xFF, FileHandler);
      fputc(RomInfos.CrcSum & 0xFF, FileHandler);
      /* Write CPU Registers */
      fputc(Register.A, FileHandler);
      fputc(Register.B, FileHandler);
      fputc(Register.C, FileHandler);
      fputc(Register.D, FileHandler);
      fputc(Register.E, FileHandler);
      fputc(Register.F, FileHandler);
      fputc(Register.H, FileHandler);
      fputc(Register.L, FileHandler);
      fputc((Register.PC >> 8) & 0xFF, FileHandler);
      fputc(Register.PC & 0xFF, FileHandler);
      fputc((Register.SP >> 8) & 0xFF, FileHandler);
      fputc(Register.SP & 0xFF, FileHandler);
      /* Write the IME */
      fputc((InterruptsState & 0xFF), FileHandler);
      /* Write internal counters and flags */
      fputc((HaltBug & 0xFF), FileHandler);
      fputc((HaltState & 0xFF), FileHandler);
      fputc(((TimerC >> 8) & 0xFF), FileHandler);
      fputc((TimerC & 0xFF), FileHandler);

      fputc((TotalCycles >> 56) & 0xFF, FileHandler);
      fputc((TotalCycles >> 48) & 0xFF, FileHandler);
      fputc((TotalCycles >> 40) & 0xFF, FileHandler);
      fputc((TotalCycles >> 32) & 0xFF, FileHandler);
      fputc((TotalCycles >> 24) & 0xFF, FileHandler);
      fputc((TotalCycles >> 16) & 0xFF, FileHandler);
      fputc((TotalCycles >> 8) & 0xFF, FileHandler);
      fputc(TotalCycles & 0xFF, FileHandler);

      fputc(JoyOldReg, FileHandler);
      fputc((VideoClkCounterMode >> 8) & 0xFF, FileHandler);
      fputc(VideoClkCounterMode & 0xFF, FileHandler);
      fputc((VideoClkCounterVBlank >> 8) & 0xFF, FileHandler);
      fputc(VideoClkCounterVBlank & 0xFF, FileHandler);

      fputc(CurLY, FileHandler);
      /*fputc(LastLY, FileHandler); */
      fputc(0, FileHandler); /* Write a useless byte */
      fputc(LastLYdraw, FileHandler);
      for (x = 0xC000; x < 0xE000; x++) fputc(MemoryInternalRAM[x], FileHandler);
      for (x = 0xFF80; x <= 0xFFFF; x++) fputc(MemoryInternalHiRAM[x], FileHandler);
      for (x = 0xA000; x < RomInfos.RamSize; x++) fputc(MemoryBankedRAM[x], FileHandler);  /* Check RAM size (if any) via RomInfos.RamSize */
      for (x = 0x8000; x < 0xA000; x++) fputc(VideoRAM[x], FileHandler);
      for (x = 0xFE00; x < 0xFEA0; x++) fputc (SpriteOAM[x], FileHandler);
      for (x = 0xFF00; x < 0xFF4C; x++) fputc(IoRegisters[x], FileHandler);
      // for (x = 0; x <= 0xFFFF; x++) fputc(MemoryMAP[x], FileHandler);
      fputc(CurRamBank, FileHandler);
      fputc(CurRomBank, FileHandler);
      fputc(Mbc1Model, FileHandler);
      /*fputc(Mbc1RamRomSelect, FileHandler); */
      fputc(0, FileHandler); /* write a dummy byte */
      /*fputc(BootRomEnabledFlag, FileHandler);*/
      fputc(0, FileHandler); /* write a dummy byte */
      fputc(DividerCycleCounter, FileHandler);
      fclose(FileHandler);
      SetUserMsg("GAME SAVED");
  }
}
