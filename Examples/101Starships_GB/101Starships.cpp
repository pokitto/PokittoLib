/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */
//#include <SPI.h>
//#include <EEPROM.h>
//#include <Gamebuino.h>
#include "Pokitto.h"
#include "WString.h"
/* Auto-generated function declarations */
void updateAff();
void updateEndScreen();
void updateEnnemis();
void newTirEnnemi(int,int,int,int);
void updateTirsEnnemis();
void newExplosion(byte,byte);
void updateExplosions();
void updatePerso();
void initPerso();
void newTir(int,int,int);
void updateTirs();
void setup();
void loop();
void initGame(byte);
void updateStars(byte);
void displayText(byte,byte,byte);
void displayText(byte,byte);
void displayInt(byte,byte,byte);
void displayText(String, byte, byte, byte);
void displayText(String, byte, byte);
void displayInt(long, byte, byte, byte);
#define MAX_TIRS 20
#define MAX_TIRS_ENNEMIS 35
#define MAX_ENNEMIS 8
#define MAX_EXPLOSIONS 3
#define VARS_ENNEMIS 14
#define STARS 12
#define PLAYER_SPD1 3
#define PLAYER_SPD2 3
Pokitto::Core gb;

const uint16_t enn_destroy[] PROGMEM = {
  0x8045,0x8851,0x8241,0x538,0x0000};
const uint16_t enn_shot[] PROGMEM = {
  0x8005,0x884D,0x81C1,0x250,0x0000};
const uint16_t player_shot[] PROGMEM = {
  0x8005,0x8141,0x14C,0x154,0x0000};
const uint16_t player_super[] PROGMEM = {
  0x8005,0x81C1,0x164,0x164,0x164,0x0000};
const uint16_t enn_hit[] PROGMEM = {
  0x8005,0x81C1,0x108,0x0000};
const uint16_t player_destroy[] PROGMEM = {
  0x8045,0x8891,0x8241,0x608,0x0000};



const uint16_t p20[] PROGMEM = {
  0x8241,0x8005,0x25C,0x264,0x440,0x848,0x240,0x250,0x458,0x85C,0x0000};//Mélodie Win
const uint16_t p21[] PROGMEM = {
  0x8241,0x8005,0x15C,0x5FC,0x15C,0x1FC,0x15C,0x7FC,0x140,0x5FC,0x140,0x1FC,0x140,0x7FC,0x0000};//Percu Win

const uint16_t p22[] PROGMEM = {
  0x8241,0x8005,0x28C,0x288,0x280,0x278,0x870,0x0000};//Mélodie Lose
const uint16_t p23[] PROGMEM = {
  0x8241,0x8005,0x15C,0x1FC,0x158,0x1FC,0x150,0x1FC,0x148,0x1FC,0x840,0x0000};//Percu Lose

const uint16_t p00[] PROGMEM = {
  0x8005,0x8101,0x410,0x2FC,0x210,0x2FC,0x210,0x4FC,0x20C,0x4FC,0x210,0x4FC,0x218,0x2FC,0x420,0x2FC,0x220,0x4FC,0x220,0x2FC,0x418,0x2FC,0x220,0x2FC,0x220,0x428,0x000};//Mélodie Intro
const uint16_t p03[] PROGMEM = {
  0x8005,0x8101,0xC40,0x23C,0x240,0x63C,0x640,0x448,0xC50,0x23C,0x240,0x648,0x650,0x458,0x0000};//Percu Intro
const uint16_t p01[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x0000};//Percu 1
const uint16_t p02[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x250,0x258,0x85C,0x870,0x0000};//Mélodie 1
const uint16_t p05[] PROGMEM = {
  0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000};//Percu 2
const uint16_t p04[] PROGMEM = {
  0x8005,0x8101,0x106C,0x664,0x65C,0x458,0x0000};//Mélodie 2
const uint16_t p07[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000};//Percu 3
const uint16_t p06[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x258,0x264,0x86C,0x870,0x0000};//Mélodie 3
const uint16_t p09[] PROGMEM = {
  0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x120,0x3FC,0x134,0x5FC,0x134,0x1FC,0x134,0x3FC,0x118,0x3FC,0x0000};//Percu 4
const uint16_t p08[] PROGMEM = {
  0x8005,0x8101,0x106C,0x664,0x66C,0x458,0x0000};//Mélodie 4
const uint16_t p11[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x134,0x5FC,0x134,0x1FC,0x13C,0x3FC,0x1EC,0x3FC,0x0000};//Percu 5
const uint16_t p10[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x258,0x250,0x864,0x86C,0x0000};//Mélodie 5
const uint16_t p13[] PROGMEM = {
  0x8005,0x8101,0x140,0x3FC,0x140,0x3FC,0x13C,0x3FC,0x13C,0x3FC,0x148,0x3FC,0x148,0x3FC,0x15C,0x3FC,0x13C,0x3FC,0x0000};//Percu 6
const uint16_t p12[] PROGMEM = {
  0x8005,0x8101,0x870,0x46C,0x470,0x878,0x45C,0x48C,0x0000};//Mélodie 6

const uint16_t p15[] PROGMEM = {
  0x8005,0x8101,0x410,0x2FC,0x210,0x8FC,0x418,0x2FC,0x23C,0x8FC,0x0000};//Percu 2-1
const uint16_t p14[] PROGMEM = {
  0x8005,0x8101,0x640,0x650,0x45C,0x648,0x658,0x464,0x0000};//Mélodie 2-1
const uint16_t p17[] PROGMEM = {
  0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x434,0x2FC,0x234,0x8FC,0x0000};//Percu 2-2
const uint16_t p16[] PROGMEM = {
  0x8005,0x8101,0x65C,0x66C,0x478,0x664,0x66C,0x458,0x0000};//Mélodie 2-2
const uint16_t p19[] PROGMEM = {
  0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x440,0x2FC,0x240,0x8FC,0x0000};//Percu 2-3
const uint16_t p18[] PROGMEM = {
  0x8005,0x8101,0x65C,0x66C,0x478,0x66C,0x678,0x480,0x0000};//Mélodie 2-3


const uint16_t* patternSet[] PROGMEM = {
  p00,p01,p02,p03,p04,p05,p06,p07,p08,p09,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23};
const unsigned int track1[] PROGMEM = {
  0,2,4,6,8,2,4,10,12,14,16,14,18,14,16,0xFFFF};
const unsigned int track2[] PROGMEM = {
  3,1,5,7,9,1,5,11,13,15,17,15,19,15,17,0xFFFF};


