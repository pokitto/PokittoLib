/*
   ----------------------------------------
    MMU emulation (Memory Management Unit)
    This file is part of the zBoy project.
    Copyright (C) Mateusz Viste 2010, 2011
   ----------------------------------------
*/

enum mbc1_models {
  MBC1_16_8 = 1,
  MBC1_4_32 = 2
};


uint8_t _MemoryInternalRAM[0x2000];    /* Internal RAM Area  [8KiB] */
uint8_t _MemoryInternalHiRAM[128]; /* Internal RAM (high area + IE register) */
uint8_t MemoryBankedRAM[0]; // 0x20A001];    /* Banked RAM [2MiB] */
#ifdef EMBEDROM
    #include EMBEDROM
  #else
    uint8_t MemoryROM[4194304];       /* Declare an empty table for ROM Area (redim later at loadrom time) */
#endif
uint8_t _VideoRAM[0x2000];      /* Video RAM [8KiB] */
uint8_t _SpriteOAM[0xA0];     /* Sprite OAM memory */
uint8_t _IoRegisters[0x80];   /* All I/O memory-mapped registers */
uint8_t * const MemoryInternalRAM = _MemoryInternalRAM - 0xC000;    /* Internal RAM Area  [8KiB] */
uint8_t * const MemoryInternalHiRAM = _MemoryInternalHiRAM - 0xFF80;
uint8_t * const VideoRAM = _VideoRAM - 0x8000;      /* Video RAM [8KiB] */
uint8_t * const SpriteOAM = _SpriteOAM - 0xFE00;     /* Sprite OAM memory */
uint8_t * const IoRegisters = _IoRegisters - 0xFF00;   /* All I/O memory-mapped registers */


// uint8_t MemoryMAP[0x10000];    /* Regular memory (fallback for unmapped regions) */
int Mbc1Model = MBC1_16_8;    /* MBC1 memory model (MbcModel can be 1 or 2)  1=16/8 ; 2=4/32 */
int CurRomBank = 0;           /* Used for ROM bank switching (must be at least 9 bits long for MBC5 support!) */
int CurRamBank = 0;           /* Current RAM bank selection */
// int SaveScoresWriteProtection[2048];

#include "mbc0.c"  /* Support for ROM-ONLY ROMs */

//#include "mbc1.c"  /* Support for MBC1 memory controllers */
//#include "mbc2.c"  /* Support for MBC2 memory controllers */
//#include "mbc3.c"  /* Support for MBC3 memory controllers (without TIMER support so far) */
//#include "mbc5.c"  /* Support for MBC5 memory controllers (usually found in GBC games) */

// uint8_t (*MemoryReadSpecial)(register int memaddr);
// void (*MemoryWriteSpecial)(register int memaddr, uint8_t DataByte);

#define MemoryWriteSpecial MBC0_MemoryWrite
#define MemoryReadSpecial MBC0_MemoryRead



void InitRAM(void) {  /* Init the RAM areas to random values, as in a real GameBoy */
  /*
  FOR x = LBOUND(MemoryInternalRAM) TO UBOUND(MemoryInternalRAM)
    MemoryInternalRAM(x) = INT(RND * 256)
  NEXT x
  FOR x = LBOUND(MemoryInternalHiRAM) TO UBOUND(MemoryInternalHiRAM)
    MemoryInternalHiRAM(x) = INT(RND * 256)
  NEXT x
  FOR x = LBOUND(MemoryBankedRAM) TO UBOUND(MemoryBankedRAM)
    MemoryBankedRAM(x) = INT(RND * 256)
  NEXT x */
  // for (int x = 0; x < 65536; x++) MemoryMAP[x] = (((unsigned) rand()) % 256);
}


/* Below are generic MemoryRead and MemoryWrite routines. These routines  *
 * check if the called address is a well-known address. If this address   *
 * is behaving the same on all known MBC controllers, then it is answered *
 * here (and it is FAST). Otherwise, a Memory routine specialized for the *
 * given MBC is called.                                                   */

uint8_t * const ramidx = (uint8_t *) 0x20000000;
uint8_t * const RAMette[] = {
  MemoryROM,
  _VideoRAM - 0x8000,
  _MemoryInternalRAM - 0xC000,
  _MemoryInternalRAM - 0xC000 - 8192,
  _SpriteOAM - 0xFE00,
  _IoRegisters - 0xFF00,
  _MemoryInternalHiRAM - 0xFF80
};

void indexRAM(){
  int i=0;
  for(; i<0x8000>>5; i++ )
    ramidx[i] = 0;
  for(; i<0xA000>>5; i++ )
    ramidx[i] = 1;  
  for(; i<0xE000>>5; i++ )
    ramidx[i] = 2;
  for(; i<0xFE00>>5; i++ )
    ramidx[i] = 3;
  for(; i<0xFEA0>>5; i++ )
    ramidx[i] = 4;
  for(; i<0xFF80>>5; i++ )
    ramidx[i] = 5;
  for(; i<0x10000>>5; i++ )
    ramidx[i] = 6;  
}

