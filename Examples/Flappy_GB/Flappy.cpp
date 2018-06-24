/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */
/****** FLAPPY BIRDO ***** clone of Dong Nguyen's Flappy Bird.
- coded by Jerom (2015) + help from Skyrunner65, clement.
- SFX with the help of FX Synth by yodasvideoarcade http://gamebuino.com/forum/viewtopic.php?f=17&t=1018
*/

//#include <SPI.h> //imports the SPI library (needed to communicate with Gamebuino's screen)
//#include <Gamebuino.h> //imports the Gamebuino library
#include "Pokitto.h"
/* Auto-generated function declarations */
void setup();
void initGame();
void initVariables();
void loop();
void drawBackground();
void drawGround();
void gameOver();
void gameWin();
void resetGame();
void initDifficulty();
void pipesStart();
void updatePipes();
void drawPipes();
void playerMove();
void playerAnimation();
void drawPlayerAlive();
void drawPlayerEnd();
void updateScore();
void drawScore();
void updateHighscore();
void playsoundfx(int,int);
void soundMute();
Pokitto::Core gb; //creates a Gamebuino object named gb

//IMAGES
const byte bird1Bitmap[] PROGMEM = {16, 10, 0xF, 0x0, 0x3C, 0x80, 0x79, 0x40, 0xF8, 0x40, 0xFD, 0xC0, 0xC6, 0x20, 0x8D, 0xC0, 0x9E, 0x20, 0x7F, 0xC0, 0x1E, 0x0,};
const byte bird1MaskBitmap[] PROGMEM = {16, 10, 0xF, 0x0, 0x3F, 0x80, 0x7F, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xE0, 0xFF, 0xC0, 0xFF, 0xE0, 0x7F, 0xC0, 0x1E, 0x0,};
const byte bird2Bitmap[] PROGMEM = {16, 10, 0xF, 0x0, 0x7C, 0x80, 0x99, 0x40, 0x88, 0x40, 0x8D, 0xC0, 0xC6, 0x20, 0xFD, 0xC0, 0xFE, 0x20, 0x7F, 0xC0, 0x1E, 0x0,};
const byte bird2MaskBitmap[] PROGMEM = {16, 10, 0xF, 0x0, 0x7F, 0x80, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xE0, 0xFF, 0xC0, 0xFF, 0xE0, 0x7F, 0xC0, 0x1E, 0x0,};
const byte cityBitmap[] PROGMEM = {16, 19, 0x7E, 0x0, 0x42, 0x0, 0x56, 0x3F, 0x42, 0x21, 0x57, 0xAB, 0xC8, 0xA1, 0x3A, 0xEA, 0xA8, 0xA2, 0x3A, 0xAA, 0xA8, 0xA2, 0x3A, 0xAA, 0xA8, 0xA2, 0xA, 0x7A, 0xBC, 0x86, 0xC3, 0x19, 0xC, 0x81, 0x11, 0xC2, 0x38, 0x20, 0x44, 0x0,};
const byte gameoverBitmap[] PROGMEM = {32, 25, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x80, 0x0, 0x0, 0x10, 0x80, 0x0, 0x70, 0x20, 0x80, 0x0, 0x88, 0x27, 0xBF, 0xFD, 0x4, 0x24, 0xC2, 0x3, 0x24, 0x24, 0x82, 0x1, 0x24, 0x24, 0x92, 0x49, 0x4, 0x20, 0x92, 0x49, 0x1C, 0x20, 0x82, 0x49, 0x1C, 0x31, 0xC2, 0x49, 0x90, 0x1E, 0x3F, 0xFF, 0xF0, 0xC, 0x1F, 0xE3, 0x70, 0x4, 0x1F, 0xC1, 0x7C, 0x4, 0x92, 0x49, 0x84, 0x4, 0x92, 0x49, 0x4, 0x4, 0x92, 0x41, 0x1C, 0x4, 0x10, 0x47, 0x3C, 0x4, 0x10, 0xC7, 0x20, 0x6, 0x31, 0xE5, 0x20, 0x3, 0xFF, 0x3D, 0xE0, 0x1, 0xDE, 0x1D, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,};
const byte gameoverMaskBitmap[] PROGMEM = {32, 25, 0x1F, 0xC0, 0x0, 0x0, 0x3F, 0xE0, 0x0, 0xF8, 0x7F, 0xE0, 0x1, 0xFC, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xF8, 0xF, 0xFF, 0xFF, 0xF8, 0x7, 0xFF, 0xFF, 0xF8, 0x3, 0xFF, 0x3F, 0xF0,};
const byte pipeBitmap[] PROGMEM = {16, 24, 0xFF, 0xF0, 0x80, 0x10, 0x83, 0xF0, 0xC5, 0xD0, 0xFF, 0xF0, 0xCF, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0, 0x85, 0xD0, 0x83, 0xF0,};
const byte skyBitmap[] PROGMEM = {24, 6, 0x1C, 0x0, 0x0, 0xA2, 0x7, 0xA0, 0x41, 0x8, 0x40, 0x43, 0xD0, 0x40, 0x4, 0x30, 0x0, 0x0, 0x10, 0x0,};
const byte skyMaskBitmap[] PROGMEM = {24, 6, 0x1C, 0x0, 0x0, 0xBE, 0x7, 0xA0, 0xFF, 0xF, 0xE0, 0xFF, 0xDF, 0xE0, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xE0,};
const byte titleBitmap[] PROGMEM = {64, 30, 0x0, 0xF, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x44, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x44, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x44, 0x0, 0x0, 0x7, 0xFC, 0x0, 0x0, 0x47, 0xC4, 0xFF, 0xE3, 0xE4, 0x44, 0x0, 0x0, 0x40, 0x45, 0x4, 0x14, 0x14, 0x44, 0x0, 0x0, 0x40, 0x46, 0x4, 0xC, 0xC, 0x44, 0x0, 0x0, 0x40, 0x44, 0x4, 0x4, 0x4, 0x44, 0x0, 0x0, 0x47, 0xC4, 0x44, 0x44, 0x44, 0x44, 0x0, 0x0, 0x47, 0xC4, 0x44, 0x44, 0x44, 0x44, 0x0, 0x0, 0x44, 0x44, 0x4, 0x4, 0x4, 0x4, 0x0, 0x0, 0x44, 0x46, 0x4, 0xC, 0xE, 0x4, 0x0, 0x0, 0x44, 0x47, 0x4, 0x1C, 0x1F, 0x4, 0x0, 0x0, 0x7C, 0x7D, 0xFC, 0x74, 0x7B, 0xC4, 0x0, 0x0, 0x7C, 0x7C, 0xFC, 0x64, 0x71, 0x4, 0x0, 0x0, 0x0, 0xF, 0xC4, 0x44, 0x5F, 0xC, 0x0, 0x0, 0x0, 0x8, 0x24, 0x44, 0x51, 0x1C, 0x0, 0x0, 0x0, 0x8, 0x17, 0xC7, 0xD1, 0xF8, 0x0, 0x0, 0x0, 0x8, 0xF, 0xC7, 0xD1, 0x0, 0x0, 0x0, 0x0, 0x8, 0x8F, 0x9F, 0x31, 0x38, 0x0, 0x0, 0x0, 0x8, 0x88, 0xA1, 0x41, 0x44, 0x0, 0x0, 0x0, 0x8, 0x8, 0xC1, 0x81, 0x82, 0x0, 0x0, 0x0, 0x8, 0xF, 0x81, 0x1, 0x1, 0x0, 0x0, 0x0, 0x8, 0x88, 0x87, 0x11, 0x11, 0x0, 0x0, 0x0, 0x8, 0x88, 0x8F, 0x11, 0x11, 0x0, 0x0, 0x0, 0x8, 0x8, 0x89, 0x1, 0x1, 0x0, 0x0, 0x0, 0x8, 0x18, 0x89, 0x81, 0x83, 0x0, 0x0, 0x0, 0x8, 0x38, 0x88, 0xC1, 0xC6, 0x0, 0x0, 0x0, 0xF, 0xEF, 0xF8, 0x7F, 0x7C, 0x0, 0x0, 0x0, 0xF, 0xCF, 0xF8, 0x3F, 0x38, 0x0,};
const byte trophyBitmap[] PROGMEM = {24, 32, 0x0, 0x0, 0x0, 0x3F, 0xFF, 0xF0, 0x20, 0x0, 0x10, 0x3F, 0xFF, 0xF0, 0x1C, 0x0, 0xE0, 0x20, 0x0, 0x10, 0x48, 0x0, 0x48, 0x58, 0x0, 0x68, 0x58, 0x0, 0x68, 0x4C, 0x0, 0xC8, 0x64, 0x0, 0x98, 0x3A, 0x1, 0x70, 0x1F, 0x3, 0xE0, 0x7, 0x87, 0x80, 0x0, 0xFC, 0x0, 0x0, 0x78, 0x0, 0x0, 0x48, 0x0, 0x0, 0xFC, 0x0, 0x3, 0x3, 0x0, 0x3F, 0xF0, 0x80, 0x24, 0x9F, 0x80, 0x24, 0x9F, 0x80, 0x24, 0x9F, 0xE0, 0x24, 0x92, 0x10, 0x24, 0x9E, 0x8, 0x24, 0x92, 0x48, 0x24, 0x92, 0x48, 0x20, 0x12, 0x48, 0x20, 0x32, 0x48, 0x3F, 0xFF, 0xF8, 0x3F, 0xDF, 0xF8, 0x0, 0x0, 0x0,};
const byte trophyMaskBitmap[] PROGMEM = {24, 32, 0x7F, 0xFF, 0xF8, 0x7F, 0xFF, 0xF8, 0x7F, 0xFF, 0xF8, 0x7F, 0xFF, 0xF8, 0x7F, 0xFF, 0xF8, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xF8, 0x3F, 0xFF, 0xF0, 0xF, 0xFF, 0xC0, 0x1, 0xFE, 0x0, 0x1, 0xFE, 0x0, 0x7, 0xFF, 0x80, 0x7F, 0xFF, 0xC0, 0x7F, 0xFF, 0xC0, 0x7F, 0xFF, 0xC0, 0x7F, 0xFF, 0xF0, 0x7F, 0xFF, 0xF8, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFC,};

