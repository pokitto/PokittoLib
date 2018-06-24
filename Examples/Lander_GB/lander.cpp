/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */
//#include <SPI.h>
//#include <Gamebuino.h>
#include "Pokitto.h"
#include "WString.h"
/* Auto-generated function declarations */
void setup();
void loop();
void newgame();
void newlevel();
void newlife();
void showscore();
void nextlevelcheck();
void handledeath();
void checkbuttons();
void drawlandscape();
void drawspaceship();
void checkpickup();
void selectlevel();
void checkcollission();
void showgameover();
void showtitle();
void playsoundfx(int,int);
//Gamebuino gb;
Pokitto::Core gb;
//----------------------------------------------------------------------
//                               L A N D E R
//                              by Yoda Zhang
//----------------------------------------------------------------------

//----------------------------------------------------------------------------
// define variables and constants
//----------------------------------------------------------------------------
String gamestatus;
int score;
int highscore;
byte lives;
byte gamelevel;
byte screen;
byte maxlevel=9;
int shipx; // position x 10
int shipy; // position x 10
byte shipxspeed; // 0 bis 20, 10 abziehen!
byte shipyspeed; // 0 bis 20, 10 abziehen!
int fuel;
byte landed;
byte gravitycounter;
byte gravity;
byte landscape[21][12];
byte goalx[]={9,8,11,15,12,14,11,18,14,13};
byte goaly[]={11,11,11,9,11,5,11,11,4,3};
int yeahtimer;
int deadcounter;
byte i;
//----------------------------------------------------------------------------
// define images & sounds
//----------------------------------------------------------------------------
extern const byte PROGMEM gamelogo[];
extern const byte PROGMEM landscapetiles[58][6];
extern const byte PROGMEM spaceship[6][8];
extern const byte PROGMEM thrust[];
extern const byte PROGMEM levels[10][38];
extern const int soundfx[5][8];
//----------------------------------------------------------------------------
// setup
//----------------------------------------------------------------------------
void setup(){
  gb.begin();

  gb.setFrameRate(20);
  gb.titleScreen(("    Yoda's"),gamelogo);
  gb.display.setFont(font3x5);
  gb.pickRandomSeed();
  gamestatus="title";
  gb.battery.show=false;
}

//----------------------------------------------------------------------------
// loop
//----------------------------------------------------------------------------
void loop(){
  if(gb.update()){

     // new game
    if (gamestatus=="newgame") { newgame(); }

    // select level
    if (gamestatus=="selectlevel") { selectlevel(); }

    // new level
    if (gamestatus=="newlevel") { newlevel(); }

    // new life
    if (gamestatus=="newlife") { newlife(); }

     // game running
    if (gamestatus=="running") {

      checkbuttons(); // check buttons and change ship direction
      drawlandscape(); // draw the landscape
      checkcollission(); // check collission with landscape
      drawspaceship(); // draw ship, fuel and landing platforms
      checkpickup(); // check fuel pickup and landing
      nextlevelcheck(); // next level?
      handledeath(); // handle deathcounter
      showscore(); // show lives, score, level
    } // end of: gamestatus = running

    // title
    if (gamestatus=="title") { showtitle(); }

    // game over
    if (gamestatus=="gameover") { showgameover(); }

  } // end of update
 } // end of loop
