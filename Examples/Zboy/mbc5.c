/*
   --------------------------------------
   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010
   --------------------------------------

   * MBC5 short documentation *

   The MBC5 controller is similar to the MBC3 (but no RTC) but can access up to 64 Mbits of ROM and up to
   1 Mbit of RAM. The lower 8 bits of the 9-bit ROM bank select is written to the 2000-2FFF area while the
   upper bit is written to the least significant bit of the 3000-3FFF area.

   Writing a value (XXXXBBBB - X = Don't care, B = bank select bits) into 4000-5FFF area will select an
   appropriate RAM bank at A000-BFFF if the cart contains RAM. Ram sizes are 64 Kbits, 256 Kbits, and 1 Mbit.

   Also, this is the first MBC that allows ROM bank 0 to appear in the 4000-7FFF range by writing $000 to the
   rom bank select.
*/


inline uint8_t MBC5_MemoryRead(int memaddr) {
  if ((memaddr >= 0xA000) && (memaddr < 0xC000)) {        /* RAM bank n */
    return(MemoryBankedRAM[(CurRamBank << 13) + memaddr]);
  } else if ((memaddr >= 0x4000) && (memaddr < 0x8000)) {    /* ROM bank #n */
    if (((CurRomBank << 14) + (memaddr - 0x4000)) >= RomInfos.RomSize) {  /* SHL 14 is the same than *16384 (but faster) */
      PrintMsg("INVALID ROM ADDRESS!\nCURBANK=%u", CurRomBank);
      QuitEmulator = 1;
      PressAnyKey();
    }
    return(MemoryROM[(CurRomBank << 14) + (memaddr - 0x4000)]);
  } else {                                                /* Else it's something I don't know about */
    /*PRINT "MMU fatal error: Tried to read from an unknown memory address: " & HEX(memaddr) & "h" */
    /*SLEEP */
    /*QuitEmulator = 1 */
    return(MemoryMAP[memaddr]);
  }
}


void MBC5_MemoryWrite(int memaddr, uint8_t DataByte) {
  if (memaddr < 0x1FFF) {  /* Enables / disables current RAM bank (not supported yet) */
    /* Before using any RAM bank, you need to enable it by writing xxxx1010 into 0000-1FFF (writing there */
    /* any non-xxxx1010 value disables the RAM bank). */
  } else if ((memaddr >= 0x2000) && (memaddr < 0x3000)) {     /* Select current ROM bank on MBC5 (8 lowest bits) */
    CurRomBank &= 256;
    CurRomBank |= DataByte;
  } else if ((memaddr >= 0x3000) && (memaddr < 0x4000)) {     /* Select current ROM bank on MBC5 (most significant bit) */
    if ((DataByte & bx00000001) == 0) {
        CurRomBank &= bx11111111;   /* Reset the most significant bit of the 9bit bank select */
      } else {
        CurRomBank |= 256;    /* Set the most significant bit of the 9bit bank select */
    }
  } else if ((memaddr >= 0x4000) && (memaddr < 0x6000)) {        /* Select RAM bank (xxxxBBBB) */
    CurRamBank = (DataByte & bx00001111);
  } else if ((memaddr >= 0xA000) && (memaddr < 0xC000)) {        /* RAM bank n */
    MemoryBankedRAM[(CurRamBank << 13) + memaddr] = DataByte;
  } else {                                                    /* Else it's something I don't know about */
    /*PRINT "MMU fatal error: Tried to write to an unknown memory address: "; HEX(memaddr) */
    /*SLEEP */
    /*QuitEmulator = 1 */
    MemoryMAP[memaddr] = DataByte;
  }
}
