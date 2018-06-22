#include <SPI.h>
#include <EEPROM.h>
#include <Gamebuino.h>
#define MAX_TIRS 20
#define MAX_TIRS_ENNEMIS 35
#define MAX_ENNEMIS 8
#define MAX_EXPLOSIONS 3
#define VARS_ENNEMIS 14
#define STARS 12
#define PLAYER_SPD1 3
#define PLAYER_SPD2 3
Gamebuino gb;

const unsigned int enn_destroy[] PROGMEM = {
  0x8045,0x8851,0x8241,0x538,0x0000};
const unsigned int enn_shot[] PROGMEM = {
  0x8005,0x884D,0x81C1,0x250,0x0000};
const unsigned int player_shot[] PROGMEM = {
  0x8005,0x8141,0x14C,0x154,0x0000};
const unsigned int player_super[] PROGMEM = {
  0x8005,0x81C1,0x164,0x164,0x164,0x0000};
const unsigned int enn_hit[] PROGMEM = {
  0x8005,0x81C1,0x108,0x0000};
const unsigned int player_destroy[] PROGMEM = {
  0x8045,0x8891,0x8241,0x608,0x0000};



const unsigned int p20[] PROGMEM = {
  0x8241,0x8005,0x25C,0x264,0x440,0x848,0x240,0x250,0x458,0x85C,0x0000};//Mélodie Win
const unsigned int p21[] PROGMEM = {
  0x8241,0x8005,0x15C,0x5FC,0x15C,0x1FC,0x15C,0x7FC,0x140,0x5FC,0x140,0x1FC,0x140,0x7FC,0x0000};//Percu Win

const unsigned int p22[] PROGMEM = {
  0x8241,0x8005,0x28C,0x288,0x280,0x278,0x870,0x0000};//Mélodie Lose
const unsigned int p23[] PROGMEM = {
  0x8241,0x8005,0x15C,0x1FC,0x158,0x1FC,0x150,0x1FC,0x148,0x1FC,0x840,0x0000};//Percu Lose

const unsigned int p00[] PROGMEM = {
  0x8005,0x8101,0x410,0x2FC,0x210,0x2FC,0x210,0x4FC,0x20C,0x4FC,0x210,0x4FC,0x218,0x2FC,0x420,0x2FC,0x220,0x4FC,0x220,0x2FC,0x418,0x2FC,0x220,0x2FC,0x220,0x428,0x000};//Mélodie Intro
const unsigned int p03[] PROGMEM = {
  0x8005,0x8101,0xC40,0x23C,0x240,0x63C,0x640,0x448,0xC50,0x23C,0x240,0x648,0x650,0x458,0x0000};//Percu Intro 
const unsigned int p01[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x0000};//Percu 1
const unsigned int p02[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x250,0x258,0x85C,0x870,0x0000};//Mélodie 1
const unsigned int p05[] PROGMEM = {
  0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000};//Percu 2
const unsigned int p04[] PROGMEM = {
  0x8005,0x8101,0x106C,0x664,0x65C,0x458,0x0000};//Mélodie 2
const unsigned int p07[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000};//Percu 3
const unsigned int p06[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x258,0x264,0x86C,0x870,0x0000};//Mélodie 3
const unsigned int p09[] PROGMEM = {
  0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x120,0x3FC,0x134,0x5FC,0x134,0x1FC,0x134,0x3FC,0x118,0x3FC,0x0000};//Percu 4
const unsigned int p08[] PROGMEM = {
  0x8005,0x8101,0x106C,0x664,0x66C,0x458,0x0000};//Mélodie 4
const unsigned int p11[] PROGMEM = {
  0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x134,0x5FC,0x134,0x1FC,0x13C,0x3FC,0x1EC,0x3FC,0x0000};//Percu 5
const unsigned int p10[] PROGMEM = {
  0x8005,0x8101,0xC5C,0x258,0x250,0x864,0x86C,0x0000};//Mélodie 5
const unsigned int p13[] PROGMEM = {
  0x8005,0x8101,0x140,0x3FC,0x140,0x3FC,0x13C,0x3FC,0x13C,0x3FC,0x148,0x3FC,0x148,0x3FC,0x15C,0x3FC,0x13C,0x3FC,0x0000};//Percu 6
const unsigned int p12[] PROGMEM = {
  0x8005,0x8101,0x870,0x46C,0x470,0x878,0x45C,0x48C,0x0000};//Mélodie 6

const unsigned int p15[] PROGMEM = {
  0x8005,0x8101,0x410,0x2FC,0x210,0x8FC,0x418,0x2FC,0x23C,0x8FC,0x0000};//Percu 2-1
const unsigned int p14[] PROGMEM = {
  0x8005,0x8101,0x640,0x650,0x45C,0x648,0x658,0x464,0x0000};//Mélodie 2-1
const unsigned int p17[] PROGMEM = {
  0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x434,0x2FC,0x234,0x8FC,0x0000};//Percu 2-2
const unsigned int p16[] PROGMEM = {
  0x8005,0x8101,0x65C,0x66C,0x478,0x664,0x66C,0x458,0x0000};//Mélodie 2-2
const unsigned int p19[] PROGMEM = {
  0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x440,0x2FC,0x240,0x8FC,0x0000};//Percu 2-3
const unsigned int p18[] PROGMEM = {
  0x8005,0x8101,0x65C,0x66C,0x478,0x66C,0x678,0x480,0x0000};//Mélodie 2-3


const uint16_t* const patternSet[] PROGMEM = {
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
PROGMEM  prog_uchar ennemiSet[]  = {
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
}

void loop(){
  if(gb.update()){




    if(!end_screen){

      if(!gb.sound.trackIsPlaying[1] && perso.wait_end < 10 && perso.vies > 0)
      {
        gb.sound.playTrack(track1,1);
        gb.sound.playTrack(track2,2);
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
  if(!retry) gb.titleScreen(F("   "), logo);

  highscore = EEPROM.read(0) & 0x00FF; //LSB
  highscore += (EEPROM.read(1) << 8) & 0xFF00; //MSB
  if(highscore > 60000){
    highscore = 0; 
  }

  gb.sound.changePatternSet(patternSet,1);
  gb.sound.changePatternSet(patternSet,2);
  gb.sound.playTrack(track1,1);
  gb.sound.playTrack(track2,2);


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
  nextennemi = pgm_read_byte_near(ennemiSet+VARS_ENNEMIS*readennemi);

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
  gb.display.print(s);
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
  gb.display.print(sl);
}