//SOUND PATTERNS made with FX Synth
const byte soundfx[5][8] = {
  {0, 0, 61, 1, 0, 0, 2, 3}, // sound 0  = fly.
  {0, 6, 107, 1, 5, 4, 3, 8}, // sound 1 = +1 point.
  {0, 24, 55, 1, 0, 0, 5, 7}, // sound 2 = death.
  {0, 57, 2, 1, 7, 19, 5, 57}, // sound 3 = melody played when maximum score is reached.
  {0, 4, 37, 4, 7, 12, 3, 1}, // sound 4 = menu bip.
};

//ARRAY to store...
typedef struct {
  float x, y, //... X & Y positions of each pipes displayed on the screen.
        hs; //... 3 difficulty highscores.
} Pipe;
Pipe pipe[4]; //maximum size of the array.

//FONTS
extern const byte font3x5[];
extern const byte font5x7[];
//CONSTANTS
#define PIPEW 12 //pipe width.
#define PIPEH 24 //pipe height. //24
#define PIPEGAPV 24 //constant vertical gap between 2 pipes.
#define PIPEGAPH 30 //constant horizontal gap between 2 pipes.
#define GROUNDH 4 //height of the ground.
#define SCOREMAX 99 //99 points is the constant maximum score.
#define GAMEOVERX floor((LCDWIDTH - 30) / 2) //constant X position of the gameover sprite.
#define TROPHYX floor((LCDWIDTH - 22) / 2) //constant X position of the trophy sprite.
#define PLAYERX 12 //constant player X position.
#define PLAYERW 11 //constant player width.
#define PLAYERH 10 //constant player height.
// VARIABLES
float player_y = 0; //the player starts at the middle of the LCD screen height minus the ground height.
float gravity = 0; //gravity variable.
long difficulty_level = 0; //easy=0.5 < normal=1.0 < hard=2.0

