/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */
//#include <SPI.h>
//#include <Gamebuino.h>
#include "Pokitto.h"
/* Auto-generated function declarations */
void setup();
void main_initGame();
void loop();
void fnctn_checkbuttons();
void fnctn_definePlayer();
void fnctn_checkJump();
void fnctn_checkPlayerPos();
void fnctn_nextPlayer();
void fnctn_selectUnit();
void fnctn_checkDead();
void fnctn_gameOver();
void fn_nextProjPosition();
void fn_checkCollision();
void fnctn_newlevel();
void fnctn_buildLandscape();
void fnctn_rebuildMap();
void fnctn_unzip(byte,byte);
void fnctn_ia();
void outpt_selectMap();
void outpt_options();
void outpt_landscape();
void outpt_players();
void outpt_power();
void outpt_life();
void outpt_team();
void outpt_cursor();
void outpt_projectile();
void outpt_selectUnit();
void outpt_damage();
void outpt_pause();
void outpt_boom();
void outpt_gameOver();
void outpt_title();
void outpt_soundfx(byte);
Pokitto::Core gb;
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
int gamelevel;
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
  gb.display.setFont(font3x5);
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
//##################################################################
//##################################################################
void fnctn_checkbuttons() {
  switch(gamestatus)
  {
     case SELECT_MAP :
          if(gb.buttons.pressed(BTN_DOWN)  && gamelevel+3<=nbAvailableLevel){gamelevel = gamelevel + 3;}
          if(gb.buttons.pressed(BTN_UP)    && gamelevel-3>=0)               {gamelevel = gamelevel - 3;}
          if(gb.buttons.pressed(BTN_RIGHT) && gamelevel+1<=nbAvailableLevel){gamelevel = gamelevel + 1;}
          if(gb.buttons.pressed(BTN_LEFT)  && gamelevel-1>=0)               {gamelevel = gamelevel - 1;}
          if(gb.buttons.pressed(BTN_A)){
            if(gamelevel==0){
              gamestatus=OPTIONS;
              setting=0;
            }else{
              randomVal = random(0,21);
              gamestatus=NEW_LEVEL;
            }
          }
          if(gb.buttons.pressed(BTN_C)){
            gb.titleScreen(gamelogo);
            gb.battery.show = false;
          }
          break;

     case OPTIONS :
          if(gb.buttons.pressed(BTN_DOWN)){
            if(setting < 5) setting=setting+1; else setting = 0;
          }
          if(gb.buttons.pressed(BTN_UP)){
            if(setting > 0) setting=setting-1; else setting = 5;
          }
          if(gb.buttons.pressed(BTN_RIGHT)){
            switch(setting){
              case 0:
                   if(nbTeam<4) nbTeam=nbTeam+1; else nbTeam = 2;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 1:
                   if(nbPlayer<4) nbPlayer=nbPlayer+1; else nbPlayer = 1;
                   break;
              case 2:
                   if(unitLife<3) unitLife=unitLife+1; else unitLife = 1;
                   break;
              case 3:
                   if(nbCpuTeam<nbTeam-1) nbCpuTeam=nbCpuTeam+1; else nbCpuTeam = 0;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 4:
                   if(gravity<3) gravity=gravity+1; else gravity = 1;
                   break;
            }
          }
          if(gb.buttons.pressed(BTN_LEFT)){
            switch(setting){
              case 0:
                   if(nbTeam>2) nbTeam=nbTeam-1; else nbTeam = 4;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 1:
                   if(nbPlayer>1) nbPlayer=nbPlayer-1; else nbPlayer = 4;
                   break;
              case 2:
                   if(unitLife>1) unitLife=unitLife-1; else unitLife = 3;
                   break;
              case 3:
                   if(nbCpuTeam>0) nbCpuTeam=nbCpuTeam-1;  else nbCpuTeam = nbTeam;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 4:
                   if(gravity>1) gravity=gravity-1; else gravity = 3;
                   break;
            }
          }
          if(gb.buttons.pressed(BTN_B) || (gb.buttons.pressed(BTN_A) && setting==5)){
            gamestatus=SELECT_MAP;
          }
          if(gb.buttons.pressed(BTN_C)){
            gb.titleScreen(gamelogo);
            gb.battery.show = false;
          }
          break;

     case PAUSE :
          if(gb.buttons.pressed(BTN_DOWN)){
            if(setting < 1) setting=setting+1; else setting = 0;
          }
          if(gb.buttons.pressed(BTN_UP)){
            if(setting > 0) setting=setting-1; else setting = 1;
          }
          if(gb.buttons.pressed(BTN_B)) gamestatus = RUNNING;
          if(gb.buttons.released(BTN_A)){
            switch(setting){
              case 0:
                   gamestatus=RUNNING;
                   break;

              case 1:
                   gamestatus=SELECT_MAP;
                   break;
            }
          }
          if(gb.buttons.pressed(BTN_C)){
            gamestatus=SELECT_MAP;
          }
          break;


     case GAMEOVER :
          if(gb.buttons.pressed(BTN_A)){
            gamestatus=SELECT_MAP;
          }
          break;

     case SELECT_UNIT :
          if(gb.buttons.pressed(BTN_LEFT) || gb.buttons.pressed(BTN_RIGHT) || gb.buttons.pressed(BTN_B)){allPlayer[currentPlayer].timer=0;}
          break;

     case RUNNING :
          if(gb.buttons.repeat(BTN_A,1) && power<10){
            power = power+1;
          }else{
            if(power>0 ){
              //define original position of rocket
              rocket.x = allPlayer[currentPlayer].x+(1+(allPlayer[currentPlayer].dir))+(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)));
              rocket.y = (allPlayer[currentPlayer].y+((trajParamY[angle]/10))+1);
              rocket.x_traj = trajParamX[angle]/10*(power/2)*((allPlayer[currentPlayer].dir*2)-1);
              rocket.y_traj = trajParamY[angle]/10*(power/2);
              gamestatus=ANIMFIRE;
            }else{
              if(gb.buttons.repeat(BTN_UP,1)   || gb.buttons.pressed(BTN_UP))     {if(angle>0){angle = angle-1;}}
              if(gb.buttons.repeat(BTN_DOWN,1) || gb.buttons.pressed(BTN_DOWN))   {if(angle<8){angle = angle+1;}}

              if(gb.buttons.pressed(BTN_RIGHT)){
                allPlayer[currentPlayer].dir = 1;
              }else{
                if(gb.buttons.repeat(BTN_RIGHT,1)){
                  if(gb.display.getPixel(allPlayer[currentPlayer].x+4,allPlayer[currentPlayer].y+3)==0){
                    allPlayer[currentPlayer].x = allPlayer[currentPlayer].x + 1;
              } } }

              if(gb.buttons.pressed(BTN_LEFT)){
                allPlayer[currentPlayer].dir = 0;
              }else{
                if(gb.buttons.repeat(BTN_LEFT,1)){
                  if(gb.display.getPixel(allPlayer[currentPlayer].x-1,allPlayer[currentPlayer].y+3)==0){
                    allPlayer[currentPlayer].x = allPlayer[currentPlayer].x - 1;
              } } }

              if(gb.buttons.pressed(BTN_B)){
                if(jumpStatus==0 && power==0
                    && gb.display.getPixel(allPlayer[currentPlayer].x,allPlayer[currentPlayer].y-3)==0
                    && gb.display.getPixel(allPlayer[currentPlayer].x+3,allPlayer[currentPlayer].y-3)==0)
                {
                  outpt_soundfx(5);
                  jumpStatus=6;
                }
              }
              if(gb.buttons.pressed(BTN_C)){
                setting=0;
                gamestatus=PAUSE;
              }
            }
          }
          break;
  } // end switch
}



