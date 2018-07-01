/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */

#include "PokittoCore.h"
#include "PokittoCookie.h"

#define NUM_HIGHSCORES 5
#define NAME_SIZE 10
#define ENTRY_SIZE 15

/* Writing directly to EEPROM is deprecated */
/*
uint32_t highscores[NUM_HIGHSCORES] = {10000,7000,5000,4000,2000};
char names[NUM_HIGHSCORES][NAME_SIZE+1] = {"Crack Shot", "Defender", "Gunner", "We Tried", "No Hope"};
*/

/* This is how you do it with the PokittoCookie class !!!*/
class gdata : public Pokitto::Cookie {
    public:
    uint32_t highscores[NUM_HIGHSCORES];
    char names[NUM_HIGHSCORES][NAME_SIZE+1];
    int check_number;
    gdata() {
    }
};

gdata gamedata;

/* Auto-generated function declarations */
void setup();
void nextStage();
void nextLull();
void drawScore();
void drawTargets();
void drawCities();
void drawAmmo();
void drawMissiles();
void drawDetonations();
void launchMissile(uint8_t);
void tryLaunchEnemy();
void stepMissiles();
void stepDetonations();
void stepCollision();
void checkMenu();
void checkWin();
void checkLose();
void stepGame();
void drawLull();
void stepLull();
void stepDead();
void stepPregame();
void loop();
void loadHighscores();
uint8_t isHighscore(uint32_t);
void saveHighscore(uint32_t,char*);
void drawHighscores();
void initSound();
void playSound(uint8_t);


Pokitto::Core gb; // game object


int main() {
    setup();
    gb.display.setDefaultPalette();
    while (gb.isRunning()) {
        loop();
    }
}

#define WIDTH 110
#define HEIGHT 88
#define TEXT_WIDTH 6
#define TEXT_HEIGHT 8

