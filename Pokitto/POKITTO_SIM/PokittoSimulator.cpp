/**************************************************************************/
/*!
    @file     PokittoSimulator.cpp
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

#include "PokittoSimulator.h"
#include "Pokitto_settings.h"
#include "PokittoGlobs.h"
#include "PokittoSound.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "WriteWav.h"

#if POK_USE_CONSOLE > 0
    #include "PokittoConsole.h"
#endif // POK_USE_CONSOLE



bool PromptForChar( const char* prompt, char& readch )
{
    std::string tmp;
    std::cout << prompt << std::endl;
    if (std::getline(std::cin, tmp))
    {
        // Only accept single character input
        if (tmp.length() == 1)
        {
            readch = tmp[0];
        }
        else if (tmp.length() == 0)
        {
            readch = 'y';
        }
        else
        {
            // For most input, char zero is an appropriate sentinel
            readch = '\0';
        }
        return true;
    }
    return false;
}

using namespace Pokitto;

/** Internal to this compilation unit */
SDL_AudioSpec wanted, got;
SDL_AudioDeviceID audioDevice;
uint8_t sound_on;
unsigned int aud_position; /* which sample we are up to */
int aud_len; /* how many samples left to play, stops when <= 0 */
float aud_frequency; /* audio frequency in cycles per sample */
float aud_volume; /* audio volume, 0 - ~32000 */

Simulator simulator;

int Simulator::simRunning;
SDL_Window *Simulator::sdlSimWin;
SDL_Renderer *Simulator::sdlBgRen;
SDL_Renderer *Simulator::sdlRen;
SDL_Texture *Simulator::sdlTex;
SDL_Rect Simulator::SrcR;
SDL_Rect Simulator::DestR;
SDL_Event Simulator::sdlEvent;
SDL_Surface* Simulator::Loading_Surf;
SDL_Texture* Simulator::Background_Tx;
uint8_t Simulator::lcdpixels[SIMW*SIMH*4]; //used to create SDL texture of LCD screen
int Simulator::ww,Simulator::wh;
uint16_t Simulator::gfxbuf[SIMH*SIMW]; // simulated dram buffer of the LCD screen
uint16_t* Simulator::dramptr; // points to current pixel in simulated DRAM
uint16_t Simulator::datalines; // simulates the 16-bit datalines to the LCD
uint16_t Simulator::prevdata; // if data does not change, do not adjust LCD bus lines
uint8_t Simulator::lcd_shaking; // legacy variable from rboy (screen offset shake)
SDL_GameController* Simulator::gGameController;
bool Simulator::joystickPresent;
uint8_t Simulator::amp;
bool Simulator::mutex;
uint8_t Simulator::cleaned;
uint16_t Simulator::sc_count;
uint16_t Simulator::framenumber;
uint16_t Simulator::audframenumber;
uint32_t Simulator::videotime, Simulator::audiotime, Simulator::videoframesize, Simulator::audioframesize;

uint64_t recordingstarttime=0;

SDL_Event sdlEvent2;

Uint32 rfrsh_callback(Uint32 interval, void *param)
{
    if (simulator.isRunning()) {
            simulator.refreshDisplay();
    }
    return(interval);
}

Simulator::Simulator() {
    recordingstarttime = 0;
    videoframesize = POK_FRAMEDURATION * (SCREENCAPTURE ? SCREENCAPTURE : 1);
    audioframesize = ((SFBUFSIZE*1000)/POK_AUD_FREQ);
}

void Simulator::wait_ms(uint16_t t) {
    waitSDL(t);
}

