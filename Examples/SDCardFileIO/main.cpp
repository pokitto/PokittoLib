#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "Pokitto.h"
#ifdef POK_SIM
#include "io.h"
#else
#include "SDFileSystem.h"
#endif

Pokitto::Core mygame;

FILE *filep = NULL;
const int32_t blockSize = 1000;
const int32_t totalFileSize = 200000;
char bufferOrg[blockSize];
char buffer[blockSize];
int32_t len = 0;
//uint32_t start_us = 0;
//uint32_t stop_us = 0;
uint32_t start_ms = 0;
uint32_t stop_ms = 0;

int main () {

    mygame.begin();
    mygame.display.persistence = true;

    mygame.display.println("Testing...");
    /*
    while (mygame.isRunning()) {
        if (mygame.update()) {
            mygame.update();
            break;
         }
    }*/

    // *** Test SDFileSystem

    mygame.display.println("*SDFS*");

    #ifdef POK_SIM
    char* filePathAndNamePFFS = "sdtest/200KB.txt";
    char* filePathAndNameSDFS = "sdtest/200KB.txt";
    char* dirNameSDFS = "sdtest";

    mkdir(dirNameSDFS);
    #else
    SDFileSystem sd(/*MOSI*/P0_9, /*MISO*/P0_8, /*SCK*/P0_6, /*CS*/P0_7, /*Mountpoint*/"sd");
    char* filePathAndNamePFFS = "sdtest/200KB.txt";
    char* filePathAndNameSDFS = "/sd/sdtest/200KB.txt";
    char* dirNameSDFS = "/sd/sdtest";

    mkdir(dirNameSDFS, 0777);
    #endif

    // Write to a directory in SD.
    filep = fopen(filePathAndNameSDFS, "wb");
    if(filep == NULL) {
        mygame.display.println("Could not open file for write!"); goto draw_loop;
    }

    // Init buffer
    for(int32_t i=0;i<blockSize; i++) bufferOrg[i] = (char)i;

    // Create 1MB file
    start_ms = mygame.getTime();
    for(int32_t i=0; i<totalFileSize; i+=blockSize) {
        // read the block into the buffer:
        len = fwrite (bufferOrg , sizeof(char), blockSize, filep);
        if( len != blockSize ) {mygame.display.println("Could not write!"); goto draw_loop;}
   }
    fclose(filep);
    stop_ms = mygame.getTime();
    mygame.display.print("Wrote:");
    if( stop_ms-start_ms > 0 )
        mygame.display.print( totalFileSize/(stop_ms-start_ms) ); // b/ms == kb/s
    else
        mygame.display.print( 0 ); // b/ms == kb/s
    mygame.display.println(" kb/s");

    // Open file
    filep = NULL;
    filep = fopen(filePathAndNameSDFS, "rb");
     if(filep == NULL) {
        mygame.display.println("Could not open file for read"); goto draw_loop;
    }

    // Read from SD
    start_ms = mygame.getTime();

    for(int32_t i=0;; i+=blockSize) {
        // read the block into the buffer:
        len = fread (buffer, sizeof(char), blockSize, filep);
        if (len != blockSize) break;
    }
    stop_ms = mygame.getTime();
    fclose(filep);

    // Check the result
    for(int32_t i=0;i<blockSize; i++)
        if(buffer[i] != bufferOrg[i]) {mygame.display.println("Invalid data!"); goto draw_loop;}

    mygame.display.print("Read:");
    if( stop_ms-start_ms > 0 )
        mygame.display.print( totalFileSize/(stop_ms-start_ms) ); // b/ms == kb/s
    else
        mygame.display.print( 0 ); // b/ms == kb/s
    mygame.display.println(" kb/s");

    // *** Test PetitFatFS

    pokInitSD(); // Call init always.
    (void)fileOpen(filePathAndNamePFFS, FILE_MODE_READONLY);
    mygame.display.println("*PetitFFS*");

    //start_us = us_ticker_read();
    start_ms = mygame.getTime();
    for(int32_t i=0;; i+=blockSize) {
      char cbuffer[blockSize];
      uint16_t len = fileReadBytes((uint8_t*)cbuffer, blockSize);
      if( len == 0 ) break;
      fileSeekRelative(blockSize);
    }
    stop_ms = mygame.getTime();
    fileClose(); // close any open files

    // Check the result
    for(int32_t i=0;i<totalFileSize; i++)
        if(bufferOrg[i] != bufferOrg[i]) {mygame.display.println("Data invalid!"); goto draw_loop;}

    mygame.display.print("Read:");
    if( stop_ms-start_ms > 0 )
        mygame.display.print( totalFileSize/(stop_ms-start_ms) ); // b/ms == kb/s
    else
        mygame.display.print( 0 ); // b/ms == kb/s
    mygame.display.print(" kb/s");

draw_loop:
    while (mygame.isRunning()) {
        if (mygame.update()) {
         }
    }
}
