#ifndef RBOYSOUNDSIM_H
#define RBOYSOUNDSIM_H

#define RBTRACKER_VERSION 0.03f

#include <Arduino.h>
#include "Rboy_sound.h"
#include "portaudio.h"

#define SAMPLE_RATE   (57000)
#define NUMFRAMES 570 //1 ms before refresh was 357

extern void fakeISR(); // was defined in Rboy_soundsim.h

extern PaTime streamTime;
extern PaTime getStreamTime();
extern PaTime firstSampleTime;
extern PaTime getFirstSampleTime();

#endif // RBOYSOUNDSIM

