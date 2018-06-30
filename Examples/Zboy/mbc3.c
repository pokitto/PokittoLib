/*
   --------------------------------------
   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010, 2011
   --------------------------------------

   * MBC3 short documentation *
   (zboy doesn't provide timer support!)

   This controller is similar to MBC1 except it accesses all 16 Mbits of ROM without requiring any writes to
   the 4000-5FFF area. Writing a value (XBBBBBBB - X = Don't care, B = bank select bits) into 2000-3FFF area
   will select an appropriate ROM bank at 4000-7FFF.
   Also, this MBC has a built-in battery-backed Real Time Clock (RTC) not found in any other MBC. Some MBC3
   carts do not support it (WarioLand II non color version) but some do (Harvest Moon/Japanese version.)

   Note: The content of the MBC1 RAM can be saved during poweroff if the cartridge comes with a battery.
*/


uint8_t MBC3_MemoryRead(int memaddr) {
  if ((memaddr >= 0xA000) && (memaddr < 0xC000)) {        /* RAM bank n */
    return(MemoryBankedRAM[(CurRamBank << 13) + memaddr]);     /* SHL 13 is the same than *8192 (but faster) */
  } else if ((memaddr >= 0x4000) && (memaddr < 0x8000)) {    /* ROM bank #n */
    /*IF RomInfos.RomSize = 32768 THEN */
    /*    RETURN MemoryROM(memaddr) */
    /*  ELSE */
        if ((CurRomBank == 0) || (RomInfos.RomSize == 32768)) {
            return(MemoryROM[memaddr]);
          } else {
            if (((CurRomBank << 14) + (memaddr - 0x4000)) > RomInfos.RomSize) {    /* SHL 14 is the same than *16384 (but faster) */
              PrintMsg("INVALID ROM ADDRESS!\nCURBANK=%u", CurRomBank);
              PressAnyKey();
              exit(0);
            }
            return(MemoryROM[(CurRomBank << 14) + (memaddr - 0x4000)]);    /* SHL 14 is the same than *16384 (but faster) */
        }
    /*END IF */
  } else {                                               /* Else it's something I don't know about */
    /*IF (memaddr > 0xFFFF) THEN */
    /*  PrintMsg("MMU FATAL ERROR!" & CHR(10) & "MEMORYREAD() -> INVALID MEMADDR! [" & HEX(memaddr) & "]") : PressAnyKey() : END */
    /*END IF */
    /*PRINT "MMU fatal error: Tried to read from an unknown memory address: " & HEX(memaddr) & "h" */
    /*SLEEP */
    /*QuitEmulator = 1 */
    return(MemoryMAP[memaddr]);
  }
}


void MBC3_MemoryWrite(int memaddr, uint8_t DataByte) {
  if (memaddr < 0x2000) {  /* Enables / disables current RAM bank (not supported yet) */
    /* Before using any RAM bank, you need to enable it by writing xxxx1010 into 0000-1FFF (writing there */
    /* any non-xxxx1010 value disables the RAM bank). */
    if ((DataByte & bx00001111) == bx00001010) {   /* Enables currently selected RAM bank */
      } else {  /* Disables currently selected RAM bank */
        /*UserMessage = "RAM BANK DISABLED" */
    }
  } else if ((memaddr >= 0x2000) && (memaddr < 0x4000)) {       /* Select current ROM bank on MBC3 */
    CurRomBank = (DataByte & bx01111111);
  } else if ((memaddr >= 0x4000) && (memaddr < 0x6000)) {       /* Select current RAM bank */
   CurRamBank = (DataByte & bx00000011);
  } else if ((memaddr >= 0xA000) && (memaddr < 0xC000)) {       /* RAM bank n */
    MemoryBankedRAM[(CurRamBank << 13) + memaddr] = DataByte;   /* SHL 13 is the same than * 8192 (but faster) */
  } else if ((memaddr >= 0xFEA0) && (memaddr <= 0xFEFF)) {      /* This area is not usable, but some games writes to it... */
                                                                /* For example Dr. Mario tries to write to FEFF... who knows why? */
  } else if ((memaddr >= 0xFF4C) && (memaddr <= 0xFF7F)) {      /* This area is not usable, but some games writes to it... */
                                                                /* For example Dr. Mario tries to write to FF7F... who knows why? */
  } else {                                                      /* Else it's something I don't know about */
    /*PRINT "MMU fatal error: Tried to write to an unknown memory address: "; HEX(memaddr) */
    /*QuitEmulator = 1 */
    MemoryMAP[memaddr] = DataByte;
  }
}
