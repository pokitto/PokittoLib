/* This file was automatically parsed from an Arduino sourcefile  */
/* by PokittoParser v0.1 Copyright 2016 Jonne Valola              */
/* USE AT YOUR OWN RISK - NO GUARANTEES GIVEN OF 100% CORRECTNESS */
#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include "Defines.h"
#include "Sprites.h"
#include "Level.h"
#include "Pokitto.h"
/* Auto-generated function declarations */
void setup();
void loop();
void initGame();
void newGame();
void initWorld(byte);
byte getBits(byte,byte);
void setTile(byte);
void nextLevel();
void loadEEPROM();
void cleanEEPROM();
void saveEEPROM();
void monster_die(byte,byte);
void moveMonster(byte,byte,byte);
void sIND(byte,byte,byte);
byte getNewDirection(byte,byte);
void updatePhysics();
void falling(byte,byte,byte);
void moveRock(char,char);
void movePlayer(char,char);
void handlePlayer(char,char);
void die();
void updateCursor();
void drawWorld();
Gamebuino gb;

const char strLoad[] PROGMEM = "Load";
const char strSave[] PROGMEM = "Save";
const char strExitMenu[] PROGMEM = "Resume";
const char strNewGame[] PROGMEM = "New Game";
const char lvlUp[] PROGMEM = "Level up";
const char lvlDown[] PROGMEM = "Level down";
const char strMode[] PROGMEM = "Toggle Mode";


//Put all the different items together in a menu (an array of strings actually)
const char* const menu[MENULENGTH] PROGMEM = {
  strNewGame,
  strExitMenu,
  strLoad,
  strSave,
  lvlUp,
  lvlDown,
  strMode,
};

byte world[WORLD_W][WORLD_H];



// cursor
int cursor_x, cursor_y;
int camera_x, camera_y;

byte diams_need, diamonds;
byte curLevel = 0;
byte maxlevel = 0;
byte maxhard = 0;
byte curhard = 0;
byte maxeasy = 0;
byte cureasy = 0;
byte lives = LIVES;
bool dead = false;
bool gamemode = EASY;
byte gamestate = RUNNING;
int  codepos = 0;
int  worldpos = 0;
byte dir = 7;
byte timer = 0;
void setup()
{
  gb.begin();
  initGame();
  gb.battery.show = false;
}

void loop() {
  if (gb.update()) {
    switch (gamestate) {
      case PAUSED:
        switch (gb.menu(menu, MENULENGTH)) {
          case -1: //nothing selected
            gb.titleScreen(F("DIGGER"));
            break;
          case 3: //save
            if (lives > 0) {
              lives--;
              saveEEPROM();
              gamestate = RUNNING;
              dead = false;
            }
            break;
          case 2: //load game
            loadEEPROM();
            gamestate = RUNNING;
            dead = false;
            initWorld(curLevel);
            break;
          case 1: //resume game
            gamestate = RUNNING;
            break;
          case 0: //new game
            newGame();

            initWorld(curLevel);
            break;
          case 4: //lvlup
            gamestate = RUNNING;
            dead = false;
            if (curLevel < ((gamemode == EASY) ? maxeasy : maxhard)) nextLevel();
            break;
          case 5: //lvldown
            if (curLevel > 0) {
              (gamemode == EASY) ? cureasy-- : curhard--;
              curLevel--;
            }
            dead = false;
            gamestate = RUNNING;
            initWorld(curLevel);
            break;
          case 6: //gamemode
            gamemode = !gamemode;
            curLevel = (gamemode == EASY) ? cureasy : curhard;
            dead = false;
            initWorld(curLevel);
            gamestate = RUNNING;
            break;
          default:
            break;
        }; break;

      case RUNNING:
        //pause the game if C is pressed
        if (gb.buttons.pressed(BTN_C)) {
          gamestate = PAUSED;
        }
        updateCursor();
        updatePhysics();
        drawWorld();
        if (lives <1 || lives>LIVES) {
          //curLevel=NUM_LEVELS-2;
          gamestate=GAMEOVER;
        }
        break;
      case GAMEOVER:
        //initWorld(NUM_LEVELS-2);
        newGame();
        //saveEEPROM();
        dead = false;
        gamestate = RUNNING;
        break;
      default:
        break;
    }
  }
}