// ##################################################################################################################################
// ################## PLAYER RELATED FUNCTIONS ######################################################################################
// ##################################################################################################################################


//##################################################################
//##### DEFINE PLAYERS POSITION ####################################
void fnctn_definePlayer(){
  boolean check;
  byte tmp;
  byte tmp2;
  byte randm[21];

  for(fct_countr=0;fct_countr<21;fct_countr++){randm[fct_countr] = (fct_countr*4)%21;}

  tmp = random(0,21);
  for(fct_countr=0;fct_countr<21-tmp;fct_countr++){
    tmp2 = randm[fct_countr];
    randm[fct_countr] = randm[fct_countr+tmp];
    randm[fct_countr+tmp] = tmp2;
  }


  for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
    allPlayer[fct_countr].dead = 0;
    allPlayer[fct_countr].life = unitLife;
    allPlayer[fct_countr].team = fct_countr / nbPlayer;
    allPlayer[fct_countr].fall=0;
    allPlayer[fct_countr].isIA = 0;
    if(allPlayer[fct_countr].team>=nbTeam-nbCpuTeam){
      allPlayer[fct_countr].isIA=1;
    }
    allPlayer[fct_countr].timer = 0;


    //Define position
    allPlayer[fct_countr].x = randm[fct_countr]*4;

    switch(randm[fct_countr]%2){
      case 0:
           allPlayer[fct_countr].y = 0;
           check=0;
           while(check==0 && allPlayer[fct_countr].y<11){
             if(gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1)==0 && gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1+1)==1){
               allPlayer[fct_countr].y = allPlayer[fct_countr].y*4;
               check=1;
             }else{
               allPlayer[fct_countr].y = allPlayer[fct_countr].y+1;
             }
           }
           if(check==0){
              allPlayer[fct_countr].y = ((randm[fct_countr])%11)*4;
              if(gb.display.getPixel(randm[fct_countr]+1,(randm[fct_countr]%11)+1)==0){
                gb.display.setColor(BLACK);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1)+1);
              }else{
                gb.display.setColor(WHITE);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1));
                gb.display.setColor(BLACK);
              }
           }
           break;

      case 1:
           allPlayer[fct_countr].y = 10;
           check=0;
           while(check==0 && allPlayer[fct_countr].y>0){
             if(gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1)==0 && gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1+1)==1){
               allPlayer[fct_countr].y = allPlayer[fct_countr].y*4;
               check=1;
             }else{
               allPlayer[fct_countr].y = allPlayer[fct_countr].y-1;
             }
           }

           if(check==0){

              allPlayer[fct_countr].y = ((randm[fct_countr])%11)*4;
              if(gb.display.getPixel(randm[fct_countr]+1,(randm[fct_countr]%11)+1)==0){
                gb.display.setColor(BLACK);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1)+1);
              }else{
                gb.display.setColor(WHITE);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1));
                gb.display.setColor(BLACK);
              }
           }
           break;
    }

    //initial direction (1:right, -1:left)
    if(allPlayer[fct_countr].x>40){allPlayer[fct_countr].dir = 0;}
    else{allPlayer[fct_countr].dir = 1;}
  }
}

//##################################################################
//##################################################################
void fnctn_checkJump(){
  switch(jumpStatus){
    case 6:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-3;
         break;

    case 5:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-2;
         break;

    case 4:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-1;
         break;
  }
  if(jumpStatus>0){jumpStatus=jumpStatus-1;}
}

//##################################################################
//##################################################################
void fnctn_checkPlayerPos(){
  //check all players

  for(fct_countr2=0;fct_countr2<nbPlayer*nbTeam;fct_countr2++){
    //s'il n'y a rien

    if(gb.display.getPixel(allPlayer[fct_countr2].x,allPlayer[fct_countr2].y+4)==0 && gb.display.getPixel(allPlayer[fct_countr2].x+3,allPlayer[fct_countr2].y+4)==0){

      for(fct_countr3=0; fct_countr3<allPlayer[fct_countr2].fall+1;fct_countr3++){
        if(gb.display.getPixel(allPlayer[fct_countr2].x,allPlayer[fct_countr2].y+4)==0 && gb.display.getPixel(allPlayer[fct_countr2].x+3,allPlayer[fct_countr2].y+4)==0){
          if(allPlayer[fct_countr2].y>48 && allPlayer[fct_countr2].y<200){
            if(allPlayer[fct_countr2].dead==0){
              teamInfo[allPlayer[fct_countr2].team].nbAlive = teamInfo[allPlayer[fct_countr2].team].nbAlive-1;
              power = 0;
              allPlayer[fct_countr2].life=0;
              allPlayer[fct_countr2].fall=0;
              allPlayer[fct_countr2].dead=1;
            }
          }else{
            allPlayer[fct_countr2].y=allPlayer[fct_countr2].y+1;
          }
        }
      }
      if(allPlayer[fct_countr2].fall<4 && allPlayer[fct_countr2].dead==0){allPlayer[fct_countr2].fall=allPlayer[fct_countr2].fall + 1;}
    }else{
      allPlayer[fct_countr2].fall=0;
    }
    if(gb.display.getPixel(allPlayer[fct_countr2].x+2,allPlayer[fct_countr2].y+3)==1
      || gb.display.getPixel(allPlayer[fct_countr2].x+1,allPlayer[fct_countr2].y+3)==1
      && allPlayer[fct_countr2].dead==0){
      allPlayer[fct_countr2].y=allPlayer[fct_countr2].y-1;
      allPlayer[fct_countr2].fall = 0;
    }
  }
}

//##################################################################
//##################################################################
void fnctn_nextPlayer(){
  do{
    currentTeam = (currentTeam + 1)%nbTeam;
  }while(teamInfo[currentTeam].nbAlive==0);

  do{
    currentPlayer = (currentTeam*nbPlayer)+(teamInfo[currentTeam].lastPlayer+1)%nbPlayer;
    teamInfo[currentTeam].lastPlayer = (teamInfo[currentTeam].lastPlayer+1)%nbPlayer;
  }while(allPlayer[currentPlayer].dead==1);
  allPlayer[currentPlayer].timer = consttimer;
  power = 0;
  angle = 4;
  ia.angle = 2;
  ia.power = 10;
  ia.targetlocked=0;
  gamestatus = SELECT_UNIT;
  fnctn_gameOver();
}


//##################################################################
//##################################################################
void fnctn_selectUnit(){
  out_countr3=0;
  outpt_selectUnit();
  if(out_countr3==0){gamestatus=RUNNING;}
}