const byte logo[] PROGMEM = {
  64,30,
  B00011110,B00111111,B11100000,B01111000,B00000000,B00000000,B00000111,B00000000,
  B00111110,B11111111,B11111000,B11111000,B00000000,B00000000,B00011100,B00000000,
  B01111110,B11111000,B11111001,B11111000,B11000000,B00000000,B00111111,B00000000,
  B11111110,B11111000,B11111011,B11111000,B01110011,B10000000,B00111111,B00000000,
  B11011110,B11111000,B11111011,B01111000,B01001000,B00111111,B00011100,B00000000,
  B00011110,B11111000,B11111000,B01111000,B01110011,B10001100,B00000111,B00111111,
  B00011110,B11111000,B11111000,B01111000,B11000000,B00011110,B00000000,B00001100,
  B00011110,B11111000,B11111000,B01111000,B00000000,B00011110,B00000000,B00011110,
  B00011110,B11111111,B11111000,B01111000,B00000000,B00001100,B00000000,B00011110,
  B00011110,B01111111,B11110000,B01111000,B00000000,B00111111,B00000000,B00001100,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00111111,
  B00000000,B00000000,B01100000,B00000000,B00000000,B11000000,B11000000,B00000000,
  B00000000,B00000000,B01100000,B00000000,B00000000,B11000000,B00000000,B00000000,
  B00000000,B00011110,B11110011,B11001111,B10011110,B11111000,B11011111,B00011110,
  B00000000,B00110000,B01100000,B01101110,B00110000,B11001100,B11011001,B10110000,
  B00000000,B00110000,B01100000,B01101100,B00110000,B11001100,B11011001,B10110000,
  B00000000,B00011100,B01100011,B11101100,B00011100,B11001100,B11011001,B10011100,
  B00000000,B00000110,B01100110,B01101100,B00000110,B11001100,B11011001,B10000110,
  B00000000,B00000110,B01100110,B01101100,B00000110,B11001100,B11011001,B10000110,
  B00000000,B00111100,B00110011,B11101100,B00111100,B11001100,B11011111,B00111100,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000100,B00000000,B00000100,
  B00000000,B00000000,B00000000,B00000000,B01110000,B00000100,B00000000,B00000100,
  B00000000,B00000000,B00010101,B01010100,B00100111,B01110101,B01000110,B01110110,
  B00000000,B00000000,B00011101,B11011100,B01000101,B01010101,B01000101,B01100100,
  B00000000,B00000000,B00011101,B11011101,B01110111,B01110101,B11010101,B01110111,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00010000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B01110000,B00000000,B00000000,
};

//define Player object
typedef struct{
  byte x;
  byte y;
  boolean destroy;
  byte cadencetir;
  byte repop;
  byte vies;
  byte wait_end;
  byte wait_gameover;
  byte supershot;
  byte supershot_nb;
}
Player;

typedef struct{
  boolean on;
  int x;
  int y;
  char xvit;
  char yvit;
}
Tir;

typedef struct{
  byte x1;
  byte x2;
  byte x3;
  byte y1;
  byte y2;
  byte y3;
  byte tot_frames;
}
EBezier;

typedef struct{
  byte nb;
  byte ang1;
  byte ang2;
  byte tot_frames;
}
ETirs;

typedef struct{
  boolean on;
  byte x;
  byte y;
  char vie;
  byte image;
  EBezier bez;
  byte bez_frame;
  ETirs tir;
  byte tirs_frame;
  boolean justhit;
}
Ennemi;

typedef struct{
  boolean on;
  int x;
  int y;
  char xvit;
  char yvit;
}
TirEnnemi;

typedef struct{
  byte on;
  char x;
  char y;
  byte frame;
}
Explosion;

Player perso;
Tir tirs[MAX_TIRS];
Ennemi ennemis[MAX_ENNEMIS];
TirEnnemi tirs_ennemis[MAX_TIRS_ENNEMIS];
Explosion explosions[MAX_EXPLOSIONS];
int nbframes = 0;
boolean end_screen = 0; //0 : jeu, 1 : game over
boolean pause = 0;
byte wait_end_screen = 0;
unsigned int bonus_end_screen = 0;
unsigned int score, scoreaff;
byte ships_destroyed;
byte star[STARS*2];
byte starspeed;
byte phase,checkpoint,phase_display;
boolean continu = 0;
int nextennemi;
byte readennemi;
byte totennemis = 101;
boolean EndGame;
int bossframe = 0;
byte boss_shots1 = 0, boss_shots2 = 0, boss_shots3 = 0;
unsigned int highscore;
boolean score_saved;
boolean new_highscore;
byte rank_letter;



