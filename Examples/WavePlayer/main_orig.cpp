#include <stdio.h>
#include "Pokitto.h"
#include "SDFileSystem.h"
#include "wave_player.h"

SDFileSystem sd(/*MOSI*/P0_9, /*MISO*/P0_8, /*SCK*/P0_6, /*CS*/P0_7, /*Mountpoint*/"sd");

mbed::Ticker temptick;

Pokitto::Core game;

const char *fileNameList[] = {
    "/sd/1.mp3",
    "/sd/2.mp3",
};

#define BLOCK_SIZE (1024)

int ttcounter=0;

void tt () {
    ttcounter++;
   //game.display.print("TT");
}


int main ()
{

    game.begin();
    game.display.setColor(0,2);

    temptick.attach_us(&tt,500);


    static FILE *fp = NULL;
    static FILE *fp2 = NULL;
    size_t      sizeRead = 0;
    size_t      totalSizeSent=0;
    size_t      fileSize=0;
    uint8_t     buf[BLOCK_SIZE];


    fp = fopen("/sd/test.dat", "rb");
    fp2 = fopen("/sd/test.txt", "w");

    fseek( fp, 0, SEEK_END );
    fileSize = ftell( fp );
    game.display.persistence=1;
    game.display.setCursor(0,0);
    game.display.print("file size: ");
    game.display.println(fileSize);
    while (!game.buttons.upBtn()) {
            game.update();
    }
    rewind(fp);

    if (fp) {
        clearerr(fp);
        totalSizeSent = 0;
    }
    while(game.isRunning())
    {

        if(totalSizeSent>=fileSize)
        {   // Close when the track reaches the end
            //mp3.stop();
            fprintf(fp2, "We're writing to an SD card!");
            fclose(fp);
            fclose(fp2);
            fp = NULL;
            game.display.clear();
            game.display.println("stop");
            game.display.println(ttcounter);
            game.display.update();

            while(1)
            {
            }
        }
        else
        {
            sizeRead = fread(buf, sizeof(uint8_t), BLOCK_SIZE, fp);
            totalSizeSent += sizeRead;//mp3.sendDataBlock(buf, sizeRead);
            //printf("total %d Send %d\r\n",totalSizeSent,sizeRead);
            if (game.update()) {
                    game.display.println(totalSizeSent);
                    game.wait(100);
            }
        }
    }
}