//##################################################################
//##################################################################
void fnctn_checkDead(){
  //only check if killed by rocket
  for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
    //if player is still alive
    if(allPlayer[fct_countr].dead==0){
      //if player is hitten by bullet
      if(allPlayer[fct_countr].x <= rocket.x && allPlayer[fct_countr].x+4 >= rocket.x && allPlayer[fct_countr].y <= rocket.y && allPlayer[fct_countr].y+4 >= rocket.y ){
        allPlayer[fct_countr].life = allPlayer[fct_countr].life-1;
        if(allPlayer[fct_countr].life<=0){
          power = 0;
          allPlayer[fct_countr].dead=1;
          teamInfo[allPlayer[fct_countr].team].nbAlive = teamInfo[allPlayer[fct_countr].team].nbAlive-1;
        }else{
          allPlayer[fct_countr].timer = consttimer;
        }
      }
    }
  }
}

//##################################################################
//##################################################################
void fnctn_gameOver(){
  byte nbAliveTeam = 0;
  //gamestatus=GAMEOVER;
  for(fct_countr=0;fct_countr<nbTeam;fct_countr++){
    if(teamInfo[fct_countr].nbAlive>0){nbAliveTeam=nbAliveTeam+1;}
  }
  if(nbAliveTeam<=1){
    gamestatus = GAMEOVER;
    //gamestatus = DAMAGE;
  }
}





// ##################################################################################################################################
// ################## BULLET RELATED FUNCTIONS ######################################################################################
// ##################################################################################################################################

//##################################################################
//##################################################################
void fn_nextProjPosition(){
  rocket.x = rocket.x + (rocket.x_traj/3);
  rocket.y = rocket.y + (rocket.y_traj/3);
  rocket.x_traj = rocket.x_traj + 0; //Wind
  rocket.y_traj = rocket.y_traj + gravity; //Gravity
  if(abs(rocket.x_traj)>12){rocket.x_traj=12*rocket.x_traj/abs(rocket.x_traj);}
  if(abs(rocket.y_traj)>12){rocket.y_traj=12*rocket.y_traj/abs(rocket.y_traj);}
}

//##################################################################
//##################################################################
void fn_checkCollision(){
  if(rocket.x>84 || rocket.x<0 || (rocket.y>48 && rocket.y<100)){
    fnctn_nextPlayer();
  }else{
    if(landscape[rocket.x/4][rocket.y/4]<60 && rocket.y/4>=0){gamestatus=BOOM;}
    for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
      if(allPlayer[fct_countr].x <= rocket.x && allPlayer[fct_countr].x+4 >= rocket.x && allPlayer[fct_countr].y <= rocket.y && allPlayer[fct_countr].y+4 >= rocket.y ){gamestatus=BOOM;}
    }
  }
}



// ##################################################################################################################################
// ################# LANDSCAPE RELATED FUNCTIONS ####################################################################################
// ##################################################################################################################################

//##################################################################
//##################################################################
void fnctn_newlevel() {
  // create landscape array from level bitmap
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(BLACK);
  gb.display.drawRect(0, 0, 23, 14);
  gb.display.drawBitmap(1,1,levels[screen]);

  fnctn_definePlayer();
  fnctn_buildLandscape();

  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,23,14);
  gb.display.setColor(BLACK);
}

//##################################################################
//##################################################################
void fnctn_buildLandscape(){
  for (fct_countr=1; fct_countr<13; fct_countr++) {
    for (fct_countr2=1; fct_countr2<22; fct_countr2++) {
      if(gb.display.getPixel(fct_countr2,   fct_countr))  {pixelCheck.L0=0;} else {pixelCheck.L0=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr-1)){pixelCheck.L1=0;} else {pixelCheck.L1=60;}
      if(gb.display.getPixel(fct_countr2,   fct_countr-1)){pixelCheck.L2=0;} else {pixelCheck.L2=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr-1)){pixelCheck.L3=0;} else {pixelCheck.L3=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr))  {pixelCheck.L4=0;} else {pixelCheck.L4=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr))  {pixelCheck.L5=0;} else {pixelCheck.L5=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr+1)){pixelCheck.L6=0;} else {pixelCheck.L6=60;}
      if(gb.display.getPixel(fct_countr2,   fct_countr+1)){pixelCheck.L7=0;} else {pixelCheck.L7=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr+1)){pixelCheck.L8=0;} else {pixelCheck.L8=60;}
      fnctn_unzip(fct_countr2-1, fct_countr-1);
    }
  }
}
//##################################################################
//##################################################################
void fnctn_rebuildMap(){

  if(rocket.x_traj>=0){
    if(rocket.y_traj>=0){
      if(landscape[(rocket.x/4)-1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)-1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)-1>=0
              && (rocket.x/4)-1<=21
              && rocket.y/4>=0
              && rocket.y/4<=12){
          landscape[(rocket.x/4)-1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
              && rocket.x/4<=21
              && (rocket.y/4)-1>=0
              && (rocket.y/4)-1<=12){
          landscape[rocket.x/4][(rocket.y/4)-1]=60;}
        }
      }else{
        if(rocket.x/4>=0
            && rocket.x/4<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }else{
      if(landscape[(rocket.x/4)-1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)+1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)-1>=0
              && (rocket.x/4)-1<=21
              && rocket.y/4>=0
              && rocket.y/4<=12){
          landscape[(rocket.x/4)-1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
              && rocket.x/4<=21
              && (rocket.y/4)+1>=0
              && (rocket.y/4)+1<=12){
          landscape[rocket.x/4][(rocket.y/4)+1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }
  }else{
    if(rocket.y_traj>=0){
      if(landscape[(rocket.x/4)+1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)-1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)+1>=0
            && (rocket.x/4)+1<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
          landscape[(rocket.x/4)+1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
            && rocket.x/4<=21
            && (rocket.y/4)-1>=0
            && (rocket.y/4)-1<=12){
          landscape[rocket.x/4][(rocket.y/4)-1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }else{
      if(landscape[(rocket.x/4)+1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)+1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)+1>=0
            && (rocket.x/4)+1<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
          landscape[(rocket.x/4)+1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
            && rocket.x/4<=21
            && (rocket.y/4)+1>=0
            && (rocket.y/4)+1<=12){
          landscape[rocket.x/4][(rocket.y/4)+1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }
  }

  for (fct_countr=0; fct_countr<12; fct_countr++) {
    for (fct_countr2=0; fct_countr2<21; fct_countr2++) {
      pixelCheck.L0 = landscape[fct_countr2][fct_countr];
      if (fct_countr2>0 and fct_countr>0)   { pixelCheck.L1 = landscape[fct_countr2-1][fct_countr-1]; } else { pixelCheck.L1=0; }
      if (fct_countr>0)                     { pixelCheck.L2 = landscape[fct_countr2][fct_countr-1];   } else { pixelCheck.L2=0; }
      if (fct_countr2<20 and fct_countr>0)  { pixelCheck.L3 = landscape[fct_countr2+1][fct_countr-1]; } else { pixelCheck.L3=0; }
      if (fct_countr2>0)                    { pixelCheck.L4 = landscape[fct_countr2-1][fct_countr];   } else { pixelCheck.L4=0; }
      if (fct_countr2<20)                   { pixelCheck.L5 = landscape[fct_countr2+1][fct_countr];   } else { pixelCheck.L5=0; }
      if (fct_countr2>0 and fct_countr<11)  { pixelCheck.L6 = landscape[fct_countr2-1][fct_countr+1]; } else { pixelCheck.L6=0; }
      if (fct_countr<11)                    { pixelCheck.L7 = landscape[fct_countr2][fct_countr+1];   } else { pixelCheck.L7=0; }
      if (fct_countr2<20 and fct_countr<11) { pixelCheck.L8 = landscape[fct_countr2+1][fct_countr+1]; } else { pixelCheck.L8=0; }

      fnctn_unzip(fct_countr2, fct_countr);
    }
  }
}

//##################################################################
//##################################################################
void fnctn_unzip(byte x, byte y){
  if (pixelCheck.L0<60) {
    landscape[x][y]=0;
    //BLACK

    //1 corner
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L5>=60){landscape[x][y]=10;}
    if(pixelCheck.L4>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=11;}
    if(pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=12;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60){landscape[x][y]=13;}

    //2 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60){landscape[x][y]=20;}
    if(pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=21;}
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=22;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=23;}

    //3 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=40;}
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=41;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=42;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=43;}

    //4 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=50;}

  }else{
    landscape[x][y]=60;
    //WHITE

    //1 corner
    if(pixelCheck.L2<60 && pixelCheck.L5<60){landscape[x][y]=70;}
    if(pixelCheck.L4<60 && pixelCheck.L7<60){landscape[x][y]=71;}
    if(pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=72;}
    if(pixelCheck.L2<60 && pixelCheck.L4<60){landscape[x][y]=73;}

    //2 corner
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60){landscape[x][y]=80;}
    if(pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=81;}
    if(pixelCheck.L2<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=82;}
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L7<60){landscape[x][y]=83;}

    //3 corner
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=100;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=101;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=102;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=103;}

    //4 corner
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=110;}

  }
}







