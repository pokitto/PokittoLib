/**************************************************************************/
/*!
    @file     PokittoCore.cpp
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

#include "PokittoCore.h"
#include "Pokitto_settings.h"
#include "PokittoConsole.h"
#include "PokittoFonts.h"
#include "PokittoTimer.h"
#include "PokittoLogos.h"
#include <stdlib.h>
#ifndef DISABLEAVRMIN
#define max(a,b) ((a)>(b)?(a):(b))
#endif // DISABLEAVRMIN

char selectedfile[25];

//#define F
#ifdef __ARMCC_VERSION
typedef void (*func_t)(void);
#endif

#ifndef POK_SIM
#include "iap.h"
/** start the user application
* https://community.nxp.com/thread/417695
*
*/
void start_application(unsigned long app_link_location){
    //asm(" ldr sp, [r0,#0]");
    //asm(" ldr pc, [r0,#4]");
    //This code is not valid for the Cortex-m0+ instruction set.
    // The equivalent for this (as used by the KL26) is
__disable_irq();// Start by disabling interrupts, before changing interrupt vectors

// delete buttons
//pokDeleteButtons();

// completely kill button interrupts in preparation for reset
LPC_PINT->IENR = 0;
LPC_PINT->IENF = 0;

SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk & ~(SysTick_CTRL_ENABLE_Msk); //disable systick
LPC_SYSCON->PDRUNCFG     |=  (1 << 10);       /* Power-down USB PHY         */
LPC_SYSCON->PDRUNCFG     |=  (1 <<  8);       /* Power-down USB PLL         */

// reset clock source to IRC
LPC_SYSCON->MAINCLKUEN    = 0x01;             /* Update MCLK Clock Source   */
LPC_SYSCON->MAINCLKUEN    = 0x00;             /* Toggle Update Register     */
while (LPC_SYSCON->MAINCLKUEN & 0x01);     /* Wait Until Updated         */
// switch clock selection to IRC
LPC_SYSCON->MAINCLKSEL    = 0;             /* Select Clock Source        */
LPC_SYSCON->MAINCLKUEN    = 0x01;             /* Update MCLK Clock Source   */
LPC_SYSCON->MAINCLKUEN    = 0x00;             /* Toggle Update Register     */
while (LPC_SYSCON->MAINCLKUEN & 0x01);     /* Wait Until Updated         */
//disable PLL clock output
LPC_SYSCON->SYSPLLCLKUEN = 0;
while (LPC_SYSCON->SYSPLLCLKUEN & 0x00);
LPC_SYSCON->SYSPLLCTRL = 0;

//kill peripherals
LPC_SYSCON->MAINCLKSEL = 0;
LPC_SYSCON->PRESETCTRL = 0; //disable all peripherals

//power down PLL
volatile uint32_t tmp;
tmp = (LPC_SYSCON->PDRUNCFG & 0x000025FFL);
tmp |= ((1<<7) & 0x000025FFL);
LPC_SYSCON->PDRUNCFG = (tmp | 0x0000C800L); /* Power-down SYSPLL          */

//Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC); //switch to IRC

// clear all gpio states
LPC_GPIO_PORT->PIN[0] = 0;
LPC_GPIO_PORT->PIN[1] = 0;
LPC_GPIO_PORT->PIN[2] = 0;

SCB->VTOR = app_link_location;//APPL_ADDRESS; /* Change vector table address */
#ifndef __ARMCC_VERSION
asm(" mov r0, %[address]"::[address] "r" (app_link_location));
asm(" ldr r1, [r0,#0]"); // get the stack pointer value from the program's reset vector
asm(" mov sp, r1");      // copy the value to the stack pointer
asm(" ldr r0, [r0,#4]"); // get the program counter value from the program's reset vector
asm(" blx r0");          // jump to the' start address
#else
uint32_t *app_loc = (uint32_t*)app_link_location;
__set_MSP (app_loc[0]);
((func_t)(app_loc[1]))();
#endif
}
#endif

// returns a random integar between 0 and maxVal
int random(int maxVal)
{
  return random( 0, maxVal);
}

// returns a random integar between minVal and maxVal
int random(int minVal, int maxVal)
{
  // int rand(void); included by default from newlib
  return rand() % (maxVal-minVal+1) + minVal;
}

using namespace Pokitto;

bool Core::run_state; // this definition needed

/** Components */
Backlight Core::backlight;
Buttons Core::buttons;
Battery Core::battery;
#if POK_ENABLE_SOUND > 0
Sound Core::sound;
#endif
Display Core::display;

//GB Related
uint8_t Core::startMenuTimer;
uint8_t Core::timePerFrame;
uint32_t Core::nextFrameMillis;
uint32_t Core::frameCount;
const char* Core::popupText;
uint8_t Core::popupTimeLeft;
uint16_t Core::frameDurationMicros;
uint32_t Core::frameStartMicros, Core::frameEndMicros;
uint8_t Core::volbar_visible=0;

uint32_t Core::fps_counter;
bool Core::fps_counter_updated;
uint32_t Core::fps_refreshtime;
uint32_t Core::fps_frameCount;

Core::Core() {

}


int Core::updateLoader (uint32_t version, uint32_t jumpaddress) {
    #ifndef POK_SIM
    #ifndef NOPETITFATFS
    uint32_t counter=0;
    uint8_t data[256];
    /** prepare the flash writing **/
    float progress=0;
    int opg=-1;
    uint32_t fsize=0;
    fileEnd(); //
    fsize = fileGetPosition();
    if (fsize>0x40000-jumpaddress) fsize = 0x40000-jumpaddress; // shouldn't happen!!
    fileRewind();
    display.println("PLEASE WAIT");
    while (1) {
        //if (counter >= fsize-0x200) {
        //    display.println("gotcha");
        //}
        if (counter >= fsize) {
                break;
        }
        opg=progress;
        if (fileReadBytes(&data[0],0x100)<0x100) {
                if (fsize-counter>0x100) {
                        display.println("ERROR READING LOA.DER FILE");
                        return 1; // 1 means error
                }
        }
        if (CopyPageToFlash(jumpaddress+counter,data)) {
                    display.println("FLASH WRITE ERROR");
                    return 1;
        } else {
            counter += 0x100;
            display.print(".");
        }
        }
        #endif // NOPETITFATFS
        #endif // POK_SIM
    return 0; //success
}

