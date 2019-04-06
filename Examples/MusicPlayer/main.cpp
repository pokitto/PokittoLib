#include "Pokitto.h"
#include "HWSound.h"
#include "gfx.h"
#include "snd.h"
#include "tune.h"

Pokitto::Core mygame;
Pokitto::Display d;
#include "drawing.h"

const uint16_t* currentPal;

/**********[Sound]*********************************************************/
bool playMusic = true;
int pattern=0;
int currentLine=0;
bool hasJumped = false;

Ticker sounder;

const int note_speed[]={
16,17,18,19,20,21,23,24,25,27,29,30, // C, C#, D, D#, E, F, F#, G, G#, A, A#, B,
32,34,36,38,40,43,45,48,51,54,57,60,
64,68,72,76,81,85,91,96,102,108,114,121,
128,136,144,152,161,171,181,192,203,215,228,242,
256,271,287,304,323,342,362,384,406,431,456,483,
512,542,575,609,645,683,724,767,813,861,912,967,
1024,1085,1149,1218,1290,1367,1448,1534,1625,1722,1825,1933,
2048,2170,2299,2435,2580,2734,2896,3068,3251,3444,3649,3866,
4096,4339,4598,4871,5161,5467,5793,6137,6502,6889,7298,7732
};

// new playsound function
uint8_t playSound(int channel, const unsigned char *sound, int volume = 255, int speed=255, int offset=0){
    int dataOffset = 14 + offset;


    // get sound length
    uint32_t soundSize = 0;
    for(int t=0; t<4; t++){
        soundSize <<= 8;
        soundSize |= sound[t] & 0xFF;
    }

    int sampleRate = 0;
    for(int t=0; t<2; t++){
        sampleRate <<= 8;
        sampleRate |= sound[t+4] & 0xFF;
    }

    // get repeat start
    int repeatStart = 0;
    for(int t=0; t<4; t++){
        repeatStart <<= 8;
        repeatStart |= sound[t+6] & 0xFF;
    }

    // get repeat end
    int repeatEnd = 0;
    for(int t=0; t<4; t++){
        repeatEnd <<= 8;
        repeatEnd |= sound[t+10] & 0xFF;
    }

    // cheat, if there is a looping sample, make the size the length of the loop
    if(repeatEnd != 0){
        soundSize = repeatEnd;
        // also cheat for start loop
        if(repeatStart == 0){
           repeatStart = 1;
        }
    }

    float spd = (POK_AUD_FREQ / (float)sampleRate);

    Pokitto::snd[channel].currentSound = &sound[dataOffset];          // sound to play
    Pokitto::snd[channel].volume = volume;               // volume
    Pokitto::snd[channel].speed = (speed/spd);           // recalculated above
    Pokitto::snd[channel].currentSoundSize = soundSize;  // length of sound array
    Pokitto::snd[channel].soundPoint = 0;                // where the current sound is upto
    Pokitto::snd[channel].repeat = repeatStart * 256 / Pokitto::snd[channel].speed;          // repeat point
    Pokitto::snd[channel].playSample = 1;                // trigger sample playing

    return channel;
}