void Simulator::initSDLGfx() {
    if (simRunning) return;
    simRunning = true;
    #if POK_USE_CONSOLE > 0
    console.visible=true;
    #endif // POK_USE_CONSOLE
    buttons_state = buttons_held = buttons_released = 0;

    SrcR.x = 0; SrcR.y = 0; SrcR.w = SIMW; SrcR.h = SIMH;

    #if SIM_FULLSCREEN
    DestR.x = 0; DestR.y = 0; DestR.w = 1280; DestR.h = 800;
    #else
    #if SIM_SHOWDEVICE
    DestR.x = (int)(400.0f*0.3275f); DestR.y = (int)(533.0f*0.185f);
    DestR.w = (int)(400.0f*0.54f); DestR.h = (int)(533.0f*0.08f);
    DestR.w = SIMW; DestR.h = SIMH;
    #else
    DestR.x = 0; DestR.y = 0; DestR.w = SIMW*2; DestR.h = SIMH*2;SrcR.w = SIMW*2; SrcR.h = SIMH*2;
    #endif // SIM_SHOWDEVICE
    #endif // SIM_FULLSCREEN

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { //{ SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

    #if USE_JOYSTICK > 0
        std::cout<<SDL_NumHaptics()<<std::endl<<SDL_NumJoysticks()<<std::endl;
        //Check for joysticks
        if( SDL_NumJoysticks() < 1 )
        {
            std::cout << "Warning: No joysticks connected!\n";
        } else {
        //Load joystick
            gGameController = SDL_GameControllerOpen( 0 );
            joystickPresent = true;
            if( gGameController == NULL ) {
                std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError();
            }
        }
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
        SDL_JoystickEventState(SDL_ENABLE);
    #endif // USE_JOYSTICK

    #if SIM_FULLSCREEN == 0
    #if SIM_SHOWDEVICE
    ww = 2.2f*SIMW;
    wh = 3.6f*SIMH;
    #else
    ww = SIMW*2;
    wh = SIMH*2;
    #endif // SIM_SHOWDEVICE
    sdlSimWin = SDL_CreateWindow("Pokitto simulator", 100, 100, ww, wh, SDL_WINDOW_SHOWN);
    #else
    ww = 1280;
    wh = 800;
    sdlSimWin = SDL_CreateWindow("Pokitto simulator", 0, 0, ww, wh, SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN);
    #endif


    if (sdlSimWin == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    sdlRen = SDL_CreateRenderer(sdlSimWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRen == nullptr){
        SDL_DestroyWindow(sdlSimWin);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    /* create the hardware-accelerated SDL texture, that will be used to draw the simulated screen */
    sdlTex = SDL_CreateTexture(sdlRen,
                               SDL_PIXELFORMAT_ABGR8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               SIMW, SIMH);


    dramptr = gfxbuf; // point simulated DRAM pointer to beginning of memory

    /** BACKGROUND **/
    sdlBgRen = SDL_CreateRenderer(sdlSimWin, -1, SDL_RENDERER_ACCELERATED);
    #if SIM_SHOWDEVICE > 0
        Loading_Surf = SDL_LoadBMP("Background.bmp");
        Background_Tx = SDL_CreateTextureFromSurface(sdlRen, Loading_Surf);
        SDL_FreeSurface(Loading_Surf); /* we got the texture now -> free surface */
    #endif // SIM_SHOWDEVICE

    SDL_TimerID rfrsh = SDL_AddTimer(100, rfrsh_callback, NULL);
}

void Simulator::waitSDL(uint16_t t) {
    SDL_Delay(t);
}

void Simulator::quit() {
    simRunning = false;
    CleanUp();
    exit(0);
}

int Simulator::isRunning() {
    //refreshDisplay(); //<-- crash!!
    return simRunning;
}

void Simulator::refreshDisplay() {
    if (mutex) return;
    mutex=1;
    #if POK_USE_CONSOLE > 0
    if (console.mode & CONS_VISIBLE && console.visible) {
        console.Draw();
    }
    #endif // POK_USE_CONSOLE

    #if POK_ARDUBOY_SUPPORT==0
    pollButtons();
    #endif // POK_ARDUBOY_SUPPORT

    uint16_t p=0;
    //convert simulated dram buffer to a pixel texture
    uint32_t q=0;
    for (uint16_t x=0; x < SIMW; x++) {
        for (uint16_t y=0; y < SIMH; y++, p++) {
            q = x*4 + y * SIMW *4;
            uint16_t R,G,B;
            uint16_t c;
            c = gfxbuf[p];
            B = ((c & 0x1F) * 255)>>5;
            c >>= 5;
            G = ((c & 0x3F) * 255)>>6;
            c >>= 6;
            R = (c*255)>>5;
            lcdpixels[q] = R;
            lcdpixels[q+1] = G;
            lcdpixels[q+2] = B;
            lcdpixels[q+3] = 255;
            }
    }

    /* update the SDL hardware texture with the pixels */
    SDL_UpdateTexture(sdlTex, NULL, lcdpixels, SIMW * sizeof (Uint32));
    //SDL_SetTextureAlphaMod(sdlTex, LCD_OPAQUENESS + random(FLICKER_AMOUNT));
    //SDL_SetTextureBlendMode(sdlTex, SDL_BLENDMODE_BLEND);


    /* update renderer with the texture */
    SDL_RenderClear(sdlRen);
    /** BACKGROUND **/
    #if SIM_SHOWDEVICE && (SIM_FULLSCREEN == 0)
    SDL_RenderCopy(sdlRen, Background_Tx, NULL, NULL);
    #endif
    /** LCD **/
    SDL_RenderCopy(sdlRen, sdlTex, &SrcR, &DestR);
    SDL_RenderPresent(sdlRen);

    // Create an empty RGB surface that will be used to create the screenshot bmp file
    #if SCREENCAPTURE > 0
        #if SIM_SHOWDEVICE >0
            SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, ww, wh, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        #else
            SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, SIMW*2, SIMH*2, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        #endif // SIM_SHOWDEVICE
    #endif // SCREENCAPTURE

   #if SCREENCAPTURE > 0
   if(pScreenShot && (cleaned == false))
   {
      if (sc_count==SCREENCAPTURE) {

        char framestr[40];
        sprintf(framestr,"c:\\screencap\\%i.bmp",framenumber);
        sc_count = 1; framenumber++;
        videotime += videoframesize;
        // Read the pixels from the current render target and save them onto the surface
        #if SIM_SHOWDEVICE > 0
            SDL_RenderReadPixels(sdlRen, NULL, SDL_GetWindowPixelFormat(sdlSimWin), pScreenShot->pixels, pScreenShot->pitch);
        #else
            SDL_RenderReadPixels(sdlRen, &DestR, SDL_GetWindowPixelFormat(sdlSimWin), pScreenShot->pixels, pScreenShot->pitch);
        #endif // SIM_SHOWDEVICE

        if (!recordingstarttime) recordingstarttime = SDL_GetTicks();
        SDL_SaveBMP(pScreenShot, framestr); // Create the bmp screenshot file
        SDL_FreeSurface(pScreenShot); // Destroy the screenshot surface
        } else sc_count++;

   }
   #endif

    #if SOUNDCAPTURE > 0
        if (activesfbuf != writesfbuf && cleaned == false) {
            fwrite(&soundfilebuffer[writesfbuf],1,SFBUFSIZE,soundfile);
            writesfbuf = activesfbuf;
            audiotime += audioframesize;
            audframenumber++;
        }
    #endif // SOUNDCAPTURE

    #if POK_ENABLE_REFRESHWITHWAIT
    /* this is not the best way of doing this, but since in lcd everything gets drawn to lcd immediately
    we need to simulate this somehow. However, polling buttons all the time messes up response in direct mode
    and therefore we use this option to turn it off*/
    pollButtons();
    #endif
    mutex=0;
}

void Simulator::directSDLPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= POK_LCD_W) || (y < 0) || (y >= POK_LCD_H))
	return;
    //uint16_t rgbColor = palette[color];
    uint16_t p = (x*POK_LCD_H) + y;
    gfxbuf[p]=color;
}