//##################################################################
//##################################################################
void fnctn_ia(){
  //this function can run only if landscape + player (only) are displayed (no cursor, no lifebar, no power, no level)
  if(timer > 0){
    timer = timer -1;
  }else{
    if(ia.targetlocked==0){
      //direction
      for(fct_countr=0;fct_countr<2;fct_countr++){
        //angle
        for(fct_countr2=8;fct_countr2>0;fct_countr2--){
          //power
          for(fct_countr3=0;fct_countr3<10;fct_countr3++){
            check = 0;
            rocket.x = allPlayer[currentPlayer].x+(1+(fct_countr))+(((trajParamY[fct_countr2]/10)*((fct_countr*2)-1)));
            rocket.y = (allPlayer[currentPlayer].y+((trajParamY[fct_countr2]/10))+1);
            rocket.x_traj = trajParamX[fct_countr2]/10*(fct_countr3/2)*((fct_countr*2)-1);
            rocket.y_traj = trajParamY[fct_countr2]/10*(fct_countr3/2);
            while(check == 0){
              for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
                if(    allPlayer[out_countr].x-4 <= rocket.x
                    && allPlayer[out_countr].x+8 >= rocket.x
                    && allPlayer[out_countr].y-4 <= rocket.y
                    && allPlayer[out_countr].y+8 >= rocket.y
                    && allPlayer[out_countr].dead == 0
                    && allPlayer[out_countr].team != allPlayer[currentPlayer].team
                    ){
                      if(ia.targetlocked==0){
                        allPlayer[currentPlayer].dir = fct_countr;
                        ia.angle = fct_countr2;
                        ia.power = fct_countr3;
                        if(allPlayer[out_countr].x-2 <= rocket.x
                          && allPlayer[out_countr].x+6 >= rocket.x
                          && allPlayer[out_countr].y-2 <= rocket.y
                          && allPlayer[out_countr].y+6 >= rocket.y
                          && allPlayer[out_countr].dead == 0
                          && allPlayer[out_countr].team != allPlayer[currentPlayer].team)
                      {
                        ia.targetlocked=1;
                        timer = 20;
                      }
                    }
                    check=1;
                }
                if(rocket.x>84)                         {check=1;}
                if(rocket.x<0)                          {check=1;}
                if(rocket.y>48)                         {check=1;}
                if(landscape[rocket.x/4][rocket.y/4]<60){check=1;}
                fn_nextProjPosition();
              }
            }
          }
        }
      }
      if(ia.targetlocked==0){
        ia.targetlocked=1;
        timer = 20;
      }
    }else{
      //animation
      if(angle!=ia.angle){
        if(angle>ia.angle){angle = angle -1;}
        else {
          angle = angle +1;
          if(angle==ia.angle){timer = 20;}
        }
      }else{
        if(power<ia.power){
          power = power + 1;
        }else{
          rocket.x = allPlayer[currentPlayer].x+(1+(allPlayer[currentPlayer].dir))+(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)));
          rocket.y = (allPlayer[currentPlayer].y+((trajParamY[angle]/10))+1);
          rocket.x_traj = trajParamX[angle]/10*(power/2)*((allPlayer[currentPlayer].dir*2)-1);
          rocket.y_traj = trajParamY[angle]/10*(power/2);
          gamestatus = ANIMFIRE;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
const byte PROGMEM gamelogo[]=
{72,35,
B00000000,B00000000,B00000000,B10000010,B01010100,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000001,B01011011,B00010101,B10110101,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000001,B11010010,B01010101,B00100101,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000001,B01010001,B01010100,B10100011,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000000,B00000000,B00000000,
B00000000,B00000000,B00001111,B11111111,B11111111,B11111010,B01100000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00010100,B00000000,B00000000,B00000000,B00000001,B00101000,B00000000,B00000000,
B00000000,B00010101,B01011010,B01001100,B00010100,B11000010,B10100011,B01100000,B00000000,
B00000000,B00011101,B01010101,B00101010,B00010100,B10000011,B10101010,B01010000,B00000000,
B00000000,B00010101,B10010101,B01101010,B00001001,B10000010,B10101001,B01010000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111111,B10000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000011,B11111111,B11100000,
B00000000,B00001110,B00000000,B00000000,B00000000,B00000000,B00000111,B11111111,B11100000,
B00000000,B00110001,B10000000,B00000000,B00000000,B00000000,B00001111,B11111111,B11110000,
B00000000,B01000000,B01000000,B00000000,B00000000,B00000000,B00001111,B11111111,B11110000,
B00000000,B10001111,B00100000,B00000000,B00000000,B00000000,B00001111,B11111111,B11110000,
B00000000,B10010000,B10100000,B00000000,B00000000,B00000000,B00001111,B11111111,B11100000,
B00000001,B00111111,B11010000,B00000000,B00000000,B00000000,B00001111,B11111111,B11000000,
B00000001,B00111111,B11010000,B00000000,B00000000,B00000000,B00000010,B01111111,B10000000,
B00000001,B01100101,B01010000,B00000000,B00000000,B00000000,B00001111,B11111110,B00000000,
B00000011,B11100000,B01010000,B00000000,B00000000,B00000000,B00001111,B11111100,B00000000,
B00000101,B01100110,B10010000,B00000000,B00000000,B00000000,B00001111,B11111110,B00000000,
B00000110,B10110000,B10100000,B00000000,B00000000,B00000000,B00001010,B10001111,B10000000,
B00000101,B01011111,B01100000,B00000000,B00000000,B00000000,B00000000,B00000111,B11000000,
B00000011,B10101010,B10100000,B00000000,B00000000,B00000000,B00000000,B00111111,B11110000,
B00000100,B01110101,B01100000,B00000000,B00000001,B11110000,B00000001,B11111111,B11111000,
B00000101,B00001111,B11000000,B00010000,B00000001,B00011011,B11000001,B11111111,B11111000,
B00001000,B11000011,B11111111,B11111000,B00000000,B00001111,B11111100,B11111110,B11111000,
B00001000,B00110111,B11111100,B11111000,B00000000,B01111111,B11111100,B11111101,B11101000,
B00001010,B00001111,B11111111,B11110000,B00000000,B00000111,B10111111,B01100111,B11011000,
B00010001,B10000000,B00111111,B11110000,B00000010,B00011110,B01010011,B11111111,B01111000,
B00010000,B01111000,B00010100,B10000000,B00000011,B11110011,B11101111,B11111101,B11110000,
B00100000,B00000111,B11110011,B00000000,B00000000,B00000000,B11111111,B11110111,B11110000,
};
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const byte PROGMEM landscapetiles[12][6] = {
  {4,4, B11110000,B11110000,B11110000,B11110000}, // full
  {4,4, B11100000,B11110000,B11110000,B11110000}, // 1corner
  {4,4, B01100000,B11110000,B11110000,B11110000}, // 2corner
  {4,4, B01110000,B11110000,B11110000,B11100000},
  {4,4, B01100000,B11110000,B11110000,B01110000}, // 3corner
  {4,4, B01100000,B11110000,B11110000,B01100000}, // 4corner
  {4,4, B00000000,B00000000,B00000000,B00000000}, // empty
  {4,4, B00010000,B00000000,B00000000,B00000000}, // 1corner
  {4,4, B10010000,B00000000,B00000000,B00000000}, // 2corner
  {4,4, B10000000,B00000000,B00000000,B00010000},
  {4,4, B10010000,B00000000,B00000000,B10000000}, // 3corner
  {4,4, B10010000,B00000000,B00000000,B10010000}, // 4corner
};

const byte PROGMEM units[4][6] = {
  {4,4, B01100000,B11110000,B01100000,B10010000}, // Char 1
  {4,4, B10010000,B11110000,B01100000,B10010000}, // Char 2
  {4,4, B11110000,B11110000,B01100000,B10010000}, // Char 3
  {4,4, B01100000,B01100000,B01100000,B10010000}  // Char 4
};


//----------------------------------------------------------------------------
/*const byte PROGMEM levels[11][38] = {
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00001000, B01000000,B00000000,B00011000, B11100000,B00000000,B01111000, B11100000,B00000000,B00111000, B11000000,B00000000,B00111000, B11100000,B00000000,B01111000, B11110001,B00000100,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B10000000,B00010000,B00000000, B11000000,B00011000,B00000000, B11000000,B00001000,B00001000, B11000000,B00011100,B00111000, B11000010,B00011100,B01111000, B11000110,B00011100,B01111000, B11100110,B00011100,B01111000, B11110110,B00011100,B01111000, B11111111,B00111111,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000001,B00000000, B00000001,B00000011,B00000000, B00000001,B10010110,B00000000, B00000000,B10111100,B00010000, B00000000,B11111000,B00010000, B10000000,B01110000,B00010000, B10000000,B01111000,B00111000, B11000000,B01111000,B01111000, B11100100,B01111000,B01111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B10000000, B00000000,B00000001,B10000000, B10010000,B00000001,B11001000, B11111100,B00000011,B11001000, B11111110,B00000111,B11011000, B11110000,B00001111,B11111000, B11100000,B00000111,B11111000, B11000000,B00000011,B11111000, B11111000,B00000111,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00111111,B11111000, B00000000,B00001001,B11111000, B00000000,B00000000,B11111000, B00000000,B00000000,B10111000, B00000010,B00000000,B00111000, B01110110,B00000000,B00011000, B11111111,B00000000,B00111000, B11111111,B11000000,B10111000, B11111111,B10000001,B11111000, B11111111,B00000001,B11111000, B11111111,B10000000,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B00000000,B00001000, B10000000,B00000000,B00011000, B11001000,B00000000,B00001000, B11111110,B11011000,B00001000, B11111111,B11111100,B00001000, B11111111,B11110000,B00001000, B11111111,B10100000,B00001000, B11111111,B00000000,B00011000, B11111111,B10000000,B11111000, B11111111,B11111111,B11111000},
  {21,12, B00000011,B01111000,B10011000, B00000011,B11010000,B00001000, B00000001,B11000000,B00011000, B10000000,B10000000,B00001000, B10000000,B00000000,B00001000, B10000000,B00000111,B11000000, B11000000,B00001111,B11101000, B11100000,B00011111,B11101000, B11000000,B11111000,B01111000, B11100000,B01110011,B00111000, B01111101,B11100111,B10011000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B00000000,B00000000, B00000000,B00000000,B00000000, B00000000,B01000101,B00000000, B10000001,B11111111,B10000000, B11000111,B11111111,B11000000, B11100011,B11111111,B11100000, B11000001,B11111111,B11000000, B11110000,B10010011,B10000000, B11111000,B00000000,B00000000, B11111100,B00000000,B00000000, B11111110,B00000000,B00001000, B11111111,B11111111,B11111000},
  {21,12, B00000111,B11000000,B00111000, B00000111,B11100000,B00011000, B00000111,B10000000,B00011000, B00000011,B00000000,B00001000, B00000011,B10000000,B00000000, B00000011,B00000100,B00000000, B10000001,B00000100,B00000000, B11000000,B00001100,B00000000, B11100000,B00001110,B00000000, B11110000,B00011110,B00000000, B11100000,B11111111,B10000000, B11111111,B11111111,B11111000},
  {21,12, B00000000,B11110000,B00111000, B00000000,B01100000,B00001000, B00111000,B11000000,B00001000, B11100000,B11000000,B00000000, B11000001,B10000011,B11000000, B10000001,B10000111,B01000000, B10000011,B00001111,B11100000, B00000010,B00011011,B11100000, B10000000,B00001111,B10110000, B10010000,B00011111,B11110000, B11010000,B00111110,B11110000, B11111111,B11111111,B11111000},
  {21,12, B00000001,B11100000,B00111000, B00000000,B11110000,B00011000, B01001000,B01111000,B00001000, B11111100,B00111111,B10001000, B11111100,B00111111,B11000000, B11111000,B01111111,B10000000, B11110000,B01111111,B00000000, B11110000,B11110110,B00000000, B11010000,B01000000,B00001000, B10010000,B00000000,B00011000, B10000000,B00000000,B01111000, B11111111,B11111111,B11111000},
 };
*/
const byte PROGMEM levels[26][38] = {
  {21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11000000,B00000000,B00011000,B11110000,B00000000,B00111000,B11111000,B00000000,B01111000,B11000001,B11111100,B01111000,B11111011,B00000110,B11111000,B11111100,B00000001,B11111000,B11111000,B00000000,B11111000,B11111000,B00000000,B11111000,B11111000,B00000000,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00110000,B00000000,B00000000,B01000100,B11010011,B00000000,B01010010,B10101010,B00000000,B01110110,B10101001,B00000000,B00000000,B00000000,B00000000,B00011000,B00000000,B00000000,B00011101,B01010110,B00110000,B00010101,B01010101,B01010000,B00011101,B10010101,B01100000,B00000000,B00000000,B00000000,B10101010,B10101010,B10101000}
 ,{21,12, B00000000,B01111111,B11111000,B00000000,B00010011,B11111000,B00000000,B00000001,B11111000,B00000000,B00000001,B00111000,B00000100,B00000000,B00111000,B01101100,B00000000,B00011000,B11111110,B00000000,B00111000,B11111111,B10000000,B10111000,B11111111,B00000001,B11111000,B11111110,B00000001,B11111000,B11111111,B00000000,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000000,B00000001,B00000011,B00000000,B00000001,B10010110,B00000000,B00000000,B10111100,B00010000,B00000000,B11111000,B00010000,B10000000,B01110000,B00010000,B10000000,B01111000,B00111000,B11000000,B01111000,B01111000,B11100100,B01111000,B01111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01000101,B00000000,B10000001,B11111111,B10000000,B11000111,B11111111,B11000000,B11100011,B11111111,B11100000,B11000001,B11111111,B11000000,B11110000,B10010011,B10000000,B11111000,B00000000,B00000000,B11111100,B00000000,B00000000,B11111110,B00000000,B00000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B10011001,B11110000,B00000000,B01100101,B11011000,B00000000,B11110000,B01111110,B00000000,B01001000,B11001111,B00000000,B01000000,B00001111,B10000000,B01000000,B00010111,B11000000,B01010000,B00000011,B01100000,B11100000,B00000110,B00111000,B11111111,B11111111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000110,B00001010,B00000000,B00001111,B10000100,B00000000,B00000111,B00000000,B11110000,B00000000,B00000000,B00010000,B00100000,B00000000,B01110000,B01110000,B00000000,B11110000,B11111000,B00000001,B10010000,B00100000,B00000010,B10010000,B11111100,B00000100,B10010000,B11111000,B11111111,B11111000,B11110011,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000001,B11000000,B00000000,B00000011,B11100000,B00000000,B00000010,B10100000,B00000010,B00000011,B11100000,B00000110,B00000001,B11000000,B01000101,B10000000,B10000000,B01000111,B10000011,B11100000,B00111101,B00001100,B10000000,B00111100,B11110001,B11000000,B00100100,B00000001,B01000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B10000000,B00000000,B00001000,B11000000,B00000000,B10011000,B10000000,B11011011,B11111000,B10000001,B11111111,B11111000,B10000000,B01111111,B11111000,B10000000,B00101111,B11111000,B11000000,B00000111,B11111000,B11111000,B00001111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000111,B11000000,B00111000,B00000111,B11100000,B00011000,B00000111,B10000000,B00011000,B00000011,B00000000,B00001000,B00000011,B10000000,B00000000,B00000011,B00000100,B00000000,B10000001,B00000100,B00000000,B11000000,B00001100,B00000000,B11100000,B00001110,B00000000,B11110000,B00011110,B00000000,B11100000,B11111111,B10000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,B01000000,B00000000,B00011000,B11100000,B00000000,B01111000,B11100000,B00000000,B00111000,B11000000,B00000000,B00111000,B11100000,B00000000,B01111000,B11110001,B00001001,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B11000000,B01100000,B11100000,B00000000,B00000000,B00000000,B00001110,B00000000,B00000111,B00000000,B00000000,B00000000,B01100000,B11000000,B00110000,B00000000,B00110000,B00000000,B00000111,B00000000,B00011110,B00000000,B00000000,B00000000,B00011100,B00011000,B11100000,B00000000,B00000000}
 ,{21,12, B00000000,B00000000,B00000000,B11111111,B10011111,B11111000,B11111111,B11001111,B11111000,B10000000,B11100110,B00001000,B11101111,B11110011,B11011000,B11000000,B01110000,B00001000,B11111101,B11100100,B11111000,B10000000,B11001110,B01111000,B11011111,B10011111,B00111000,B11000000,B00111111,B10111000,B11111111,B11111111,B10001000,B11111111,B11111111,B10111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11110000,B00000000,B00000000,B11111000,B00000000,B00000000,B11111000,B00000000,B00000000,B11111100,B00000000,B00011000,B11111100,B00000000,B01111000,B11111100,B00001111,B11111000,B11111110,B00011111,B11111000,B11111111,B11111111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B01110000,B00011111,B00000000,B00110000,B00101000,B10000000,B01110000,B00101000,B01000000,B00110000,B01111111,B11110000,B01110000,B11111111,B11111000,B00110000,B11001111,B00111000,B00010000,B01001111,B00110000,B00010000,B00110000,B11000000,B00010000,B11111111,B11111111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001111,B11111111,B11111000,B10000000,B00001000,B00000000,B11110000,B11111111,B00000000,B11001111,B11100100,B10000000,B01001111,B11100110,B01000000,B00110000,B11111111,B11100000,B00000000,B01111111,B11000000,B00000000,B00010001,B00010000,B00000000,B01111111,B11100000,B00000000,B00000000,B00000000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B11000000,B00000000,B00000000,B11100000,B00000000,B00000111,B11111100,B00000000,B00000001,B11111110,B00000000,B00000010,B10101011,B11110000,B00011111,B11111111,B11100000,B00011111,B11111111,B11000000,B00011111,B11111111,B10000000,B10101111,B11111111,B10101000,B01010101,B01010101,B01010000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B10000000,B00010000,B00000000,B11000000,B00011000,B00000000,B11000000,B00001000,B00001000,B11000000,B00011100,B00111000,B11000010,B00011100,B01111000,B11000110,B00011100,B01111000,B11100110,B00011100,B01111000,B11110110,B00011100,B01111000,B11111111,B00111111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000001,B11100000,B00111000,B00000000,B11110000,B00011000,B01001000,B01111000,B00001000,B11111100,B00111111,B10001000,B11111100,B00111111,B11000000,B11111000,B01111111,B10000000,B11110000,B01111111,B00000000,B11110000,B11110110,B00000000,B11010000,B01000000,B00001000,B10010000,B00000000,B00011000,B10000000,B00000000,B01111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01000000,B00000000,B00000001,B11000000,B10010000,B00000001,B11101000,B11111100,B00000011,B11101000,B11111110,B00000111,B11111000,B11110000,B00001111,B11111000,B11100000,B00000111,B11111000,B11000000,B00000011,B11111000,B11111000,B00000111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00100000,B00000000,B00000000,B00100000,B00000000,B00000000,B00100000,B00000000,B00000000,B01110000,B00000000,B00000000,B01010000,B00000000,B01000100,B01010001,B00010000,B11101110,B11111011,B10111000,B11101110,B11011011,B10111000,B01000101,B10001101,B00010000,B11111111,B11111111,B11111000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000010,B10000000,B00000000,B10101011,B10000000,B00000000,B01110001,B00000000,B00000000,B01110001,B00000000,B00000000,B01110011,B00000000,B00000000,B00100110,B00000000,B00000000,B01111100,B00000000,B00000000,B11111000,B00000000,B00000000,B11110000,B00000000,B00000000,B01110000,B00000000,B00000000,B11110000,B00000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B10101000,B00000000,B00000000,B01010000,B00000000,B00011111,B11111111,B11000000,B00011111,B11111111,B11000000,B00001010,B10001010,B10000000,B11111010,B10001010,B11111000,B10101010,B10001010,B10101000,B10101010,B10001010,B10101000,B10101010,B10001010,B10101000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011000,B00001000,B00100000,B00111100,B00000100,B01000000,B01111110,B00001111,B11100000,B11011011,B00011011,B10110000,B11111111,B00111111,B11111000,B00100100,B00101111,B11101000,B01011010,B00101000,B00101000,B10100101,B00000110,B11000000,B00000000,B00000000,B00000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00000000,B00011111,B00000000,B00000000,B00111111,B10000011,B11000000,B01111011,B00000111,B11100000,B01111110,B00001111,B11110000,B01111100,B00001100,B10010000,B01111110,B00001101,B10110000,B01111111,B00001111,B11110000,B00111111,B10001111,B11110000,B00011111,B00001010,B01010000,B00000000,B00000000,B00000000,B11111111,B11111111,B11111000}
 ,{21,12, B00000000,B00000000,B00111000,B00000000,B00000000,B00010000,B01000000,B00010001,B00111000,B00110000,B00000100,B00000000,B01111010,B10101010,B00000000,B00110000,B00000100,B00000000,B01000000,B00010001,B01110000,B00000000,B00000000,B00100000,B00000000,B00001000,B01110000,B10000000,B00011100,B00000000,B11000000,B00111110,B00001000,B11111111,B11111111,B01111000}
};


 //----------------------------------------------------------------------------
const byte PROGMEM options[] = {24,10,
B00110000,B10010000,B00000000,
B01111000,B11110000,B00000000,
B00110000,B01100000,B11000000,
B01001000,B10010000,B00100000,
B00000000,B00000000,B01000000,
B00000000,B00000000,B00000000,
B01111000,B01100000,B01000000,
B01111000,B01100000,B00000000,
B00110000,B01100000,B00000000,
B01001000,B10010000,B00000000
};


//----------------------------------------------------------------------------
const char trajParamX[] = {0  , 10, 20, 30,30 ,30 ,20 ,10 ,0 };
const char trajParamY[] = {-30,-30,-20,-10,0  ,10 ,20 ,30 ,30};
//##################################################################
//##################################################################
void outpt_selectMap(){
  byte x,y,z;
  if((((gamelevel)/3)*17)+20>48){
    z=(((gamelevel-3)/3))*17;
  }else{
    z=0;
    gb.display.drawRect(3, 7, 23, 14);
    gb.display.drawBitmap(4,9,options);
  }
  for (out_countr=0; out_countr<nbAvailableLevel; out_countr++) {
    x = 4+(((out_countr+1)%3)*27);
    y = 8+(((out_countr+1)/3)*17)-z;
    if(y<100){
      gb.display.drawRect(x-1, y-1, 23, 14);
      gb.display.drawBitmap(x,y,levels[out_countr]);
      if(out_countr+1==gamelevel){gb.display.drawRect(x-2, y-2, 25, 16);}
    }
  }

  gb.display.setColor(WHITE);
  gb.display.fillRect(0, 0, 84, 6);
  gb.display.setColor(BLACK);
  if(gamelevel==0){
    gb.display.cursorX = 27;
    gb.display.print("Settings");
    gb.display.drawRect(2, 6, 25, 16);
  }else{
    gb.display.cursorX = 28;
    gb.display.print("Level ");
    gb.display.print(gamelevel);
}
}


//##################################################################
//##################################################################
void outpt_options(){
  gb.display.cursorY = 0;
  gb.display.cursorX = 28;
  gb.display.print("Settings");

  gb.display.cursorY = 12;
  gb.display.cursorX = 16;
  gb.display.print("Player:  <");
  gb.display.print(nbTeam);
  gb.display.print(">");

  gb.display.cursorY = 18;
  gb.display.cursorX = 16;
  gb.display.print("Units:   <");
  gb.display.print(nbPlayer);
  gb.display.print(">");

  gb.display.cursorY = 24;
  gb.display.cursorX = 16;
  gb.display.print("Life:    <");
  gb.display.print(unitLife);
  gb.display.print(">");

  gb.display.cursorY = 30;
  gb.display.cursorX = 16;
  gb.display.print("NbCPU:   <");
  gb.display.print(nbCpuTeam);
  gb.display.print(">");

  gb.display.cursorY = 36;
  gb.display.cursorX = 16;
  gb.display.print("Gravity: <");
  gb.display.print(gravity);
  gb.display.print(">");

  gb.display.cursorY = 42;
  gb.display.cursorX = 16;
  gb.display.print("Back");

  gb.display.drawBitmap(11,13+(setting*6),units[0]);
}

//##################################################################
//##################################################################
void outpt_landscape() {
  for (out_countr=0; out_countr<12; out_countr++) {
    for (out_countr2=0; out_countr2<21; out_countr2++) {
      /*gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10]);*/

      switch(landscape[out_countr2][out_countr]%10){
        case 0:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10]);
             break;
        case 1:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROT180,NOFLIP);
             break;
        case 2:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROTCW,NOFLIP);
             break;
        case 3:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROTCCW,NOFLIP);
             break;
      }
    }
  }
}