//----------------------------------------------------------------------------
const byte PROGMEM gamelogo[]=
{
  64,26,
 B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,
 B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
 B00110000,B00000000,B10000000,B11000000,B10011111,B10000111,B11110000,B11111000,
 B01111000,B00000000,B10000001,B11100000,B10110011,B11001111,B10000001,B11101100,
 B01111000,B00000001,B11000001,B11100000,B10100001,B11001111,B00000011,B11000110,
 B01111000,B00000001,B11000001,B11110000,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00000011,B11100001,B11110000,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00000011,B11100001,B01111000,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00000010,B11100001,B01111000,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00000110,B11110001,B00111100,B10100001,B11101111,B11100011,B11000110,
 B01111000,B00000100,B11110001,B00111100,B10100001,B11101111,B00000011,B11111100,
 B01111000,B00000100,B01111001,B00011110,B10100001,B11101111,B00000011,B11000110,
 B01111000,B00001111,B11111001,B00011110,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00001000,B01111001,B00001111,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00001000,B00111101,B00001111,B10100001,B11101111,B00000011,B11000010,
 B01111000,B00011000,B00111101,B00000111,B10100001,B11001111,B00000011,B11000010,
 B01111100,B00010000,B00111101,B00000111,B10110011,B11001111,B10000011,B11000010,
 B00111111,B11010000,B00011001,B00000011,B00011111,B10000111,B11111001,B10000010,
 B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
 B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,
 B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
 B10100110,B11000110,B01101010,B11101100,B11100110,B01101110,B01100110,B11001110,
 B10101010,B10101010,B10001010,B01001010,B10001010,B10101010,B10001010,B10101000,
 B10101010,B10101110,B11101010,B01001010,B11001010,B11101100,B10001110,B10101100,
 B01001010,B10101010,B00101010,B01001010,B10001010,B10101010,B10001010,B10101000,
 B01001110,B11001010,B11100100,B11101100,B11101110,B10101010,B11101010,B11001110,
};
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const byte PROGMEM landscapetiles[58][6] = {
  {4,4, B11110000,B11110000,B11110000,B11110000}, // full
  {4,4, B11110000,B01110000,B01110000,B11110000}, // 4 x links frei
  {4,4, B11110000,B00110000,B11110000,B11110000},
  {4,4, B11110000,B11110000,B01110000,B11110000},
  {4,4, B11110000,B01110000,B00110000,B11110000},
  {4,4, B11110000,B11100000,B11100000,B11110000}, // 4 x rechts frei
  {4,4, B11110000,B11000000,B11110000,B11110000},
  {4,4, B11110000,B11110000,B11100000,B11110000},
  {4,4, B11110000,B11100000,B11000000,B11110000},
  {4,4, B10010000,B11110000,B11110000,B11110000}, // 4 x oben frei
  {4,4, B11010000,B11010000,B11110000,B11110000},
  {4,4, B10110000,B11110000,B11110000,B11110000},
  {4,4, B10010000,B10110000,B11110000,B11110000},
  {4,4, B11110000,B11110000,B11110000,B10010000}, // 4 x unten frei
  {4,4, B11110000,B11110000,B10110000,B10110000},
  {4,4, B11110000,B11110000,B11110000,B11010000},
  {4,4, B11110000,B11110000,B11010000,B10010000},
  {4,4, B00010000,B01110000,B11110000,B11110000}, // 4 x links oben frei
  {4,4, B00010000,B00010000,B01110000,B11110000},
  {4,4, B00010000,B01110000,B00110000,B11110000},
  {4,4, B00010000,B01010000,B01110000,B11110000},
  {4,4, B11000000,B11100000,B11100000,B11110000}, // 4 x rechts oben frei
  {4,4, B10000000,B11000000,B11000000,B11110000},
  {4,4, B10000000,B10100000,B11100000,B11110000},
  {4,4, B10000000,B11100000,B11000000,B11110000},
  {4,4, B11110000,B01110000,B01110000,B00110000}, // 4 x links unten frei
  {4,4, B11110000,B00110000,B00110000,B00010000},
  {4,4, B11110000,B01110000,B01010000,B00010000},
  {4,4, B11110000,B00110000,B01110000,B00010000},
  {4,4, B11110000,B11100000,B11100000,B11000000}, // 4 x rechts unten frei
  {4,4, B11110000,B11000000,B11000000,B10000000},
  {4,4, B11110000,B11100000,B10100000,B10000000},
  {4,4, B11110000,B11000000,B11100000,B10000000},
  {4,4, B00010000,B00110000,B11110000,B00110000}, // 4 x spitze links
  {4,4, B00110000,B11110000,B00110000,B00010000},
  {4,4, B00010000,B00110000,B01110000,B00110000},
  {4,4, B00110000,B01110000,B00110000,B00010000},
  {4,4, B11000000,B11110000,B11000000,B10000000}, // 4 x spitze rechts
  {4,4, B10000000,B11000000,B11110000,B11000000},
  {4,4, B11000000,B11100000,B11000000,B10000000},
  {4,4, B10000000,B11000000,B11100000,B11000000},
  {4,4, B01000000,B01000000,B11100000,B11110000}, // 4 x spitze oben
  {4,4, B00100000,B00100000,B01110000,B11110000},
  {4,4, B00000000,B01000000,B11100000,B11110000},
  {4,4, B00000000,B00100000,B01110000,B11110000},
  {4,4, B11110000,B11100000,B01000000,B01000000}, // 4 x spitze unten
  {4,4, B11110000,B01110000,B00100000,B00100000},
  {4,4, B11110000,B11100000,B01000000,B00000000},
  {4,4, B11110000,B01110000,B00100000,B00000000},
  {4,4, B11010000,B11110000,B11110000,B10110000}, // 4 x horizontale
  {4,4, B10110000,B11110000,B11110000,B11010000},
  {4,4, B10010000,B11010000,B11110000,B10110000},
  {4,4, B11010000,B11110000,B10110000,B10010000},
  {4,4, B11110000,B01110000,B11100000,B11110000}, // 4 x vertikale
  {4,4, B11110000,B11100000,B01110000,B11110000},
  {4,4, B11110000,B00110000,B01100000,B11110000},
  {4,4, B11110000,B01100000,B11000000,B11110000},
  {4,4, B01100000,B11010000,B10110000,B01100000}, // fuel
};
//----------------------------------------------------------------------------
const byte PROGMEM spaceship[6][8] = {
  {6,6, B01001000,B00110000,B01111000,B00110000,B01001000,B00000000},
  {6,6, B01001000,B00110000,B01111000,B00110000,B01001000,B10000100},
  {6,6, B01101000,B10010100,B01000100,B10001000,B10100100,B01011000},
  {6,6, B00010000,B01000100,B10010000,B00100100,B10001000,B00100000},
  {6,6, B10000100,B00100000,B00001000,B01000000,B00010000,B10000100},
  {6,6, B00010000,B00000000,B10000000,B00000100,B00000000,B00100000},
};
//----------------------------------------------------------------------------
const byte PROGMEM thrust[] = {4,3, B01100000,B11110000,B01100000};
//----------------------------------------------------------------------------
const byte PROGMEM levels[10][38] = {
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00001000, B01000000,B00000000,B00011000, B11100000,B00000000,B01111000, B11100000,B00000000,B00111000, B11000000,B00000000,B00111000, B11100000,B00000000,B01111000, B11110001,B00000100,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B10000000, B00000000,B00000001,B10000000, B10010000,B00000001,B11001000, B11111100,B00000011,B11001000, B11111110,B00000111,B11011000, B11110000,B00001111,B11111000, B11100000,B00000111,B11111000, B11000000,B00000011,B11111000, B11111000,B00000111,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00111111,B11111000, B00000000,B00001001,B11111000, B00000000,B00000000,B11111000, B00000000,B00000000,B10111000, B00000010,B00000000,B00111000, B01110110,B00000000,B00011000, B11111111,B00000000,B00111000, B11111111,B11000000,B10111000, B11111111,B10000001,B11111000, B11111111,B00000001,B11111000, B11111111,B10000000,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000111,B11111111,B11111000, B00000011,B11111111,B11111000, B00000011,B11111101,B01111000, B00000001,B01111000,B01011000, B10000000,B01010000,B00011000, B10000000,B00000000,B00001000, B10000000,B00000000,B00011000, B11010000,B00000000,B00001000, B11110100,B00000000,B00011000, B11111101,B00000011,B11111000, B11111111,B00100111,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00001000, B10000000,B00000000,B00011000, B11001000,B00000000,B00001000, B11111110,B11011000,B00001000, B11111111,B11111100,B00001000, B11111111,B11110000,B00001000, B11111111,B10100000,B00001000, B11111111,B00000000,B00011000, B11111111,B10000000,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000011,B01111000,B10011000, B00000011,B11010000,B00001000, B00000001,B11000000,B00011000, B10000000,B10000000,B00001000, B10000000,B00000000,B00001000, B10000000,B00000111,B11000000, B11000000,B00001111,B11101000, B11100000,B00011111,B11101000, B11000000,B11111000,B01111000, B11100000,B01110011,B00111000, B01111101,B11100111,B10011000, B00111111,B10001111,B11001000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B01000101,B00000000, B10000001,B11111111,B10000000, B11000111,B11111111,B11000000, B11100011,B11111111,B11100000, B11000001,B11111111,B11000000, B11110000,B10010011,B10000000, B11111000,B00000000,B00000000, B11111100,B00000000,B00000000, B11111110,B00000000,B00001000, B11111110,B11111111,B11111000},
  {21,12, B00000111,B11000000,B00111000, B00000111,B11100000,B00011000, B00000111,B10000000,B00011000, B00000011,B00000000,B00001000, B00000011,B10000000,B00000000, B00000011,B00000100,B00000000, B10000001,B00000100,B00000000, B11000000,B00001100,B00000000, B11100000,B00001110,B00000000, B11110000,B00011110,B00000000, B11100000,B11111111,B10000000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B11110000,B00111000, B00000000,B01100000,B00001000, B00111000,B11000000,B00001000, B11100000,B11000000,B00000000, B11000001,B10000011,B11000000, B10000001,B10000111,B01000000, B10000011,B00001111,B11100000, B00000010,B00011011,B11100000, B10000000,B00001111,B10110000, B10010000,B00011111,B11110000, B11010000,B00111110,B11110000, B11111111,B11111111,B11111000},
  {21,12, B00000001,B11100000,B00111000, B00000000,B11110000,B00011000, B01001000,B01111000,B00001000, B11111100,B00111111,B10001000, B11111100,B00111111,B11000000, B11111000,B01111111,B10000000, B11110000,B01111111,B00000000, B11110000,B11110110,B00000000, B11010000,B01000000,B00001000, B10010000,B00000000,B00011000, B10000000,B00000000,B01111000, B11111111,B11111111,B11111000},
 };