void Core::showWarning() {
    display.enableDirectPrinting(true);
    display.directbgcolor = COLOR_BLACK;
    display.clearLCD();
    display.directcolor = COLOR_RED;
    display.setFont(fntC64UIGfx);
    display.adjustCharStep = 0;
    display.adjustLineStep = 0;
    display.setCursor(10*8,9);
    display.print("WARNING!");
    display.directcolor = COLOR_WHITE;
    display.setCursor(5*8,4*9);
    display.print("LOUD SOUND THROUGH");
    display.setCursor(2*8,5*9);
    display.print("HEADPHONES COULD DAMAGE");
    display.setCursor(7*8,6*9);
    display.print("YOUR HEARING.");
    display.setCursor(5*8,8*9);
    display.print("USE "); display.directcolor = COLOR_GREEN;
    display.print("0-100% VOLUME");
    display.directcolor = COLOR_WHITE;
    display.setCursor(5*8,9*9);
    display.print("FOR LISTENING WITH");
    display.setCursor(8*8,10*9);
    display.print("HEADPHONES");
    display.setCursor(5*8,12*9);
    display.print("USE "); display.directcolor = COLOR_RED;
    display.print("> 100% VOLUME"); display.directcolor = COLOR_GREEN;
    display.directcolor = COLOR_WHITE;
    display.setCursor(1*8,13*9);
    display.print("ONLY FOR LISTENING VIA THE");
    display.setCursor(5*8,14*9);
    display.print("BUILT-IN SPEAKER");
    display.setCursor(5*8,17*9);
    display.print("PRESS ");display.directcolor = COLOR_GREEN;
    display.print("C ");display.directcolor = COLOR_WHITE;
    display.print("TO ACCEPT");
    while (!buttons.cBtn()) {
        wait(100);
    }
    display.clearLCD();
    display.enableDirectPrinting(false);
}

void Core::jumpToLoader() {
    //display.setFont(font5x7);
    //display.adjustCharStep=1;
    //display.adjustLineStep=2;
    display.fontSize=1;
    display.directbgcolor=COLOR_BLACK;
    display.directcolor=COLOR_GREEN;
    display.clearLCD();
    display.setCursor(0,0);
    display.enableDirectPrinting(true);
    #ifdef POK_SIM
    display.println("LOADER IS NOT AVAILABLE ON THE SIMULATOR. PRESS A TO RETURN.");
    #else
    uint32_t* bootinfo;
    uint32_t bootversion=0, sdversion=0, sdjump=0;
    bool flashloader=false, checkforboot=true;
    //check for loa.der on SD card
    #if POK_ENABLE_LOADER_UPDATES > 0
    #ifndef NOPETITFATFS
    pokInitSD();
    if (fileOpen("LOA.DER", FILE_MODE_BINARY)==0) {
        //LOA.DER found on SD
        fileEnd(); // go to end
        fileSeekRelative(-8); //rewind 8 bytes
        uint32_t* tptr = &sdversion;
        fileReadBytes((uint8_t*)tptr,4); //read version number of loader on SD card
        tptr = &sdjump;
        fileReadBytes((uint8_t*)tptr,4); //read jump address of loader on sd card
        fileRewind();
    }
    #endif //NOPETITFATFS
    #endif
    //now start searching for bootkey
    while (checkforboot)
    {
    checkforboot=false; flashloader=false;
    bootinfo = (uint32_t*)0x3FFF4;
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3FF04; //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3FE04; //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3F004; //for futureproofing
    if (*bootinfo != 0xB007AB1E) {
        // no bootkey found at all
        display.directcolor=COLOR_YELLOW;
        display.println("NO LOADER INSTALLED");
        if (sdversion==0 || sdjump < 0x38000) {
                //file open of loader failed
                display.println("NO VALID LOA.DER ON SD");
                display.println("");
                display.directcolor=COLOR_GREEN;
                display.println("PUT LOA.DER ON SD & REBOOT");
        } else flashloader=true;
    } else {
        //loader was found
        //check if we should update the loader
        display.directcolor=COLOR_CYAN;
        display.print("LOADER V.");
        display.directcolor=COLOR_WHITE;
        display.println(*(bootinfo+1));
        #if POK_ENABLE_LOADER_UPDATES
        if (sdversion>(*(bootinfo+1))) flashloader=true;
        else start_application(*(bootinfo+2)); //never returns
        #else
        start_application(*(bootinfo+2)); //never returns
        #endif
    }
    // update loader if it was requested
    if(flashloader) {
            display.directcolor=COLOR_MAGENTA;
            display.print("NEW LOADER ON SD V.");
            display.directcolor=COLOR_WHITE;
            display.println(sdversion);
            display.directcolor=COLOR_GREEN;
            display.println("UPDATE LOADER?\n(UP=YES, DOWN=CANCEL)");
            while(1) {
                    if (buttons.upBtn()) {
                        if (updateLoader(sdversion,sdjump)) {
                            display.println("PUT LOA.DER ON SD AND RETRY");
                        } else {
                        display.println("SUCCESS!!");
                        checkforboot=true; //recheck
                        }
                    break;
                    }
                    if (buttons.downBtn()) return;
            }
    } // if flashloader
    } // while checkforboot
    #endif // POK_SIM
    while (!buttons.aBtn()) {
        buttons.pollButtons();
        if (buttons.aBtn()) {
            while (buttons.aBtn()) {
                buttons.pollButtons();
            }
            return;
        }
    }
}