const byte armageddon[] PROGMEM = {112,88,
B11111111,B11111111,B11111000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111111,B11111111,B11111100,
B11111111,B11111111,B11111000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111111,B11111111,B11111100,
B11111111,B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001111,B11111100,
B11111111,B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001111,B11111100,
B11111111,B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001111,B11111100,
B11111000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111100,
B11111000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111100,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B00000001,B10000001,B11100000,B00110000,B11000000,B11000000,B00111000,B00111111,B10011110,B00000111,B10000000,B00011000,B00011000,B01100000,
B00000001,B10000001,B11100000,B00110000,B11000000,B11000000,B00111000,B00111111,B10011110,B00000111,B10000000,B00011000,B00011000,B01100000,
B00000001,B10000001,B10011000,B00111001,B11000000,B11000000,B11000110,B00110000,B00011001,B10000110,B01100000,B01100110,B00011100,B01100000,
B00000001,B10000001,B10011000,B00111001,B11000000,B11000000,B11000110,B00110000,B00011001,B10000110,B01100000,B01100110,B00011100,B01100000,
B00000001,B10000001,B10011000,B00111001,B11000000,B11000000,B11000110,B00110000,B00011001,B10000110,B01100000,B01100110,B00011100,B01100000,
B00000110,B01100001,B11111000,B00111001,B11000011,B00110000,B11000000,B00111111,B00011000,B01100110,B00011001,B10000001,B10011011,B01100000,
B00000110,B01100001,B11111000,B00111001,B11000011,B00110000,B11000000,B00111111,B00011000,B01100110,B00011001,B10000001,B10011011,B01100000,
B00000111,B11100001,B10011000,B00110110,B11000011,B11110000,B11001110,B00110000,B00011000,B01100110,B00011001,B10000001,B10011000,B11100000,
B00000111,B11100001,B10011000,B00110110,B11000011,B11110000,B11001110,B00110000,B00011000,B01100110,B00011001,B10000001,B10011000,B11100000,
B00000111,B11100001,B10011000,B00110110,B11000011,B11110000,B11001110,B00110000,B00011000,B01100110,B00011001,B10000001,B10011000,B11100000,
B00000110,B01100001,B10011100,B00110000,B11000011,B00110000,B11000110,B00110000,B00011001,B10000110,B01100000,B01100110,B00011000,B01100000,
B00000110,B01100001,B10001110,B00110000,B11000011,B00110000,B11000110,B00110000,B00011001,B10000110,B01100000,B01100110,B00011000,B01100000,
B00011110,B01111001,B10000110,B00110000,B11001111,B00111100,B00111000,B00111111,B10011110,B00000111,B10000000,B00011000,B00011000,B01100000,
B00011110,B01111001,B10000110,B00110000,B11001111,B00111100,B00111000,B00111111,B10011110,B00000111,B10000000,B00011000,B00011000,B01100000,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001100,
B11100000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011100,
B11100000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011100,
B11111000,B00000000,B00000000,B00000000,B01111111,B10000000,B00000000,B00000000,B00000111,B11111000,B00000000,B00000000,B00000000,B01111100,
B11111000,B00000000,B00000000,B00000000,B01111111,B10000000,B00000000,B00000000,B00000111,B11111000,B00000000,B00000000,B00000000,B01111100,
B11111000,B00000000,B00000000,B00000000,B01111111,B10000000,B00000000,B00000000,B00000111,B11111000,B00000000,B00000000,B00000000,B01111100,
B11111111,B10000000,B00000000,B00111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11110000,B00000000,B00000111,B11111100,
B11111111,B10000000,B00000000,B00111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11110000,B00000000,B00000111,B11111100,
B11111111,B11110000,B00000111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B10000000,B00111111,B11111100,
B11111111,B11110000,B00000111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B10000000,B00111111,B11111100,
B11111111,B11110000,B00000111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B10000000,B00111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11000000,B00001111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11000000,B00001111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11000000,B00001111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11000000,B00001111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11000000,B00001111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000000,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000000,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B11000000,B00001111,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B11000000,B00001111,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B11000000,B00001111,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111110,B01111111,B11111111,B00000000,B00000011,B11111111,B11111001,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B00000000,B00000011,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B00000000,B00000011,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B10000000,B11111111,B00000000,B00000011,B11111100,B00000111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000011,B00000000,B00000011,B00000011,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000011,B00000000,B00000011,B00000011,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B11111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111000,B00000000,B00000000,B01111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111000,B00000000,B00000000,B01111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111000,B00000000,B00000000,B01111111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11100000,B00000000,B00000000,B00011111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B11100000,B00000000,B00000000,B00011111,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000000,B00000000,B00000011,B11111111,B11111111,B11111111,B11111111,B11111100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000000,B00000000,B00000011,B11010101,B01010100,B01000111,B00010001,B01000100,
B11111111,B11111111,B11111111,B11111111,B11111111,B00000000,B00000000,B00000000,B00000011,B11010101,B01010101,B11011111,B11010101,B01110100,
B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B00000000,B00000000,B11000101,B01010100,B11001111,B00110101,B01101100,
B11111111,B11111111,B11111111,B11111111,B11111100,B00000000,B00000000,B00000000,B00000000,B11010100,B01000101,B11011111,B00010001,B01011100,
};

/*const byte city[] PROGMEM = {8,8,
B00000000,
B00000000,
B00000000,
B00000000,
B00100010,
B01110110,
B01111110,
B11111111,
};*/

const byte city[] PROGMEM = {8,8,
B00000000,
B00000000,
B00000000,
B00000000,
B01100100,
B01101110,
B01111111,
B11111111,
};

const byte deadcity[] PROGMEM = {8,8,
B00000000,
B00000000,
B00000000,
B00000000,
B00000000,
B00000000,
B00100000,
B01100010,
};

const byte launcher[] PROGMEM = {8,8,
B00011000,
B00011000,
B00011000,
B00100100,
B00100100,
B01000010,
B01000010,
B10000001,
};

const byte deadlauncher[] PROGMEM = {8,8,
B00000000,
B00000000,
B00000000,
B00000000,
B00000000,
B01100110,
B01011010,
B10000001,
};

#define SOUND_PLAUNCH 0
#define SOUND_ELAUNCH 1
#define SOUND_DETONATE 2
#define SOUND_SCORE 3
#define SOUND_DEAD 4
#define SOUND_LOSE 5

#define MODE_PREGAME 0
#define MODE_GAME 1
#define MODE_LULL 2
#define MODE_DEAD 3
#define MODE_POSTDEAD 4

#define TARGET_SPEED 3