void initGame() {
  gb.titleScreen(F("DIGGER"));
  loadEEPROM();
  initWorld(curLevel);
}

void newGame() {
  curLevel = 0;
  lives = LIVES;
  maxeasy = 0;
  cureasy = 0;
  maxhard = 0;
  curhard = 0;
  gamestate = RUNNING;
  dead = false;
}
void initWorld(byte _level) {
  diamonds = 0;
  codepos = 0;
  worldpos = 0;
  diams_need = getBits(_level, 8);
  while (worldpos < WORLD_H * WORLD_W) {
    if (getBits(_level, 1)) {
      byte r = getBits(_level, BITREF);
      byte c = getBits(_level, BITCOUNT) + MINMATCH;

      for (byte i = 0; i < c; i++) {
        if (worldpos < WORLD_W * WORLD_H) {
          int refpos = worldpos - r;
          setTile(world[refpos % WORLD_W][refpos / WORLD_W]);
          worldpos++;
        }
      }
    } else {
      setTile(getBits(_level, CODELENGTH));
      worldpos++;
    }

  }
}

byte getBits(byte _level, byte bits) {
  //uses global var codepos for bitoffset
  const byte *pos = levels[_level] + codepos / 8;
  int w = pgm_read_byte(pos) * 256 + pgm_read_byte(pos + 1);
  w >>= (16 - (codepos % 8) - bits); //shift bits to right
  w = ( w & ((1 << bits) - 1)); //mask desired bits
  codepos += bits;
  return (byte)w;
}

void setTile( byte sprite) {
  byte x = worldpos % WORLD_W;
  byte y = worldpos / WORLD_W;
  switch (sprite) {
    case MONSTER_RT: world[x][y] = MONSTER_R | TYPE_RT; break;
    case MONSTER_LT: world[x][y] = MONSTER_L | TYPE_LT; break;
    case MONSTER_D:  world[x][y] = MONSTER_D | TYPE_UD; break;
    case MONSTER_U:  world[x][y] = MONSTER_U | TYPE_UD; break;
    case MONSTER_L:  world[x][y] = MONSTER_L | TYPE_LR; break;
    case MONSTER_R:  world[x][y] = MONSTER_R | TYPE_LR; break;
    default: world[x][y] = sprite; break;
  }
  if (sprite == PLAYER) {
    cursor_x =  x;
    cursor_y = y;
  }
}
void nextLevel() {
  if (curLevel < NUM_LEVELS-3) {
    (gamemode == EASY) ? cureasy++ : curhard++;
    maxeasy = max(maxeasy, cureasy);
    maxhard = max(maxhard, curhard);
    curLevel++;
  }
  initWorld(curLevel);
}

void loadEEPROM() {
  if (EEPROM.read(0) != EEPROMTOKEN1 || EEPROM.read(1) != EEPROMTOKEN2 || EEPROM.read(2) != EEPROMTOKEN3) {
    cleanEEPROM;
    return;
  }

  gamemode = EEPROM.read(3);
  cureasy = EEPROM.read(4);
  maxeasy = EEPROM.read(5);
  lives   = EEPROM.read(6);
  curhard = EEPROM.read(8);
  maxhard = EEPROM.read(9);
  maxeasy = max(maxeasy, cureasy);
  maxhard = max(maxhard, curhard);
  curLevel = (gamemode == EASY) ? cureasy : curhard;
  maxlevel = (gamemode == EASY) ? maxeasy : maxhard;
}
void cleanEEPROM() {
  for (int i = 0; i < 1024; i++) {
    if (EEPROM.read(i)) {
      EEPROM.write(i, 0);
    }
  }
}