void Core::askLoader() {
    display.enableDirectPrinting(true);
    display.directbgcolor = COLOR_BLACK;
    display.clearLCD();
    display.directcolor = COLOR_RED;
    display.setFont(fntC64UIGfx);
    display.fontSize=1;
    display.adjustCharStep = 0;
    display.adjustLineStep = 0;
    display.directcolor=COLOR_GREEN;
    display.set_cursor(12*8,6*8);
    display.print("ijkl");
    display.set_cursor(12*8,7*8);
    display.print("mnop");
    display.set_cursor(12*8,8*8);
    display.print("qrst");
    display.set_cursor(12*8,9*8);
    display.print("uvwx");
    display.set_cursor(5*8,12*8);
    display.print("PRESS");
    display.directcolor=COLOR_WHITE;
    display.print(" C ");
    display.directcolor=COLOR_GREEN;
    display.print("FOR LOADER");
    display.directcolor=COLOR_WHITE;
    display.fontSize=2;
    int countd = 0;
    #ifndef POK_SIM
    //read countdown time from settings
    countd = eeprom_read_byte((uint16_t*)EESETTINGS_LOADERWAIT);
    #endif
    if (countd==0 || countd > 5) countd=3;
    uint16_t c2 = getTime();
    while (countd) {
        buttons.pollButtons();
        display.set_cursor(13*8,15*8);
        display.print(countd);
        if (getTime()>c2+1000) {
            c2=getTime();
            countd--;
        }
        if (cBtn()) {while (cBtn()) buttons.pollButtons();jumpToLoader();countd=0;}
        if (aBtn()) {while (aBtn()) buttons.pollButtons();countd=0;}
        if (bBtn()) {while (bBtn()) buttons.pollButtons();countd=0;}
    }
    display.fontSize=1;
    display.clearLCD();
    display.enableDirectPrinting(false);
}


void Core::drawvolbar(int x, int y, int level, bool text) {
    uint16_t oldcol = display.directcolor;
    level = level >> 5;
    if (text) display.directRectangle(0,0,50,50,COLOR_BLACK);
    if (level<4)  display.directcolor = COLOR_GREEN;
    if (level==4) display.directcolor = COLOR_YELLOW;
    if (level>4) display.directcolor = COLOR_RED;

    if (text) {
            bool temp = display.isDirectPrintingEnabled();
            display.enableDirectPrinting(true);
            display.print(x-1,y-20,(int)sound.getVolume());
            display.print("  ");
            display.enableDirectPrinting(temp);
    }
    if (level<1) display.directcolor = COLOR_GRAY_80;
    display.directBitmap(x,y,Pokitto_volumebar,1,1);
    if (level<2) display.directcolor = COLOR_GRAY_80;
    display.directBitmap(x+8,y,Pokitto_volumebar,1,1);
    display.directBitmap(x+8,y-4,Pokitto_volumebar,1,1);
    if (level<3) display.directcolor = COLOR_GRAY_80;
    display.directBitmap(x+16,y,Pokitto_volumebar,1,1);
    display.directBitmap(x+16,y-4,Pokitto_volumebar,1,1);
    display.directBitmap(x+16,y-8,Pokitto_volumebar,1,1);
    if (level<4) {
            display.directcolor = COLOR_GRAY_80;
    }
    display.directBitmap(x+24,y,Pokitto_volumebar,1,1);
    display.directBitmap(x+24,y-4,Pokitto_volumebar,1,1);
    display.directBitmap(x+24,y-8,Pokitto_volumebar,1,1);
    display.directBitmap(x+24,y-12,Pokitto_volumebar,1,1);

    if (level<5) {
            display.directcolor = COLOR_GRAY_80;
    }
    display.directBitmap(x+32,y,Pokitto_volumebar,1,1);
    display.directBitmap(x+32,y-4,Pokitto_volumebar,1,1);
    display.directBitmap(x+32,y-8,Pokitto_volumebar,1,1);
    display.directBitmap(x+32,y-12,Pokitto_volumebar,1,1);
    display.directBitmap(x+32,y-16,Pokitto_volumebar,1,1);

    if (level<6) {
            display.directcolor = COLOR_GRAY_80;
    }
    display.directBitmap(x+40,y,Pokitto_volumebar,1,1);
    display.directBitmap(x+40,y-4,Pokitto_volumebar,1,1);
    display.directBitmap(x+40,y-8,Pokitto_volumebar,1,1);
    display.directBitmap(x+40,y-12,Pokitto_volumebar,1,1);
    display.directBitmap(x+40,y-16,Pokitto_volumebar,1,1);
    display.directBitmap(x+40,y-20,Pokitto_volumebar,1,1);
    display.directcolor = oldcol;
}