bool player_death = false;//false=player alive. true=player dead.
byte player_animation = 0; //animation timer.
byte score = 0; // current score : each time the player pass tubes, the score increases of 1 point.
byte score_timer = 0; //each time a pipe passed the player X position, 1 point is added to the score.
bool score_units = false; //if the score is lower than 10, score_units equals 0, else score_units equals 1.
byte score_maxreached = 0; //used as a timer too when the SCOREMAX is reached.
byte highscore_1 = 0; //highscore
byte highscore_2 = 0;
byte highscore_3 = 0;
byte gameover_y = 0; //Y position of the gameover sprite.
byte gameover_timer = 0; //game over timer.
bool difficulty_menu = true;
bool sound_volume = true;

//SETUP
void setup() {
  gb.begin();
  initGame();
}

void initGame() {
  gb.titleScreen((""), titleBitmap); //
  gb.titleScreen(("FLAPPY BIRD clone\n\nHOW TO PLAY\n.ARROWS/A to fly\n.B to mute\n.C to reset")); //Sub-menu with keys infos.
  initVariables(); //reset all variables.
  difficulty_menu = true; //will display the menu.
  difficulty_level = 1; //reset to NORMAL.
  sound_volume = true; //reset to 1.
}

void initVariables() {
  gb.pickRandomSeed(); //randomize an original random every restart.
  pipesStart(); //each pipes starts at a random height.
  //gb.changeGame(); //Really reset the game without pressing "C" two times...
  //gb.battery.show = false;
  //reset variables.
  player_y = ((LCDHEIGHT - GROUNDH) / 2) - 8; //the player starts at the middle of the LCD screen height minus the ground height.
  gravity = 0; //gravity variable.
  player_death = false;
  player_animation = 0;
  updateHighscore();
  score = 0;
  score_timer = - PLAYERX - (PLAYERW / 2);
  score_units = false;
  score_maxreached = 0;
  gameover_timer = 0;
  gameover_y = LCDHEIGHT;
}