//frame d'apparition, vie, image, bézier(7), tirs(4)
const unsigned char ennemiSet[]  = {
  5,2,2,190,145,90,125,140,125,130,1,17,18,200,//Premiers
  10,2,2,190,145,90,115,100,115,130,1,18,19,200,
  50,3,0,190,145,90,104,150,94,120,1,19,19,50,
  20,3,0,190,145,90,124,100,114,120,1,18,18,50,
  50,3,0,190,145,90,145,100,115,100,2,16,19,40,
  10,3,0,190,145,90,145,100,115,100,2,16,19,40,
  50,2,2,190,145,90,134,140,132,120,1,20,20,50,//Mur de 4
  2,2,2,190,145,90,110,100,112,120,1,16,16,50,
  30,3,0,190,145,90,115,110,115,120,1,17,17,50,
  2,3,0,190,145,90,129,134,129,120,1,19,19,50,
  50,3,0,190,145,90,130,120,114,100,3,15,20,25,//Vague de 2
  20,3,0,190,145,90,130,120,114,100,3,15,20,25,
  60,2,2,195,175,145,110,180,70,100,2,16,20,50,//4 Zig-Zag
  20,2,2,195,175,145,130,60,170,100,2,16,20,45,
  10,2,2,195,175,145,110,180,70,100,2,17,19,50,
  20,2,2,195,175,145,130,60,170,100,2,17,19,45,
  50,3,0,190,145,90,114,120,114,100,2,17,19,20,//Vague de 2
  20,3,0,190,145,90,114,120,114,100,2,17,19,20,
  34,3,0,190,145,90,120,140,135,90,3,17,23,40,//Vague de 3
  15,3,0,190,145,90,120,140,135,90,3,17,23,40,
  15,3,0,190,145,90,120,140,135,90,3,17,23,40,
  30,3,0,190,145,90,110,145,124,90,2,17,19,30,//Vague de 2
  15,3,0,190,145,90,140,95 ,115,90,2,17,19,30,
  80,2,2,200,175,125,120,165,85,90,4,15,21,40,//Vague de 3
  10,2,2,200,175,125,120,165,85,90,4,16,20,40,
  10,2,2,200,175,125,120,165,85,90,4,16,20,40,
  70,7,1,190,170,90,130,120,115,120,6,15,21,25,//Première pieuvre
  70,3,0,190,145,90,103,108,98,80,2,14,16,30,//2 haut, 2 bas
  10,3,0,190,145,90,103,108,98,80,2,14,16,30,
  2,3,0,190,145,90,136,131,141,80,2,20,22,30,
  10,3,0,190,145,90,136,131,141,80,2,22,20,30,
  60,2,2,190,165,120,125,80,150,80,2,15,21,20,//Vague de 6
  8,2,2,190,165,120,125,80,150,80,1,17,17,40,
  8,2,2,190,165,120,125,80,150,80,1,16,16,40,
  8,2,2,190,165,120,125,80,150,80,2,15,21,20,
  8,2,2,190,165,120,125,80,150,80,1,17,17,40,
  8,2,2,190,165,120,125,80,150,80,1,16,16,40,
  40,2,2,190,165,120,115,160,80,80,3,15,21,25,//Vague de 4
  10,2,2,190,165,120,115,160,80,80,3,16,20,25,
  10,2,2,190,165,120,115,160,80,80,3,16,20,25,
  10,2,2,190,165,120,115,160,80,80,3,15,21,25,
  40,3,0,190,145,90,120,100,110,90,3,13,19,25,//Vague de 3
  15,3,0,190,145,90,120,100,110,90,3,13,19,25,
  15,3,0,190,145,90,120,100,110,90,3,13,19,25,
  60,2,2,190,145,90,115,95,110,70,2,14,16,40,//2 Vagues de 2 rapides
  1,2,2,190,145,90,125,145,130,70,2,20,22,40,
  30,2,2,190,145,90,115,95,110,70,2,14,16,40,
  1,2,2,190,145,90,125,145,130,70,2,20,22,40,
  80,5,1,190,170,90,110,120,105,110,4,14,22,35,//2 pieuvres
  5,5,1,190,170,90,130,120,135,110,4,14,22,35,
  80,3,0,195,165,95,120,140,130,80,3,19,23,40,//3 haut
  15,3,0,195,165,95,120,140,130,80,3,19,23,40,
  15,3,0,195,165,95,120,140,130,80,3,17,21,40,
  10,3,0,195,165,95,120,100,110,80,3,13,17,40,//3 bas
  15,3,0,195,165,95,120,100,110,80,3,13,17,40,
  15,3,0,195,165,95,120,100,110,90,3,15,19,40,
  40,2,2,190,145,90,135,135,139,110,2,16,20,25,//Mur de 7
  2,2,2,190,145,90,125,125,127,110,2,16,20,25,
  2,2,2,190,145,90,115,115,113,110,2,16,20,25,
  2,2,2,190,145,90,105,105,101,110,2,16,20,25,
  20,2,2,190,145,90,130,130,128,110,2,16,20,25,
  2,2,2,190,145,90,120,120,120,110,2,16,20,25,
  2,2,2,190,145,90,110,110,112,110,2,16,20,25,
  70,7,1,190,170,90,130,120,115,120,6,15,21,25,//Pieuvre
  50,3,0,190,165,90,120,140,130,80,3,19,23,40,//3 haut
  15,3,0,190,165,90,120,140,130,80,3,19,23,40,
  15,3,0,190,165,90,120,140,130,80,3,17,21,40,
  10,3,0,190,165,90,120,100,110,80,3,13,17,40,//3 bas
  15,3,0,190,165,90,120,100,110,80,3,13,17,40,
  15,3,0,190,165,90,120,100,110,90,3,15,19,40,
  80,2,2,195,175,145,110,170,70,70,3,15,21,20,//6 Zig-Zag
  20,2,2,195,175,145,130,70,170,70,3,15,21,15,
  10,2,2,195,175,145,110,170,70,70,3,16,20,20,
  20,2,2,195,175,145,130,70,170,70,3,15,21,15,
  20,3,0,190,145,90,110,145,124,90,2,16,20,15,
  10,3,0,190,145,90,140,95,115,90,2,16,20,15,
  70,5,1,190,170,90,110,120,105,110,4,14,22,35,//3 Pieuvres
  5,5,1,190,170,90,130,120,135,110,4,14,22,35,
  30,7,1,190,170,90,120,120,120,80,6,15,21,25,
  100,2,2,190,145,90,118,93,113,80,3,13,17,40,//2 Vagues de 3 rapides
  1,2,2,190,145,90,125,150,130,80,3,19,23,40,
  20,2,2,190,145,90,118,93,113,80,3,13,17,40,
  1,2,2,190,145,90,125,150,130,80,3,19,23,40,
  20,2,2,190,145,90,118,93,113,80,3,13,17,40,
  1,2,2,190,145,90,125,150,130,80,3,19,23,40,
  60,3,0,190,145,90,130,140,128,140,3,20,22,25,//Mur de 4
  2,3,0,190,145,90,110,100,112,140,3,16,14,25,
  10,3,0,190,145,90,115,110,115,140,3,17,15,25,
  2,3,0,190,145,90,125,130,125,140,3,19,21,25,
  90,5,1,190,170,90,130,120,115,100,4,15,21,30,//Pieuvre rapide
  20,3,0,190,165,90,120,140,130,80,3,19,23,40,//3 haut
  10,3,0,190,165,90,120,140,130,80,3,19,23,40,
  10,3,0,190,165,90,120,140,130,80,3,17,21,40,
  10,3,0,190,165,90,120,100,110,80,3,13,17,40,//3 bas
  10,3,0,190,165,90,120,100,110,80,3,13,17,40,
  10,3,0,190,165,90,120,100,110,90,3,15,19,40,
  100,7,1,190,170,90,110,120,105,120,3,14,22,25,//2 pieuvres
  5,7,1,190,170,90,130,120,135,120,3,14,22,25,
  10,5,1,190,170,90,110,120,105,130,4,12,24,25,//2 pieuvres
  5,5,1,190,170,90,130,120,135,130,4,12,24,25,
  120,30,3,194,188,178,122,122,122,20,0,12,24,255//Boss
};





void setup()
{
  gb.begin();
  initGame(0);
  gb.display.setFont(font3x5);
}