#define LAUNCHER_ONE 1
#define LAUNCHER_TWO 2
#define MAX_PMISSILES 10
#define PSPEED 4
#define PRADIUS 7
#define EXPAND 0
#define SHRINK 1

#define MAX_EMISSILES 10
#define MAX_CHANCE 50

// State variables
uint8_t mode = 0;
uint8_t counter = 0;
uint8_t flash = 0;
uint8_t stage = 0; //Maximum of 255 stages
uint32_t score = 0; //Score needs more space than other vars

uint32_t lullMissiles = 0;
uint8_t lullCities[8] = {0,0,0,0,0,0,0,0};

uint8_t cities[8] = {1,1,1,1,1,1,1,1}; //Whether the cities or launchers are alive

uint8_t targetX = WIDTH/2;
uint8_t targetY = HEIGHT/2;
uint8_t pammo[2] = {10,10};
uint8_t pDests[MAX_PMISSILES][2] = {{100,100},{100,100},{100,100},{100,100},{100,100},{100,100},{100,100},{100,100},{100,100},{100,100}};
float pMissiles[MAX_PMISSILES][3] = {{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0},{100,100,0}};
uint8_t pDetonations[MAX_PMISSILES][4] = {{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0},{100,100,0,0}};

uint8_t etotal = 5;
uint8_t echance = 1;
float espeed = 0.2;
uint8_t eDests[MAX_EMISSILES] = {100,100,100,100,100,100,100,100,100,100};
float eMissiles[MAX_EMISSILES][4] = {{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100},{100,100,100,100}};

void doTitle(uint8_t is_pause){
  while (gb.isRunning()) {
    if( gb.update() ){
      gb.display.setColor(7);
      gb.display.drawBitmap(0,0,armageddon);
      if( counter%8 == 0 ){
        flash ^= 255;
      }

      if( flash ){
        gb.display.setColor(4);
        gb.display.cursorY = HEIGHT - TEXT_HEIGHT*2;
        if( is_pause ){
          gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3;
          gb.display.print(("PAUSED"));
        }else{
          gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3;
          gb.display.print(("PRESS \25"));
        }
      }
      counter++;
      if( gb.buttons.pressed(BTN_A) ){
        gb.display.setColor(WHITE);
        break;
      }
    }
  }
}

void setup() {
  loadHighscores(); // do this before gb.begin !!
  gb.begin();



  //We need to do extra Gamebuino-formatting-specific initialization if we don't use the title screen
  gb.display.setFont(font5x7);
  gb.display.fontSize = 1;
  gb.display.textWrap = false;
  gb.display.persistence = false;
  gb.battery.show = false;
  gb.display.setColor(BLACK);
  gb.display.setColorDepth(1);

  doTitle(0);
  gb.pickRandomSeed();
}

void nextStage(){
  stage++;
  //Reset cities (since we cleared them for the visual effect
  for( uint8_t i = 0; i < 8; i++ ){
    cities[i] = lullCities[i];
  }
  //Reset launchers
  cities[2] = 1;
  cities[5] = 1;
  //Reset missiles
  pammo[0] = 10;
  pammo[1] = 10;
  //Reset in-flight missiles
  for( uint8_t i = 0; i < 10; i++ ){
    pDests[i][0] = 127;
    pDetonations[i][0] = 127;
    eDests[i] = 127;
  }

  etotal = stage > 10 ? 20 : 10+stage; //Max of 20 missiles per stage, start at 10
  //echance = stage > 5 ? 5 : stage; //Max of 5/100 chance per frame
  espeed = stage > 18 ? 2 : 0.2+(stage*0.1); //Max speed of 2, up 0.1 per stage, starts at 0.2
}

void nextLull(){
  //Reset lullMissiles
  lullMissiles = 0;
  //Reset lullCities
  for( uint8_t i = 0; i < 8; i++ ){
    lullCities[i] = 0;
  }
}

void drawScore(){
  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3;
  gb.display.cursorY = 0;

  if( score < 100000 ){
    gb.display.print(0);
  }
  if( score < 10000 ){
    gb.display.print(0);
  }
  if( score < 1000 ){
    gb.display.print(0);
  }
  if( score < 100 ){
    gb.display.print(0);
  }
  if( score < 10 ){
    gb.display.print(0);
  }
  gb.display.print(score);
}