//LOOP
void loop() {
  if (gb.update()) {
    if (difficulty_menu == true) {
      initDifficulty();
    } //end of difficulty menu.
    else {
      resetGame();
      soundMute();
      //ALWYAYS DRAW
      drawBackground();
      drawPipes();
      drawGround();
      drawScore();
      //Player actions
      if (player_death == false) {
        if (score < SCOREMAX) {
          playerMove();
          updatePipes();
        }
        else {
          gameWin();
        }
        drawPlayerAlive();
      }
      else { //when the player is dead...
        drawPlayerEnd();
        gameOver();
      }
    }//end of normal game.
  } //end of update().
} // end of loop().
void drawBackground() {
  gb.display.setColor(GRAY);
  gb.display.fillRect(0, 0, LCDWIDTH, 17); //the top of the sky is gray.

  for (byte i = 0; i < (256 / 16); i++) { //loop...
    gb.display.setColor(WHITE);
    gb.display.drawBitmap((i * 16), 13, skyMaskBitmap); //display clouds mask.
    gb.display.setColor(BLACK);
    gb.display.drawBitmap((i * 16), 13, skyBitmap); //display clouds.
    gb.display.setColor(GRAY);
    gb.display.drawBitmap((i * 16), 23, cityBitmap); //display city.
  }
}