void update_tune(){

    if(playMusic==true){

        char pat = my_pattern[pattern];

        uint8_t special = 0;
        uint8_t specialValue = 0;
        signed jump = -1;
        for(int t=0; t<4; t++){
            int note =       tune[pat][currentLine][t][0] >> 4;
            int octave =     tune[pat][currentLine][t][0] & 15;
            int volume =     tune[pat][currentLine][t][1];// max volume from xm/mod was 64
            int instrument = tune[pat][currentLine][t][2];
            special =            tune[pat][currentLine][t][3];
            specialValue =       tune[pat][currentLine][t][4];
            int offset = 0;

            // if valid volume change, change it, if not use previous
            if(volume >=128){
                volume -=128;
                volume *= 4;
            }else{
                // don't change volume
                volume = Pokitto::snd[t].volume;
            }

            if(special == 0x00){ // 00 = arpeggio
                // can't be implemented because of my timing method
                // I'm changing the timer instead of counting the ticks.
            }

            if(special == 0x01){ // 01 = portamento up
                Pokitto::snd[t].speed += specialValue;
            }

            if(special == 0x02){ // 02 = portamento down
                Pokitto::snd[t].speed -= specialValue;
            }

            if(special == 0x03){ // 03 = portamento slide from oldNote
                // not a clue what this does or how wrong it is.
                //Pokitto::snd[t].speed = (oldNote<<8) / specialValue;
            }


            if(special == 0x09){ // 09 = sample offset
                offset = specialValue * 256;
            }


            if(special == 0x0A){ // A = volume up/down
                if(specialValue >15){
                    volume += (specialValue >> 4);
                }else{
                    volume -= (specialValue & 0x0F);
                }
            }

            if(special == 0x0B){ // position jump
                if(hasJumped == false){
                    hasJumped= true;
                    currentLine=specialValue;
                }
            }

            if(special == 0x0C){ // set volume
                volume = specialValue*4;
            }

            if(special == 0x0D){
                currentLine=specialValue;
                if(++pattern>=sizeof(my_pattern))pattern=0;
            }


            if(special == 0x0F){
                // F in MOD format. 0.02 = 1 'tick'
                // any value below 32 changes the number of 'ticks' per note
                if(specialValue <32){
                    sounder.detach();
                    sounder.attach(&update_tune, 0.02 * specialValue); // speed of song
                }else{
                    // any value above 32 sets beats per minute
                    sounder.detach();
                    sounder.attach(&update_tune, 10.0 / specialValue); // speed of song
                }
            }

            if(volume > 255) volume = 255;

            if(note > 0){
                int speed = note_speed[note-1];
                for(int s=1; s<octave; s++){
                    speed *=2;
                }

                switch(instrument){
                    case 1: playSound(t, s_01, volume, speed, offset);  break;
                    case 2: playSound(t, s_02, volume, speed, offset);  break;
                    case 3: playSound(t, s_03, volume, speed, offset);  break;
                    case 4: playSound(t, s_04, volume, speed, offset);  break;
                    case 5: playSound(t, s_05, volume, speed, offset);  break;
                    case 6: playSound(t, s_06, volume, speed, offset);  break;
                    case 7: playSound(t, s_07, volume, speed, offset);  break;
                    case 8: playSound(t, s_08, volume, speed, offset);  break;
                    case 9: playSound(t, s_09, volume, speed, offset);  break;
                    case 10: playSound(t, s_10, volume, speed, offset);  break;
                    case 11: playSound(t, s_11, volume, speed, offset);  break;
                    case 12: playSound(t, s_12, volume, speed, offset);  break;
                    case 13: playSound(t, s_13, volume, speed, offset);  break;
                    case 14: playSound(t, s_14, volume, speed, offset);  break;
                    case 15: playSound(t, s_15, volume, speed, offset);  break;
                    case 16: playSound(t, s_16, volume, speed, offset);  break;
                    case 17: playSound(t, s_17, volume, speed, offset);  break;
                    case 18: playSound(t, s_18, volume, speed, offset);  break;
                    case 19: playSound(t, s_19, volume, speed, offset);  break;
                    case 20: playSound(t, s_20, volume, speed, offset);  break;
                    case 21: playSound(t, s_21, volume, speed, offset);  break;
                    case 22: playSound(t, s_22, volume, speed, offset);  break;
                    case 23: playSound(t, s_23, volume, speed, offset);  break;
                    case 24: playSound(t, s_24, volume, speed, offset);  break;
                    case 25: playSound(t, s_25, volume, speed, offset);  break;
                    case 26: playSound(t, s_26, volume, speed, offset);  break;
                    case 27: playSound(t, s_27, volume, speed, offset);  break;
                }
            }
//            oldNote[t] = speed;
            Pokitto::snd[t].volume = volume;
        }

        // patterns are 64 lines long, or until special code 20.
        if(currentLine++==63){
            currentLine=0;
            hasJumped = false;
            if(++pattern >= sizeof(my_pattern)){pattern=0;}
        }
    } // playMusic
}

/**************************************************************************/

// print text
void print(int16_t x, uint16_t y, const char* text){

    if(x==-1){
        int num=0;
        for(uint8_t t = 0; t < strlen(text); t++){
            num+=letter_widths[text[t]-32];
        }
        x = 110 - (num/2);

    }

    // 0 = 8bit
    // 1 = 4bit
    // 2 = 2bit
    for(int t=y/2; t<(y/2)+8; t++){
        mygame.display.scanType[t]=2; // 2bit
    }


    for(uint8_t t = 0; t < strlen(text); t++){
        uint8_t character = text[t];
        drawBitmap(x,y,largefont[character-32], 2);
        x += letter_widths[character-32];
    }
}

void drawPic(int16_t x, int16_t y, const uint8_t *bitmap, int16_t bpp){

    int16_t h = *(bitmap + 1); // second byte is height

    for(int t=y/2; t<(y+(h*2))/2; t++){
        mygame.display.scanType[t]=1; // 4bit
    }


    int y1 = y;
    for(int t=y; t<(y+h); t++){
        drawBitmapLines(x, y1++, t-y , 1, bitmap, bpp);
    }
}

int main ()
{
	mygame.begin();
    mygame.setFrameRate(255);
    d.persistence=1;

    // Init audio stream.
    pokPlayStream(); // activate stream
    mygame.sound.ampEnable(true);
    mygame.sound.playMusicStream();

    d.setInvisibleColor(0);
    load2bitPalette(tinyfont_pal,0,4);

    sounder.attach(&update_tune, 0.08); // default speed of song

	while (mygame.isRunning())
	{
		if (mygame.update())
		{

            print(0, 0, "Hello World!");


		}
	}
}

