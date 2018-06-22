#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include "Defines.h"
#include "Sprites.h"
#include "Level.h"
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