//##################################################################
//##################################################################
void outpt_players() {
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].dead==0){
      gb.display.drawBitmap(allPlayer[out_countr].x,allPlayer[out_countr].y,units[allPlayer[out_countr].team]);
    }else{
      gb.display.drawFastVLine(allPlayer[out_countr].x+1,allPlayer[out_countr].y,4);
      switch(allPlayer[out_countr].team){
        case 0:
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y+1,3);
          break;
        case 1:
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y,3);
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y+2,3);
          break;
        case 2:
          gb.display.drawPixel(allPlayer[out_countr].x,allPlayer[out_countr].y);
          gb.display.drawPixel(allPlayer[out_countr].x+2,allPlayer[out_countr].y+1);
          break;
        case 3:
          gb.display.drawPixel(allPlayer[out_countr].x,allPlayer[out_countr].y+1);
          gb.display.drawPixel(allPlayer[out_countr].x+2,allPlayer[out_countr].y);
          break;
      }
    }
  }
}

//##################################################################
//##################################################################
void outpt_power(){
  if(power>0){
    gb.display.setColor(0);
    gb.display.fillRect(0, 0, 12, 4);
    gb.display.setColor(1);
    gb.display.drawPixel(0,0);
    gb.display.drawPixel(0,3);
    gb.display.drawPixel(11,0);
    gb.display.drawPixel(11,3);

    for(out_countr=1; out_countr<power+1; out_countr++){
      gb.display.drawPixel(out_countr,1);
      gb.display.drawPixel(out_countr,2);
    }
  }
}