void saveEEPROM() {
  EEPROM.write(0, EEPROMTOKEN1);
  EEPROM.write(1, EEPROMTOKEN2);
  EEPROM.write(2, EEPROMTOKEN3);
  EEPROM.write(3, gamemode);
  EEPROM.write(4, cureasy);
  EEPROM.write(5, maxeasy);
  EEPROM.write(6, lives);
  EEPROM.write(8, curhard);
  EEPROM.write(9, maxhard);


}

/*
"C:\Program Files\Arduino\hardware\tools\avr/bin/avr-objdump" -h -S "C:\Users\xxxxxxx\AppData\Local\Temp\buildxxxxxxxxx.tmp\Digger.cpp.elf" > digger.lst
*/
void monster_die(byte x, byte y) {
  if (   world[x - 1][y - 1] == PLAYER
         || world[x - 1][y] == PLAYER
         || world[x - 1][y + 1] == PLAYER
         || world[x][y - 1] == PLAYER
         || world[x][y + 1] == PLAYER
         || world[x + 1][y - 1] == PLAYER
         || world[x + 1][y] == PLAYER
         || world[x + 1][y + 1] == PLAYER
     ) die();
  sIND(x - 1, y + 1, EMPTY);
  sIND(x  , y + 1, EMPTY);
  sIND(x + 1, y + 1, EMPTY);
  if (gamemode == EASY) {
    world[x][y] = DIAMOND;
    sIND(x - 1, y - 1, DIAMOND);
    sIND(x - 1, y  , DIAMOND);
    sIND(x  , y - 1, DIAMOND);
    sIND(x + 1, y - 1, DIAMOND);
    sIND(x + 1, y  , DIAMOND);
  } else {
    world[x][y] = EMPTY;
    sIND(x - 1, y - 1, EMPTY);
    sIND(x - 1, y  , EMPTY);
    sIND(x  , y - 1, EMPTY);
    sIND(x + 1, y - 1, EMPTY);
    sIND(x + 1, y  , EMPTY);
  }

}

