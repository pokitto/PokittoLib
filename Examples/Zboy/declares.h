/*
   This file is part of the zBoy project
*/

/* Declarations of some SUBs */
void SetUserMsg(char *msg);
void PrintMsg(char *txtmessage, unsigned char TransparentFlag);
void LoadGame(void);
void SaveGame(void);
void RefreshScreen_EPX(void);
void ResetEmulator(void);

/* Declarations of some FUNCTIONs */
void SavePcxFile(char *OutFile);

/* Declarations of all debug routines */
#ifdef DEBUGMODE
  void DebugShowCpuRegisters(void);
  void DebugShowCPUspeed(void);
  /* void PrintDebug(char *DebugLog); */
  void DebugOnScreen(void);
#endif