//##################################################################
//##################################################################
void outpt_life(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].dead==0 && allPlayer[out_countr].fall==0){
      if(out_countr==currentPlayer && jumpStatus!=0){}
      else{
        gb.display.setColor(BLACK);
        switch(allPlayer[out_countr].life){
          case 3:
               gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y-3,4);
               break;
          case 2:
               gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y-3,2);
               break;
        }
      }
    }
  }
}

//##################################################################
//##################################################################
void outpt_team(){
  if(power==0){
    gb.display.print("P");
    gb.display.print(currentTeam+1);
  }
}

//##################################################################
//##################################################################
void outpt_cursor(){

  gb.display.setColor(WHITE);
  gb.display.drawPixel(allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1
                      ,allPlayer[currentPlayer].y+1);
  gb.display.drawPixel(allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5));

  gb.display.setColor(BLACK);
  gb.display.drawLine (allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  -1
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +0
                      ,allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +0
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         -1);
  gb.display.drawLine (allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +1
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +0
                      ,allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +0
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +1);

}

//##################################################################
//##################################################################
void outpt_projectile(){
  gb.display.drawPixel(rocket.x,rocket.y);
  gb.display.drawPixel(rocket.x+1,rocket.y);
  gb.display.drawPixel(rocket.x,rocket.y-1);
  gb.display.drawPixel(rocket.x+1,rocket.y-1);
}

