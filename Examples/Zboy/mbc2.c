/*
  ------------------------------------------
  This file is part of the zBoy project.
  Copyright (C) Mateusz Viste 2010,2011,2012
  ------------------------------------------

  * MBC2 short documentation *

  This memory controller works much like the MBC1 controller with the following exceptions:
  MBC2 will work with ROM sizes up to 2Mbit. Writing a value (XXXXBBBB - X = Don't cares, B = bank select
  bits) into 2000-3FFF area will select an appropriate ROM bank at 4000-7FFF.
  RAM switching is not provided. Unlike the MBC1 which uses external RAM, MBC2 has 512 x 4 bits of RAM
  which is in the controller itself. It still requires an external battery to save data during power-off
  though.
  The least significant bit of the upper address byte must be zero to enable/disable cart RAM. For example
  the following addresses can be used to enable/disable cart RAM:
  0000-00FF, 0200-02FF, 0400-04FF, ..., 1E00-1EFF.
  enable/disable is 0000-00FF. The suggested address range to use for MBC2 ram The least significant bit of
  the upper address byte must be one to select a ROM bank. For example the following addresses can be used
  to select a ROM bank: 2100-21FF, 2300-23FF, 2500-25FF, ..., 3F00-3FFF. The suggested address range to use
  for MBC2 rom bank selection is 2100-21FF.

  Note: The content of the MBC2 RAM can be saved during poweroff if the cartridge comes with a battery.
*/


uint8_t MBC2_MemoryRead(int memaddr) {
  if ((memaddr >= 0xA000) && (memaddr < 0xA200)) {        /* RAM on cartridge (512x4 bits) */
    return(MemoryBankedRAM[memaddr]);
  } else if ((memaddr >= 0x4000) && (memaddr < 0x8000)) {    /* ROM bank #n */
    /*IF RomInfos.RomSize = 32768 THEN */
    /*    RETURN MemoryROM(memaddr) */
    /*  ELSE */
        if ((CurRomBank == 0) || (RomInfos.RomSize == 32768)) {
            /*RETURN MemoryROM(16384 + (memaddr - 0x4000)) */
            return(MemoryROM[memaddr]);
          } else {
            if (((CurRomBank << 14) + (memaddr - 0x4000)) > RomInfos.RomSize) {                  /* << 14 is the same than *16384 (but faster) */
              char msg[1024];
              // sprintf(msg, "INVALID ROM ADDRESS!\nCURBANK=%d", CurRomBank);
              PrintMsg(msg, 0);
              PressAnyKey();
              exit(0);
            }
            return(MemoryROM[(CurRomBank << 14) + (memaddr - 0x4000)]);    /* << 14 is the same than *16384 (but faster) */
        }
    /*END IF */
  } else {                                                  /* Else it's something I don't know about */
    /*IF (memaddr > 0xFFFF) THEN */
    /*  PrintMsg("MMU FATAL ERROR!" & CHR(10) & "MEMORYREAD() -> INVALID MEMADDR! [" & HEX(memaddr) & "]") : PressAnyKey() : END */
    /*END IF */
    /*PRINT "MMU fatal error: Tried to read from an unknown memory address: " & HEX(memaddr) & "h" */
    /*SLEEP */
    /*QuitEmulator = 1 */
    return(MemoryMAP[memaddr]);
  }
}


void MBC2_MemoryWrite(int memaddr, uint8_t DataByte) {
  if (memaddr < 0x2000) {  /* Enables / disables current RAM bank (not supported yet) */
    /*  Before using any RAM bank, you need to enable it by writing xxxx1010 into 0000-1FFF (writing there */
    /*  any non-xxxx1010 value disables the RAM bank). */
    if ((DataByte & bx00001111) == bx00001010) { /* Enables currently selected RAM bank */
      } else {  /* Disables currently selected RAM bank */
        /*UserMessage = "RAM BANK DISABLED" */
    }
  } else if ((memaddr >= 0x2000) && (memaddr < 0x4000)) {    /* Select current ROM bank (as on MBC1) */
    CurRomBank = (DataByte & bx00001111);
  } else if ((memaddr >= 0xA000) && (memaddr < 0xA200)) {    /* RAM on cartridge (512x4 bits) */
    MemoryBankedRAM[memaddr] = (DataByte & bx00001111);      /* write only lower 4 bits (this is a 4bit memory) */
  } else if ((memaddr >= 0xFEA0) && (memaddr <= 0xFEFF)) {   /* This area is not usable, but some games writes to it... */
                                                             /* For example Dr. Mario tries to write to FEFF... who knows why? */
  } else if ((memaddr >= 0xFF4C) && (memaddr <= 0xFF7F)) {   /* This area is not usable, but some games writes to it... */
                                                             /* For example Dr. Mario tries to write to FF7F... who knows why? */
  } else {                                                   /* Else it's something I don't know about */
    /*PRINT "MMU fatal error: Tried to write to an unknown memory address: "; HEX(memaddr) */
    /*QuitEmulator = 1 */
    MemoryMAP[memaddr] = DataByte;
  }
}