void drawGround() {
  gb.display.setColor(BLACK); //1st top black line.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH), LCDWIDTH, 1);
  gb.display.setColor(GRAY); //gray background.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH + 1), LCDWIDTH, (GROUNDH - 1));
  gb.display.setColor(WHITE); // white highlight.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH + 1), LCDWIDTH, 1);
  gb.display.setColor(BLACK); //shadow on the bottom.
  gb.display.fillRect(0, (LCDHEIGHT - 1), LCDWIDTH, 1);
  for (byte j = 0; j < LCDWIDTH / 2; j++) { //loop to display small shadow "dents".
    gb.display.fillRect( (j * 4), (LCDHEIGHT - 2 ), 2, 1);
  }
}
void gameOver() { //= DEATH. When the player hits a pipe or the ground.
  if (gameover_y > floor(LCDHEIGHT / 3)) gameover_y -= 4; //update Y position.
  //update ending timer.
  if (gameover_timer < LCDHEIGHT / 2) {
    gameover_timer += 2;
  } else initVariables(); //simple death = simple reset.

  //DRAW game over logo.
  gb.display.setColor(WHITE);
  gb.display.drawBitmap(GAMEOVERX, gameover_y, gameoverMaskBitmap); //white mask placed under the outline bitmap.
  gb.display.setColor(BLACK);
  gb.display.drawBitmap(GAMEOVERX, gameover_y, gameoverBitmap); //outline
}

void gameWin() { //==WIN
  if (score_maxreached <= 1)  playsoundfx(3, 0); //start the WINNING melody.
  score_maxreached++;
  if (score_maxreached > (LCDHEIGHT / 1)) initGame(); //END = RESET + go back to titlescreen.

  //DRAW TROPHY.
  if (gameover_y > floor(LCDHEIGHT / 3) - 2 ) gameover_y -= 2; //update Y position.
  gb.display.setColor(WHITE);
  gb.display.drawBitmap(TROPHYX, gameover_y, trophyMaskBitmap); //white mask placed under the outline bitmap.
  gb.display.setColor(BLACK);
  gb.display.drawBitmap(TROPHYX, gameover_y, trophyBitmap); //outline
}

//PRESS "C" BUTTON TO RESET GAME & RETURN TO THE TITLESCREEN.
void resetGame() {
  if (gb.buttons.pressed(BTN_C)) {
    initGame();
  }
}
void initDifficulty() {
  //MOVE ARROW
  if (gb.buttons.pressed(BTN_UP))  {
    //gb.sound.setVolume(1, 0);
    playsoundfx(4, 0); //gb.sound.playOK();
    difficulty_level --;
    player_animation = 1 ;
  }
  if (gb.buttons.pressed(BTN_DOWN)) {
    //gb.sound.setVolume(1, 0);
    playsoundfx(4, 0); //gb.sound.playOK();
    difficulty_level ++;
    player_animation = 1 ;
  }
  if (difficulty_level < 0) difficulty_level = 2;
  if (difficulty_level > 2) difficulty_level = 0;

  gb.display.cursorX = 0;
  gb.display.cursorY = 16 + (8 * floor(difficulty_level));
  //gb.display.print(("\20")); //arrow
  playerAnimation();
  gb.display.setColor(BLACK);
  if (player_animation > 2) {
    gb.display.drawBitmap(0, 22 + (8 * floor(difficulty_level)), bird2Bitmap);
  } else gb.display.drawBitmap(0, 22 + (8 * floor(difficulty_level)), bird1Bitmap); //bird

  if (gb.buttons.pressed(BTN_A)) { //validate level
    if (difficulty_level == 0) difficulty_level = 0.5;
    gb.sound.playOK();
    difficulty_menu = false ;
  }
  //DRAW TEXTS
  gb.display.cursorX = 0;
  gb.display.setFont(font5x7);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print(("DIFFICULTY:\n \n\n  SLOW\n  NORMAL\n  FAST"));
  gb.display.setFont(font3x5);
  gb.display.cursorY = 2;
  gb.display.print(("\nUP/DOWN to choose\nA to validate"));
  //gb.display.println(difficulty_level);//test
  //HIGHSCORE
  for (int pp = 0; pp < 3; pp++) { //
    gb.display.cursorX = 49;
    gb.display.cursorY = 25 + (pp * 8);
    gb.display.print(("HIGH:"));
    gb.display.cursorY = 25 + (pp * 8);
    gb.display.cursorX = 70;
    gb.display.print(round(pipe[pp].hs));
  }
}
void pipesStart() {
  for (byte bb = 0; bb < 3; bb++) {
    pipe[bb].x = LCDWIDTH + (PIPEGAPH * bb); //pipe X position
    pipe[bb].y = random(2, 17) + PIPEGAPV; //pipe Y position. 2-->17
  }
}