#ifdef POK_SIM
#define VINCMULT 0.9f
#else
#define VINCMULT 50
#endif //POK_SIM
void Core::setVolLimit() {
    display.enableDirectPrinting(true);
    display.adjustCharStep = 0;
    //sound.setMaxVol(VOLUME_HEADPHONE_MAX);
    int dstate=1;
    bool wipe = true;
    float vol = sound.getVolume(); float tvol;
    #ifndef POK_SIM
    vol=eeprom_read_byte((uint16_t*)EESETTINGS_VOL);
    Pokitto::Sound::globalVolume=vol;
    #endif
    if (vol>VOLUME_HEADPHONE_MAX) sound.setMaxVol(VOLUME_SPEAKER_MAX);
    else sound.setMaxVol(VOLUME_HEADPHONE_MAX);
    #ifdef PRODUCTIONTESTING
    vol=240;
    sound.setMaxVol(VOLUME_SPEAKER_MAX);
    #endif
    for (uint8_t t=0;t<=vol;t++) {
            sound.setVolume(t);
    }
    volbar_visible=0;
    int countd=0;
    #ifndef POK_SIM
    //read countdown time from settings
    countd = eeprom_read_byte((uint16_t*)EESETTINGS_VOLWAIT);
    #endif
    if (countd==0 || countd > 10) countd=0xFFFF;
    #ifdef PRODUCTIONTESTING
    countd=2;
    #endif
    uint16_t c2 = getTime();
    while (core.isRunning() && dstate && countd){
        if (getTime()>c2+1000) {
            c2=getTime();
            if (countd<0xFFFF) countd--;
        }
        switch (dstate) {
        case 1:
            //redraw
            if (wipe) {
            display.clearLCD();
            display.directcolor = COLOR_WHITE;
            display.setCursor(4*8,2*8);
            display.print("SELECT VOLUME LIMIT");
            display.setCursor(5*8,17*9);
            display.print("PRESS ");
            display.directcolor = COLOR_GREEN;
            display.print("A");
            display.directcolor = COLOR_WHITE;
            display.print(" TO ACCEPT");
            display.directcolor = COLOR_GREEN;
            // draw frame below first
            display.setCursor(0,11*8);
            display.println(" abbbbbbbbbbbbbbbbbbbbbbbc");
            display.println(" |                       |");
            display.println(" |                       |");
            display.println(" |                       |");
            display.println(" |                       |");
            display.println(" dbbbbbbbbbbbbbbbbbbbbbbbe");
            } // wipe = true
            display.setCursor(6*8,17*9);
            if (discrete_vol<4) display.directcolor = COLOR_WHITE;
            else display.directcolor = COLOR_RED;
            display.directBitmap(21*8-4,6*8,Pokitto_headphones,1,2);
            display.setCursor(3*8,6*8+6);
            display.print("HEADPHONES");
            display.setCursor(3*8,8*8+2);
            if (discrete_vol>3) display.print("TOO LOUD!");
            else display.print("OK        ");
            display.directcolor = COLOR_GREEN;
            display.directBitmap(21*8-4,12*8,Pokitto_speaker,1,2);
            display.setCursor(3*8,12*8+6);
            display.print("VOLUME MAX");
            display.setCursor(3*8,14*8+2);
            //tvol = (vol/float(VOLUME_HEADPHONE_MAX))*100;
            //if (tvol > 100 && tvol < 120) tvol=100;
            //if (sound.getVolume()-5>VOLUME_HEADPHONE_MAX) { display.directcolor=COLOR_RED;}
            if ((sound.getVolume()>>5)>3) { display.directcolor=COLOR_RED;}
            else display.directcolor=COLOR_GREEN;
            if (discrete_vol==4) display.directcolor=COLOR_YELLOW;
            if (discrete_vol==7) discrete_vol=6;
            display.print((int)discrete_vol);
            //display.print(int(tvol));
            display.print("  ");
            display.directcolor=COLOR_GREEN;
            drawvolbar(14*8,14*8+4+3,sound.getVolume(),false);
            //display.setCursor(1,10);
            //display.print(vol);
            dstate=2; break;
        case 2:
            #ifndef POK_SIM
            buttons.pollButtons();
            #endif // POK_SIM
            buttons.update();
            if (aBtn()) {dstate=0;break;}
            if (buttons.pressed(BTN_RIGHT)) {
                    countd=0xFFFF; //disable countdown
                    /*if (vol >= VOLUME_HEADPHONE_MAX && vol < VOLUME_HEADPHONE_MAX+1 ) vol += 0.00025f*VINCMULT;
                    else if (vol >= VOLUME_HEADPHONE_MAX) vol += 0.25f*VINCMULT;
                    else vol += 0.05f*VINCMULT;
                    if (vol > VOLUME_HEADPHONE_MAX + 20) {
                            sound.setMaxVol(VOLUME_SPEAKER_MAX);
                    }
                    if (vol > VOLUME_SPEAKER_MAX) vol=VOLUME_SPEAKER_MAX;
                    sound.setVolume(vol);*/
                    sound.volumeUp();
                    dstate=1; wipe=false;
                    break;
                    }
            if (buttons.pressed(BTN_LEFT)) {
                    countd=0xFFFF; //disable countdown
                    /*if (vol >= VOLUME_HEADPHONE_MAX) vol -= 0.25f*VINCMULT;
                    else vol -= 0.05f*VINCMULT;
                    if (vol <= VOLUME_HEADPHONE_MAX) sound.setMaxVol(VOLUME_HEADPHONE_MAX);
                    if (vol < 0) vol=0;
                    sound.setVolume(vol);*/
                    sound.volumeDown();
                    dstate=1; wipe=false;
                    break;
                    }
            break;
        }
    }
    vol = sound.getVolume();
    #ifndef POK_SIM
    if (vol != eeprom_read_byte((uint16_t*)EESETTINGS_VOL)) eeprom_write_byte((uint16_t*)EESETTINGS_VOL,(uint8_t)vol);
    #endif
    sound.setVolume(vol);
    //sound.volumeUp();
    display.setCursor(0,0);
}

