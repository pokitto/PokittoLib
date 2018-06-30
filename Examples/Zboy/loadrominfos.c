/*
    Part of the zBoy project
    Copyright (C) Mateusz Viste 2010, 2011, 2012
*/

#include <stdint.h>
#include <string.h>
#include "binary.h"

void LoadRomInfos(struct RomInformations *RomInfos) {
  const uint8_t *MemoryROM = RomInfos->MemoryROM_PTR;

  /* Check bit 7 of the CGB flag (&80 usually means that the game is GB compatible) */
  if ((MemoryROM[0x143] & 128) > 0) RomInfos->ColorGB = 1;
  if (MemoryROM[0x143] == 0xC0) RomInfos->ColorGB = 2;  /* GBC flag + not compatible with older GB */
  if (MemoryROM[0x146] == 0x03) RomInfos->SuperGB = 1;

  switch (MemoryROM[0x147]) {   /* Get the cartridge type */
    case 0x00:
      RomInfos->MbcModel = 0;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x01:
      RomInfos->MbcModel = 1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x02:
      RomInfos->MbcModel = 1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x03:
      RomInfos->MbcModel = 1;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x05:
      RomInfos->MbcModel = 2;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x06:
      RomInfos->MbcModel = 2;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x08:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x09:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x0B:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x0C:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x0D:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x0F:
      RomInfos->MbcModel = 3;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 1;
      break;
    case 0x10:
      RomInfos->MbcModel = 3;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 1;
      break;
    case 0x11:
      RomInfos->MbcModel = 3;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x12:
      RomInfos->MbcModel = 3;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x13:
      RomInfos->MbcModel = 3;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x19:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1A:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1B:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1C:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 1;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1D:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 1;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1E:
      RomInfos->MbcModel = 5;
      RomInfos->Battery = 1;
      RomInfos->Rumble = 1;
      RomInfos->TimerRTC = 0;
      break;
    case 0x1F:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0xFD:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0xFE:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    case 0xFF:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
    default:
      RomInfos->MbcModel = -1;
      RomInfos->Battery = 0;
      RomInfos->Rumble = 0;
      RomInfos->TimerRTC = 0;
      break;
  }

  switch (MemoryROM[0x148]) {
    case 0x00: /* 256 Kbit */
      RomInfos->RomSize = 32768;
      break;
    case 0x01: /* 512 Kbit */
      RomInfos->RomSize = 65536;
      break;
    case 0x02: /* 1 Mbit */
      RomInfos->RomSize = 131072;
      break;
    case 0x03: /* 2 Mbit */
      RomInfos->RomSize = 262144;
      break;
    case 0x04: /* 4 Mbit */
      RomInfos->RomSize = 524288;
      break;
    case 0x05: /* 8 Mbit */
      RomInfos->RomSize = 1048576;
      break;
    case 0x06: /* 16 Mbit */
      RomInfos->RomSize = 2097152;
      break;
    case 0x52: /* 9 Mbit */
      RomInfos->RomSize = 1179648;
      break;
    case 0x53: /* 10 Mbit */
      RomInfos->RomSize = 1310720;
      break;
    case 0x54: /* 12 Mbit */
      RomInfos->RomSize = 1572864;
      break;
  }

  switch (MemoryROM[0x149]) {
    case 0: /* no RAM */
      RomInfos->RamSize = 0;
      break;
    case 1: /* 16 kBit */
      RomInfos->RamSize = 2048;
      break;
    case 2: /* 64 kBit */
      RomInfos->RamSize = 8192;
      break;
    case 3: /* 256 kBit */
      RomInfos->RamSize = 32768;
      break;
    case 4: /* 1 MBit */
      RomInfos->RamSize = 131072;
      break;
  }

  /* All MBC2 chips contain a 512x4 bits internal RAM, although the byte at 149h always indicates 0 bytes of RAM. */
  if (RomInfos->MbcModel == 2) RomInfos->RamSize = 512;

  if (MemoryROM[0x14A] == 0) {
      RomInfos->Destination = 0;
    } else {
      RomInfos->Destination = 1;
  }

}
