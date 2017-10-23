#include "Pokitto.h"

/** the output holder **/
uint8_t fakeOCR2B;

/** Sound Variables **/

uint8_t data[NUMFRAMES];

/** SDL sound variables **/
#define FREQ POK_AUD_FREQ /* the frequency we want */

unsigned int audio_position; /* which sample we are up to */
int audio_len; /* how many samples left to play, stops when <= 0 */
float audio_frequency; /* audio frequency in cycles per sample */
float audio_volume; /* audio volume, 0 - ~32000 */

uint8_t amp=false;

SDL_AudioSpec want, have;
SDL_AudioDeviceID dev;

uint8_t soundon = false;

#ifdef SOUNDCAPTURE
FILE* soundfile;
uint8_t soundfilebuffer[2][SFBUFSIZE];
uint8_t activesfbuf=0, writesfbuf=0; uint16_t sfbufindex=0;
#endif // SOUNDCAPTURE

#if (POK_STREAMING_MUSIC==1)
    #define SPEAKER 3
    unsigned char buffers[4][BUFFER_SIZE];
    volatile int currentBuffer = 0, oldBuffer = 0;
    volatile int bufindex = 0, vol=1;
    volatile unsigned char * currentPtr;
    volatile unsigned char * endPtr;
    int8_t streamvol=3;
    uint32_t streamcounter=0;
    uint8_t streamstep=0;
#endif

 uint8_t pokAmpIsOn() {
    return 1-amp; // enabled is LOW, so return the inverse !!
 }

 void pokAmpEnable(uint8_t v) {
    if (v>1) v=1; // limit against funny values
    amp=1-v; // Because v=true=enabled, but LOW is the enable condition
 }

void dac_write(uint8_t value) {
    #if POK_USE_DAC > 0
    /*if (value & 1) SET_DAC0 else CLR_DAC0;
    value >>= 1;
    if (value & 1) SET_DAC1 else CLR_DAC1;
    value >>= 1;
    if (value & 1) SET_DAC2 else CLR_DAC2;
    value >>= 1;
    if (value & 1) SET_DAC3 else CLR_DAC3;
    value >>= 1;
    if (value & 1) SET_DAC4 else CLR_DAC4;
    value >>= 1;
    if (value & 1) SET_DAC5 else CLR_DAC5;
    value >>= 1;
    if (value & 1) SET_DAC6 else CLR_DAC6;
    value >>= 1;
    if (value & 1) SET_DAC7 else CLR_DAC7;*/
    #endif
}



/** SOUND SIMULATION ONLY FUNCTIONS **/


/** SDL AUDIO CALLBACK FUNTION **/
/* The audio function callback takes the following parameters:
       stream:  A pointer to the audio buffer to be filled
       len:     The length (in bytes) of the audio buffer
*/

void sdlAudioCallback(void* userdata, uint8_t* stream, int len) {
  uint8_t* buf = (uint8_t*)stream;
  unsigned long j =0;

  /** create sound buffer by using the ISR **/
  for (j=0;j<want.samples;j++) {
        /** create sample **/
        fakeISR();
        /** Move outputted sound to output buffer **/
        if (soundon == false) fakeOCR2B = 0;
        *buf++ = fakeOCR2B;
        #if SOUNDCAPTURE > 0
        soundfilebuffer[activesfbuf][sfbufindex++] = fakeOCR2B;
        if (sfbufindex == SFBUFSIZE) {
            activesfbuf = 1-activesfbuf; // toggle other buffer
            sfbufindex =0;
        }
        #endif // SOUNDCAPTURE
    }
  return;
}


/** SDL AUDIO SETUP FUNCTION **/
int initSDLAudio() {
SDL_InitSubSystem(SDL_INIT_AUDIO);
SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
want.freq = SAMPLE_RATE;
want.format = AUDIO_U8;
want.channels = 1;
want.samples = NUMFRAMES;
want.callback = sdlAudioCallback;
dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
if (dev == 0) {
    printf("Failed to open audio: %s\n", SDL_GetError());
    return 0;
} else {
    if (have.format != want.format) { // we let this one thing change.
        printf("We didn't get the right audio format.\n");
        return 0;
    }
}
  audio_len = have.freq * 5; /* 5 seconds */
  audio_position = 0;
  audio_frequency = 1.0 * FREQ / have.freq; /* 1.0 to make it a float */
  audio_volume = 255/10; /* ~1/5 max volume */

  SDL_PauseAudioDevice(dev, 0); /* play! */
  return 1;
}


/** SOUND INIT **/
void initSound() {

    #if SOUNDCAPTURE > 0
    soundfile = fopen("c:\\screencap\\soundcapture.raw", "wb");
    #endif // SOUNDCAPTURE

    #if POK_ENABLE_SYNTH
        emptySong();
        emptyOscillators();
    #endif
    //emptyOscillators();
    //emptyBlocks();
    //emptyPatches();
    #ifdef TEST_SOUND
        testOsc();
    #endif // TEST_SOUND
    int res=initSDLAudio();
    #if POK_USE_CONSOLE
    if(res) console.AddMessage(MSOURCE_SOUND,MSG_INIT_OK);
    else console.AddMessage(MSOURCE_SOUND,MSG_INIT_OK);
    #endif // POK_USE_CONSOLE
}

/** SOUND STREAMING **/

void update_SDAudioStream() {
    #if POK_STREAMING_MUSIC > 0
    if (oldBuffer != currentBuffer) {
        if (currentBuffer==1) fileReadBytes(&buffers[0][0],BUFFER_SIZE);
        else if (currentBuffer==2) fileReadBytes(&buffers[1][0],BUFFER_SIZE);
        else if (currentBuffer==3) fileReadBytes(&buffers[2][0],BUFFER_SIZE);
        else fileReadBytes(&buffers[3][0],BUFFER_SIZE);

        oldBuffer = currentBuffer;
        streamcounter += BUFFER_SIZE;
    } else return;
    if ( streamcounter > getFileLength() - (BUFFER_SIZE*6)) {
        streamcounter=0;
        fileRewind();
    }
    #endif
}



/** SOUND TERMINATION **/

void terminateSound() {
    SDL_CloseAudioDevice(dev);
    #if SOUNDCAPTURE
        fwrite(&soundfilebuffer[writesfbuf],1,SFBUFSIZE,soundfile); // write remaining buffer to disk
        fclose(soundfile);
    #endif // SOUNDCAPTURE
}


