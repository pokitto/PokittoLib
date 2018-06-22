#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
//----------------------------------------------------------------------------    
// define variables and constants
//----------------------------------------------------------------------------    
typedef struct {
  char x;
  char y;
  boolean dir;
  boolean dead;
  byte team;
  byte fall;
  boolean isIA;
  byte life;
  byte timer;
} Player;


typedef struct{
  byte nbAlive;
  byte lastPlayer;
} Team;

typedef struct{
  byte x;
  byte y;
  char x_traj;
  char y_traj;  
} Rocket;

typedef struct {
  char angle;
  byte power;
  byte targetlocked; 
} Ia;

typedef struct{
  byte L0;
  byte L1;
  byte L2;
  byte L3;
  byte L4;
  byte L5;
  byte L6;
  byte L7;
  byte L8;
} PxChecks;

Player allPlayer[16];
Team teamInfo[4];
Rocket rocket;
Ia ia;
PxChecks pixelCheck;

//Game
byte gamestatus; 
byte nbAvailableLevel;
byte nbAliveTeam;
byte gamelevel;
byte screen;
byte landscapeZip[21][12];
byte landscape[21][12];
byte setting;
byte fct_countr;
byte fct_countr2;
byte fct_countr3;
byte out_countr;
byte out_countr2;
byte out_countr3;
byte check;
byte consttimer;
byte randomVal;

//Player
byte currentTeam;
byte currentPlayer;
byte jumpStatus;
byte nbPlayer;
byte nbTeam;
byte nbCpuTeam;
byte unitLife;


//projectile
byte power;
byte angle;
byte timer;
byte gravity;

//----------------------------------------------------------------------------    
// define images & sounds
//----------------------------------------------------------------------------    
extern const byte PROGMEM gamelogo[];
extern const byte PROGMEM landscapetiles[12][6];
extern const byte PROGMEM levels[26][38];
extern const byte PROGMEM options[];
extern const char trajParamX[];
extern const char trajParamY[];
extern const byte soundfx[6][8];

//----------------------------------------------------------------------------    
// setup
//----------------------------------------------------------------------------    
void setup(){
  gb.begin();
  main_initGame();
}

//----------------------------------------------------------------------------    
// init game
//----------------------------------------------------------------------------  
void main_initGame(){
  gb.titleScreen(gamelogo);
  gb.battery.show=false;
  #define SELECT_MAP 1
  #define NEW_LEVEL 2
  #define PAUSE 3
  #define RUNNING 4
  #define ANIMFIRE 5
  #define BOOM 6
  #define GAMEOVER 7
  #define OPTIONS 8
  #define WAIT 9
  #define SELECT_UNIT 10
  #define DAMAGE 11
   
  gamestatus=SELECT_MAP;
  
  gamelevel=0;
  nbAvailableLevel=26;
  consttimer = 20;
  setting=0;
  timer=0;
  randomVal = random(0,21);

  nbTeam = 2; 
  nbPlayer = 3;
  nbCpuTeam =1;
  unitLife = 3;
  gravity=1;
  
  
}

//----------------------------------------------------------------------------    
// loop
//----------------------------------------------------------------------------    
void loop(){
  if(gb.update()){
    gb.battery.show = false;
    switch(gamestatus)
    {
      case SELECT_MAP :
           fnctn_checkbuttons();
           outpt_selectMap(); 
           break;
           
      case OPTIONS :
           gb.battery.show = true;
           outpt_options();
           fnctn_checkbuttons();
           break;
           
      case NEW_LEVEL :
           screen=gamelevel-1;
           fnctn_newlevel();
           nbAliveTeam = nbTeam;
           power = 0;
           angle = 4;
           jumpStatus=0;
           currentTeam=0;
           currentPlayer=0;
           allPlayer[0].timer = consttimer;
           for(fct_countr = 0; fct_countr < nbTeam; fct_countr++){
             teamInfo[fct_countr].nbAlive = nbPlayer;
             teamInfo[fct_countr].lastPlayer = 0;
           }
           gamestatus=SELECT_UNIT;
           break;
           
      case PAUSE :
           outpt_pause();
           fnctn_checkbuttons();
           break;

      case SELECT_UNIT:
           outpt_landscape();
           outpt_players();
           outpt_life();
           outpt_team();
           fnctn_selectUnit();
           break;
           
      case RUNNING :  
           outpt_landscape();    
           outpt_players();

           if(allPlayer[currentPlayer].isIA==0){
             fnctn_checkbuttons();
           } else {
             fnctn_ia();
             if(gb.buttons.pressed(BTN_C)){
               gamestatus = PAUSE;
             }
           }      
           
           fnctn_checkJump();
           if(jumpStatus<3){
             fnctn_checkPlayerPos();
             if(allPlayer[currentPlayer].dead==1){fnctn_nextPlayer();}
           }
           
           outpt_power();
           outpt_team();
           outpt_life();
           if(power==0 && jumpStatus==0){outpt_cursor();}
           break;
           
      case ANIMFIRE :
           outpt_soundfx(0);
           outpt_landscape();
           outpt_players();
           fn_nextProjPosition();
           fn_checkCollision();
           outpt_projectile();
           break;
           
      case BOOM :
           outpt_soundfx(1);
           outpt_landscape();
           outpt_players();
           timer = timer + 1; 
           outpt_boom();
           switch(timer){
             case  2:
                   fnctn_rebuildMap();
                   break;
             case  7:
                   timer = 0;
                   fnctn_checkDead();
                   gamestatus = WAIT;
                   break;
           }
           break;
                
      case WAIT:
           fct_countr=0;
           outpt_landscape();
           outpt_players();
           fnctn_checkPlayerPos();
           for(fct_countr2=0;fct_countr2<nbPlayer*nbTeam;fct_countr2++){
             if(allPlayer[fct_countr2].fall>0){fct_countr=1;}
           }
           if(fct_countr==0){gamestatus=DAMAGE;}
           break;
      
      case DAMAGE:
           out_countr3=0;
           outpt_players();
           outpt_damage();
           outpt_landscape();
           if(out_countr3==0){fnctn_nextPlayer();}
           fnctn_gameOver();
           break;
           
      case GAMEOVER :
           outpt_landscape();
           outpt_players();
           outpt_gameOver();
           fnctn_checkbuttons();
           break;
      
    } // end switch
  } // end update
} // end loop
