/*
  --------------------------------------
  This file is part of the zBoy project.
  Copyright (C) Mateusz Viste 2010, 2011
  --------------------------------------
 
  ROM-ONLY support
*/


uint8_t MBC0_MemoryRead(int memaddr) {
  if (memaddr < 0x8000) {                                   /* ROM memory [0000-7FFF] */
    return(MemoryROM[memaddr]);
  } else {                                                  /* Else it's something I don't know about */
    /* IF (memaddr > 0xFFFF) THEN
         PrintMsg("MMU FATAL ERROR!" & CHR(10) & "MEMORYREAD() -> INVALID MEMADDR! [" & HEX(memaddr) & "]") : PressAnyKey() : END
       END IF
       PRINT "MMU fatal error: Tried to read from an unknown memory address: " & HEX(memaddr) & "h"
       SLEEP
       QuitEmulator = 1 */
    return 0; // (MemoryMAP[memaddr]);
  }
}


void MBC0_MemoryWrite(int memaddr, uint8_t DataByte) {
  /*PRINT "MMU fatal error: Tried to write to an unknown memory address: "; HEX(memaddr)
    QuitEmulator = 1 */
  // MemoryMAP[memaddr] = DataByte;
}
