/**************************************************************************/
/*!
    @file     Pokitto_disk.cpp
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#include "Pokitto.h"

#define SD_MOSI_PORT   0
#define SD_MISO_PORT   0
#define SD_SCK_PORT    0
#define SD_CS_PORT     0
#define SD_MOSI_PIN   9
#define SD_MISO_PIN   8
#define SD_SCK_PIN    6
#define SD_CS_PIN     7

#if POK_ENABLE_SD > 0
BYTE res;
FATFS fs;            /* File system object */
FATDIR dir;            /* Directory object */
FILINFO fno;        /* File information */

//static FATFS *FatFs;    /* Pointer to the file system object (logical drive) */

bool diropened=false;

#define SPEAKER 3
//#define BUFFER_SIZE 256 // was 128
#define SONGLENGTH 0x1BFBCD // 1072223
#define FILESIZE 0x1BFBCD

uint8_t filemode = FILE_MODE_UNINITIALIZED;
char currentfile[15]; // holds current file's name

SPI device(CONNECT_MOSI,CONNECT_MISO,CONNECT_SCK);
//DigitalOut mmccs(CONNECT_CS);

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

__attribute__((section(".SD_Code"))) void initSDGPIO() {
    LPC_GPIO_PORT->DIR[SD_MOSI_PORT] |= (1  << SD_MOSI_PIN );
    LPC_GPIO_PORT->DIR[SD_MISO_PORT] |= (1  << SD_MISO_PIN );
    LPC_GPIO_PORT->DIR[SD_SCK_PORT]  |= (1  << SD_SCK_PIN );
    LPC_GPIO_PORT->DIR[SD_CS_PORT]   |= (1  << SD_CS_PIN );
}

__attribute__((section(".SD_Code"))) int pokInitSD() {
    initSDGPIO();
    res = disk_initialize();
    res = (pf_mount(&fs));
    res = pf_opendir(&dir,"");
    if (res) diropened=false;
    else diropened=true;
    return res;
}


/** PUBLIC FUNCTIONS **/

char* getFirstDirEntry() {
    res=0;
    if (!diropened) {
            pokInitSD();
    }
    res = pf_opendir(&dir,"");
    res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
    if (res) return 0;
    while (res==0) { //while res is ok
        if ((fno.fattrib & 0x02)==0) {
                if (fno.fattrib & 0x10) {
                    fno.fname[8]='.';
                    fno.fname[9]='D';
                    fno.fname[10]='I';
                    fno.fname[11]='R';
                    fno.fname[12]='\0';
                }
                return fno.fname;
        }
        res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
        if (res==0 && dir.index==0) break;
    }
    return 0;
}

char* getNextDirEntry() {
    if (!diropened) pokInitSD();
	res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
	if (res==0) {
	        while (fno.fattrib & 0x02 && !res) res = pf_readdir(&dir,&fno); //system/hidden file
	        if (fno.fattrib & 0x10) {
	                int a=12;
                    while (a) {
                            fno.fname[a] = fno.fname[a-1];
                            a--;
                    }
                    if (fno.fname[0]) {
                            fno.fname[0]='/';
                            a=0;
                            while (fno.fname[a]) a++;
                            fno.fname[a]='/';
                    }

	                /*fno.fname[a++]='.';
                    fno.fname[a++]='D';
                    fno.fname[a++]='I';
                    fno.fname[a++]='R';
                    fno.fname[a]='\0';*/
	        }
	        return fno.fname;
	}
    return NULL;
}

char* getNextFile (char* ext){

    if (!diropened) pokInitSD();
	int a=1;
	res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
	while (res==0 || a) { //while there are entries and
        if (dir.index==0) return 0; //end of list
        a = strcmp((const char*)get_filename_ext(fno.fname),(const char*)ext); // returns 0 if strings are identical
        if (strcmp(ext,"")==0 && (fno.fattrib & 0x10) == 0) a=0;
        if (a == 0 && (fno.fattrib & 0x10) == 0) return fno.fname;
        if (fno.fattrib&0x10) return NULL; //its a directory
	}
return 0;
}


char* getNextFile() {
    return getNextFile("");
}

char* getFirstFile(char* ext) {
    res=0;
    if (!diropened) {
            pokInitSD();
    }
    res = pf_opendir(&dir,"");
    res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
    if (res) return 0;
    while (res==0 || (fno.fattrib & 0x10) == 0) {
        int a=0;
        a = strcmp((const char*)get_filename_ext(fno.fname),(const char*)ext); // returns 0 if strings are identical
        if (!strcmp(ext,"")) a=0;
        if ( a == 0 && (fno.fattrib & 0x10) == 0) return fno.fname;
        res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
        if (res==0 && dir.index==0) break;
    }
    return 0;
}

char* getFirstFile() {
    return getFirstFile("");
}

int isThisFileOpen(char* buffer){
    int a=0;
    a = strcmp((const char*)buffer,(const char*)currentfile); // returns 0 if strings are identical
    if ( a == 0 && filemode != FILE_MODE_FAILED) return 1;
    return 0;
}

int fileOK() {
    if (filemode != FILE_MODE_FAILED) return 1;
    return 0;
}

uint8_t fileOpen(char* buffer, char fmode) {
    int err;
    if (filemode == FILE_MODE_UNINITIALIZED) {
        int a = pf_mount(&fs);
        if (a) return 1; // 1 means error in this context
    }

    filemode = fmode;
    err = pf_open(buffer);
    if (err==0) {
            strcpy(currentfile,(const char*)buffer);
            return 0; // 0 means all clear
    }
    // file open failed
    filemode = FILE_MODE_FAILED;
    return 1; // 1 means failed
}

void fileClose() {
    filemode = FILE_MODE_UNINITIALIZED;
    for (uint8_t i=0; i<15; i++) currentfile[i]=0;
}

int fileGetChar() {
    BYTE buff[1];
    WORD br;
    int err = pf_read(buff, 1, &br);    /* Read data to the buff[] */
    return buff[0];
}

void filePutChar(char c) {
    WORD bw;
    pf_write((const void*)&c, 1, &bw);
    pf_write(0, 0, &bw);
}

void fileWriteBytes(uint8_t * b, uint16_t n) {
    WORD bw;
    pf_write((const void*)&b, n, &bw);
    pf_write(0, 0, &bw);
}

uint16_t fileReadBytes(uint8_t * b, uint16_t n) {
    WORD br;
    pf_read(b, n, &br);    /* Read data to the buff[] */
    return br;             /* Return number of bytes read */
}

void fileSeekAbsolute(long n) {
    res = pf_lseek(n);
}

void fileSeekRelative(long n) {
    if (n<0) if (fs.fptr < -n) n=-fs.fptr;
    else if (n>0) if (fs.fptr+n > fs.fsize) n=fs.fsize-fs.fptr;
    res = pf_lseek(fs.fptr + n);
}

void fileRewind() {
  res = pf_lseek(0);
}

void fileEnd() {
  res = pf_lseek(fs.fsize);
}

long int fileGetPosition() {
    return fs.fptr;
}

uint8_t filePeek(long n) {
    pf_lseek(n);
    return fileGetChar();
}

void filePoke(long n, uint8_t c) {
    pf_lseek(n);
    filePutChar(c);
}

int dirOpen() {
    return pf_opendir(&dir,"");
}

int dirUp() {

return 0;
}

#endif // POK_ENABLE_SD