void loop(){
  if(gb.update()){




    if(!end_screen){

      if(!gb.sound.trackIsPlaying[1] && perso.wait_end < 10 && perso.vies > 0)
      {
        gb.sound.playTrack((uint16_t*)track1,1);
        gb.sound.playTrack((uint16_t*)track2,2);
      }

      if(!pause){

        //displayInt(gb.getFreeRam(),0,0,4);


        updateStars(starspeed);
        if(nbframes < 15){
          starspeed = 2;
        }
        else{
          starspeed = 1;
        }
        updateEnnemis();
        updateTirsEnnemis();
        updatePerso();
        updateTirs();
        updateExplosions();
        updateAff();
        nbframes++;

        if((nbframes >= nextennemi - 20) && perso.vies > 0 && (phase == 0 && readennemi >= 2 || phase == 1 && readennemi >= 23 || phase == 2 && readennemi >= 48 || phase == 3 && readennemi >= 70 || phase == 4 && readennemi >= 96)){
          phase++;
          phase_display = 40;
          checkpoint = readennemi;
        }

        if(phase_display > 0){
          phase_display--;
          if(phase < 5){
            String disp_phase = "STAGE  /5";
            disp_phase = disp_phase.substring(0,(40-phase_display)/2);
            displayText(disp_phase,48,0);
            if(phase_display<28){
              displayInt(phase,72,0,1);
            }
            if(phase_display>22 && phase_display % 2 == 0){
              gb.sound.playTick();
            }
          }
          else{
            String disp_phase = "FINAL STAGE";
            disp_phase = disp_phase.substring(0,(40-phase_display)/2);
            displayText(disp_phase,40,0);
            if(phase_display>18 && phase_display % 2 == 0){
              gb.sound.playTick();
            }

          }
        }



        //pause the game if C is pressed
        if(gb.buttons.pressed(BTN_C) && perso.vies > 0 && perso.wait_end < 10 && perso.repop < 45){
          pause = 1;
          gb.sound.playCancel();
          gb.display.persistence = true;

          gb.display.setColor(WHITE);
          gb.display.fillRect(16, 0, 48, 10);
          gb.display.fillRect(0, 12, LCDWIDTH, 11);
          gb.display.fillRect(16, 29, LCDWIDTH-32, 12);
          gb.display.setColor(BLACK);


          displayText("PAUSED",16,0,2);

          displayText("Hint: stop shooting",0,12);
          displayText("to charge super shot!",0,18);
          displayText("\25/\27 Continue",20,29);
          displayText("\26 Title Screen",16,36);


        }

        if(perso.vies > 0 && perso.wait_end > 180 && end_screen == 0){
          end_screen = 1;
          wait_end_screen = 0;
          gb.sound.stopTrack(1);
          gb.sound.stopTrack(2);
          gb.sound.playPattern(p20,1);
          gb.sound.playPattern(p21,2);
        }
        if(perso.vies == 0 && perso.wait_gameover > 210 && end_screen == 0){
          end_screen = 1;
          wait_end_screen = 0;
          gb.sound.stopTrack(1);
          gb.sound.stopTrack(2);
          gb.sound.playPattern(p22,1);
          gb.sound.playPattern(p23,2);
        }
      }
      else{
        if(gb.buttons.pressed(BTN_C) || gb.buttons.pressed(BTN_A)){
          pause = 0;
          gb.sound.playOK();
          gb.display.persistence = false;
        }
        if(gb.buttons.pressed(BTN_B)){
          gb.display.persistence = false;
          gb.sound.stopTrack(1);
          gb.sound.stopTrack(2);
          gb.sound.playCancel();
          initGame(0);
        }
      }
    }
    else{
      updateEndScreen();
    }


  }
}



void initGame(byte retry){
  if(!retry) gb.titleScreen(("   "), logo);

// TodDo jonne  highscore = EEPROM.read(0) & 0x00FF; //LSB
//  highscore += (EEPROM.read(1) << 8) & 0xFF00; //MSB
  if(highscore > 60000){
    highscore = 0;
  }

  gb.sound.changePatternSet(patternSet,1);
  gb.sound.changePatternSet(patternSet,2);
  gb.sound.playTrack((uint16_t*)track1,1);
  gb.sound.playTrack((uint16_t*)track2,2);


  gb.pickRandomSeed();
  gb.battery.show = false;

  initPerso();
  nbframes = 0;
  score = 0;
  scoreaff = 0;
  ships_destroyed = 0;
  end_screen = 0;
  pause = 0;
  EndGame = 0;
  bossframe = 0;
  boss_shots1 = 0;
  boss_shots2 = 0;
  boss_shots3 = 0;
  starspeed = 1;
  phase = 0;
  checkpoint = 0;
  phase_display = 0;
  continu = 0;
  score_saved = 0;
  new_highscore = 0;


  for(byte j=0;j<MAX_TIRS;j++)
  {
    tirs[j].on = 0;
  }

  for(byte j=0;j<MAX_ENNEMIS;j++)
  {
    ennemis[j].on = 0;
  }
  for(byte j=0;j<MAX_TIRS_ENNEMIS;j++)
  {
    tirs_ennemis[j].on = 0;
  }
  for(byte j=0;j<MAX_EXPLOSIONS;j++)
  {
    explosions[j].on = 0;
  }
  readennemi = 0; //Modifier pour démarrer plus tard dans le jeu
  nextennemi = pgm_read_byte(ennemiSet+VARS_ENNEMIS*readennemi);

  for(byte j=0;j<STARS;j++){
    star[j*2] = 100+random(0,LCDWIDTH/STARS+3)+j*(LCDWIDTH/STARS);
    star[j*2+1] = random(0,LCDHEIGHT);
  }



}

void updateStars(byte dx){
  for(byte j=0;j<STARS;j++){
    star[j*2] -= dx;
    if(star[j*2] < 100){
      star[j*2] += LCDWIDTH;
      star[j*2+1] = random(0,LCDHEIGHT);
    }
    gb.display.drawPixel(star[j*2]-100, star[j*2+1]);
  }
}

void displayText(String s, byte x, byte y, byte t){
  gb.display.cursorX = x;
  gb.display.cursorY = y;
  gb.display.fontSize = t;
  char dumbo[50];
  s.toCharArray(dumbo,50);
  gb.display.print(dumbo);
}
void displayText(String s, byte x, byte y){
  displayText(s,x,y,1);
}
void displayInt(long l, byte Tx, byte Ty, byte fig){
  gb.display.fontSize = 1;
  String sl = String(l);
  while(sl.length() < fig){
    sl = "0" + sl;
  }
  gb.display.cursorX = Tx;
  gb.display.cursorY = Ty;
  //gb.display.print(sl);
  char dumbo[50];
  sl.toCharArray(dumbo,50);
  gb.display.print(dumbo);
}










const byte vaisseau[] PROGMEM = {8,5,
B11000000,
B01110000,
B01001000,
B01110000,
B11000000,
};

const byte vaisseauS[] PROGMEM = {8,5,
B00000000,
B11110000,
B01001000,
B11110000,
B00000000,
};

const byte vaisseauXS[] PROGMEM = {8,5,
B00000000,
B00000000,
B11111000,
B00000000,
B00000000,
};

const byte vaisseau2[] PROGMEM = {8,6,
B11111100,
B00110000,
B01111000,
B01111000,
B00110000,
B11111100,
};

const byte vaisseau3[] PROGMEM = {8,6,
B00011100,
B01110000,
B11111100,
B11111100,
B01110000,
B00011100,
};