void moveMonster(byte x, byte y, byte sprite) {
  if (gb.frameCount % 10 == 0) {
    if (world[x - 1][y] > EMPTY && world[x + 1][y] > EMPTY && world[x][y - 1] > EMPTY && world[x][y + 1] > EMPTY) return; //no way out

    //straight forward preferred, if not: search new direction
    if (sprite == MONSTER_D) {
      switch (world[x][y + 1] & SPRITEMASK) {
        case EMPTY: world[x][y + 1] = world[x][y] | MOVINGSTATE; world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_U)) {
      switch (world[x][y - 1] & SPRITEMASK ) {
        case EMPTY: world[x][y - 1] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_L)) {
      switch (world[x - 1][y] & SPRITEMASK ) {
        case EMPTY: world[x - 1][y] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_R)) {
      switch (world[x + 1][y] & SPRITEMASK ) {
        case EMPTY: world[x + 1][y] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }

  }
}
void sIND(byte x, byte y, byte sprite) { //setIfNotWorldBorder
  if (x > 0 && x < WORLD_W - 1 && y > 0 && y < WORLD_H - 1)   world[x][y] = sprite;
}

byte getNewDirection(byte x, byte y) {
  byte type = world[x][y] & TYPEMASK;
  byte sprite = world[x][y] & SPRITEMASK;
  switch (type) {
    case TYPE_UD:
      sprite = (sprite == MONSTER_D) ? MONSTER_U : MONSTER_D;
      break;
    case TYPE_LR:
      sprite = (sprite == MONSTER_R) ? MONSTER_L : MONSTER_R;
      break;
    case TYPE_RT:
      while (!testMove(x, y, sprite)) {
        sprite++;
        if (sprite > MONSTER_R) sprite = MONSTER_D;
      }
      break;
    case TYPE_LT:
      while (!testMove(x, y, sprite)) {
        sprite--;
        if (sprite < MONSTER_D) sprite = MONSTER_R;
      }
      break;
  }
  return sprite + type;
}

bool testMove(byte x, byte y, byte monster) {
  switch (monster) {
    case MONSTER_U:
      if (world[x][y - 1] == PLAYER) die();
      return (world[x][y - 1] == EMPTY) ? true : false; break;

    case MONSTER_L:
      if (world[x - 1][y] == PLAYER) die();
      return (world[x - 1][y] == EMPTY) ? true : false; break;
    case MONSTER_D:
      if (world[x][y + 1] == PLAYER) die();
      return (world[x][y + 1] == EMPTY) ? true : false; break;
    case MONSTER_R:
      if (world[x + 1][y] == PLAYER) die();
      return (world[x + 1][y] == EMPTY) ? true : false; break;
    default:
      return true; break;
  }
}
void updatePhysics() {
  //Skip borders in Check
  if (gb.frameCount % 5 == 0) {
    for (byte y = WORLD_H - 2; y > 0; y--) {
      for (byte x = 1; x < WORLD_W - 1; x++) {
        byte sprite = world[x][y] & ~FALLINGSTATE & ~TYPEMASK;
        switch (sprite) {
          case ROCK: falling(x, y, ROCK); break; //ROCK
          case DIAMOND: falling(x, y, DIAMOND); break; //diam
          case MONSTER_D: moveMonster(x, y, MONSTER_D); break; //mon_down
          case MONSTER_U: moveMonster(x, y, MONSTER_U); break; //mon_up
          case MONSTER_L: moveMonster(x, y, MONSTER_L); break;
          case MONSTER_R: moveMonster(x, y, MONSTER_R); break;

          default: break;
        }
      }
    }
    for (byte y = WORLD_H - 2; y > 0; y--) {
      for (byte x = 1; x < WORLD_W - 1; x++) {
        world[x][y] &= ~MOVINGSTATE;
      }
    }
  }
}



void falling(byte x, byte y, byte sprite) {
  if ( (world[x][y] & FALLINGSTATE) > 0) {
    switch (world[x][y + 1] & SPRITEMASK) {
      case PLAYER: die(); break;
      case MONSTER_D: monster_die(x, y + 1);return; break;
      case MONSTER_U: monster_die(x, y + 1);return; break;
      case MONSTER_L: monster_die(x, y + 1);return; break;
      case MONSTER_R: monster_die(x, y + 1);return; break;
      default: break;
    }
  }

  world[x][y] &= ~FALLINGSTATE;
  if (world[x][y + 1] == EMPTY) {
    world[x][y + 1] = sprite;
    world[x][y] = EMPTY;
    world[x][y + 1] |= FALLINGSTATE;
    return;
  }
  if (world[x][y + 1] == ROCK || world[x][y + 1] == DIAMOND) {
    if (world[x + 1][y + 1] == EMPTY && world[x + 1][y] == EMPTY) {
      world[x + 1][y + 1] = sprite;
      world[x][y] = EMPTY;
    } else if (world[x - 1][y + 1] == EMPTY && world[x - 1][y] == EMPTY) {
      world[x - 1][y + 1] = sprite;
      world[x][y] = EMPTY;
    }
    return;
  }
  if ((sprite == ROCK) && (world[x][y + 1] == CHANGER) && (world[x][y + 2] == EMPTY)) {
    world[x][y] = EMPTY;
    world[x][y + 2] = DIAMOND | FALLINGSTATE;
  }

}
void moveRock(char xoff, char yoff) {
  if (yoff != 0) {
    gb.sound.playTick();
    return;
  }
  if (gb.buttons.held(BTN_RIGHT, 8) && world[cursor_x + 2][cursor_y] == EMPTY ) {
    world[cursor_x + 2][cursor_y] = ROCK;
    movePlayer(1, 0);
    gb.sound.playCancel();
  }
  if (gb.buttons.held(BTN_LEFT, 8) && world[cursor_x - 2][cursor_y] == EMPTY ) {
    world[cursor_x - 2][cursor_y] = ROCK;
    movePlayer(-1, 0);
    gb.sound.playCancel();
  }
}
void movePlayer(char xoff, char yoff) {
  world[cursor_x][cursor_y] = EMPTY;
  cursor_x = cursor_x + xoff;
  cursor_y = cursor_y + yoff;
  world[cursor_x][cursor_y] = PLAYER;
}
void handlePlayer(char xoff, char yoff) {
  dir = 7 + xoff * 2 + yoff;
  timer = 5;
  byte target = world[cursor_x + xoff][cursor_y + yoff];
  target &= ~FALLINGSTATE;
  switch ( target ) {
    case EMPTY: movePlayer(xoff, yoff); break; //empty
    case WALL: gb.sound.playTick(); break; //wall
    case ROCK: moveRock(xoff, yoff); break; //ROCK
    case SOIL: movePlayer(xoff, yoff); break; //soil
    case DIAMOND: movePlayer(xoff, yoff); diamonds++; gb.sound.playOK(); break; //diamond
    case CHANGER: gb.sound.playTick(); break; //changer
    case DOOR:
      if (diams_need <= diamonds) {
        movePlayer(xoff, yoff);
        gb.sound.playOK();
        nextLevel();

      } else {
        gb.sound.playTick();
      }
      break;
    default: die(); //monsters or Player (wich shouldn't happen) :(
  }
}
void die() {
  byte x = cursor_x; 
  byte y = cursor_y;
  world[x][y] = DEATH;
  dead = true;
  lives--;
  
}

void updateCursor() {
  if (gb.buttons.repeat(BTN_A, 4) && !dead) {
    gb.sound.playOK();
    die();
  }

  if (gb.buttons.repeat(BTN_B, 4) && dead ) {
    gb.sound.playCancel();
    initWorld(curLevel);
    dead = false;
  }
  if (!dead) {
    if (gb.buttons.repeat(BTN_RIGHT, 4)) {
      handlePlayer(1, 0);
    }
    if (gb.buttons.repeat(BTN_LEFT, 4)) {
      handlePlayer(-1, 0);
    }
    if (gb.buttons.repeat(BTN_DOWN, 4)) {
      handlePlayer(0, 1);
    }
    if (gb.buttons.repeat(BTN_UP, 4)) {
      handlePlayer(0, -1);
    }
  }
  //target position of the camera for the cursor to be centered
  int camera_x_target = cursor_x * 6 - LCDWIDTH / 2 + 3;
  int camera_y_target = cursor_y * 6 - LCDHEIGHT / 2 + 3;
  //apply the target coordinate to the current coordinates with some smoothing
  camera_x = (camera_x * 3 + camera_x_target) / 4;
  camera_y = (camera_y * 3 + camera_y_target) / 4;

}


void drawWorld() {
  for (byte y = 0; y < WORLD_H; y++) {
    for (byte x = 0; x < WORLD_W; x++) {
      //coordinates on the screen depending on the camera position
      int x_screen = x * SPRITE_W - camera_x;
      int y_screen = y * SPRITE_H - camera_y;
      byte sprite = world[x][y];
      sprite &= SPRITEMASK;
      if (sprite == DIAMOND && gb.frameCount % 2) {
        sprite = DIAMOND2;
      }
      if (sprite == PLAYER) sprite = PLAYER + dir;
      if (x_screen < -SPRITE_W || x_screen > LCDWIDTH || y_screen < 0 || y_screen > LCDHEIGHT) {
        continue; // don't draw sprites which are out of the screen
      }
      gb.display.drawBitmap(x_screen, y_screen, sprites[sprite], 0, 0);
    }
  }
  if (timer == 0) dir = 7; timer--;
  gb.display.setColor(gamemode ? BLACK : WHITE);
  gb.display.fillRect(0, 0, LCDWIDTH, SPRITE_H);
  gb.display.setColor(gamemode ? WHITE : BLACK);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print(F("\04:"));
  gb.display.print(diamonds);
  gb.display.print(F("/"));
  gb.display.print(diams_need);
  gb.display.print(F(" \03:"));
  gb.display.print(lives);
  gb.display.print(F(" L:"));
  gb.display.print(curLevel + 1);
}