void Simulator::directSDLRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	int16_t sx,sy,ex,ey;
	if (x1>x2) {sx=x2; ex=x1;}
	else {sx=x1; ex=x2;}
	if (y1>y2) {sy=y2; ey=y1;}
	else {sy=y1; ey=y2;}
	for (int x=sx;x<ex;x++) {
        for(int y=sy;y<ey;y++) {
            directSDLPixel(x,y,color);
        }
	}
}

void Simulator::fillSDLSurface(uint16_t color) {
    for (int p=0;p<SIMH*SIMW;p++) {
        gfxbuf[p]=color;
	}
}

void Simulator::killSound() {
    sound_on=false;
    SDL_PauseAudioDevice(audioDevice, 1); /* pause sound immediately */
}

void Simulator::CleanUp() {
    if (cleaned) return;
    #if USE_JOYSTICK > 0
    SDL_GameControllerClose( gGameController );
    gGameController = NULL;
    #endif // USE_JOYSTICK
    std::ostringstream os;
    std::ostringstream os2;
    std::ostringstream os3, os4;
    std::string s;
    #if SCREENCAPTURE > 0
        killSound();
        SDL_DestroyWindow(sdlSimWin);
        char type = '\0';
        uint64_t elapsedtime = SDL_GetTicks()-recordingstarttime;
        float realfps = ((framenumber)/(float)(elapsedtime*0.9f))*1000;
        int64_t realfreq = (audframenumber*SFBUFSIZE)/(float)(elapsedtime*0.9f)*1000;
        std::cout << "Recording time in ticks: " << elapsedtime << "\n";
        std::cout << "Videoframes: " << framenumber << "\n";
        std::cout << "Audioframes: " << audframenumber << "\n";
        std::cout << "Video FPS: " << realfps << "\n";
        std::cout << "Audio sample frequency: " << realfreq << "\n";

        while( PromptForChar( "Save output video? [Y/n]", type ) )
        {
            if (type == 'y' || type == 'Y')
            {
                std::cout << "Converting video, please wait...\n";
                system("del c:\\screencap\\output.avi");
                system("del c:\\screencap\\output2.avi");
                system("del c:\\screencap\\output.mp4");

                //float frcorr = POK_FPS*1.42f/SCREENCAPTURE; //was 1.42, smaller value slows video stream down
                //float fcorr = ((float)videotime/(float)audiotime)*POK_FPS;
                //float frcorr = (float)POK_FPS/(float)SCREENCAPTURE;
                //int fr = (int)frcorr;
                #if SIM_SHOWDEVICE > 0
                    os << "c:\\screencap\\ffmpeg -r " << realfps << " -i c:\\screencap\\%d.bmp -qscale:v 0 -r " << realfps << " c:\\screencap\\output2.avi";
                    s = os.str();
                    system(s.c_str());
                    std::cout << "Cropping video, please wait...\n";
                    /** fix stupid green line at bottom **/
                    os3 << "c:\\screencap\\ffmpeg -i c:\\screencap\\output2.avi -qscale:v 0 -vf \"crop=" << ww << ":" << wh-2 << ":0:0\" -c:a copy c:\\screencap\\output.avi";
                    s = os3.str();
                    system(s.c_str());
                    #if MAKE_GIF > 0
                    std::cout << "Making GIF...";
                    os4 << "c:\\screencap\\convert -delay 0 -loop 0 c:\\screencap\\*.bmp c:\\screencap\\output.gif";
                    s = os4.str();
                    system(s.c_str());
                    #endif // MAKE_GIF
                #else
                    //os << "c:\\screencap\\ffmpeg -r " << realfps << " -i c:\\screencap\\%d.bmp -vf \"pad=width=330:height=264:x=55:y=44:color=black\" -qscale:v 0 -r " << realfps << " c:\\screencap\\output.avi";
                    os << "c:\\screencap\\ffmpeg -r " << realfps << " -i c:\\screencap\\%d.bmp -qscale:v 0 -r " << realfps << " c:\\screencap\\output.avi";
                    s = os.str();
                    system(s.c_str());
                    #if MAKE_GIF > 0
                    std::cout << "Making GIF...";
                    os4 << "c:\\screencap\\convert -delay 100 -loop 0 c:\\screencap\\*.bmp c:\\screencap\\output.gif";
                    s = os4.str();
                    system(s.c_str());
                    #endif // MAKE_GIF
                #endif // SIM_SHOWDEVICE

                #if SOUNDCAPTURE > 0
                    std::cout << "Converting sound, please wait...\n";
                    //system("del c:\\screencap\\output.wav");
                    //frcorr = (float)POK_AUD_FREQ;// * ((float)audiotime/(float)videotime);
                    //fr = (int)frcorr;
                    s.clear();
                    os2 << "c:\\screencap\\ffmpeg -f u8 -ar " << realfreq << " -ac 1 -i c:\\screencap\\soundcapture.raw c:\\screencap\\output.wav";
                    s = os2.str();
                    system(s.c_str());
                    fclose(soundfile);
                    writeWav("c:\\screencap\\soundcapture.raw", "c:\\screencap\\soundcapture.wav", realfreq);

                    /** video and sound joined here **/
                    system("c:\\screencap\\ffmpeg -i c:\\screencap\\output.avi -i c:\\screencap\\output.wav -shortest -c:v copy -c:a aac -strict experimental c:\\screencap\\output.mp4");
                #else
                    /** only video **/
                    system("c:\\screencap\\ffmpeg -i c:\\screencap\\output.avi -c:v copy -strict experimental c:\\screencap\\output.mp4");
                #endif // SOUNDCAPTURE
                PromptForChar( "Press any key...", type );
                std::cout << "Deleting temporary files\n";

                system("del c:\\screencap\\*.bmp");
                system("del c:\\screencap\\soundcapture.raw");
                system("del c:\\screencap\\output.wav");
                //system("del c:\\screencap\\output.avi");
                //system("del c:\\screencap\\output2.avi");
                //system("explorer c:\\screencap");
                break;
            }

            if (type == 'n' || type ==  'N')
            {
                std::cout << "Deleting temporary files\n";
                system("del c:\\screencap\\*.bmp");
                system("del c:\\screencap\\soundcapture.raw");
                system("del c:\\screencap\\output.wav");
                system("del c:\\screencap\\output.avi");
                break;
            }
        }
    #endif // SCREENCAPTURE
    cleaned = true; // prevent running cleanup many times
}