//Update the Pipes positions.
void updatePipes() {
  for (byte z = 0; z < 3; z++) {
    pipe[z].x -= difficulty_level; //the pipes move to the left.
    if (pipe[z].x <= -PIPEW) pipe[z].x = (LCDWIDTH ); //if a pipe is too far left, the pipe is relocated to the right.
    if (pipe[z].x >= LCDWIDTH) pipe[z].y = random(2, 17) + PIPEGAPV; //new random height.
    if (pipe[z].x == PLAYERX) updateScore(); //when 1 point is obtained.
  }
}

void drawPipes() {
  for (byte a = 0; a < 3; a++) {
    gb.display.setColor(GRAY);
    gb.display.fillRect(pipe[a].x, pipe[a].y, PIPEW, 24); //bottom pipe
    gb.display.fillRect(pipe[a].x, (pipe[a].y - PIPEH - PIPEGAPV), PIPEW, 24); //top pipe
    //highlights
    gb.display.setColor(WHITE);
    gb.display.fillRect(pipe[a].x, (pipe[a].y + 1), PIPEW, 1); //bottom pipe
    gb.display.fillRect(pipe[a].x + 2, pipe[a].y, 2, 24); //bottom pipe

    gb.display.fillRect(pipe[a].x, (pipe[a].y - PIPEGAPV - 1), PIPEW, 1); //top pipe
    gb.display.fillRect(pipe[a].x + 2, (pipe[a].y - PIPEH - PIPEGAPV), 2, 24); //top pipe
    //outline
    gb.display.setColor(BLACK);
    gb.display.drawBitmap(pipe[a].x, pipe[a].y, pipeBitmap); //bottom pipe
    gb.display.drawBitmap(pipe[a].x, (pipe[a].y - PIPEH - PIPEGAPV), pipeBitmap, NOROT, FLIPV); //top pipe
  }
}
//UPDATE Player position.
void playerMove() {
  //REGULAR GRAVITY
  //gravity += 0.5; //regular down.
  gravity += 0.18;
  gravity *= 0.95; //increasing friction.
  player_y += gravity; //update the player's Y position.

  //PRESS "A" BUTTON TO JUMP
  if (player_y > 0) {
    if (gb.buttons.pressed(BTN_A) || (gb.buttons.pressed(BTN_RIGHT)) || (gb.buttons.pressed(BTN_DOWN)) || (gb.buttons.pressed(BTN_LEFT)) || (gb.buttons.pressed(BTN_UP))) {
      if (gravity > -0.5) {//limit the maximum...
        gravity -= 2; //update the player's Y position.
      }
      playsoundfx(0, 0); //sound FX
      player_animation = 1; //wings animation.
    }
  }
  //COLLISION WITH TOP
  if (player_y < 0) {
    player_y = 1;
    gravity = 0;
  }
  //DEADFUL COLLISION...
  for (byte cc = 0; cc < 3; cc++) { //loop for pipes.
    if ( ((player_y + PLAYERH) > (LCDHEIGHT - GROUNDH))//...with GROUND
         || (gb.collideBitmapBitmap(PLAYERX, player_y, bird1Bitmap, pipe[cc].x, pipe[cc].y, pipeBitmap) ) //...with BOTTOM pipes.
         || (gb.collideBitmapBitmap(PLAYERX, player_y, bird1Bitmap, pipe[cc].x, pipe[cc].y - PIPEH - PIPEGAPV, pipeBitmap))) { //...with TOP pipes.
      playsoundfx(2, 0);
      player_death = true;
    }
  }
}
//Animation
void playerAnimation() {
  if ((player_animation > 0) && (player_animation < 4)) {
    player_animation++;
  } else player_animation = 0;
}