void Core::begin() {

    init(); // original functions
    timePerFrame = POK_FRAMEDURATION;
	//nextFrameMillis = 0;
	//frameCount = 0;
	frameEndMicros = 1;
	startMenuTimer = 255;
	//read default settings from flash memory (set using settings.hex)
	readSettings();
	//init everything
	backlight.begin();
	backlight.set(BACKLIGHT_MAX);
	buttons.begin();
	buttons.update();
	battery.begin();
	display.begin();
	#if POK_DISPLAYLOGO
        #if PROJ_DEVELOPER_MODE != 1
        showLogo();
        #endif // PROJ_DEVELOPER_MODE
	#endif // POK_DISPLAYLOGO

	display.enableDirectPrinting(true);
    display.directbgcolor = COLOR_BLACK;
    display.clearLCD();
    display.setFont(fntC64UIGfx);

    display.enableDirectPrinting(true);
    display.directbgcolor = COLOR_BLACK;
    display.directcolor = COLOR_GREEN;
    display.clearLCD();
    display.setFont(fntC64UIGfx);
    display.adjustCharStep=0;
    display.adjustLineStep=1;
    #ifdef JUSTLOAD
	jumpToLoader();
	#endif

    #ifndef DISABLE_LOADER
    #if PROJ_DEVELOPER_MODE != 1
    askLoader();
    #endif // PROJ_DEVELOPER_MODE
    #endif

	#if PROJ_DEVELOPER_MODE==1
	sound.setMaxVol(VOLUME_SPEAKER_MAX);
	sound.setVolume(VOLUME_SPEAKER_MAX);
	#else
	//showWarning();
	setVolLimit();
	display.clear();
	display.update();

	while(buttons.states[BTN_A])
    {
        buttons.update();
    }

	//sound.setVolume(sound.getVolume());//make sure we're at set volume before continue
	#endif
	display.enableDirectPrinting(false);
	display.adjustCharStep=1;
	display.adjustLineStep=1;
	display.fontSize=1;
	display.textWrap=true;
	#if POK_GAMEBUINO_SUPPORT > 0
	display.setFont(font5x7);
	#else
	display.setFont(fontC64);
    #endif
	#if POK_ENABLE_SOUND > 0
        sound.begin();

	//mute when B is held during start up or if battery is low
	battery.update();
	if(buttons.pressed(BTN_B)){
		sound.setVolume(0);
	}
	else{ //play the startup sound on each channel for it to be louder
		#if POK_GBSOUND > 0
		#if(NUM_CHANNELS > 0)
			sound.playPattern(startupSound, 0);
		#endif
		#if(NUM_CHANNELS > 1)
			sound.playPattern(startupSound, 1);
		#endif
		#if(NUM_CHANNELS > 2)
			sound.playPattern(startupSound, 2);
		#endif
		#if(NUM_CHANNELS > 3)
			sound.playPattern(startupSound, 3);
		#endif
		#endif // POK_GBSOUND
	}
	#endif // POK ENABLE_SOUND
}

void Core::init() {
    run_state = true;
    display.enableDirectPrinting(false);
    display.setFont(DEFAULT_FONT);
    initClock();
    initGPIO();
    initButtons();
    initRandom();
    //initAudio();
    //initDisplay();
}

void Core::init(uint8_t switches) {
    run_state = true;
    display.enableDirectPrinting(false);
    display.setFont(DEFAULT_FONT);
    initClock();
    initGPIO();
    initButtons();
    initRandom();
    //initAudio();
    //initDisplay();
}

void Core::initButtons() {
    #ifndef POK_SIM
    Pokitto::initButtons();
    #endif
}

bool Core::isRunning() {
    #ifdef POK_SIM
    run_state = simulator.isRunning();
    #endif // POK_SIM
    return run_state;
}

void Core::initDisplay() {
    #if POK_DISPLAYLOGO > 0
        showLogo();
    #endif
    #if POK_USE_CONSOLE > 0
        console.AddMessage(MSOURCE_LCD,MSG_INIT_OK);
    #endif
}

void Core::showLogo() {
    uint32_t now;
    uint8_t state=0; //jump directly to logo, bypass teeth
    uint16_t counter=0, i=0;
    uint16_t sc;
    while (state < 255/6) {
    now=getTime();
    if (now>refreshtime) {
            refreshtime=now+30;
            switch (state) {
            case 0:
                /** POKITTO CLEAN **/
                display.directbgcolor = COLOR_BLACK;
                display.fillLCD(display.directbgcolor);
                sc = COLOR_BLACK;
                state++;
                break;
            case 1:
                /** POKITTO FADE IN **/
                display.directcolor = display.interpolateColor(sc, COLOR_GREEN, i);
                display.directBitmap(POK_LCD_W/2 - (*Pokitto_logo/2),POK_LCD_H/2-(*(Pokitto_logo+1)/2),Pokitto_logo,1,1);
                i += 28;
                if (i>=0xFF) { state++; i=0;}
                break;
            case 2:
                /** POKITTO WAIT **/
                display.directcolor = COLOR_GREEN;
                display.directBitmap(POK_LCD_W/2 - (*Pokitto_logo/2),POK_LCD_H/2-(*(Pokitto_logo+1)/2),Pokitto_logo,1,1);
                i+= 0x3F;
                if (i>0x3FF) state = 255;
                break;
            }
            if(buttons.aBtn()) state=255;
    }
    }
}

void Core::readSettings() {
    // ToDo
        /*display.contrast = SCR_CONTRAST;
		backlight.backlightMin = BACKLIGHT_MIN;
		backlight.backlightMax = BACKLIGHT_MAX;
		backlight.ambientLightMin = AMBIENTLIGHT_MIN;
		backlight.ambientLightMax = AMBIENTLIGHT_MAX;
*/
		sound.setMaxVol(VOLUME_HEADPHONE_MAX);
		sound.globalVolume = VOLUME_STARTUP;

		startMenuTimer = START_MENU_TIMER;
/*
		battery.thresolds[0] = BAT_LVL_CRITIC;
		battery.thresolds[1] = BAT_LVL_LOW;
		battery.thresolds[2] = BAT_LVL_MED;
		battery.thresolds[3] = BAT_LVL_FULL;*/
}

void Core::titleScreen(const char* name){
	titleScreen(name, 0);
}

void Core::titleScreen(const uint8_t* logo){
	titleScreen((""), logo);
}

void Core::titleScreen(){
	titleScreen((""));
}