void drawTargets(){
  gb.display.drawFastHLine(targetX-1,targetY,3);
  gb.display.drawFastVLine(targetX,targetY-1,2);

  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
    //Check for a valid destination without a current detonation
    if( pDests[i][0] <= WIDTH && pDetonations[i][0] > WIDTH ){
      gb.display.drawPixel(pDests[i][0], pDests[i][1]);
    }
  }
}

void drawCities(){
  gb.display.setColor(4);
  //uint8_t alldead = 1;
  for(uint8_t i = 0; i < 8; i++){
      if( i == 2 || i == 5 ){
        if( cities[i] ){
          gb.display.drawBitmap(i*14+2,HEIGHT-8,launcher);
        }else{
          gb.display.drawBitmap(i*14+2,HEIGHT-8,deadlauncher);
        }
      }else{
        if( cities[i] ){
          //alldead = 0;
          gb.display.drawBitmap(i*14+2,HEIGHT-8,city);
        }else{
          gb.display.drawBitmap(i*14+2,HEIGHT-8,deadcity);
        }
      }
    }

    gb.display.setColor(1);
    /*
    if( alldead ){
      gb.display.cursorX = 84/2 - 5*3;
      gb.display.cursorY = 48/2 - 5;
      gb.display.print("THE END");
    }
    */
}

void drawAmmo(){
  gb.display.setColor(3);
  for( uint8_t i = 0; i < 2; i++ ){
    uint8_t xcoord = i == 0 ? 2*14 + 5 : 5*14 + 5;
    uint8_t ycoord = HEIGHT-1;
    if( cities[i*3+2] ){ //Is launcher alive?
      for( uint8_t j = 0; j < pammo[i]; j++ ){
        gb.display.drawPixel(xcoord,ycoord);
        if( xcoord % 2 == 0 ){
          xcoord--;
          ycoord--;
        }else{
          xcoord++;
        }
      }
    }
  }
  gb.display.setColor(1);
}

void drawMissiles(){
  //Player Missiles
  gb.display.setColor(3);
  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
    //Check for a valid destination without a current detonation
    if( pDests[i][0] <= WIDTH && pDetonations[i][0] > WIDTH ){
      if(pMissiles[i][2] == LAUNCHER_ONE){
        gb.display.drawLine(2*14 + 5,HEIGHT-8,pMissiles[i][0], pMissiles[i][1]);
      }else{
        gb.display.drawLine(5*14 + 6,HEIGHT-8,pMissiles[i][0], pMissiles[i][1]);
      }
    }
  }

  //Enemy Missiles
  gb.display.setColor(7);
  for(uint8_t i = 0; i < MAX_EMISSILES; i++){
    //Check for a valid destination
    if( eDests[i] <= WIDTH ){
        gb.display.drawLine(eMissiles[i][0],eMissiles[i][1],eMissiles[i][2], eMissiles[i][3]);
    }
  }
  gb.display.setColor(1);
}

uint8_t explosionColor = 0;
void drawDetonations(){
  gb.display.setColor(explosionColor+1);
  if(counter%2 == 0){
    explosionColor++;
    explosionColor%=7;
  }
  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
    if( pDetonations[i][0] <= WIDTH ){
      gb.display.fillCircle(pDetonations[i][0],pDetonations[i][1],pDetonations[i][2]);
    }
  }
  gb.display.setColor(1);
}

void launchMissile(uint8_t launcher){
  //If there is no free slot in pDests, do not launch
  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
    if( pDests[i][0] > WIDTH ){
      // Which launcher?             Alive?       Has ammo?
      if(launcher == LAUNCHER_ONE && cities[2] && pammo[0]){
        pDests[i][0] = targetX;
        pDests[i][1] = targetY;
        pMissiles[i][0] = 2*14 + 5; //X-coord of left launcher
        pMissiles[i][2] = LAUNCHER_ONE; //Launched from launcher one
        pammo[0]--;
        playSound(SOUND_PLAUNCH);
      }else if(launcher == LAUNCHER_TWO && cities[5] && pammo[1]){
        pDests[i][0] = targetX;
        pDests[i][1] = targetY;
        pMissiles[i][0] = 5*14 + 6; //X-coord of right launcher
        pMissiles[i][2] = LAUNCHER_TWO; //Launched from launcher two
        pammo[1]--;
        playSound(SOUND_PLAUNCH);
      }
      pMissiles[i][1] = HEIGHT-8; //Y-coord of both launchers
      break;
    }
  }
}