//DRAW animated player.
void drawPlayerAlive() {
  playerAnimation();
  //DRAW player sprites
  gb.display.setColor(WHITE); //white mask placed under the outline bitmap.
  if (player_animation < 2) {
    gb.display.drawBitmap(PLAYERX, player_y, bird1MaskBitmap);
  }
  else gb.display.drawBitmap(PLAYERX, player_y, bird2MaskBitmap);
  gb.display.setColor(BLACK); //outline
  if (player_animation > 2) {
    gb.display.drawBitmap(PLAYERX, player_y, bird1Bitmap);
  }
  else gb.display.drawBitmap(PLAYERX, player_y, bird2Bitmap);
}

//DRAW DEAD player
void drawPlayerEnd() {
  if ((player_death == true) && (player_y < (LCDHEIGHT - GROUNDH - PLAYERH))) player_y += 4; //the image move to the top.
  gb.display.setColor(WHITE); //white mask placed under the outline bitmap.
  gb.display.drawBitmap(PLAYERX, player_y, bird1MaskBitmap, ROTCW, NOFLIP);
  gb.display.setColor(BLACK); //outline
  gb.display.drawBitmap(PLAYERX, player_y, bird1Bitmap, ROTCW, NOFLIP);
}
//Update score
void updateScore() {
  if (score + 1 <= SCOREMAX) {
    score++; //+1 point.
    playsoundfx(1, 0); //sound FX.
  } else { //=score maximum!
    score = SCOREMAX;
  }
}

//DRAW score.
void drawScore() {
  gb.display.setFont(font3x5);
  if (score > 9) score_units = true; //the outline is larger is the score is 10 minimum. //no need for brackets here
  gb.display.setColor(BLACK);
  gb.display.fillRect(floor((LCDWIDTH - (3 * (1 + score_units))) / 2) - 1, 3, 5 + (4 * score_units), 7); //outline of the score.
  gb.display.cursorX = (floor(LCDWIDTH - (3 * (1 + score_units))) / 2); //X position of the score.
  gb.display.cursorY = 4; //Y position of the score.
  gb.display.setColor(WHITE);
  gb.display.println(score);//test
}

//HIGHSCORES update
void updateHighscore() {
  if ((difficulty_level == 0.5) && (score > pipe[0].hs)) pipe[0].hs = score;
  if ((difficulty_level == 1.0) && (score > pipe[1].hs)) pipe[1].hs = score;
  if ((difficulty_level == 2.0) && (score > pipe[2].hs)) pipe[2].hs = score;
}
//FX Synth script
void playsoundfx(int fxno, int channel) {
  gb.sound.command(0, soundfx[fxno][6], 0, channel); // set volume
  gb.sound.command(1, soundfx[fxno][0], 0, channel); // set waveform
  gb.sound.command(2, soundfx[fxno][5], -soundfx[fxno][4], channel); // set volume slide
  gb.sound.command(3, soundfx[fxno][3], soundfx[fxno][2] - 58, channel); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1], soundfx[fxno][7], channel); // play note
}

//MUTE ALL
void soundMute() {
  if (gb.buttons.pressed(BTN_B)) {
    switch (sound_volume) {
      case 0: sound_volume = true; break;
      case 1: sound_volume = false; break;
    }
  }
  gb.sound.setVolume(sound_volume, 0);
}