void Core::titleScreen(const char*  name, const uint8_t *logo){
	display.setFont(font5x7);
	if(startMenuTimer){
		display.fontSize = 1;
		display.textWrap = false;
		display.persistence = false;
		battery.show = false;
		display.setColor(BLACK);
		while(isRunning()){
			if(update()){
				uint8_t logoOffset = name[0]?display.fontHeight:0; //add an offset the logo when there is a name to display
				//draw graphics
				display.setColorDepth(1);
				display.setColor(3);
				//display.drawBitmap(0,0, gamebuinoLogo);
				display.setColor(1);
				if(logo){
					display.drawBitmap(0, 12+logoOffset, logo);
				}
				display.cursorX = 0;
				display.cursorY = 12;
				display.print(name);

				//A button
				display.cursorX = LCDWIDTH - display.fontWidth*3 -1;
				display.cursorY = LCDHEIGHT - display.fontHeight*3 - 3;
				if((frameCount/16)%2)
				  display.println(("\25 \20"));
				else
				  display.println(("\25\20 "));
				//B button
				display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
				display.cursorY++;
				if(sound.globalVolume)
					display.println(("\26\23\24"));
				else
					display.println(("\26\23x"));
				//C button
				display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
				display.cursorY++;
				//display.println(F("\27SD"));

				//toggle volume when B is pressed
				if(buttons.pressed(BTN_B)){
					sound.setVolume(sound.getVolume() + 1);
					sound.playTick();
				}
				//leave the menu
				if(buttons.pressed(BTN_A) || ((frameCount>=startMenuTimer)&&(startMenuTimer != 255))){
					startMenuTimer = 255; //don't automatically skip the title screen next time it's displayed
					sound.stopPattern(0);
					sound.playOK();
					break;
				}
				//flash the loader
				//if(buttons.pressed(BTN_C))
					// ToDo changeGame();
			}
		}
		battery.show = true;
	}
}

/**
 * Update all the subsystems, like graphics, audio, events, etc.
 * Note: the update rect is used for drawing only part of the screen buffer to LCD. Because of speed optimizations,
 * the x, y, and width of the update rect must be dividable by 4 pixels, and the height must be dividable by 8 pixels.
 * The update rect is currently used for 220x176, 4 colors, screen mode only.
 * @param useDirectMode True, if only direct screen drawing is used. False, if the screen buffer is drawn. Note: If sprites are enabled, they are drawn in both modes.
 * @param updRectX The update rect.
 * @param updRectY The update rect.
 * @param updRectW The update rect.
 * @param updRectH The update rect.
 */
bool Core::update(bool useDirectMode, uint8_t updRectX, uint8_t updRectY, uint8_t updRectW, uint8_t updRectH) {

    #if POK_STREAMING_MUSIC
        sound.updateStream();
    #endif

    uint32_t now = getTime();
	if ((((nextFrameMillis - now)) > timePerFrame) && frameEndMicros) { //if time to render a new frame is reached and the frame end has ran once
		nextFrameMillis = now + timePerFrame;
		frameCount++;

		frameEndMicros = 0;
		backlight.update();
		buttons.update();
		battery.update();

        // FPS counter
		#if defined(PROJ_USE_FPS_COUNTER) ||  defined(PROJ_SHOW_FPS_COUNTER)
        const uint32_t fpsInterval_ms = 1000*3;

        fps_frameCount++;
        if (now > fps_refreshtime) {
            fps_counter = (1000*fps_frameCount) / (now - fps_refreshtime + fpsInterval_ms);
            fps_refreshtime = now + fpsInterval_ms;
            fps_frameCount = 0;
            fps_counter_updated = true;
        }
        #endif

		return true;

	} else {
		if (!frameEndMicros) { //runs once at the end of the frame
			#if POK_ENABLE_SOUND > 0
			sound.updateTrack();
			sound.updatePattern();
			sound.updateNote();
			#endif
			updatePopup();
			displayBattery();

            display.update(useDirectMode, updRectX, updRectY, updRectW, updRectH); //send the buffer to the screen

            frameEndMicros = 1; //jonne

		}
		return false;
	}
}

void Core::displayBattery(){
#if (ENABLE_BATTERY > 0)
	//display.setColor(BLACK, WHITE);
	uint8_t ox,oy;
	ox=display.cursorX;
	oy=display.cursorY;
	display.cursorX = LCDWIDTH-display.fontWidth+1;
	display.cursorY = 0;
	switch(battery.level){
	case 0://battery critic, power down
		sound.stopPattern();
		backlight.set(0);
		display.clear();
		display.fontSize = 1;
		display.print(("LOW BATTERY\n"));
		display.print(battery.voltage);
		display.print(("mV\n\nPLEASE\nTURN OFF"));
		display.update();
		break;
	case 1: //empty battery
		if((frameCount % 16) < 8) display.print('\7'); //blinking battery
		else display.print('x');
		break;
	case 2://low battery
	case 3://full battery
	case 4://full battery
		if(battery.show){
			display.print(char(5+battery.level));
		}
		break;
	default:
		if(battery.show){
			display.print('/');
		}
		break;
	}
display.cursorX = ox;
display.cursorY = oy;
#endif
}