inline uint8_t MemoryRead(int ReadAddr) {
  return RAMette[ ramidx[ReadAddr>>5] ][ ReadAddr ];
  //   PrintDebug("MemoryRead 0x%04X\n", ReadAddr);
  //   if (ReadAddr < 0x4000) {                                     /* ROM bank #0 */
    //     return(MemoryROM[ReadAddr]);
    //   } else if ( ReadAddr < 0xE000 ) {    /* Internal 8KiB RAM */
    //     return(MemoryInternalRAM[ReadAddr]);
    //   } else if ( ReadAddr < 0xFE00 ) {    /* RAM mirror */
    //     return(MemoryInternalRAM[ReadAddr - 8192]);
    //   } else if ( ReadAddr < 0xFEA0 ) {    /* Sprite OAM memory */
    //     return(SpriteOAM[ReadAddr]);
    //   } else if ((ReadAddr >= 0xFF80) && (ReadAddr <= 0xFFFF)) {   /* Hi RAM area */
    //     return (MemoryInternalHiRAM[ReadAddr]);
    //   } else if ((ReadAddr >= 0xFF00) && (ReadAddr <= 0xFF4B)) {   /* I/O registers */
    //     return(IoRegisters[ReadAddr]);
    //   } else if ((ReadAddr >= 0x8000) && (ReadAddr < 0xA000)) {    /* Video RAM (8KiB) */
    //     return(VideoRAM[ReadAddr]);
    //   } else {
    //     return 0;
    //   }
}

uint8_t JoyRegA = 0, JoyRegB = 0, JoyOldReg;

void IOWrite(uint8_t JoyNewReg){
  if( (JoyNewReg & bx00100000) != 0) {
    JoyNewReg &= 0xF0;
    /* P14 selected (bit 4 is low) -> down/up/left/right */
    JoyNewReg |= JoyRegA;
  } else if ( (JoyNewReg & bx00010000) != 0) {
    JoyNewReg &= 0xF0;
    /* P15 selected (bit 5 is low) -> Start/Select/B/A */
    JoyNewReg |= JoyRegB;
  } else if ( JoyNewReg == 3){
    /* no bits 4 & 5 set, maybe the game wants to get the system type... */    
      JoyNewReg = 0xF0; /* returns FXh to indicate a classic GameBoy device */
  }

  IoRegisters[0xFF00] = JoyNewReg;   /* update the joypad register [FF00h] */
}

inline void MemoryWrite(uint32_t WriteAddr, uint8_t DataHolder) {
  PrintDebug("MemoryWrite 0x%04X [%02Xh]\n", WriteAddr, DataHolder);

  uint8_t *bank = RAMette[ ramidx[WriteAddr>>5] ];
  if( bank != IoRegisters ){
    bank += WriteAddr;
    if( ((uint32_t) bank) > 0x10000000 ){
      *bank = DataHolder;
    }
    return;
  }

  
  // if ((WriteAddr >= 0xC000) && (WriteAddr < 0xE000)) {    /* Internal 8KiB RAM */
    //   MemoryInternalRAM[WriteAddr] = DataHolder;
    // } else if ((WriteAddr >= 0xE000) && (WriteAddr < 0xFE00)) {    /* RAM mirror */
    //   MemoryInternalRAM[WriteAddr - 8192] = DataHolder;
    // } else if ((WriteAddr >= 0xFE00) && (WriteAddr < 0xFEA0)) {    /* Sprite OAM memory */
    //   SpriteOAM[WriteAddr] = DataHolder;
    // } else if ((WriteAddr >= 0xFF80) && (WriteAddr <= 0xFFFF)) {   /* Hi RAM area */
    //   MemoryInternalHiRAM[WriteAddr] = DataHolder;
    // } else if ((WriteAddr >= 0x8000) && (WriteAddr < 0xA000)) {   /* Video RAM (8KiB) */
    //   VideoRAM[WriteAddr] = DataHolder;
    // } else

  if (WriteAddr == 0xFF41) {                            /* STAT register: Do not allow to write into 2 last bits of the STAT */
    IoRegisters[0xFF41] = ((IoRegisters[0xFF41] & bx00000011) | (DataHolder & bx11111100)); /* register, as these bits are the mode flag. */
  } else if (WriteAddr == 0xFF44) { /* CURLINE [RW] Current Scanline. */
    IoRegisters[WriteAddr] = 0;     /* Writing into this register resets it. */
    /* SetUserMsg("LY RESET"); */
  } else if (WriteAddr == 0xFF46) {   /* Starts LCD OAM DMA transfer */
      int x;
      for( x = 0; x < 160; x++) { /* Let's copy XX00-XX9F to FE00-FE9F */
	SpriteOAM[0xFE00 | x] = MemoryRead((DataHolder << 8) | x);
      }
  } else if (WriteAddr == 0xFF04) {
    IoRegisters[0xFF04] = 0;      /* Divide register: Writing any value sets it to 0 */
  } else if (WriteAddr == 0xFF00) {
    IOWrite( DataHolder );
  } else if ((WriteAddr >= 0xFF00) && (WriteAddr <= 0xFF4B)) {   /* I/O registers */
    IoRegisters[WriteAddr] = DataHolder;
    //} else if (WriteAddr <= 65535) {
    // MemoryWriteSpecial(WriteAddr, DataHolder);
  }
}
