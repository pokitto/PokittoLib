/*
    This file is part of the zBoy project
    Copyright (C) Mateusz Viste 2010, 2011
*/

#include <stdio.h>
#include <stdint.h>
#include "crc32.h"
#include "zboystructs.h"
#include "loadrominfos.c"


char *LoadRomErrStr(int errcode) {
  switch (errcode) {
    case -1:
      return("ERROR: THE FILE IS TOO SMALL TO BE A VALID GAMEBOY ROM!");
    case -2:
      return("ERROR: THE FILE IS TOO BIG TO BE A VALID GAMEBOY ROM!");
    case -3:
      return("ERROR: THE SPECIFIED ROM FILE COULD NOT BE OPENED!");
    case -4:
      return("ERROR: THE FILE IS NOT A VALID GAMEBOY ROM!");
    default:
      return("ERROR: UNEXPECTED ERROR OCCURED WHILE LOADING THE ROM!");
  }
}


int CheckChecksums(int RomSize, uint8_t *MemoryROM) {
  /* Returns 0 if GameBoy checksums are okay, non-zero otherwise.
     14Dh: A complement check is computed as the sum of the bytes in the header of the program subtracted by one and then two's complemented. The complement check is required by the NINTENDO GAMEBOY device. */
  int x, CmpChecksum = 0, TotalChecksum = 0;
  uint8_t tByteBuff, Result = 0;
  for (x = 0x134; x <= 0x14C; x++) {
    tByteBuff = MemoryROM[x];
    CmpChecksum = ((CmpChecksum + tByteBuff) & 255);
  }
  CmpChecksum = (231 - CmpChecksum);
  if (CmpChecksum < 0) {   /* If negative, get the 2-complement */
    CmpChecksum = (0 - CmpChecksum);   /* make the value positive... */
    CmpChecksum = (CmpChecksum ^ 255); /* ...then xor all bits... */
    CmpChecksum += 1;                  /* ...and add one. */
  }
  if (MemoryROM[0x14D] == CmpChecksum) {
      /* 014E-014F: Checksum (higher byte first) produced by adding all bytes of a cartridge except for two checksum bytes and taking two lower bytes of the result. (GameBoy ignores this value.) */
      for (x = 0; x < RomSize; x++) {
        tByteBuff = MemoryROM[x];
        if ((x != 0x14E) && (x != 0x14F)) TotalChecksum += tByteBuff;
        TotalChecksum = (TotalChecksum & 65535);  /* Keep the checksum 16bit wide */
      }
      tByteBuff = MemoryROM[0x14E];
      if (tByteBuff != ((TotalChecksum >> 8) & 255)) Result = 3;   /* Check first byte of checksum */
      tByteBuff = MemoryROM[0x14F];
      if (tByteBuff != (TotalChecksum & 255)) Result = 2;         /* Check second byte of checksum */
    } else {
      Result = 1;
  }
  return(Result);
}

/*
int LoadRom(struct RomInformations *RomInfos, char *UserMessage) { /* Returns 0 on success, non-zero on failure * /
  uint8_t NintendoSignature[48] = {0xCE,0xED,0x66,0x66,0xCC,0x0D,0x00,0x0B,
                                   0x03,0x73,0x00,0x83,0x00,0x0C,0x00,0x0D,
                                   0x00,0x08,0x11,0x1F,0x88,0x89,0x00,0x0E,
                                   0xDC,0xCC,0x6E,0xE6,0xDD,0xDD,0xD9,0x99,
                                   0xBB,0xBB,0x67,0x63,0x6E,0x0E,0xEC,0xCC,
                                   0xDD,0xDC,0x99,0x9F,0xBB,0xB9,0x33,0x3E};
  int x, y, LastDot;
  FILE *RomHandler;
  unsigned int RomSize;
  y = 0;
  LastDot = 0;
  for (x = 0; RomInfos->Filename[x] > 0; x++) {
    RomInfos->ShortFilename[y] = RomInfos->Filename[x];
    RomInfos->ShortFilenameNoExt[y] = RomInfos->Filename[x];
    if (RomInfos->Filename[x] == '.') LastDot = y;
    if ((RomInfos->Filename[x] == '/') || (RomInfos->Filename[x] == '\\')) {
        y = 0;
        LastDot = 0;
      } else {
        y++;
    }
  }
  RomInfos->ShortFilename[y] = 0;
  RomInfos->ShortFilenameNoExt[y] = 0;

  if (LastDot != 0) RomInfos->ShortFilenameNoExt[LastDot] = 0;

  RomHandler = fopen(RomInfos->Filename, "rb");
  if (RomHandler != NULL) {
      fseek(RomHandler, 0, SEEK_END); /* seek to end of file * /
      RomSize = ftell(RomHandler); /* get current file pointer (ROM's size) * /
      fseek(RomHandler, 0, SEEK_SET); /* seek back to beginning of file * /
      if (RomSize < 16384) {
          fclose(RomHandler);
          return(-1);
        } else {
          if (RomSize > 4194304) {
              fclose(RomHandler);
              return(-2);
            } else {
              /*for (x = 0; x < RomSize; x++) { * /
              /*  GET #RomHandler, x + 1, MemoryROM(x); * /
              /*  fread(&MemoryROM[x], 1, 1, RomHandler); * /
              /*} * /
              fread(RomInfos->MemoryROM_PTR, RomSize, 1, RomHandler);
              /*fread(MemoryROM, 1, RomSize, RomHandler); * /
              fclose(RomHandler);
          }
      }
    } else {
      return(-3);
  }

  RomInfos->CrcSum = getCRC32(RomInfos->MemoryROM_PTR, RomSize);    /* Calculate CRC32 value of the ROM * /

  /* Check for ROM signature * /
  for (x = 0; x < 48; x++) {
    if (RomInfos->MemoryROM_PTR[0x104 + x] != NintendoSignature[x]) {
      return(-4);
    }
  }
  if (CheckChecksums(RomSize, RomInfos->MemoryROM_PTR) != 0) {
    sprintf(UserMessage, "CHECKSUM ERROR: THIS ROM IS PROBABLY CORRUPTED.");
  }
  LoadRomInfos(RomInfos);
  return(0);
}
*/