char* Core::filemenu(char *ext) {
    display.persistence = false;
    uint16_t oldpal0=display.palette[0];
    uint16_t oldpal1=display.palette[1];
    uint16_t oldpal2=display.palette[2];
    display.palette[2]=COLOR_GREEN;
    display.palette[1]=COLOR_WHITE;
    display.palette[0]=COLOR_BLACK;
    uint8_t oldbg=display.bgcolor;
    uint8_t oldfg=display.color;
    display.color=1;
    display.bgcolor=0;

    int8_t activeItem = 0;
	int16_t currentY = 100;
	int16_t targetY = 0, rowh = display.fontHeight + 2;
	boolean exit = false;

	char* txt;


	while (isRunning()) {
		if (update()) {
            getFirstFile(ext);
			if (buttons.pressed(BTN_A) || buttons.pressed(BTN_B) || buttons.pressed(BTN_C)) {
				exit = true; //time to exit menu !
				targetY = - display.fontHeight * 10 - 2; //send the menu out of the screen
				if (buttons.pressed(BTN_A)) {
					//answer = activeItem;
					sound.playOK();
				} else {
					sound.playCancel();
				}
			}
			if (exit == false) {
				if (buttons.repeat(BTN_DOWN,4)) {
					activeItem++;
					sound.playTick();
				}
				if (buttons.repeat(BTN_UP,4)) {
					activeItem--;
					sound.playTick();
				}
				//don't go out of the menu
				//if (activeItem == length) activeItem = 0;
				//if (activeItem < 0) activeItem = length - 1;
                if (currentY>targetY) currentY-=16;
                if (currentY<targetY) currentY=targetY;
				//targetY = -rowh * activeItem + (rowh+4); //center the menu on the active item
			} else { //exit :
			    if (currentY>targetY) currentY-=16;
                if (currentY<targetY) currentY=targetY;
				if ((currentY - targetY) <= 1)
				{
				    display.bgcolor=oldbg;
				    display.color=oldfg;
				    display.palette[0] = oldpal0;
				    display.palette[1] = oldpal1;
				    display.palette[2] = oldpal2;
				    return selectedfile;
				}

			}
			//draw a fancy menu
			//currentY = 0;//(currentY + targetY) / 2 + 5;
			display.cursorX = 0;
			display.cursorY = currentY;
			display.textWrap = false;
			uint16_t fc,bc;
			fc = display.color;
            bc = display.bgcolor;
            //getFirstFile(ext);
			for (int i = 0; i<20; i++) {
				display.invisiblecolor=255;
				display.cursorY = currentY + rowh * i;
				if (i==3) display.color=1;
				if (i == activeItem){
					display.cursorX = 3;

                    //display.fillRoundRect(0, currentY + display.fontHeight * activeItem - 2, LCDWIDTH, (display.fontHeight+3), 3);
                    display.color=2;
                    display.fillRect(0, currentY + rowh * activeItem - 2, LCDWIDTH, (rowh));
                    display.setColor(0,2);
				} else display.setColor(1,0);
				//display.println((char*)*(const unsigned int*)(items+i));
				//display.println((int)i);
                txt = getNextFile(ext);
                if (txt) {
                        display.println(txt);
                        if (i == activeItem) {
                            strcpy(selectedfile,txt);
                        }
                } else i--;
                display.setColor(1,0);
			} // draw menu loop
		} // update
	}
	return 0;
}

char* Core::filemenu() {
    return filemenu("");
}

int8_t Core::menu(const char* const* items, uint8_t length) {
#if (ENABLE_GUI > 0)
	display.persistence = false;
	int8_t activeItem = 0;
	int16_t currentY = display.height;
	int16_t targetY = 0, rowh = display.fontHeight + 2;
	boolean exit = false;
	int8_t answer = -1;
	while (isRunning()) {
		if (update()) {
			if (buttons.pressed(BTN_A) || buttons.pressed(BTN_B) || buttons.pressed(BTN_C)) {
				exit = true; //time to exit menu !
				targetY = - display.fontHeight * length - 2; //send the menu out of the screen
				if (buttons.pressed(BTN_A)) {
					answer = activeItem;
					sound.playOK();
				} else {
					sound.playCancel();
				}
			}
			if (exit == false) {
				if (buttons.repeat(BTN_DOWN,4)) {
					activeItem++;
					sound.playTick();
				}
				if (buttons.repeat(BTN_UP,4)) {
					activeItem--;
					sound.playTick();
				}
				//don't go out of the menu
				if (activeItem == length) activeItem = 0;
				if (activeItem < 0) activeItem = length - 1;

				targetY = -rowh * activeItem + (rowh+4); //center the menu on the active item
			} else { //exit :
				if ((currentY - targetY) <= 1)
				return (answer);
			}
			//draw a fancy menu
			currentY = (currentY + targetY) / 2;
			display.cursorX = 0;
			display.cursorY = currentY;
			display.textWrap = false;
			uint16_t fc,bc;
			fc = display.color;
            bc = display.bgcolor;
			for (byte i = 0; i < length; i++) {
				display.cursorY = currentY + rowh * i;
				if (i == activeItem){
					display.cursorX = 3;

                    //display.fillRoundRect(0, currentY + display.fontHeight * activeItem - 2, LCDWIDTH, (display.fontHeight+3), 3);
                    display.fillRect(0, currentY + rowh * activeItem - 2, LCDWIDTH, (rowh));
                    display.setColor(bc,fc);
				} else display.setColor(fc,bc);

				display.println((char*)*(const unsigned int*)(items+i));
				display.setColor(fc,bc);
			}

		}
	}
#else
	return 0;
#endif
	return 0;
}

