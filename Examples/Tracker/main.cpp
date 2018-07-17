#include "Pokitto.h"
#include "Arrays.h"
//#include "SDFileSystem.h"

//SDFileSystem sd(/*MOSI*/P0_9, /*MISO*/P0_8, /*SCK*/P0_6, /*CS*/P0_7, /*Mountpoint*/"sd");

//#include "FileIO.h"
Pokitto::Core pok;
//global variables

uint8_t screenW = 220, screenH = 176, frameRate = 30, fontW = 5, fontH = 7, screen = 0, buttonRepeatFrame = 0;
//tracker class
#include "Tracker.h"
Tracker tracker;
//Patch settings mode (screen 1)
#include "PatchSettings.h"
//main
int main(){
    bool updatenow=true; // we will replace screen updating to need-to-change instances only
    // the reason to declare it here, inside main, is to not misuse it as a global variable
    // if I now try to access updatenow inside the Tracker class or some other place, its a compile error
    // this is called limiting the variable scope in C
    pok.display.persistence=true;
    #ifndef POK_SIM
    buttonRepeatFrame=1;
    #endif // POK_SIM

	trCookie.begin("PTracker",trCookie);
	pok.begin();
	pokInitSD();
	tracker.fillArrays();

	if (trCookie.firstsave == 12345678) {
            tracker.loadSong(trCookie.songname);
    }
	pok.display.width = screenW;
	pok.display.height = screenH;
	pok.display.setFont(font5x7);
	pok.setFrameRate(frameRate);

	while(pok.isRunning()){
        if (screen){ //Settings mode
            buttonRepeatFrame=3;
            if(pok.update(true)){
                updatenow=true;
                checkButtons();
                tracker.SetColorPalette();
                pok.display.clear();
                printSettings();
                drawPointer();
                tracker.drawHelpBar();
           }
        }
        if(!screen) { //Tracker mode
            buttonRepeatFrame=1;
            if (tracker.play){
                if (tracker.playTracker()) {
                    updatenow = true;
                }
            }
            if(pok.update(true)){
                if (tracker.checkButtons()) {
                        updatenow=true;
                }
            }

            if (!screen && updatenow) {
                tracker.SetColorPalette();
                pok.display.clear();
                tracker.drawIsPlaying();
                tracker.drawLines();
                tracker.printValues();
                tracker.printSettings();
                tracker.drawPointer();
                tracker.drawHelpBar();
            }
        }
        if (updatenow) {
	        pok.display.update(); // Update screen only when we have need to do so!
			// In this way we save ALOT of cpu power!
       		updatenow = false;
		}
	}
}