void tryLaunchEnemy(){
  uint8_t someActive = 0;

  if( etotal > 0 ){

    // Check whether there are any active missiles
    // If none are active, always spawn one to avoid
    // long pauses without any enemy missiles
    for(uint8_t i = 0; i < MAX_EMISSILES; i++){
      if( eDests[i] <= WIDTH ){
        someActive = 1;
        break;
      }
    }

    if( (!someActive || echance >= random(100)) ){ //echance of 100
      for(uint8_t i = 0; i < MAX_EMISSILES; i++){
        if( eDests[i] > WIDTH ){
          etotal--;
          eDests[i] = random(8); //Target one of the 6 cities or 2 launch sites TODO: Come back later if random's behavior changes
          eMissiles[i][0] = random(WIDTH); //Screen width //TODO: if random is supposed to be inclusive, add a -1
          eMissiles[i][1] = 0; //Top of screen
          eMissiles[i][2] = eMissiles[i][0]; //Start and end are same
          eMissiles[i][3] = 0; //Top of screen
          playSound(SOUND_ELAUNCH);
          break; //Only spawn one
        }
      }
    }

  }
}

void stepMissiles(){
  //Player Missiles
  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
    //Check for a valid destination without a current detonation
    if( pDests[i][0] <= WIDTH && pDetonations[i][0] > WIDTH ){
      //If the missile is close enough to the destination, detonate
      if( abs( int(pDests[i][0] - pMissiles[i][0]) ) < PSPEED && abs( int(pDests[i][1] - pMissiles[i][1]) ) < PSPEED ){
        pDetonations[i][0] = pDests[i][0];
        pDetonations[i][1] = pDests[i][1];
        pDetonations[i][2] = 0; //Start detonation at radius of 0
        pDetonations[i][3] = EXPAND; //Detonation is increasing in size
      //Otherwise, keep moving towards destination
      }else{
        float dir = atan2(pDests[i][1]-pMissiles[i][1], pDests[i][0]-pMissiles[i][0]);
        pMissiles[i][0] += PSPEED * cos(dir);
        pMissiles[i][1] += PSPEED * sin(dir);
      }
    }
  }

  //Enemy Missiles
  for(uint8_t i = 0; i < MAX_EMISSILES; i++){
    //Check for a valid destination
    if( eDests[i] <= WIDTH ){
       //If enemy missile is close enough to the destination, detonate
      if( abs( int((eDests[i]*14+6) - eMissiles[i][2]) ) < PSPEED && abs( int(HEIGHT-4 - eMissiles[i][3]) ) < PSPEED ){
        cities[eDests[i]] = 0; //Destroy city/launcher

        //If launcher, remove its ammo
        if( eDests[i] == 2 ){
          pammo[0] = 0;
        }
        if( eDests[i] == 5 ){
          pammo[1] = 0;
        }

        eDests[i] = 127; //Reset enemy missile
        playSound(SOUND_DEAD);
      //Otherwise, keep moving towards destination
      }else{
        float dir = atan2( HEIGHT-4 - eMissiles[i][3], (eDests[i]*14+6)-eMissiles[i][2] );
        eMissiles[i][2] += espeed * cos(dir);
        eMissiles[i][3] += espeed * sin(dir);
      }
    }
  }
}

void stepDetonations(){
  if( counter%2 == 0 ){
    for(uint8_t i = 0; i < MAX_PMISSILES; i++){
      if( pDetonations[i][0] <= WIDTH ){
        if( pDetonations[i][3] == EXPAND ){ //If detonation is expanding
          if( pDetonations[i][2] < PRADIUS ){
            pDetonations[i][2]++;
          }else{
            pDetonations[i][3] = SHRINK; //Start shrinking
          }
        }
        //Check this now instead of using else because it may have just
        //set to SHRINK; immediately checking will remove a delay at full
        //size.  If a delay is desired, this if can be switched to an else.
        if( pDetonations[i][3] == SHRINK ){ //If detonation is shrinking
          if( pDetonations[i][2] > 0 ){
            pDetonations[i][2]--;
          }else{
            pDetonations[i][0] = 127; //Detonation is complete, remove it
            pDests[i][0] = 127; //Remove this destination
          }
        }
      }
    }
  }
}

