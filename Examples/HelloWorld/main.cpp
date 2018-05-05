#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "Pokitto.h"
#include "SDFileSystem.h"

Pokitto::Core mygame;

FILE *fp = NULL;
const int32_t blockSize = 1000;
const int32_t totalFileSize = 200000;
char bufferOrg[blockSize];
char buffer[blockSize];
int32_t len = 0;
uint32_t start_us = 0;
uint32_t stop_us = 0;

int main () {

    mygame.begin();
    mygame.display.persistence = true;
    mygame.display.println("");
    mygame.display.println("");
    mygame.display.println("");
    mygame.display.println("");
    mygame.display.println("");


#ifdef NOPETITFATFS

    // *** Test SDFileSystem

    SDFileSystem sd(/*MOSI*/P0_9, /*MISO*/P0_8, /*SCK*/P0_6, /*CS*/P0_7, /*Mountpoint*/"sd");

    // Write to a directory in SD.
    mygame.display.println("Test SDFS");
    mkdir("/sd/mydir", 0777);
    char* filePathAndName = "/sd/mydir/1MB.txt";
    fp = fopen(filePathAndName, "wb");
    if(fp == NULL) {
        mygame.display.println("Could not open file for write!"); goto draw_loop;
    }

    // Init buffer
    for(int32_t i=0;i<blockSize; i++) bufferOrg[i] = (char)i;

    // Create 1MB file
    for(int32_t i=0; i<totalFileSize; i+=blockSize) {
        // read the block into the buffer:
        len = fwrite (bufferOrg , sizeof(char), blockSize, fp);
        if( len != blockSize ) {mygame.display.println("Could not write!"); goto draw_loop;}
   }
    fclose(fp);
    mygame.display.println("Wrote to SD!");

    // Open file
    fp = NULL;
    fp = fopen(filePathAndName, "rb");
     if(fp == NULL) {
        mygame.display.println("Could not open file for read"); goto draw_loop;
    }

    // Read from SD
    start_us = us_ticker_read();
    for(int32_t i=0;; i+=blockSize) {
        // read the block into the buffer:
        len = fread (buffer, sizeof(char), blockSize, fp);
        if (len != blockSize) break;
   }
    stop_us = us_ticker_read();
    fclose(fp);

    // Check the result
    for(int32_t i=0;i<blockSize; i++)
        if(buffer[i] != bufferOrg[i]) {mygame.display.println("Data invalid!"); goto draw_loop;}

    mygame.display.println("Read file in ");
    mygame.display.print( totalFileSize/((stop_us-start_us)/1000) );
    mygame.display.print(" kb/s.");

#else

    // *** Test PetitFatFS

    pokInitSD();
    char* filePathAndName = "mydir/1MB.txt";
    (void)fileOpen(filePathAndName, FILE_MODE_READONLY);
    mygame.display.println("Test PetitFFS");

    start_us = us_ticker_read();
    for(int32_t i=0;; i+=blockSize) {
      char cbuffer[blockSize];
      uint16_t len = fileReadBytes((uint8_t*)cbuffer, blockSize);
      if( len == 0 ) break;
      fileSeekRelative(blockSize);
    }
    stop_us = us_ticker_read();
    fileClose(); // close any open files

    // Check the result
    for(int32_t i=0;i<totalFileSize; i++)
        if(bufferOrg[i] != bufferOrg[i]) {mygame.display.println("Data invalid!"); goto draw_loop;}

    mygame.display.println("Read file in ");
    mygame.display.print( totalFileSize/((stop_us-start_us)/1000) );
    mygame.display.print(" kb/s.");

#endif

draw_loop:
    while (mygame.isRunning()) {
        if (mygame.update()) {
         }
    }
}