void Core::keyboard(char* text, uint8_t length) {
#if (ENABLE_GUI > 0)
	display.persistence = false;
	//memset(text, 0, length); //clear the text
	text[length-1] = '\0';
	//active character in the typing area
	int8_t activeChar = 0;
	//selected char on the keyboard
	int8_t activeX = 0;
	int8_t activeY = 2;
	//position of the keyboard on the screen
	int8_t currentX = LCDWIDTH;
	int8_t currentY = LCDHEIGHT;
	int8_t targetX = 0;
	int8_t targetY = 0;

	while (1) {
		if (update()) {
			//move the character selector
			if (buttons.repeat(BTN_DOWN, 4)) {
				activeY++;
				sound.playTick();
			}
			if (buttons.repeat(BTN_UP, 4)) {
				activeY--;
				sound.playTick();
			}
			if (buttons.repeat(BTN_RIGHT, 4)) {
				activeX++;
				sound.playTick();
			}
			if (buttons.repeat(BTN_LEFT, 4)) {
				activeX--;
				sound.playTick();
			}
			//don't go out of the keyboard
			if (activeX == KEYBOARD_W) activeX = 0;
			if (activeX < 0) activeX = KEYBOARD_W - 1;
			if (activeY == KEYBOARD_H) activeY = 0;
			if (activeY < 0) activeY = KEYBOARD_H - 1;
			//set the keyboard position on screen
			targetX = -(display.fontWidth+1) * activeX + LCDWIDTH / 2 - 3;
			targetY = -(display.fontHeight+1) * activeY + LCDHEIGHT / 2 - 4 - display.fontHeight;
			//smooth the keyboard displacement
			currentX = (targetX + currentX) / 2;
			currentY = (targetY + currentY) / 2;
			//type character
			if (buttons.pressed(BTN_A)) {
				if (activeChar < (length-1)) {
					byte thisChar = activeX + KEYBOARD_W * activeY;
					if((thisChar == 0)||(thisChar == 10)||(thisChar == 13)) //avoid line feed and carriage return
					continue;
					text[activeChar] = thisChar;
					text[activeChar+1] = '\0';
				}
				activeChar++;
				sound.playOK();
				if (activeChar > length)
				activeChar = length;
			}
			//erase character
			if (buttons.pressed(BTN_B)) {
				activeChar--;
				sound.playCancel();
				if (activeChar >= 0)
				text[activeChar] = 0;
				else
				activeChar = 0;
			}
			//leave menu
			if (buttons.pressed(BTN_C)) {
				sound.playOK();
				while (1) {
					if (update()) {
						//display.setCursor(0,0);
						display.println(("You entered\n"));
						display.print(text);
						display.println(("\n\n\n\x15:okay \x16:edit"));
						if(buttons.pressed(BTN_A)){
							sound.playOK();
							return;
						}
						if(buttons.pressed(BTN_B)){
							sound.playCancel();
							break;
						}
					}
				}
			}
			//draw the keyboard
			for (int8_t y = 0; y < KEYBOARD_H; y++) {
				for (int8_t x = 0; x < KEYBOARD_W; x++) {
					display.drawChar(currentX + x * (display.fontWidth+1), currentY + y * (display.fontHeight+1), x + y * KEYBOARD_W, 1);
				}
			}
			//draw instruction
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+1*(display.fontHeight+1);
			display.print(("\25type"));

			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+2*(display.fontHeight+1);
			display.print(("\26back"));

			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+3*(display.fontHeight+1);
			display.print(("\27save"));

			//erase some pixels around the selected character
			display.setColor(WHITE);
			display.drawFastHLine(currentX + activeX * (display.fontWidth+1) - 1, currentY + activeY * (display.fontHeight+1) - 2, 7);
			//draw the selection rectangle
			display.setColor(BLACK);
			display.drawRoundRect(currentX + activeX * (display.fontWidth+1) - 2, currentY + activeY * (display.fontHeight+1) - 3, (display.fontWidth+2)+(display.fontWidth-1)%2, (display.fontHeight+5), 3);
			//draw keyboard outline
			//display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (display.fontWidth+1) + 12, KEYBOARD_H * (display.fontHeight+1) + 12, 8, BLACK);
			//text field
			display.drawFastHLine(0, LCDHEIGHT-display.fontHeight-2, LCDWIDTH);
			display.setColor(WHITE);
			display.fillRect(0, LCDHEIGHT-display.fontHeight-1, LCDWIDTH, display.fontHeight+1);
			//typed text
			display.cursorX = 0;
			display.cursorY = LCDHEIGHT-display.fontHeight;
			display.setColor(BLACK);
			display.print(text);
			//blinking cursor
			if (((frameCount % 8) < 4) && (activeChar < (length-1)))
			display.drawChar(display.fontWidth * activeChar, LCDHEIGHT-display.fontHeight, '_',1);
		}
	}
#endif
}

void Core::popup(const char* text, uint8_t duration){
#if (ENABLE_GUI > 0)
	popupText = text;
	popupTimeLeft = duration+12;
#endif
}

void Core::updatePopup(){
#if (ENABLE_GUI > 0)
	if (popupTimeLeft){
		uint8_t yOffset = 0;
		if(popupTimeLeft<12){
			yOffset = 12-popupTimeLeft;
		}
		display.fontSize = 1;
		display.setColor(WHITE);
		display.fillRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,84,display.fontHeight+3,3);
		display.setColor(BLACK);
		display.drawRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,84,display.fontHeight+3,3);
		display.cursorX = 4;
		display.cursorY = LCDHEIGHT-display.fontHeight+yOffset-1;
		display.print(popupText);
		popupTimeLeft--;
	}
#endif
}

void Core::setFrameRate(uint8_t fps) {
	timePerFrame = 1000 / fps;
	sound.prescaler = fps / 20;
	sound.prescaler = __avrmax(1, sound.prescaler);
}

void Core::pickRandomSeed(){
        initRandom();
}

bool Core::collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h){
	if((x1>=x2)&&(x1<x2+w))
	if((y1>=y2)&&(y1<y2+h))
	return true;
	return false;
}

bool Core::collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2){
  return !( x2     >=  x1+w1  ||
            x2+w2  <=  x1     ||
            y2     >=  y1+h1  ||
            y2+h2  <=  y1     );
}

bool Core::collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2){
  int16_t w1 = pgm_read_byte(b1);
  int16_t h1 = pgm_read_byte(b1 + 1);
  int16_t w2 = pgm_read_byte(b2);
  int16_t h2 = pgm_read_byte(b2 + 1);

  if(collideRectRect(x1, y1, w1, h1, x2, y2, w2, h2) == false){
    return false;
  }

  int16_t xmin = (x1>=x2)? 0 : x2-x1;
  int16_t ymin = (y1>=y2)? 0 : y2-y1;
  int16_t xmax = (x1+w1>=x2+w2)? x2+w2-x1 : w1;
  int16_t ymax = (y1+h1>=y2+h2)? y2+h2-y1 : h1;
  for(uint8_t y = ymin; y < ymax; y++){
    for(uint8_t x = xmin; x < xmax; x++){
      if(display.getBitmapPixel(b1, x, y) && display.getBitmapPixel(b2, x1+x-x2, y1+y-y2)){
        return true;
      }
    }
  }
  return false;
}


//** EOF **//








