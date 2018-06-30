/*
   This file is part of the zBoy project
   Copyright (C) Mateusz Viste 2010,2011,2012
*/

void ResetEmulator(void) {  /* Resets all internal states of the emulator */
  int x;

  Register.PC = 0x100;   /* The execution of code starts at 0x100 (Cartridge ROM) */

  for (x = 0x8000; x <= 0x9FFF; x++) {  /* Reset all VRAM memory (this */
    VideoRAM[x] = 0;                    /* is done by a real GameBoy as */
  }                                     /* well, via the bootstrap ROM) */
  WriteRegBC(00, 0x13);  /* 0013h */
  WriteRegDE(00, 0xD8);  /* 00D8h */
  WriteRegHL(01, 0x4D);  /* 014Dh */
  Register.SP = 0xFFFE;  /* Stack pointer is inited at &hFFFE */
  IoRegisters[0xFF05] = 0x00;   /* TIMA */
  IoRegisters[0xFF06] = 0x00;   /* TMA */
  IoRegisters[0xFF07] = 0x00;   /* TAC */
  IoRegisters[0xFF10] = 0x80;   /* NR10 */
  IoRegisters[0xFF11] = 0xBF;   /* NR11 */
  IoRegisters[0xFF12] = 0xF3;   /* NR12 */
  IoRegisters[0xFF14] = 0xBF;   /* NR14 */
  IoRegisters[0xFF16] = 0x3F;   /* NR21 */
  IoRegisters[0xFF17] = 0x00;   /* NR22 */
  IoRegisters[0xFF19] = 0xBF;   /* NR24 */
  IoRegisters[0xFF1A] = 0x7F;   /* NR30 */
  IoRegisters[0xFF1B] = 0xFF;   /* NR31 */
  IoRegisters[0xFF1C] = 0x9F;   /* NR32 */
  IoRegisters[0xFF1E] = 0xBF;   /* NR33 */
  IoRegisters[0xFF20] = 0xFF;   /* NR41 */
  IoRegisters[0xFF21] = 0x00;   /* NR42 */
  IoRegisters[0xFF22] = 0x00;   /* NR43 */
  IoRegisters[0xFF23] = 0xBF;   /* NR?? */
  IoRegisters[0xFF24] = 0x77;   /* NR50 */
  IoRegisters[0xFF25] = 0xF3;   /* NR51 */

  IoRegisters[0xFF26] = 0xF1;   /* NR52  / On GB only */
  /*IoRegisters(0xFF26] = 0xF0; */ /* NR52  / On SGB only */

  IoRegisters[0xFF40] = 0x91;   /* LCDC */
  IoRegisters[0xFF42] = 0x00;   /* SCY */
  IoRegisters[0xFF43] = 0x00;   /* SCX */
  IoRegisters[0xFF45] = 0x00;   /* LYC */
  IoRegisters[0xFF47] = 0xFC;   /* BGP */
  IoRegisters[0xFF48] = 0xFF;   /* OBP0 */
  IoRegisters[0xFF49] = 0xFF;   /* OBP1 */
  IoRegisters[0xFF4A] = 0x00;   /* WY */
  IoRegisters[0xFF4B] = 0x00;   /* WX */

  MemoryInternalHiRAM[0xFFFF] = 0x00;  /* IE */

  Register.F = 0;
  SetFlagZ();      /* Set some flags */
  SetFlagH();      /* - as done by */
  SetFlagC();      /* the NO$CASH */
  Register.A = 1;  /* GB emulator. */
  InterruptsState = 0;   /* IME is OFF at start (otherise "Hook" or "Pagemaster" don't even start, plus NO$GMB disables IME at start, too) */
  HaltBug = 0;    /* Emulation of the "HALT bug", */
  HaltState = 0;  /* must be all reset at start. */
  TimerC = 0;  /* Timer Counter */
  DividerCycleCounter = 255;  /* Divider counter */
  TotalCycles = 0;                /* Reset CPU cycles counter */
  JoyOldReg = 0;
  CurRamBank = 0;       /* Reset various */
  Mbc1Model = 1;        /* MMU-related */
  /*Mbc1RamRomSelect = 0; */ /* variables */
  if (RomInfos.MbcModel == 1) {  /* On MBC1, set CurRomBank to 1 */
      CurRomBank = 1;
    } else {
      CurRomBank = 0;
  }
  VideoClkCounterMode = 0;     /* Reset some LCD variables */
  VideoClkCounterVBlank = 0;   /* LY is 0 at start (that's what NO$GMB does at least) */
}