void stepCollision(){
  //Player Detonations with Enemy Missiles
  for(uint8_t i = 0; i < MAX_PMISSILES; i++){
      if( pDetonations[i][0] <= WIDTH ){
        for(uint8_t j = 0; j < MAX_EMISSILES; j++){
          //Add extra +1 to explosion radius so that collisions are more generous
          if( eDests[j] <= WIDTH && pDetonations[i][2] + 1 >= sqrt( (eMissiles[j][2]-pDetonations[i][0])*(eMissiles[j][2]-pDetonations[i][0]) + (eMissiles[j][3]-pDetonations[i][1])*(eMissiles[j][3]-pDetonations[i][1]) ) ){
            eDests[j] = 127; //Remove enemy missile
            score+=25;
            playSound(SOUND_DETONATE);
          }
        }
      }
  }
}


void checkMenu(){
  if( gb.buttons.pressed(BTN_C) ){
    doTitle(1);
  }
}

void checkWin(){
  if( etotal == 0 ){
    for(uint8_t i = 0; i < MAX_EMISSILES; i++){
      //Check for a valid destination
      if( eDests[i] <= WIDTH ){
        return;
      }
    }
    //If we get here, all enemy missiles are destroyed and they have no more
    nextLull();
    mode = MODE_LULL;
  }
}

void checkLose(){
  for(uint8_t i = 0; i < 8; i++){
    if( i != 2 && i != 5 ){
      if( cities[i] ){
        return; // A city remains alive
      }
    }
  }
  //If we get here, all cities are dead.  It is the end of days.
  counter = 0;
  mode = MODE_DEAD;
  playSound(SOUND_LOSE);
}

void stepGame(){
  //Player input
  if( gb.buttons.pressed(BTN_A) ){
    launchMissile(LAUNCHER_TWO);
  }
  if( gb.buttons.pressed(BTN_B) ){
    launchMissile(LAUNCHER_ONE);
  }

  if( gb.buttons.repeat(BTN_LEFT,1) ){
    targetX = targetX-TARGET_SPEED > 0 ? targetX-TARGET_SPEED : 0;
  }
  if( gb.buttons.repeat(BTN_RIGHT,1) ){
    targetX = targetX+TARGET_SPEED < WIDTH ? targetX+TARGET_SPEED : WIDTH;
  }
  if( gb.buttons.repeat(BTN_UP,1) ){
    targetY = targetY-TARGET_SPEED > 0 ? targetY-TARGET_SPEED : 0;
  }
  if( gb.buttons.repeat(BTN_DOWN,1) ){
    targetY = targetY+TARGET_SPEED < HEIGHT ? targetY+TARGET_SPEED : HEIGHT;
  }

  //Game logic
  tryLaunchEnemy();
  stepMissiles();
  stepDetonations();
  stepCollision();

  //Drawing
  drawScore();
  drawTargets();
  drawCities();
  drawAmmo();
  drawMissiles();
  drawDetonations();

  checkWin();
  checkLose();
}

void drawLull(){
  uint32_t cityCount = 0;
  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*6;
  gb.display.cursorY = HEIGHT/2 - TEXT_HEIGHT*3;
  gb.display.print(("BONUS POINTS"));

  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*8;
  gb.display.cursorY += TEXT_HEIGHT*2;
  gb.display.print(lullMissiles);

  gb.display.setColor(3);
  for(uint8_t i = 0; i < lullMissiles; i++){
    gb.display.drawPixel(WIDTH/2 - TEXT_WIDTH*6 + i*2,HEIGHT/2 - 3);
  }

  gb.display.setColor(1);
  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*8;
  gb.display.cursorY += TEXT_HEIGHT*2;
  for( uint8_t i = 0; i < 8; i++ ){
    if( lullCities[i] ) cityCount++;
  }
  gb.display.print(cityCount);

  gb.display.setColor(4);
  for(uint8_t i = 0; i < cityCount; i++){
    gb.display.drawBitmap(WIDTH/2 - TEXT_WIDTH*6 + i*9,HEIGHT/2+2, city);
  }

  gb.display.setColor(1);
  drawScore();
  drawCities();
  drawAmmo();
}