//##################################################################
//##################################################################
void outpt_selectUnit(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].timer>0){
      out_countr3=1;
      gb.display.drawFastVLine(allPlayer[out_countr].x+1, allPlayer[out_countr].y-6-(allPlayer[out_countr].timer%4), 2);
      gb.display.drawPixel(allPlayer[out_countr].x  , allPlayer[out_countr].y-3-(allPlayer[out_countr].timer%4));
      gb.display.drawPixel(allPlayer[out_countr].x+1, allPlayer[out_countr].y-2-(allPlayer[out_countr].timer%4));
      gb.display.drawPixel(allPlayer[out_countr].x+2, allPlayer[out_countr].y-3-(allPlayer[out_countr].timer%4));
      allPlayer[out_countr].timer=allPlayer[out_countr].timer-1;
    }
  }
}

//##################################################################
//##################################################################
void outpt_damage(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].timer%3==2){
       gb.display.setColor(WHITE);
       gb.display.fillRect(allPlayer[out_countr].x,allPlayer[out_countr].y,4,4);
       gb.display.setColor(BLACK);
    }
    if(allPlayer[out_countr].timer>0){
      allPlayer[out_countr].timer=allPlayer[out_countr].timer-1;
      out_countr3=1;
    }
  }
}

//##################################################################
//##################################################################
void outpt_pause() {
  gb.display.cursorY = 0;
  gb.display.cursorX = 28;
  gb.display.print("Pause");
  gb.display.cursorY = 15;
  gb.display.cursorX = 16;
  gb.display.print("Back to Game");
  gb.display.cursorY = 25;
  gb.display.cursorX = 16;
  gb.display.print("Quit to New Map");

  gb.display.drawBitmap(11,16+(setting*10),units[0]);
}