const byte vaisseau4[] PROGMEM = {8,6,
B00011000,
B11111100,
B00111000,
B00111000,
B11111100,
B00011000,
};

const byte pt[] PROGMEM = {8,1,
B11000000,
};

const byte shot[] PROGMEM = {8,3,
B11100000,
B11100000,
B11100000,
};

const byte boss[] PROGMEM = {16,13,
B00100000,B00000000,
B01111111,B11100000,
B00101111,B11000000,
B00000111,B10000000,
B00110011,B00000000,
B00011111,B10000000,
B11111001,B10000000,
B00011111,B10000000,
B00110011,B00000000,
B00000111,B10000000,
B00101111,B11000000,
B01111111,B11100000,
B00100000,B00000000,
};


void updateAff(){
  gb.display.setColor(WHITE);
  gb.display.fillRect(0, 42, LCDWIDTH, 6);
  gb.display.setColor(BLACK);


  if(nbframes > 40){
    gb.display.fillRect(16, 44, 2*(perso.supershot-10), 3);

    if(score > scoreaff+50){
     scoreaff += 45;
    }
    if(score > scoreaff+25){
     scoreaff += 25;
    }
    if(score > scoreaff){
     scoreaff += min(5,score-scoreaff);
    }

    displayInt(scoreaff,60,43,6);

    gb.display.cursorY = 43;
    gb.display.cursorX = 8;
    gb.display.print(perso.vies);

    gb.display.drawBitmap(1,LCDHEIGHT-8,vaisseau,ROTCCW,NOFLIP);
  }
  else{
    displayText("HIGHSCORE:",1,43);
    displayInt(highscore,60,43,6);


  }

}
void updateEndScreen(){

  if(perso.vies == 0){
    displayText("GAME OVER",8,0,2);
  }
  else{
    displayText("COMPLETE!",8,0,2);
  }

  if(wait_end_screen == 0){
    bonus_end_screen = perso.vies*2000;
    if(continu == 1){
      bonus_end_screen = 0;
    }
  }

  if(bonus_end_screen == 0 && score_saved == 0){
    score_saved = 1;
    if(score > highscore){
      highscore = score;
//      EEPROM.write(0, highscore & 0x00FF); //LSB
//      EEPROM.write(1, (highscore >> 8) & 0x00FF); //MSB
      new_highscore = 1;
    }

    rank_letter = 0;
    unsigned int top = 38000;
    while(rank_letter < 10 && score < top){
      rank_letter++;
      top = top *0.75;
    }
  }





  displayText("Ships destroyed :",1,14);
  displayInt(ships_destroyed,72,14,3);


  if(perso.vies > 0){
    displayText("Remaining lives :",1,20);
    gb.display.cursorX = 80;
    gb.display.print(perso.vies);
    if(bonus_end_screen > 0){
      displayInt(bonus_end_screen,68,26,4);
    }

  }

  //Score Final
  if(!new_highscore || wait_end_screen <= 20){
    displayText("FINAL SCORE :",1,36);
  }
  else{
    if(wait_end_screen % 10 < 7){
      displayText("HIGHSCORE !",1,36);
    }
  }

  displayInt(score,60,36,6);



  if(bonus_end_screen == 0){
    gb.display.cursorX = 1;
    gb.display.cursorY = 30;
    gb.display.print("Rank :");
    gb.display.cursorX = 80;
    switch(rank_letter){
    case 0:
      gb.display.print('S');
      break;
    case 1:
      gb.display.cursorX = 76;
      gb.display.print("A+");
      break;
    case 2:
      gb.display.print('A');
      break;
    case 3:
      gb.display.cursorX = 76;
      gb.display.print("A-");
      break;
    case 4:
      gb.display.cursorX = 76;
      gb.display.print("B+");
      break;
    case 5:
      gb.display.print('B');
      break;
    case 6:
      gb.display.cursorX = 76;
      gb.display.print("B-");
      break;
    case 7:
      gb.display.cursorX = 76;
      gb.display.print("C+");
      break;
    case 8:
      gb.display.print('C');
      break;
    case 9:
      gb.display.cursorX = 76;
      gb.display.print("C-");
      break;
    default:
      gb.display.print('D');
      break;
    }
  }






  //Boutons
  if(bonus_end_screen == 0 && wait_end_screen > 20){
    if((wait_end_screen-20) % 60 < 30){
      displayText("\25 Retry",30,42);
    }
    else{
      displayText("\26/\27 Title Screen",12,42);
    }
  }

  if(bonus_end_screen == 0 && wait_end_screen > 20){
    if(gb.buttons.pressed(BTN_A)){
      gb.sound.playOK();
      initGame(1);
    }
    if(gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
      gb.sound.playCancel();
      initGame(0);
    }
  }


  if(bonus_end_screen > 0 && wait_end_screen > 20){
    int s = min(bonus_end_screen,97);
    bonus_end_screen -= s;
    gb.sound.playTick();
    score += s;
    wait_end_screen = 20;
  }

  wait_end_screen++;
  if(wait_end_screen > 80){
    wait_end_screen -= 60;
  }
}