void stepLull(){

  if( counter % 4 == 0 && (pammo[0] > 0 || pammo[1] > 0) ){
    lullMissiles++;
    score+=10;
    if( pammo[0] > 0 ) pammo[0]--;
    else pammo[1]--;
    playSound(SOUND_SCORE);
  }

  //If we have already iterated through the missiles
  if( counter % 8 == 0 && pammo[0] == 0 && pammo[1] == 0 ){
    for( uint8_t i = 0; i < 9; i++ ){
      if( i == 8 ){ //We have iterated through all live cities
        nextStage();
        mode = MODE_GAME;
        return;
      }
      //If not a launcher and the city is alive
      if( i != 2 && i != 5 && cities[i] != 0 ){
        lullCities[i] = 1;
        score+=100;
        cities[i] = 0;
        playSound(SOUND_SCORE);
        break;
      }
    }
  }

  drawLull();
}

void stepDead(){
  gb.display.setColor(7);
  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3 - TEXT_WIDTH/2;;
  gb.display.cursorY = HEIGHT/2 - TEXT_HEIGHT;
  gb.display.print(("THE END"));
  gb.display.setColor(1);

  if( mode == MODE_DEAD && counter%20 == 0 ){
    mode = MODE_POSTDEAD;
  }else if( mode == MODE_POSTDEAD ){
    if( counter%8 == 0 ){
      flash ^= 255;
    }
    if( flash ){
      gb.display.setColor(4);
      gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3 - TEXT_WIDTH/2;
      gb.display.cursorY = HEIGHT - TEXT_HEIGHT*2;
      gb.display.print(("PRESS \25"));

      if( isHighscore(score) ){
        gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*6 - TEXT_WIDTH/2;
        gb.display.cursorY = HEIGHT - TEXT_HEIGHT*3;
        gb.display.print(("NEW HIGHSCORE"));
      }
      gb.display.setColor(1);
    }

    if( gb.buttons.pressed(BTN_A) ){
      if( isHighscore(score) ){
        char tmp_name[11] = "User";
        //gb.getDefaultName(tmp_name);
        gb.keyboard(tmp_name, 11);
        saveHighscore(score,tmp_name);
      }
      score = 0;
      mode = MODE_PREGAME;
    }
  }
}

void stepPregame(){
  drawHighscores();

  if( counter%8 == 0 ){
    flash ^= 255;
  }
  if( flash ){
    gb.display.setColor(4);
    gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*3;
    gb.display.cursorY = HEIGHT - TEXT_HEIGHT*2;
    gb.display.print(("PRESS \25"));
    gb.display.setColor(1);
  }

  if( gb.buttons.pressed(BTN_A) ){
    stage = 255;
    for( uint8_t i = 0; i < 8; i++ ){
      lullCities[i] = 1;
    }
    nextStage(); //Reset to stage 0
    mode = MODE_GAME;
  }
}

void loop() {
  if(gb.update()){

    switch( mode ){
      case MODE_GAME:
        stepGame();
        break;

      case MODE_LULL:
        stepLull();
        break;

      case MODE_DEAD:
      case MODE_POSTDEAD:
        stepDead();
        break;

      case MODE_PREGAME:
        stepPregame();
        break;
    }

    checkMenu();

    counter++;
  }
}


void loadHighscores(){

  uint32_t defdata[] = {10000,7000,5000,4000,2000};
  char defnames[NUM_HIGHSCORES][NAME_SIZE+1] = {"Crack Shot", "Defender", "Gunner", "We Tried", "No Hope"};
  // Initialize the cookie
  gamedata.begin("ArmageDD",gamedata);

  uint32_t allscores=0;
  for (int i=0;i<NUM_HIGHSCORES;i++) allscores += gamedata.highscores[i];

  if (gamedata.check_number != 12345) {
        //checknumber indicates that no highscores saved yet, so put in "defaults"
        for (int i=0;i<NUM_HIGHSCORES;i++) gamedata.highscores[i]=defdata[i];
        for (int i=0;i<NUM_HIGHSCORES;i++) strcpy(gamedata.names[i],defnames[i]);
        gamedata.check_number = 12345; // put the checking number
        gamedata.saveCookie();//save the default values
  }
  /*
  for( uint8_t entry = 0; entry < NUM_HIGHSCORES; entry++ ){
    for( uint8_t offset = 0; offset < ENTRY_SIZE; offset++ ){
      if( offset < NAME_SIZE ){
        names[entry][offset] = EEPROM.read( ENTRY_SIZE * entry + offset );
      }else{
        uint8_t* addr = (uint8_t*) &highscores[entry];
        addr+=offset-NAME_SIZE;
        *addr = EEPROM.read( ENTRY_SIZE * entry + offset );
      }
    }
  }
  */

}