//##################################################################
//##################################################################
void outpt_boom() {
  switch(timer){
    case  0:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);
          break;

    case  1:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);
          break;

    case  2:
          gb.display.setColor(WHITE);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-2,rocket.y-1 ,rocket.x-1 ,rocket.y-2);
          gb.display.drawLine(rocket.x  ,rocket.y-1 ,rocket.x-1 ,rocket.y  );
          break;

    case  3:
          gb.display.setColor(WHITE);
          gb.display.fillRect(rocket.x-1,rocket.y-1, 2, 2);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-1 ,rocket.y-2 ,rocket.x   ,rocket.y-2);
          gb.display.drawLine(rocket.x+1 ,rocket.y-1 ,rocket.x+1 ,rocket.y  );
          gb.display.drawLine(rocket.x-2 ,rocket.y-1 ,rocket.x-2 ,rocket.y  );
          gb.display.drawLine(rocket.x-1 ,rocket.y+1 ,rocket.x   ,rocket.y+1);
          break;

    case  5:
          gb.display.setColor(WHITE);
          gb.display.drawPixel(rocket.x,rocket.y);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-1,rocket.y   ,rocket.x   ,rocket.y-1);
          gb.display.drawLine(rocket.x  ,rocket.y+1 ,rocket.x+1 ,rocket.y  );
          break;

    case  6:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x,rocket.y);
          break;

  }
}


//##################################################################
//##################################################################
void outpt_gameOver(){
    gb.display.setColor(WHITE);
    gb.display.fillRect(4,5,76,24);
    gb.display.setColor(BLACK);
    gb.display.drawRect(5,6,74,22);
    gb.display.cursorY = 11;
    gb.display.cursorX = 17;
    if(allPlayer[currentPlayer].isIA==0){
      gb.display.print("Player ");
    }else{
      gb.display.print("  CPU ");
    }
    gb.display.print(currentTeam+1);
    gb.display.println(" win!");
    gb.display.cursorX = 7;
    switch(randomVal%10){
      case 0: gb.display.println("Others are losers!");break;

      case 1: gb.display.println("    Congrats!     ");break;

      case 2: gb.display.println(" Better next time ");break;

      case 3: gb.display.println("   Frtzz Gzzuit!  ");break;

      case 4: gb.display.println(" Game by Frakasss ");break;

      case 5: gb.display.println("  Not so bad...   ");break;

      case 6: gb.display.println("  WHO'S THE BEST? ");break;

      case 7: gb.display.println("Not really fair...");break;

      case 8: gb.display.println("Quite boring game.");break;

      case 9: gb.display.println(" Just dust in eye ");break;

    }
}

//##################################################################
//##################################################################
void outpt_title() {
  gb.titleScreen(gamelogo);
}

//##################################################################
//##################################################################
void outpt_soundfx(byte fxno) {
  gb.sound.command(0,soundfx[fxno][6],0,0); // set volume
  gb.sound.command(1,soundfx[fxno][0],0,0); // set waveform
  gb.sound.command(2,soundfx[fxno][5],-soundfx[fxno][4],0); // set volume slide
  gb.sound.command(3,soundfx[fxno][3],soundfx[fxno][2]-58,0); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1],soundfx[fxno][7],0); // play note
}





const byte soundfx[6][8] = {
  {1,17,53,0,7,0,2,3}, // 0 = thrust (channel 0)          fire
  {1,17,53,0,7,0,10,3}, // 0 = thrust (channel 0)          fire
  {1,26,41,1,1,3,7,20}, // 1 = crash (channel 1)          boom
  {0,0,42,1,1,2,7,20}, // 2 = landing success (channel 1) gameover
  {0,54,0,0,0,0,7,1},  // 3 = fuel low (channel 1)
  {0,0,65,1,1,1,7,5}, // 4 = pick up fuel (channel 1)
};

