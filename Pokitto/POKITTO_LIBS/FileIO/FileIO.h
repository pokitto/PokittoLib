#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>

// File IO modes
#define FILE_MODE_APPEND 0x1
#define FILE_MODE_OVERWRITE 0
#define FILE_MODE_READONLY 0x2
#define FILE_MODE_READWRITE 0
#define FILE_MODE_BINARY 0x4
#define FILE_MODE_ASCII 0
#define FILE_MODE_FAILED 0x8

extern int pokInitSD();
extern long getFileLength();
extern uint8_t fileOpen(char*, char);
extern void fileClose();
extern int fileGetChar();
extern void filePutChar(char);
extern void fileWriteBytes(uint8_t *, uint16_t);
extern uint16_t fileReadBytes(uint8_t *, uint16_t);
extern void fileSeekAbsolute(long);
extern void fileSeekRelative(long);
extern void fileRewind();
extern void fileEnd();
extern long int fileGetPosition();
#define fileSetPosition(n)  (fileSeekAbsolute(n))
extern uint8_t filePeek(long);
extern void filePoke(long, uint8_t);
extern char* getCurrentFileName ();
extern int isThisFileOpen(char*);
extern int fileOK();

#endif // FILEIO_H
