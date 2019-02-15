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
PFFS::BYTE res;
PFFS::FATFS fs;            /* File system object */
PFFS::FATDIR dir;            /* Directory object */
PFFS::FILINFO fno;        /* File information */

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

//__attribute__((section(".SD_Code")))
void initSDGPIO() {
    LPC_GPIO_PORT->DIR[SD_MOSI_PORT] |= (1  << SD_MOSI_PIN );
    LPC_GPIO_PORT->DIR[SD_MISO_PORT] |= (1  << SD_MISO_PIN );
    LPC_GPIO_PORT->DIR[SD_SCK_PORT]  |= (1  << SD_SCK_PIN );
    LPC_GPIO_PORT->DIR[SD_CS_PORT]   |= (1  << SD_CS_PIN );
}

//__attribute__((section(".SD_Code")))
int pokInitSD() {
    initSDGPIO();
    res = PFFS::disk_initialize();
    //res = disk_initialize(0);
    res = (pf_mount(&fs));
    res = pf_opendir(&dir,"");
    if (res) diropened=false;
    else diropened=true;
    return res;
}


void emptyFname() {
    for (int i=0; i<13; i++) fno.fname[i]=NULL;
}

/** PUBLIC FUNCTIONS **/

char* getFirstDirEntry(char* path) {
    res=0;
    if (!diropened) {
            pokInitSD();
    }
    res = pf_opendir(&dir,path);
    emptyFname();
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
        emptyFname();
        res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
        if (res==0 && dir.index==0) break;
    }
    return 0;
}

char* getFirstDirEntry() {
    return getFirstDirEntry("");
}

char* getNextDirEntry() {
    if (!diropened) pokInitSD();
    emptyFname();
	res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
	if (res==0) {
	        while (fno.fattrib & 0x02 && !res) {emptyFname(); res = pf_readdir(&dir,&fno);} //system/hidden file
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
	emptyFname();
	res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
	while (res==0 || a) { //while there are entries and
        if (dir.index==0) return 0; //end of list
        a = strcmp((const char*)get_filename_ext(fno.fname),(const char*)ext); // returns 0 if strings are identical
        if (strcmp(ext,"")==0 && (fno.fattrib & 0x10) == 0) a=0;
        if (a == 0 && (fno.fattrib & 0x10) == 0) return fno.fname;
        if (fno.fname[0]==NULL) return NULL; //end of files
        //if (fno.fattrib&0x10) return NULL; //its a directory
        emptyFname();
        res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
	}
return 0;
}


char* getNextFile() {
    return getNextFile("");
}

char* getFirstFile(char* ext, char* path) {
    res=0;
    if (!diropened) {
            pokInitSD();
    }
    res = pf_opendir(&dir,path);
    emptyFname();
    res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
    if (res) return 0;
    while (res==0 || (fno.fattrib & 0x10) == 0) {
        int a=0;
        a = strcmp((const char*)get_filename_ext(fno.fname),(const char*)ext); // returns 0 if strings are identical
        if (!strcmp(ext,"")) a=0;
        if ( a == 0 && (fno.fattrib & 0x10) == 0) return fno.fname;
        emptyFname();
        res = pf_readdir(&dir,&fno); //returns 0 if everything is OK
        if (fno.fname[0]==NULL) break; //end of directory reached, no files found
        if (res==0 && dir.index==0) break;
    }
    return 0;
}

char* getFirstFile(char* ext) {
    return getFirstFile(ext, "");
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
    err = PFFS::pf_open(buffer);
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

char fileGetChar() {
    PFFS::BYTE buff[1];
    PFFS::WORD br;
    int err = PFFS::pf_read(buff, 1, &br);    /* Read data to the buff[] */
    return buff[0];
}

void filePutChar(char c) {
    PFFS::WORD bw;
    PFFS::pf_write((const void*)&c, 1, &bw);
    PFFS::pf_write(0, 0, &bw);
}

void fileWriteBytes(uint8_t * b, uint16_t n) {
    PFFS::WORD bw;
    PFFS::pf_write((const void*)&b, n, &bw);
    PFFS::pf_write(0, 0, &bw);
}

uint16_t fileReadBytes(uint8_t * b, uint16_t n) {
    PFFS::WORD br;
    PFFS::pf_read(b, n, &br);    /* Read data to the buff[] */
    return br;             /* Return number of bytes read */
}

void fileSeekAbsolute(long n) {
    res = PFFS::pf_lseek(n);
}

void fileSeekRelative(long n) {
    if (n<0) if (fs.fptr < -n) n=-fs.fptr;
    else if (n>0) if (fs.fptr+n > fs.fsize) n=fs.fsize-fs.fptr;
    res = PFFS::pf_lseek(fs.fptr + n);
}

void fileRewind() {
  res = PFFS::pf_lseek(0);
}

void fileEnd() {
  res = PFFS::pf_lseek(fs.fsize);
}

long int fileGetPosition() {
    return fs.fptr;
}

uint8_t filePeek(long n) {
    PFFS::pf_lseek(n);
    return fileGetChar();
}

void filePoke(long n, uint8_t c) {
    PFFS::pf_lseek(n);
    filePutChar(c);
}

int fileReadLine(char* destination, int maxchars) {
    int n=0;
    char c=1;
    char linebuf[80];
    fileReadBytes((uint8_t*)linebuf,80);
    int index=0;
    while (c!=NULL) {
        c = linebuf[index++];
        if (n == 0) {
            while (c == '\n' || c == '\r') c = linebuf[index++]; // skip empty lines
        }
        n++;
        if (c=='\n' || c=='\r' || n==maxchars-1) c=NULL; //prevent buffer overflow
        *destination++ = c;
    }
    fileSeekRelative(-80+index); //rewind
    return n; //number of characters read
}

int dirOpen() {
    return PFFS::pf_opendir(&dir,"");
}

int dirUp() {

return 0;
}
#endif // POK_ENABLE_SD