//----------------------------------------------------------------------------
void newgame() {
  score=0;
  lives=3;
  gravity=0;
  gamestatus="selectlevel";
}
//----------------------------------------------------------------------------
void newlevel() {

  // create landscape array from level bitmap
  screen=gamelevel%(maxlevel+1);
  if (gamelevel>maxlevel) {
    gravity=1;
  }

  gb.display.setColor(0);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(1);
  gb.display.drawBitmap(0,0,levels[screen]);

  for (int y=0; y<12; y++) {
    for (int x=0; x<21; x++) {

      int l0 = gb.display.getPixel(x,y);
      int l1=1;
      if (x>0 and y>0) { l1 = gb.display.getPixel(x-1,y-1); }
      int l2=1;
      if (y>0) { l2 = gb.display.getPixel(x,y-1); }
      int l3=1;
      if (x<20 and y>0) { l3 = gb.display.getPixel(x+1,y-1); }
      int l4=1;
      if (x>0) { l4 = gb.display.getPixel(x-1,y); }
      int l5=1;
      if (x<20) { l5 = gb.display.getPixel(x+1,y); }
      int l6=1;
      if (x>0 and y<11) { l6 = gb.display.getPixel(x-1,y+1); }
      int l7=1;
      if (y<11) { l7 = gb.display.getPixel(x,y+1); }
      int l8=1;
      if (x<20 and y<11) { l8 = gb.display.getPixel(x+1,y+1); }

      int l=0;
      if (l0==1) {
        l=1;
        if (l4==0) { l=2 + random(4); }
        if (l5==0) { l=6 + random(4); }
        if (l2==0) { l=10 + random(4); }
        if (l7==0) { l=14 + random(4); }
        if (l2==0 and l4==0) { l=18 + random(4); }
        if (l2==0 and l5==0) { l=22 + random(4); }
        if (l4==0 and l7==0) { l=26 + random(4); }
        if (l5==0 and l7==0) { l=30 + random(4); }
        if (l2==0 and l7==0) { l=50 + random(4); }
        if (l4==0 and l5==0) { l=54 + random(4); }
        if (l2==0 and l4==0 and l7==0) { l=34 + random(4); }
        if (l2==0 and l5==0 and l7==0) { l=38 + random(4); }
        if (l2==0 and l4==0 and l5==0) { l=42 + random(4); }
        if (l4==0 and l5==0 and l7==0) { l=46 + random(4); }
        if (l2==0 and l4==0 and l5==0 and l7==0) { l=58; }
      }
      landscape[x][y]=l;
    }
  }

  gb.display.setColor(0);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(1);
  gamestatus="newlife";
}
//----------------------------------------------------------------------------
void newlife() {
  shipx=8;
  shipy=0;
  shipxspeed=10;
  shipyspeed=10;
  deadcounter=-1;
  landed=0;
  yeahtimer=0;
  fuel=500;
  gamestatus="running";
}
//----------------------------------------------------------------------------
void showscore() {
  if (shipy/10>10) {
    gb.display.setColor(0);
    gb.display.fillRect(0,0,84,6);
    gb.display.setColor(1);
    gb.display.cursorY=0;
    gb.display.cursorX=81-4*(score>9)-4*(score>99)-4*(score>999)-4*(score>9999);
    gb.display.print(score);
    gb.display.cursorX=0;
    gb.display.print("F");
    gb.display.drawFastHLine(4,2,fuel/10);
  }
}
//----------------------------------------------------------------------------
void nextlevelcheck() {
  // increment timer after landed
  if (landed==1) {
    yeahtimer=++yeahtimer;
    if (fuel>9) {
      fuel=fuel-10;
      score=score+10;
      if (score%30 == 0) { playsoundfx(3,1); }
    }
    gb.display.setColor(0);
    gb.display.fillRect(5,17,74,7);
    gb.display.setColor(1);
    gb.display.cursorX=6;
    gb.display.cursorY=18;
    gb.display.print("READY FOR LEVEL");
    gb.display.cursorX=70;
    gb.display.print(gamelevel+2);
    if (yeahtimer>=50) {
      gamelevel=++gamelevel;
       gamestatus="newlevel";
       playsoundfx(2,1);
    }
  }
}
//----------------------------------------------------------------------------
void handledeath() {
  if (deadcounter != -1) {
    deadcounter=--deadcounter;
    gb.display.setColor(0);
    gb.display.fillRect(17,19,50,7);
    gb.display.setColor(1);
    gb.display.cursorX=18;
    gb.display.cursorY=20;
    gb.display.print(lives-1);
    gb.display.cursorX=26;
    gb.display.print("SHIPS LEFT");
    if (deadcounter == 0) {
      deadcounter=-1;
      lives=--lives;
      if (lives == 0) {
        gamestatus = "gameover";
      } else {
        gamestatus = "newlife";
      }
    }
  }
}
const int soundfx[5][8] = {
  {1,17,53,0,7,0,7,3}, // 0 = thrust (channel 0)
  {1,26,41,1,1,3,7,20}, // 1 = crash (channel 1)
  {0,0,42,1,1,2,7,20}, // 2 = landing success (channel 1)
  {0,54,0,0,0,0,7,1},  // 3 = fuel low (channel 1)
  {0,0,65,1,1,1,7,5}, // 4 = pick up fuel (channel 1)
};

