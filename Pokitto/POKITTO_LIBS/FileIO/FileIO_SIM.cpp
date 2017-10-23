/**************************************************************************/
/*!
    @file     FileIO_SIM.cpp
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

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "FileIO.h"
#include "tinydir.h"


tinydir_dir tinydir;
tinydir_file tinyfile;

FILE *fp;
uint8_t filemode = FILE_MODE_FAILED;
long filelen = 0;
char currentfile[15]; // holds current file's name
char nextfile[15]; // holds next file entry name
bool diropened=false;

int pokInitSD(){
tinydir_open(&tinydir, "./");
diropened = true;
return 0;
}


long getFileLength() {
    return filelen;
}

uint8_t fileOpen(char* buffer, char fmode) {

    filemode = fmode;
    if (fmode & FILE_MODE_ASCII) {
            // ASCII file
            if (fmode & FILE_MODE_READONLY) {
                // RO ASCII
                fp=fopen(buffer, "r");
            } else {
                // RW ASCII
                if (fmode & FILE_MODE_APPEND) {
                    // APPEND TO EXISTING RW ASCII
                    fp=fopen(buffer, "a+");
                } else {
                    // OVERWRITE RW ASCII
                    fp=fopen(buffer, "w+");
                }

            }
    } else {
            // Binary file
            if (fmode & FILE_MODE_READONLY) {
                // RO Binary
                fp=fopen(buffer, "rb");
            } else {
                // RW Binary
                if (fmode & FILE_MODE_APPEND) {
                    // APPEND TO EXISTING Binary
                    fp=fopen(buffer, "a+b");
                } else {
                    // OVERWRITE Binary
                    fp=fopen(buffer, "r+b"); //w+b will wipe the file !!!
                }

            }
    }

    if (fp) {
            // success !
            memset(currentfile,0,sizeof(currentfile)); // empty current file name
            strcpy(currentfile,(const char*)buffer);
            fileEnd();
            filelen = ftell(fp);
            fileRewind();
            return 0;
    }
    // file open failed
    filemode = FILE_MODE_FAILED;
    return 1;
}

char* getCurrentFileName (){
    return currentfile;
}


char* getNextFile (char* ext){

    if (!diropened) pokInitSD();
	int a=1;
	while (tinyfile.is_dir || a) {
        tinydir_readfile(&tinydir, &tinyfile);
        if (tinydir_next(&tinydir)==-1) return 0; //end of list
        a = strcmp((const char*)tinyfile.extension,(const char*)ext); // returns 0 if strings are identical
        if (strcmp(ext,"")==0 && tinyfile.is_dir == 0) a=0;
        if (a == 0 && tinyfile.is_dir==0) return tinyfile.name;

	}
}


char* getNextFile() {
    return getNextFile("");
}

char* getFirstFile(char* ext) {
    if (diropened) {
        tinydir_close(&tinydir);
        diropened=false;
    }
    tinydir_open(&tinydir, "./");
    diropened = true;
    int b = tinydir_readfile(&tinydir, &tinyfile);
    if (b==-1) return 0;
    while (tinyfile.name || tinyfile.is_dir) {
        int a=0;
        a = strcmp((const char*)tinyfile.extension,(const char*)ext); // returns 0 if strings are identical
        if (!strcmp(ext,"")) a=0;
        if ( a == 0 && tinyfile.is_dir == 0) return tinyfile.name;
        tinydir_next(&tinydir);
        int b = tinydir_readfile(&tinydir, &tinyfile);
        if (b==-1) break;
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

void fileClose() {
    if (filemode != FILE_MODE_FAILED) {
            fclose(fp);
            memset(currentfile,0,sizeof(currentfile)); // empty current file name
    }
}

int fileGetChar() {
    return fgetc(fp);
}

void filePutChar(char c) {
    fputc( (int) c, fp );
}

void fileWriteBytes(uint8_t * b, uint16_t n) {
    fwrite((uint8_t*)b, 1, n, fp);
    //for (int j =0; j<n; j++) fputc(b[n],fp);
}

uint16_t fileReadBytes(uint8_t * b, uint16_t n) {
    return fread(b, 1, n, fp);
}

void fileSeekAbsolute(long n) {
    fseek(fp, n, SEEK_SET); // seek to the 100th byte of the file
}

void fileSeekRelative(long n) {
    fseek(fp, n, SEEK_CUR); // seek to the 100th byte of the file
}

void fileRewind() {
    fseek(fp, 0, SEEK_SET);
}

void fileEnd() {
    fseek(fp, 0, SEEK_END);
}

long int fileGetPosition() {
return ftell ( fp );
}

uint8_t filePeek(long n) {
    fileSeekAbsolute(n);
    return fgetc(fp);
}

void filePoke(long n, uint8_t c) {
    fileSeekAbsolute(n);
    fputc( c, fp );
}