void updateEnnemis()
{
  if(perso.vies > 0){
    EndGame = 0;
  }
  byte i = 0;
  float t = 0.0;
  while(i<MAX_ENNEMIS)
  {
    if(ennemis[i].on == 1){

      EndGame = 1;

      //Trajectoire
      ennemis[i].bez_frame++;
      if(ennemis[i].image == 3 && ennemis[i].bez_frame > ennemis[i].bez.tot_frames){
        ennemis[i].bez_frame = ennemis[i].bez.tot_frames;
      }
      t = ((float)ennemis[i].bez_frame)/((float)ennemis[i].bez.tot_frames);
      ennemis[i].x = (1.0 - t)*(1.0 - t)*ennemis[i].bez.x1 + 2*t*(1.0 - t)*ennemis[i].bez.x2 + t*t*ennemis[i].bez.x3;
      ennemis[i].y = (1.0 - t)*(1.0 - t)*ennemis[i].bez.y1 + 2*t*(1.0 - t)*ennemis[i].bez.y2 + t*t*ennemis[i].bez.y3;

      char x = ennemis[i].x - 100;
      char y = ennemis[i].y - 100;

      //Boss
      if(t >= 1 && ennemis[i].image == 3){
        if(boss_shots3 == 0){
          bossframe++;
        }
        boss_shots1++;
        boss_shots2++;
        int tir_v = 20;
        int tir_vx = 0;
        int tir_vy = 0;
        float tir_ang = 0;
        y = 21 + 16*sin(bossframe / 6.0);
        if(boss_shots1 >= 28){ //Tirs haut et bas
          if(perso.repop < 20){
            for(byte p = 0; p < 8; p++){
              if(y < 21){
                tir_ang = (215-13*p)* 0.0174;
              }
              else{
                tir_ang = (145+13*p)* 0.0174;
              }
              tir_vx = tir_v*cos(tir_ang);
              tir_vy = tir_v*sin(tir_ang);
              if(y < 21){
                newTirEnnemi(x-4,y+4,tir_vx,tir_vy);
              }
              else{
                newTirEnnemi(x-4,y-4,tir_vx,tir_vy);
              }
            }
            boss_shots1-= 28;
          }
        }
        if(boss_shots2 >= 100 && y < 24 && y > 18){
          boss_shots3 = 1;
        }
        if(boss_shots3 > 0){
          boss_shots1 = 0;
          boss_shots2 = 0;
          boss_shots3++;
          if(boss_shots3 % 9 == 0 && perso.repop < 20){
            for(byte p = 0; p < 9; p++){
              tir_ang = (112 + 17*p+3*(boss_shots3/9))* 0.0174;
              tir_vx = tir_v*cos(tir_ang);
              tir_vy = tir_v*sin(tir_ang);
              newTirEnnemi(x-3,y,tir_vx,tir_vy);
            }
          }
          if(boss_shots3 > 50){
            boss_shots3 = 0;
          }
        }


      }

      if(ennemis[i].justhit == 0){

        char min_x = -2; //Pour que les ennemis se fassent tirer dessus
        char max_x = 3;
        char min_y = -2;
        char max_y = 3;


        switch(ennemis[i].image)
        {
        case 2:
          {
            gb.display.drawBitmap(x-2,y-2,vaisseau4);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau4,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }
          }
          break;
        case 1:
          {
            gb.display.drawBitmap(x-2,y-2,vaisseau3);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau3,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }
          }
          break;
        case 3:
          {
            gb.display.drawBitmap(x-5,y-6,boss);
            min_x = -2; //Pour que les ennemis se fassent tirer dessus
            max_x = 3;
            min_y = -5;
            max_y = 5;
            if(gb.collideBitmapBitmap(x-5,y-6,boss,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
            }
          }
          break;
        default:
          {
            gb.display.drawBitmap(x-2,y-2,vaisseau2);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau2,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }

          }
        }





        //Détection des tirs du joueur
        for(int m = 0; m < MAX_TIRS; m = m+1)
        {
          if(ennemis[i].on == 1 && tirs[m].on == 1)
          {
            if(tirs[m].x/10 >= x + min_x-2 && tirs[m].y/10 >= y + min_y - 1 && tirs[m].x/10 <= x + max_x && tirs[m].y/10 <= y + max_y+1)
            {
              if(ennemis[i].vie > 0)
              {
                ennemis[i].vie -= 1;
                score += 50;
              }
              tirs[m].on = 0;
              ennemis[i].justhit = 1;
              gb.sound.playPattern(enn_hit,0);


            }


          }
        }

      }
      else{
        ennemis[i].justhit = 0;
      }


      //Tirs
      ennemis[i].tirs_frame++;
      if(ennemis[i].tirs_frame >= ennemis[i].tir.tot_frames)
      {
        ennemis[i].tirs_frame = 0;
        int tir_v = 17;
        int tir_vx = 0;
        int tir_vy = 0;
        float tir_ang = 0;
        float tir_coef = 0.0;
        for(int k = 0; k < ennemis[i].tir.nb; k++)
        {
          if(ennemis[i].tir.nb > 1){
            tir_coef = ((float) k)/((float) (ennemis[i].tir.nb - 1));
          }
          else{
            tir_coef = 0.5;
          }
          tir_ang = tir_coef * ennemis[i].tir.ang1*10 + (1.0 - tir_coef) * ennemis[i].tir.ang2*10;
          tir_ang = tir_ang * 0.0174;
          tir_vx = tir_v*cos(tir_ang);
          tir_vy = tir_v*sin(tir_ang);
          if(perso.repop < 20){ //L'ennemi ne tire que si le joueur est prêt
            newTirEnnemi(x,y,tir_vx,tir_vy);
            gb.sound.playPattern(enn_shot,0);
          }
        }

      }

      if(ennemis[i].vie <= 0){
        newExplosion(x, y);
        ships_destroyed++;
        gb.sound.playPattern(enn_destroy,0);
        switch(ennemis[i].image)
        {
        case 1:
          {
            score += 200;
          }
          break;
        case 3:
          {
            score += 1000;
          }
          break;
        default:
          {
            score += 100;
          }
        }
      }

      if(ennemis[i].vie <= 0 || (t > 1 && ennemis[i].image != 3)){
        ennemis[i].on = 0;
      }





    }
    i++;
  }







  //Nouvel ennemi !
  while(nbframes >= nextennemi && readennemi < totennemis && perso.vies > 0){
    //Déterminer l'espace libre
    byte i = 0;
    while(ennemis[i].on == 1)
    {
      i++;
    }
    if(i>=MAX_ENNEMIS){
      i = 0;
    }

    ennemis[i].on = 1;
    ennemis[i].x = 255;
    ennemis[i].y = 255;
    ennemis[i].bez_frame = 0;
    ennemis[i].tirs_frame = 0;

    ennemis[i].vie = pgm_read_byte(ennemiSet + 1 + VARS_ENNEMIS*readennemi);
    ennemis[i].image = pgm_read_byte(ennemiSet + 2 + VARS_ENNEMIS*readennemi);

    ennemis[i].bez.x1 = pgm_read_byte(ennemiSet + 3 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.x2 = pgm_read_byte(ennemiSet + 4 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.x3 = pgm_read_byte(ennemiSet + 5 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y1 = pgm_read_byte(ennemiSet + 6 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y2 = pgm_read_byte(ennemiSet + 7 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y3 = pgm_read_byte(ennemiSet + 8 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.tot_frames = pgm_read_byte(ennemiSet + 9 + VARS_ENNEMIS*readennemi);

    ennemis[i].tir.nb = pgm_read_byte(ennemiSet + 10 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.ang1 = pgm_read_byte(ennemiSet + 11 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.ang2 = pgm_read_byte(ennemiSet + 12 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.tot_frames = pgm_read_byte(ennemiSet + 13 + VARS_ENNEMIS*readennemi);


    readennemi += 1;
    nextennemi += pgm_read_byte(ennemiSet + VARS_ENNEMIS*readennemi);
  }
}







void newTirEnnemi(int x, int y, int vx, int vy)
{
  byte i = 0;
 while(tirs_ennemis[i].on == 1)
  {
 i++;
  }
  if(i>=MAX_TIRS_ENNEMIS){
    i = 0;
  }
  tirs_ennemis[i].on = 1;
  tirs_ennemis[i].x = x*10;
  tirs_ennemis[i].y = y*10;
  tirs_ennemis[i].xvit = vx;
  tirs_ennemis[i].yvit = vy;
}

void updateTirsEnnemis()
{
 byte i = 0;
char xdiff = 0;
char ydiff = 0;
 while(i<MAX_TIRS_ENNEMIS)
 {
   if(tirs_ennemis[i].on == 1){
     EndGame = 1;
     tirs_ennemis[i].x += tirs_ennemis[i].xvit;
     tirs_ennemis[i].y += tirs_ennemis[i].yvit;
     gb.display.drawBitmap(tirs_ennemis[i].x/10-1,tirs_ennemis[i].y/10-1,shot);


     if(perso.destroy == 0){
       xdiff = perso.x - tirs_ennemis[i].x/10;
       ydiff = perso.y - tirs_ennemis[i].y/10;
       if(xdiff <= 2 && xdiff >= -1 && ydiff <= 1 && ydiff >= -1 && perso.repop <= 0){
         perso.destroy = 1;
        gb.sound.playPattern(player_destroy,0);
       }
     }

     if(tirs_ennemis[i].x/10 < -2 || tirs_ennemis[i].x/10 > LCDWIDTH + 2 || tirs_ennemis[i].y/10 < -2 || tirs_ennemis[i].y/10 > LCDHEIGHT -6)
     {
       tirs_ennemis[i].on = 0;
     }
   }
    i++;
 }
}
void newExplosion(byte x, byte y)
{
    byte i = 0;
 while(explosions[i].on == 1)
  {
 i++;
  }
  if(i>=MAX_EXPLOSIONS){
    i = 0;
  }
  explosions[i].on = 1;
  explosions[i].x = x;
  explosions[i].y = y;
  explosions[i].frame = 0;
}

void updateExplosions()
{
 byte i = 0;
 while(i<MAX_EXPLOSIONS)
 {
   if(explosions[i].on == 1){

     switch(explosions[i].frame)
      {
        case 5: {
          gb.display.drawPixel(explosions[i].x, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+6, explosions[i].y+6);
          gb.display.drawPixel(explosions[i].x-6, explosions[i].y-6);
          gb.display.drawPixel(explosions[i].x+6, explosions[i].y-6);
          gb.display.drawPixel(explosions[i].x-6, explosions[i].y+6);
        }
        break;

        case 0: {
          gb.display.drawPixel(explosions[i].x, explosions[i].y);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
        }
        case 4 : {

           gb.display.drawPixel(explosions[i].x+3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y-3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y-3);

          gb.display.drawPixel(explosions[i].x+5, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x+5, explosions[i].y-5);
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y-5);


          gb.display.drawPixel(explosions[i].x-4, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y);

          gb.display.drawPixel(explosions[i].x, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-4);
        }
        break;
       case 1: {
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);

          gb.display.drawPixel(explosions[i].x-2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y);

          gb.display.drawPixel(explosions[i].x, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-3);

          gb.display.drawPixel(explosions[i].x+2, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y-2);
        }
        case 3: {
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+5, explosions[i].y);

          gb.display.drawPixel(explosions[i].x, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-5);

          gb.display.drawPixel(explosions[i].x+4, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y-4);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y-4);
        }
        break;
       case 2: {
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);

          gb.display.drawPixel(explosions[i].x-2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y);

          gb.display.drawPixel(explosions[i].x, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-4);

          gb.display.drawPixel(explosions[i].x+3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y-3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y-3);
        }
        break;
      }





     explosions[i].frame++;
     if(explosions[i].frame >= 6){
       explosions[i].on = 0;
     }




   }
    i++;
 }
}
void updatePerso(){

  boolean bt_right;
  boolean bt_left;
  boolean bt_up;
  boolean bt_down;


  if(perso.wait_end < 10){
    bt_right = gb.buttons.repeat(BTN_RIGHT, 1);
    bt_left = gb.buttons.repeat(BTN_LEFT, 1);
    bt_up = gb.buttons.repeat(BTN_UP, 1);
    bt_down = gb.buttons.repeat(BTN_DOWN, 1);
  }
  else{  //Prise en main des contrôles pour la fin de partie
    bt_right = 0;
    bt_left = 0;
    bt_up = 0;
    bt_down = 0;
    if(perso.y > 23){
      bt_up = 1;
    }
    if(perso.y < 20){
      bt_down = 1;
    }
    if(perso.x > 11){
      bt_left = 1;
    }
    if(perso.x < 8){
      bt_right = 1;
    }
    if(perso.x >= 8 && perso.x <= 11){
      perso.x = 10;
    }
    if(perso.wait_end == 10){
      gb.sound.stopTrack(1);
      gb.sound.stopTrack(2);
    }
  }



  if(perso.destroy == 0){



    //Mouvement
    if(perso.wait_end < 50){
      if(bt_right && !bt_left){
        if(bt_down){
          perso.y += PLAYER_SPD2;
          perso.x += PLAYER_SPD2;
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD2);
          perso.x += PLAYER_SPD2;
        }
        else if(!bt_down){
          perso.x += PLAYER_SPD1;
        }
      }
      if(bt_left && !bt_right){
        if(bt_down){
          perso.y += PLAYER_SPD2;
          perso.x = max(0,perso.x-PLAYER_SPD2);
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD2);
          perso.x = max(0,perso.x-PLAYER_SPD2);
        }
        else if(!bt_down){
          perso.x = max(0,perso.x-PLAYER_SPD1);
        }
      }
      if((!bt_left && !bt_right) || (bt_left && bt_right)){
        if(bt_down){
          perso.y += PLAYER_SPD1;
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD1);
        }
      }

      if(perso.x < 2)
      {
        perso.x = 2;
      }
      if(perso.y < 2)
      {
        perso.y = 2;
      }
      if(perso.x > LCDWIDTH - 3)
      {
        perso.x = LCDWIDTH - 3;
      }
      if(perso.y > LCDHEIGHT - 9)
      {
        perso.y = LCDHEIGHT - 9;
      }
    }
    else{
      perso.x = min(100,10+85.0*((perso.wait_end - 50.0)/70.0)*((perso.wait_end - 50.0)/70.0));
      if(perso.x > 11 && perso.wait_end < 120){
        starspeed += 1 + (perso.x-11) / 10;
      }
      if(perso.wait_end >= 120){
        starspeed = max(0,5-(perso.wait_end - 120)/5);
      }
    }


    if(perso.repop >= 20){
      perso.y = LCDHEIGHT/2 - 3;
      float repop2 = ((((float)perso.repop) - 30.0)/15.0)*((((float)perso.repop) - 30.0)/15.0);
      perso.x = 25.0-30.0*repop2 ;
      if(perso.repop < 52){
        for(byte j=0;j<MAX_TIRS_ENNEMIS;j++)
        {
          tirs_ennemis[j].on = 0;
        }
      }

    }
    if(perso.repop > 0){
      perso.repop -= 1;
      perso.wait_end = 0;
    }

    byte xorig = perso.x-2;
    byte yorig = perso.y-2;
    if(perso.repop % 2 == 0 || (perso.vies == 3 && continu == 0)){
      if(perso.repop < 20 && perso.wait_end < 80){
        gb.display.drawBitmap(xorig,yorig,vaisseau);
      }
      if(perso.repop >= 20){
        if((perso.repop-20) % 8 < 2){
          gb.display.drawBitmap(xorig,yorig,vaisseau);
        }
        else{
          if((perso.repop-20) % 8 == 4 || (perso.repop-20) % 8 == 5){
            gb.display.drawBitmap(xorig,yorig,vaisseauXS);
          }
          else{
            gb.display.drawBitmap(xorig,yorig,vaisseauS);
          }
        }
      }

      if(perso.wait_end >= 80){
        if((perso.wait_end-80) % 8 < 2){
          gb.display.drawBitmap(xorig,yorig,vaisseau);
        }
        else{
          if((perso.wait_end-80) % 8 == 4 || (perso.wait_end-80) % 8 == 5){
            gb.display.drawBitmap(xorig,yorig,vaisseauXS);
          }
          else{
            gb.display.drawBitmap(xorig,yorig,vaisseauS);
          }
        }
      }
    }


    //Super Tir
    if(perso.repop > 0){
      perso.supershot = 0;
      perso.supershot_nb = 0;
    }
    if(perso.supershot == 30 && gb.buttons.repeat(BTN_A, 1) && perso.wait_end < 10){
      perso.supershot_nb = 5;
      perso.supershot = 0;
    }
    if(perso.supershot == 29 ){
      gb.sound.playPattern(player_super,0);
    }
    if(perso.supershot_nb == 5 || perso.supershot_nb == 3 || perso.supershot_nb == 1){
      newTir(45,-5,-1);
      newTir(45,5,1);
      newTir(55,0,0);
      newTir(50,0,-1);
      newTir(50,0,1);
    }
    if(perso.supershot_nb > 0){
      perso.supershot_nb -= 1;
      perso.supershot = 0;
    }

    //Tir normal, chargement du super tir
    if(gb.buttons.repeat(BTN_A, 1) && perso.cadencetir == 0 && perso.repop < 20 && perso.supershot_nb == 0 && perso.wait_end < 10){
      newTir(50,0,-1);
      newTir(50,0,1);
      perso.cadencetir = 6;
    }
    if(perso.cadencetir > 0){
      perso.cadencetir -= 1;
      perso.supershot = 0;
    }
    if(perso.cadencetir == 0 && perso.supershot < 30){
      perso.supershot += 1;
    }
    if(perso.supershot >= 10 && readennemi < totennemis){
      score += 5;
    }

  }
  else { //Si le joueur vient de se faire détuire
    perso.supershot = 0;
    if(perso.vies > 1){
      perso.repop = 60;
      perso.vies -= 1;
      perso.destroy = 0;
      newExplosion(perso.x, perso.y);
    }
    else{
      if(perso.vies != 0){
        newExplosion(perso.x, perso.y);
        perso.wait_gameover = 0;
      }
      perso.vies = 0;
      gb.sound.stopTrack(1);
      gb.sound.stopTrack(2);

    }
  }


  if(perso.vies == 0){
    perso.wait_gameover += 1;

    if(perso.wait_gameover >= 10){


      gb.display.setColor(WHITE);
      gb.display.fillRect(20, 10, LCDWIDTH-40, 10);
      gb.display.fillRect(20, 29, LCDWIDTH-40, 12);
      gb.display.setColor(BLACK);

      displayText("CONTINUE?",22,10,1);
      displayInt(min(9,(210-perso.wait_gameover)/20),60,10,1);
      displayText("\25/\27 Continue",20,29);
      displayText("\26 End game",24,36);
      if(perso.wait_gameover % 20 == 10){
        gb.sound.playTick();
      }
      if(gb.buttons.pressed(BTN_B)){
        perso.wait_gameover = 230;
        gb.sound.playCancel();
      }

      if(gb.buttons.pressed(BTN_C) || gb.buttons.pressed(BTN_A)){
        gb.sound.playOK();
        perso.vies = 3;
        perso.repop = 50;
        score = 0;
        scoreaff = 0;
        perso.destroy = 0;
        continu = 1;
        readennemi = checkpoint;
        nextennemi = nbframes + 50;
        phase = 0;
        ships_destroyed = 0;
        EndGame = 0;
        bossframe = 0;
        boss_shots1 = 0;
        boss_shots2 = 0;
        boss_shots3 = 0;

        for(byte j=0;j<MAX_ENNEMIS;j++)
        {
          ennemis[j].on = 0;
        }
        for(byte j=0;j<MAX_TIRS_ENNEMIS;j++)
        {
          tirs_ennemis[j].on = 0;
        }
        for(byte j=0;j<STARS;j++){
          star[j*2] = 100+random(0,LCDWIDTH/STARS+3)+j*(LCDWIDTH/STARS);
          star[j*2+1] = random(0,LCDHEIGHT);
        }
      }
    }


  }
  if(EndGame == 0 && readennemi >= totennemis){
    perso.wait_end += 1;

  }




}