//----------------------------------------------------------------------------
void checkbuttons() {
  byte changed=0;
  if (gb.buttons.repeat(BTN_LEFT,1) and deadcounter==-1 and yeahtimer==0) {
    shipxspeed=--shipxspeed;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10+6,shipy/10+1,thrust);
    if (shipxspeed==255) { shipxspeed=0; }
  }
  if (gb.buttons.repeat(BTN_RIGHT,1) and deadcounter==-1 and yeahtimer==0) {
    shipxspeed=++shipxspeed;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10-4,shipy/10+1,thrust);
    if (shipxspeed>20) { shipxspeed=20; }
  }
  if (gb.buttons.repeat(BTN_A,0) and deadcounter==-1 and yeahtimer==0) {
    shipyspeed=shipyspeed-2;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10+1,shipy/10+5,thrust);
    if (shipyspeed>250) { shipyspeed=0; }
  }

  if (gb.buttons.pressed(BTN_C)) {
    gb.titleScreen(("    Yoda's"),gamelogo);
    gamestatus="title";
  }

  if (fuel<0) { fuel=0; }
  gravitycounter=++gravitycounter;
  if (gravitycounter>2-gravity) {
    shipyspeed=++shipyspeed;
    gravitycounter=0;
    if (shipyspeed>20) { shipyspeed=20; }
  }

}
//----------------------------------------------------------------------------
void drawlandscape() {
  for (int y=0; y<12; y++) {
    for (int x=0; x<21; x++) {
      if (landscape[x][y] != 0) {
        gb.display.drawBitmap(x*4,y*4,landscapetiles[landscape[x][y]-1]);
      }
    }
  }
}
//----------------------------------------------------------------------------
void drawspaceship() {
  if (deadcounter==-1 and yeahtimer==0) {
    shipx=shipx+(shipxspeed-10);
    shipy=shipy+(shipyspeed-10);
    if (shipx>780) {
      shipx=780;
    }
    if (shipx<0) {
      shipx=0;
    }
    if (shipy<0) {
      shipy=0;
    }
  }

  if (deadcounter==-1) {
    if ((shipyspeed>=11 and shipyspeed<=12 and shipxspeed>8 and shipxspeed<12) or yeahtimer !=0 ) {
      gb.display.drawBitmap(shipx/10,shipy/10,spaceship[1]);
      // check if landed
      if (shipx/10>=goalx[screen]*4 and shipx/10+5<=goalx[screen]*4+11 and shipy/10+6>=goaly[screen]*4 and yeahtimer == 0) {
        landed=1;
      }
    } else {
      gb.display.drawBitmap(shipx/10,shipy/10,spaceship[0]);
    }
  } else {
    gb.display.drawBitmap(shipx/10-10+deadcounter/4,shipy/10-10+deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10-10+deadcounter/4,shipy/10+10-deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10+10-deadcounter/4,shipy/10-10+deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10+10-deadcounter/4,shipy/10+10-deadcounter/4,spaceship[5-deadcounter/10]);
    handledeath();
  }

  // draw fuel boxes

  // draw landing platform
  gb.display.setColor(0);
  gb.display.drawFastHLine(goalx[screen]*4,goaly[screen]*4+1,12);
  gb.display.setColor(1);
  gb.display.drawFastHLine(goalx[screen]*4,goaly[screen]*4,12);

}
//----------------------------------------------------------------------------
void checkpickup() {

}
//----------------------------------------------------------------------------
void selectlevel() {
  gb.display.cursorY=18;
  gb.display.cursorX=10;
  gb.display.print("SELECT LEVEL:");
  gb.display.cursorX=66;
  gb.display.print(gamelevel+1);
  gb.display.cursorY=36;
  gb.display.cursorX=2;
  gb.display.print("LEFT/RIGHT TO SELECT");
  gb.display.cursorX=18;
  gb.display.cursorY=42;
  gb.display.print("B TO CONFIRM");
  if (gb.buttons.repeat(BTN_LEFT,2) and gamelevel>0) {
    gamelevel=--gamelevel;
    playsoundfx(4,1);
  }
  if (gb.buttons.repeat(BTN_RIGHT,2) and gamelevel<maxlevel) {
    gamelevel=++gamelevel;
    playsoundfx(4,1);
  }
  if (gb.buttons.repeat(BTN_B,1)) {
    gamestatus="newlevel";
    playsoundfx(4,1);
  }
}
//----------------------------------------------------------------------------
void checkcollission() {
  if (deadcounter == -1 and yeahtimer == 0) {
    int x=shipx/10;
    int y=shipy/10;
    int l0 = gb.display.getPixel(x+1,y)+gb.display.getPixel(x+2,y+1)+gb.display.getPixel(x+3,y+1)+gb.display.getPixel(x+4,y);
    int l1 = gb.display.getPixel(x+1,y+4)+gb.display.getPixel(x+2,y+3)+gb.display.getPixel(x+3,y+3)+gb.display.getPixel(x+4,y+4);
    int l2 = gb.display.getPixel(x+1,y+2)+gb.display.getPixel(x+4,y+2);
    int l3 = 0;
    if (shipyspeed>=11 and shipyspeed<=12 and shipxspeed>8 and shipxspeed<12) {
      l3 = gb.display.getPixel(x,y+5)+gb.display.getPixel(x+5,y+5);
    }
    if (l0+l1+l2+l3 != 0) {
      deadcounter=30;
      playsoundfx(1,1);
    }
  }
}
//----------------------------------------------------------------------------
void showgameover() {
  gb.display.setColor(0);
  gb.display.fillRect(22,16,39,9);
  gb.display.setColor(1);
  gb.display.cursorX=24;
  gb.display.cursorY=18;
  gb.display.print("GAME OVER");
  gb.display.drawRect(22,16,39,9);
  gb.display.cursorX=4;
  gb.display.cursorY=42;
  gb.display.print("PRESS B TO CONTINUE");
  if (gb.buttons.pressed(BTN_B)) {
    gamestatus="title";
    gb.sound.playOK();
  }
}
//----------------------------------------------------------------------------
void showtitle() {
  if (score > highscore) { highscore = score; }
  gb.display.cursorX=0;
  gb.display.cursorY=0;
  gb.display.print("  LAST         HIGH");
  gb.display.cursorX=14-2*(score>9)-2*(score>99)-2*(score>999);
  gb.display.cursorY=6;
  gb.display.print(score);
  gb.display.cursorX=66-2*(highscore>9)-2*(highscore>99)-2*(highscore>999);
  gb.display.cursorY=6;
  gb.display.print(highscore);
  gb.display.drawBitmap(10,13,gamelogo);
  gb.display.cursorX=0;
  gb.display.cursorY=42;
  gb.display.print(" A: PLAY     C: QUIT");
  if (gb.buttons.pressed(BTN_A)) {
    gamestatus="newgame";
    gb.sound.playOK();
  }
  if (gb.buttons.pressed(BTN_C)) {
    gb.titleScreen(("Yoda's"),gamelogo);
  }
}
//----------------------------------------------------------------------------
void playsoundfx(int fxno, int channel) {
  gb.sound.command(0,soundfx[fxno][6],0,channel); // set volume
  gb.sound.command(1,soundfx[fxno][0],0,channel); // set waveform
  gb.sound.command(2,soundfx[fxno][5],-soundfx[fxno][4],channel); // set volume slide
  gb.sound.command(3,soundfx[fxno][3],soundfx[fxno][2]-58,channel); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1],soundfx[fxno][7],channel); // play note
}