uint8_t isHighscore(uint32_t score){
  if( score > gamedata.highscores[NUM_HIGHSCORES-1] ){
    return 1;
  }
  return 0;
}

void saveHighscore(uint32_t score, char* who){
  uint8_t found = 0;
  uint32_t tmp_score = 0;
  char tmp_name[10];
  for( uint8_t entry = 0; entry < NUM_HIGHSCORES; entry++ ){
    if( score > gamedata.highscores[entry] ){
      found = 1;
    }
    if( found ){
      tmp_score = gamedata.highscores[entry];
      strcpy(tmp_name,gamedata.names[entry]);

      gamedata.highscores[entry] = score;
      strcpy(gamedata.names[entry], who);

      score = tmp_score;
      strcpy(who, tmp_name);
    }
  }
  gamedata.saveCookie();
/*Writing to EEPROM directly is deprecated and not necessary
  for( uint8_t entry = 0; entry < NUM_HIGHSCORES; entry++ ){
    for( uint8_t offset = 0; offset < ENTRY_SIZE; offset++ ){
      if( offset < NAME_SIZE ){
        EEPROM.write( ENTRY_SIZE * entry + offset, names[entry][offset] );
      }else{
        uint8_t* addr = (uint8_t*) &highscores[entry];
        addr+=offset-NAME_SIZE;
        EEPROM.write( ENTRY_SIZE * entry + offset, *addr );
      }
    }
  }
  */
}

void drawHighscores(){
  gb.display.setColor(4);
  gb.display.cursorX = WIDTH/2 - TEXT_WIDTH*5;
  gb.display.cursorY = TEXT_HEIGHT;
  gb.display.print(("HIGHSCORES"));
  gb.display.setColor(1);

  for( uint8_t entry = 0; entry < NUM_HIGHSCORES; entry++ ){
    gb.display.cursorX = 0;
    gb.display.cursorY = TEXT_HEIGHT*3 + TEXT_HEIGHT*entry;
    gb.display.print(gamedata.names[entry]);
    gb.display.cursorX = WIDTH - TEXT_WIDTH*6;
    if( gamedata.highscores[entry] < 100000 ){
      gb.display.print(0);
    }
    if( gamedata.highscores[entry] < 10000 ){
      gb.display.print(0);
    }
    if( gamedata.highscores[entry] < 1000 ){
      gb.display.print(0);
    }
    if( gamedata.highscores[entry] < 100 ){
      gb.display.print(0);
    }
    if( gamedata.highscores[entry] < 10 ){
      gb.display.print(0);
    }
    gb.display.print(gamedata.highscores[entry]);
  }
}

// Wave, pitch, duration, arpeggio step duration, arpeggio step size
const int8_t sounds[][5] = {
  { 0, 20, 5, 1, 1 }, //Player launch
  { 0, 25, 5, 1, -1 }, //Enemy launch
  { 1, 10, 5, 1, -1 }, //Detonating enemy missile
  { 1, 10, 2, 0, 0 }, //Score pips
  { 1, 2, 10, 1, -1 }, //A city dies
  { 0, 20, 14, 3, -1 }, //Lose
};

void initSound(){
  gb.sound.command(CMD_VOLUME, 5, 0, 0);
  gb.sound.command(CMD_SLIDE, 0, 0, 0);
}

void playSound(uint8_t i){
  gb.sound.command(CMD_VOLUME, 5, 0, 0);
  gb.sound.command(CMD_SLIDE, 0, 0, 0);
  gb.sound.command(CMD_ARPEGGIO, sounds[i][3], sounds[i][4], 0);
  gb.sound.command(CMD_INSTRUMENT, sounds[i][0], 0, 0);
  gb.sound.playNote(sounds[i][1], sounds[i][2], 0);

  //gb.sound.playNote(20, 5, 0);
}


