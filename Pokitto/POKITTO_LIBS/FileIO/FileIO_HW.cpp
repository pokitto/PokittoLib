#include "mbed.h"
#include "FileIO.h"
//#include <petit_fatfs.h>
#include <pff.h>

uint8_t filemode = FILE_MODE_FAILED;
char currentfile[15]; // holds current file's name

WORD br;
FATFS fs;          /* Work area (file system object) for the volume */
FRESULT res;       /* Petit FatFs function common result code */

int isThisFileOpen(char* buffer){
    int a=0;
    a = strcmp((const char*)buffer,(const char*)currentfile); // returns 0 if strings are identical
    if ( a == 0 && filemode != FILE_MODE_FAILED) return 1;
    return 0;
}


uint8_t fileOpen(char* buffer, char fmode) {
    int err;
    filemode = fmode;
    if (fmode & FILE_MODE_ASCII) {
            // ASCII file
            if (fmode & FILE_MODE_READONLY) {
                // RO ASCII
                //fp=fopen(buffer, "r");
            } else {
                // RW ASCII
                if (fmode & FILE_MODE_APPEND) {
                    // APPEND TO EXISTING RW ASCII
                    //fp=fopen(buffer, "a+");
                } else {
                    // OVERWRITE RW ASCII
                    //fp=fopen(buffer, "w+");
                }

            }
    } else {
            // Binary file
            if (fmode & FILE_MODE_READONLY) {
                // RO Binary
                //fp=fopen(buffer, "rb");
            } else {
                // RW Binary
                if (fmode & FILE_MODE_APPEND) {
                    // APPEND TO EXISTING Binary
                    //fp=fopen(buffer, "a+b");
                } else {
                    // OVERWRITE Binary
                    //fp=fopen(buffer, "w+b");
                }

            }
    }

    if (res==0) return 1;
    // file open failed
    filemode = FILE_MODE_FAILED;
    return 0;
}

void fileClose() {

}

int fileGetChar() {

}

void filePutChar(char c) {

}

void fileWriteBytes(uint8_t * b, uint16_t n) {

}

uint16_t fileReadBytes(uint8_t * b, uint16_t n) {

}

void fileSeekAbsolute(long n) {

}

void fileSeekRelative(long n) {

}

void fileRewind() {

}

void fileEnd() {

}

long int fileGetPosition() {

}

uint8_t filePeek(long n) {

}

void filePoke(long n, uint8_t c) {
}