void simAudioCallback(void* userdata, uint8_t* stream, int len) {
  uint8_t* buf = (uint8_t*)stream;
  unsigned long j =0;
  Pokitto::Sound s;
  /** create sound buffer by using the ISR **/
  for (j=0;j<wanted.samples;j++) {

        /** Move outputted sound to output buffer **/
        if (sound_on == false) soundbyte = 0;
        else fakeISR(); /** create sample **/
        *buf++ = ((soundbyte*(s.getVolume()>>GLOBVOL_SHIFT))/16);
        #if SOUNDCAPTURE > 0
        soundfilebuffer[activesfbuf][sfbufindex++] = soundbyte;
        if (sfbufindex == SFBUFSIZE) {
            activesfbuf = 1-activesfbuf; // toggle other buffer
            sfbufindex =0;
        }
        #endif // SOUNDCAPTURE
    }
  return;
}

int Simulator::initSDLAudio() {
    #if SOUNDCAPTURE > 0
    soundfile = fopen("c:\\screencap\\soundcapture.raw", "wb");
    #endif // SOUNDCAPTURE

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_memset(&wanted, 0, sizeof(wanted)); /* or SDL_zero(want) */
    wanted.freq = SAMPLE_RATE;
    wanted.format = AUDIO_U8;
    wanted.channels = 1;
    wanted.samples = NUMFRAMES;
    wanted.callback = simAudioCallback;
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &wanted, &got, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (audioDevice == 0) {
        printf("Failed to open audio: %s\n", SDL_GetError());
        return 0;
    } else {
        if (got.format != wanted.format) { // we let this one thing change.
            printf("We didn't get the right audio format.\n");
        return 0;
        }
    }
    aud_len = got.freq * 5; /* 5 seconds */
    aud_position = 0;
    aud_frequency = 1.0 * SAMPLE_RATE / got.freq; /* 1.0 to make it a float */
    aud_volume = 255/10; /* ~1/5 max volume */

    SDL_PauseAudioDevice(audioDevice, 0); /* play! */
    return 1;
}

void Simulator::simSoundEnabled(int v) {
    if (v) sound_on=true;
    else sound_on=false;
}

int Simulator::simSoundIsOn() {
    return sound_on;
}



//** EOF *//
