#include "Pokitto.h"

extern uint8_t screenW, screenH, frameRate, fontW, fontH, screen, buttonRepeatFrame;
extern Pokitto::Core pok;


struct Tracker{
    private:
        uint16_t oplayerpos=255, oseqpos=0;
        uint8_t maxColumn = 3, maxRow = 64;
        int8_t colPointer = 0, rowPointer = 0;
        int8_t screenMaxInit = 20, screenMin = 0, screenMax = screenMaxInit, screenPointer = 0, playPointer = 0;
        int16_t bpm = 60, _tempo = 60;
        uint32_t playTime = 0;
        int8_t songPos = 0, lastPattern = 0, loopTo = 0, numPatches=0;
        uint8_t mode = 0; //mode0 = travel mode, mode1 = edit pitches and patches, mode2 = screen1 settings, mode3 = play, pause, stop
        int8_t edit = 0;
        uint8_t vLines[4] = {((fontW + 1) * 2) + 1, (((fontW + 1) * 9) + vLines[0]) + 1, (((fontW + 1) * 9) + vLines[1]) + 1, (((fontW + 1) * 9) + vLines[2]) + 1}; //(((fontW + 1) * number of chars) + left line) + 1
        uint8_t s_vLines[2] = {vLines[3] + 2, 0};
        int8_t _patch[30][64];
        int8_t _pitch[30][64];
        int8_t _songPos[10][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
        bool t1Mute = 0, t2Mute = 0, t3Mute = 0;
        char intToChar(int _int, uint8_t digit);
        uint8_t digitLength(int _int);
        int32_t extractNextInteger(char*);
        char* _textptr; //needed here because of scope

    public:
        void initStreams();
        void emptyPatches();
        int8_t _currpitch=34, _currpatch=1; // for faster editing and playNote();
        char patchnames[15][10];
        bool play = 0, stop = 0;
        bool checkButtons(); // returns true, if a button was pressed
        void playNote();
        void playPtn();
        void fillArrays();
        void drawLines();
        void drawPointer();
        void drawIsPlaying();
        void printValues();
        void printSettings();
        bool playTracker();
        int16_t getBPM();
        int minMax(int _val, int _min, int _max);
        void saveSong();
        int loadSong(char*);
        void filePutInt(int _int);
        void filePrint(const char *_string, uint8_t _size);
        void filePrint(char *_string, uint8_t _size);
        void NL();
        int8_t getLP(){ return lastPattern; }
};

extern Tracker tracker;
