/*
   This file is part of the zBoy project.
   Copyright (C) Mateusz Viste 2010,2011,2012,2013,2014,2015
*/


uint8_t *GetHiScoresOffsets(unsigned int *offset1, unsigned int *offset2) {
  switch (RomInfos.CrcSum) {
    case 0x0509069C:  /* Pac-Man */
    case 0xB681E243:  /* Pac-Man (another version) */
      *offset1 = 0xFF8C;
      *offset2 = 0xFF8E;
      return(MemoryInternalHiRAM);
      break;
    case 0xB3A86164:  /* Hyper Lode Runner */
      *offset1 = 0xFFA7;
      *offset2 = 0xFFA9;
      return(MemoryInternalHiRAM);
      break;
    case 0x0E5BB1C4:  /* Ms. Pac-Man */
      *offset1 = 0xD8C7;
      *offset2 = 0xD8C9;
      return(MemoryInternalRAM);
      break;
    case 0xF59CEDEA:  /* Pipe Dream */
      *offset1 = 0xC212;
      *offset2 = 0xC22C;
      return(MemoryInternalRAM);
      break;
    case 0x90776841:  /* Super Marioland v1.0 */
    case 0x2C27EC70:  /* Super Marioland v1.1 */
      *offset1 = 0xC0C0;
      *offset2 = 0xC0C3;
      return(MemoryInternalRAM);
      break;
    case 0x63F9407D:  /* Tetris v1.0 */
    case 0x46DF91AD:  /* Tetris v1.1 */
      *offset1 = 0xD000;
      *offset2 = 0xD761;
      return(MemoryInternalRAM);
      break;
    case 0xC1F88833:  /* Asteroids */
      *offset1 = 0xDB00;
      *offset2 = 0xDB53;
      return(MemoryInternalRAM);
      break;
    case 0xA37A814A:  /* BattleCity */
      *offset1 = 0xCA02;
      *offset2 = 0xCA06;
      return(MemoryInternalRAM);
      break;
    case 0xB76C769B:  /* Tetris Attack v1.0 */
      *offset1 = 0xCF00;
      *offset2 = 0xCF2C;
      return(MemoryInternalRAM);
      break;
    case 0x6C742478:  /* Dig Dug */
      *offset1 = 0xDE08;
      *offset2 = 0xDE17;
      return(MemoryInternalRAM);
      break;
    case 0x6A6ECFEC:  /* Galaga & Galaxian */
      *offset1 = 0xD70D;
      *offset2 = 0xD716;
      return(MemoryInternalRAM);
      break;
    default:  /* if not found, set offsets to 0 */
      return(NULL);
      break;
  }
}


#define XorKey 1983


void SaveHiScoresMem(void) {
  /*
  unsigned int UintVar, offset1, offset2;
  uint8_t *memarea;
  uint32_t HiScoreCheckSum = 1985;
  FILE *FileHandler;
  char SaveDir[1024];
  char SaveScoresFile[1024];
  drv_getsavedir(SaveDir, 1024);  /* provides the SaveDir variable * /
  snprintf(SaveScoresFile, 1022, "%s%08X.hi", SaveDir, (unsigned int)RomInfos.CrcSum);
  memarea = GetHiScoresOffsets(&offset1, &offset2);
  if (memarea != NULL) {
    FileHandler = fopen(SaveScoresFile, "wb");
    for (UintVar = offset1; UintVar <= offset2; UintVar++) {
      fputc(memarea[UintVar], FileHandler);
      HiScoreCheckSum += memarea[UintVar];
    }
    /* Write the checksum value into the file * /
    HiScoreCheckSum ^= XorKey;
    fputc((HiScoreCheckSum >> 24) & 0xFF, FileHandler);
    fputc((HiScoreCheckSum >> 16) & 0xFF, FileHandler);
    fputc((HiScoreCheckSum >> 8) & 0xFF, FileHandler);
    fputc(HiScoreCheckSum & 0xFF, FileHandler);
    /* Close the file * /
    fclose(FileHandler);
  }
  */
}


void LoadHiScoresMem(void) {
  /*
  unsigned int UintVar, offset1, offset2;
  unsigned int HiScoreCheckSum = 1985, SavedCheckSum;
  char SaveDir[1024];
  char SaveScoresFile[1024];
  uint8_t *memarea;
  FILE *FileHandler;
  drv_getsavedir(SaveDir, 1024);  /* provides the SaveDir variable * /
  snprintf(SaveScoresFile, 1022, "%s%08X.hi", SaveDir, (unsigned int)RomInfos.CrcSum);
  memarea = GetHiScoresOffsets(&offset1, &offset2);
  if (memarea != NULL) {
    FileHandler = fopen(SaveScoresFile, "rb");
    if (FileHandler != NULL) {
      for (UintVar = offset1; UintVar <= offset2; UintVar++) {
        memarea[UintVar] = fgetc(FileHandler);
        SaveScoresWriteProtection[UintVar - offset1] = UintVar;
        HiScoreCheckSum += memarea[UintVar];
      }
      /* Check if hiscores haven't been modified by a cheater * /
      SavedCheckSum = ((unsigned int) fgetc(FileHandler) << 24);
      SavedCheckSum |= ((unsigned int) fgetc(FileHandler) << 16);
      SavedCheckSum |= ((unsigned int) fgetc(FileHandler) << 8);
      SavedCheckSum |= (unsigned int) fgetc(FileHandler);
      SavedCheckSum ^= XorKey;
      if (HiScoreCheckSum != SavedCheckSum) {
        printf("Cksum mismatch on hiscores!\n");
        for (UintVar = 0; UintVar < 2048; UintVar++) SaveScoresWriteProtection[UintVar] = 0;
      }
      /* Close the file * /
      fclose(FileHandler);
    }
  }
  */
}
