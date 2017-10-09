/**************************************************************************/
/*!
    @file     PokittoDisk.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/


#ifndef POKITTO_DISK_H
#define POKITTO_DISK_H

#ifndef POK_SIM
// real hardware disk driver
#include "diskio.h"
#include "pff.h"
#include "connect.h"
#include "mbed.h"
extern BYTE res;
extern FATFS fs;            /* File system object */
extern FATDIR dir;            /* Directory object */
extern FILINFO fno;        /* File information */
extern SPI device;
extern DigitalOut mmccs;

/**************************************************************************/
/**                          SD CONTROL MACROS                           **/
/**************************************************************************/

// CS ... #define CONNECT_CS      P0_7 //p13
#define CLR_SD_CS LPC_GPIO_PORT->CLR[0] = (1 << 7)
#define SET_SD_CS LPC_GPIO_PORT->SET[0] = (1 << 7)
#define GET_SD_CS LPC_GPIO_PORT->PIN[0] & (1 << 7)

#else
// simulated disk driver
#endif

#include <stdint.h>



extern int pokInitSD();

// File IO modes
#define FILE_MODE_APPEND 0x1
#define FILE_MODE_OVERWRITE 0
#define FILE_MODE_READONLY 0x2
#define FILE_MODE_READWRITE 0
#define FILE_MODE_BINARY 0x4
#define FILE_MODE_ASCII 0
#define FILE_MODE_FAILED 0x8
#define FILE_MODE_UNINITIALIZED 0x10

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
extern char* getNextFile (char*);
extern char* getNextFile ();
extern char* getFirstFile(char*);
extern char* getFirstFile();
extern char* getFirstDirEntry();
extern char* getNextDirEntry();
extern int isThisFileOpen(char*);
extern int fileOK();
extern int dirOpen();
extern int dirUp();

#endif // POKITTO_DISK_H