void initPerso(){
  perso.x = 10;
  perso.y = LCDHEIGHT/2.0;
  perso.repop = 50;
  perso.vies = 3;
  perso.destroy = 0;
  perso.wait_end = 0;
  perso.wait_gameover = 0;
}







void newTir(int vx, int vy, int diffy)
{
  byte i = 0;
 while(tirs[i].on == 1)
  {
 i++;
  }
  if(i>=MAX_TIRS){
    i = 0;
  }
  tirs[i].on = 1;
  tirs[i].x = ((int)perso.x-1) *10;
  tirs[i].y = ((int)perso.y-diffy) *10+vy;
  tirs[i].xvit = vx;
  tirs[i].yvit = vy;
  gb.sound.playPattern(player_shot,0);
}

void updateTirs()
{
 byte i = 0;
 while(i<MAX_TIRS)
 {
   if(tirs[i].on == 1){
     tirs[i].x += tirs[i].xvit;
     tirs[i].y += tirs[i].yvit;
     gb.display.drawFastHLine(tirs[i].x/10, tirs[i].y/10, 3);
     if(tirs[i].x/10 > LCDWIDTH - 7 || tirs[i].y/10 < 0 || tirs[i].y/10 > LCDHEIGHT-1)
     {
       tirs[i].on = 0;
     }


   }
    i++;
 }
}
