#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
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
  gb.titleScreen(F("    Yoda's"),gamelogo);
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
