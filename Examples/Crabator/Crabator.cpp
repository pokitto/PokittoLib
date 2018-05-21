//#include <SPI.h>
#include <Pokitto.h>
#include "Crabator.h"
Pokitto::Core gb;
//#include <EEPROM.h>
//#include <avr/pgmspace.h>

uint16_t hazepalette[16];

extern const byte font3x5[];
extern const byte font5x7[];

#define WORLD_W 16
#define WORLD_H 12
uint8_t  byteWidth = (WORLD_H + 7) / 8;

//declare all the sprites which are in the "sprites" tab
extern const byte PROGMEM logo[];
extern const byte PROGMEM world[];
extern const byte PROGMEM tiles[];
extern const byte PROGMEM mobSprite[];
extern const byte PROGMEM bossSprite[];
extern const byte PROGMEM playerSprite[];
extern const byte PROGMEM splashSprite[];
extern const byte PROGMEM crateSprite[];
extern const byte PROGMEM fullHeart[];
extern const byte PROGMEM halfHeart[];
extern const byte PROGMEM emptyHeart[];

#define playerW 6
#define playerH 6
byte playerSpeed;
int playerX;
int playerY;
int playerLife;
#define playerLifeMax 6
byte playerDir;
int cameraX;
int cameraY;
byte shake_magnitude;
byte shake_timeLeft;
const PROGMEM uint16_t player_damage_sound[] = {0x0045,0x564,0x0000};

///////////////////////////////////// MOBS
#define NUMMOBS 16
#define INITNUMMOBS 4
#define MOBSRATE 6 //how often is the mob number increased (every X kills)
#define BOSSFREQ 16//one boss every X kills (initially)
#define BOSSRATE 1 //every boss killed, the next one will spawn X kills earlier
byte boss_nextSpawn;
byte boss_freq;
byte activeMobs;
int  mobs_x[NUMMOBS];
int  mobs_y[NUMMOBS];
byte mobs_dir[NUMMOBS];
int8_t mobs_life[NUMMOBS]; // was char - Jonne
byte mobs_size[NUMMOBS];
byte mob_maxLife = 10;
byte boss_maxLife = 100;
#define boss_size 6
#define mob_size 4
//const PROGMEM uint16_t mob_damage_sound[] = {0x5C1F, 0x0000};
const PROGMEM uint16_t mob_death_sound[] = {0x0045,0x184,0x0000};

#define NUMSPLASH 16
boolean splash_active[NUMSPLASH];
int splash_x[NUMSPLASH];
int splash_y[NUMSPLASH];
int splash_dir[NUMSPLASH];

///////////////////////////////////// WEAPONS
#define NUMBULLETS 10
int     bullets_x[NUMBULLETS];
int     bullets_y[NUMBULLETS];
byte    bullets_dir[NUMBULLETS];
boolean bullets_active[NUMBULLETS];
byte    bullets_weapon[NUMBULLETS];

int     blast_x;
int     blast_y;
byte    blast_lifespan;
byte    blast_bullet;

#define NUMWEAPONS 5
byte currentWeapon;
byte nextShot;

const char str357[] PROGMEM = ".357";
const char strP90[] PROGMEM = "P90";
const char strAK47[] PROGMEM = "AK47";
const char strRPG[] PROGMEM = "RPG";
const char strMG42[] PROGMEM = "MG42";
const char*  const weapon_name[NUMWEAPONS] PROGMEM = {
  str357,strP90, strAK47, strRPG, strMG42};
const byte weapon_size[NUMWEAPONS] = {
  2, 1, 2, 3, 2};
const byte weapon_damage[NUMWEAPONS] = {
  10, 2, 3, 5, 4};
const byte weapon_rate[NUMWEAPONS] = {
  30, 1, 2, 30, 1};
const byte weapon_speed[NUMWEAPONS] = {
  4, 5, 3, 2, 5};
const byte weapon_spread[NUMWEAPONS] = {
  1, 2, 1, 0, 2};
const byte weapon_ennemyRecoil[NUMWEAPONS] = {
  3, 2, 3, 0, 3};
const byte weapon_playerRecoil[NUMWEAPONS] = {
  0, 0, 1, 3, 3};
const unsigned int weapon_ammo[NUMWEAPONS] = {
  9999, 500, 300, 20, 150};
unsigned int ammo;

const uint16_t magnum_sound[] PROGMEM = {0x0045,0x7049,0x17C,0x784D,0x42C,0x0000};
const uint16_t p90_sound[] PROGMEM = {0x0045, 0x0154, 0x0000};
const uint16_t p90_alternative_sound[] PROGMEM = {0x0045, 0x014C, 0x0000};
const uint16_t ak47_sound[] PROGMEM = {0x0045, 0x012C, 0x0000};
const uint16_t mg42_sound[] PROGMEM = {0x0045,0x140,0x8141,0x7849,0x788D,0x52C,0x0000};
const uint16_t rpg_sound[] PROGMEM = {0x0045,0x8101,0x7F30,0x0000};

const uint16_t* const weapons_sounds[NUMWEAPONS] PROGMEM= {
  magnum_sound, p90_sound, ak47_sound, rpg_sound, mg42_sound};
const uint16_t blast_sound[] PROGMEM = {0x0045,0x7849,0x784D,0xA28,0x0000};
int crate_x, crate_y;
const uint16_t power_up[] PROGMEM = {0x0005,0x140,0x150,0x15C,0x170,0x180,0x16C,0x154,0x160,0x174,0x184,0x14C,0x15C,0x168,0x17C,0x18C,0x0000};

///////////////////////////////////// SCORE
#define RANKMAX 5 //number of high scores to save
int score = 0;
//int lastScore = 0;
int kills = 0;
int highscore[RANKMAX];
//byte scoreDisplayTimeLeft;
#define NAMELENGTH 10
char name[RANKMAX][NAMELENGTH+1];

// predefinitions

void loadHighscore();
void initGame();
boolean collideWorld(int16_t, int16_t, uint8_t, uint8_t);
void damageMob(byte,byte);
boolean checkMobCollisions(byte);
boolean collideOtherMobs(byte);
void pausem();
void play();
void displayHighScores();
void saveHighscore();
void shakeScreen();
void drawWorld(int16_t, int16_t);
void displayScore();

/*int16_t random (int lo,int hi) {
    return rand() % hi + lo;
}*/

///////////////////////////////////// SETUP
void setup() {

    /** Set colors for this game **/

    /** TO ADD COLORS TO YOUR GAMEBUINO GAME, SIMPLY USE VALUES ABOVE 1 !!!! */

    gb.display.palette[1] = COLOR_WHITE;
    gb.display.palette[2] = COLOR_CYAN;
    gb.display.palette[3] = COLOR_MAGENTA;
    gb.display.palette[4] = COLOR_RED;
    gb.display.palette[8] = gb.display.RGBto565(0xff,0xfc,0); // small mob
    gb.display.palette[7] = gb.display.RGBto565(0x18,0x9a,0x61); // shrub shadow
    gb.display.palette[5] = gb.display.RGBto565(0x2c,0xff,0x0b); //world (shrubs)
    gb.display.palette[6] = gb.display.RGBto565(0xf7,0xb2,0);// crate
    gb.display.palette[9] = gb.display.RGBto565(0xfc,0x14,4);// big mob fc1404
    gb.display.palette[10] = gb.display.RGBto565(0,0x53,0xae); // blue shadow
    gb.display.palette[15] = gb.display.RGBto565(0xff,0xfd,0xbf); // bright sunlight

    /** YOU HAVE 16 COLORS FREELY SELECTABLE FROM 256000 COLORS !!! **/

    /** NOW LETS REBUILD! **/



for (uint16_t k =0 ; k<16; k++) hazepalette[k] = gb.display.interpolateColor(gb.display.palette[k],gb.display.palette[15],0);
    gb.display.paletteptr = hazepalette;
  pokInitSD(); // before game.begin if streaming
  gb.sound.playMusicStream("music/bitburnr.snd"); // before game.begin if streaming
  gb.begin();
  //while(1) gb.update();
  gb.display.setFont(font5x7);
  gb.titleScreen(logo);

  gb.pickRandomSeed();
  loadHighscore();
  gb.sound.playMusicStream();
  initGame();
}

///////////////////////////////////// LOOP
void loop() {
  play();
  if (!gb.isRunning()) return;
  pausem();
  if (!gb.isRunning()) return;
  gb.titleScreen(logo);
}

///////////////////////////////////// SCREEN COORD
boolean screenCoord(int absoluteX, int absoluteY, int &x, int &y){
  x = absoluteX - cameraX + 8;
  x = (x >= 0) ? x%(WORLD_W*8) : WORLD_W*8 + x%(WORLD_W*8);
  x -= 8;
  y = absoluteY - cameraY + 8;
  y = (y >= 0) ? y%(WORLD_H*8) : WORLD_H*8 + y%(WORLD_H*8);
  y -= 8;
  if((x > LCDWIDTH) || (y > LCDHEIGHT))
    return false;
  return true;
}

#define wrap(i, imax) ((imax+i)%(imax))

///////////////////////////////////// MOVE XYDS
void moveXYDS(int &x, int &y, byte &dir, int8_t speed){
  switch(dir){ //switch case depending on the mob's movement direction
  case 0: //going upward
    y -= speed;
    break;
  case 1: //left
    x -= speed;
    break;
  case 2: //downward
    y += speed;
    break;
  case 3: //right
    x += speed;
    break;
  }
  x = wrap(x, WORLD_W*8);
  y = wrap(y, WORLD_H*8);
}

///////////////////////////////////// DISTANCE BETWEEN
byte distanceBetween(int pos1, int pos2, int worldSize){
  byte dist = abs(pos1 - pos2);
  dist = (dist < worldSize/2) ? dist : worldSize - dist;
  return dist;
}

///////////////////////////////////// ASSIGN ARRAY
void assignArray(char *array1, char *array2, byte length){
  for(byte i=0; i<length; i++)
    array1[i] = array2[i];
}

///////////////////////////////////// SHOOT
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


void shoot(){
  if(ammo){
    if(nextShot == 0){
      for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
        if(!bullets_active[thisBullet]){ //look for the first inactive bullet
          bullets_active[thisBullet] = true; //set it to active as it's fired
          bullets_weapon[thisBullet] = currentWeapon;

          nextShot = weapon_rate[currentWeapon];
          ammo--;
          //spawn a bullet with some spreading
          int8_t spreadMax = weapon_spread[currentWeapon]; // this was char, jonne
          int8_t spreadMin = (weapon_size[currentWeapon]%2==0) ? -spreadMax : -spreadMax-1;
          bullets_x[thisBullet] = playerX + playerW/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;
          bullets_y[thisBullet] = playerY + playerH/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;

          bullets_dir[thisBullet] = playerDir;
          blast_bullet = thisBullet;
          if(((currentWeapon == 1)||(currentWeapon==4))&&(gb.frameCount%2))
          {
          }
          else{
            gb.sound.playPattern((uint16_t*)pgm_read_word(&(weapons_sounds[currentWeapon])), 0);
          }
          if(currentWeapon == 1){//with P90 cancel every two sounds to avoid continuous beep
            if(rand()%2)
              gb.sound.playPattern(p90_alternative_sound, 0);
          }
          //player recoil
          byte recoil = weapon_playerRecoil[currentWeapon];
          moveXYDS(playerX, playerY, playerDir, -recoil);
          for(byte i = 0; i<recoil; i++){
            if(collideWorld(playerX, playerY, playerW, playerH))
              moveXYDS(playerX, playerY, playerDir, 1);
            else
              break;
          }
          if(currentWeapon == 4){ //MG42
            shake_magnitude = 1;
            shake_timeLeft = 2;
          }
          break;
        }
      }
    }
  }
  else{
    currentWeapon = max(0, currentWeapon-1); //cut... no, magnum finally
    ammo = weapon_ammo[currentWeapon];
    nextShot = 20;
    gb.popup(("Out of ammo!"), 30);
  }
}

///////////////////////////////////// MOVE BULLETS
void moveBullets(){
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      byte s = weapon_size[bullets_weapon[thisBullet]];
      moveXYDS(bullets_x[thisBullet], bullets_y[thisBullet], bullets_dir[thisBullet], weapon_speed[bullets_weapon[thisBullet]]);

      //collide world
      if(collideWorld(bullets_x[thisBullet], bullets_y[thisBullet], s, s)){
        bullets_active[thisBullet] = false;
        if(bullets_weapon[thisBullet] == 3){ //RPG
          blast_x = bullets_x[thisBullet];
          blast_y = bullets_y[thisBullet];
          blast_lifespan = 8;
          gb.sound.playPattern(blast_sound, 0);
        }
        else{
        }
        continue;
      }

      for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //for each mob

        if(gb.collideRectRect(bullets_x[thisBullet], bullets_y[thisBullet], s, s,
        mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob])){

          if(bullets_weapon[thisBullet] == 3){ //RPG
            blast_x = bullets_x[thisBullet];
            blast_y = bullets_y[thisBullet];
            blast_lifespan = 8;
            gb.sound.playPattern(blast_sound, 0);
          }
          else {
            damageMob(thisMob, thisBullet);
          }
          bullets_active[thisBullet] = false;
          break;
        }
      }

    }
  }
}

///////////////////////////////////// EXPLODE
void explode(){
  if(blast_lifespan){
    blast_lifespan--;
    //gb.buzz(50+random(0,100),40);
    shake_magnitude = 4;
    shake_timeLeft = 2;
    //pick a random blast
    byte s = 10 + random (0,6);
    int x = blast_x + random(-4,4) -s/2;
    int y = blast_y + random(-4,4) -s/2;
    //damages
    for(byte thisMob=0; thisMob<activeMobs; thisMob++){
      if(gb.collideRectRect(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
      x,y,s,s))
        damageMob(thisMob,blast_bullet);
    }
    //display
    int x_screen, y_screen;
    if(screenCoord(x, y, x_screen, y_screen))
      gb.display.fillRect(x_screen, y_screen, s, s);
  }
}

///////////////////////////////////// DRAW BULLETS
void drawBullets(){
  gb.display.setColor(1);
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      int x, y;
      if(screenCoord(bullets_x[thisBullet], bullets_y[thisBullet], x, y)){
        byte s = weapon_size[bullets_weapon[thisBullet]];
        if(s==1)
          gb.display.drawPixel(x, y);
        else
          gb.display.fillRect(x, y, s, s);
      }
    }
  }
}

///////////////////////////////////// DRAW AMMO OVERLAY
void drawAmmoOverlay(){
  if(ammo){
    // text shadow
    /*
    gb.display.setColor(10);
    gb.display.cursorX = -1;
    gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight-1;
    gb.display.print((const __FlashStringHelper*)pgm_read_word(weapon_name+currentWeapon)); //some crazy casts
    gb.display.cursorX = -1;
    gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight+1;
    gb.display.print((const __FlashStringHelper*)pgm_read_word(weapon_name+currentWeapon)); //some crazy casts
    gb.display.cursorX = 1;
    gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight-1;
    gb.display.print((const __FlashStringHelper*)pgm_read_word(weapon_name+currentWeapon)); //some crazy casts
    gb.display.cursorX = +1;
    gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight+1;
    gb.display.print((const __FlashStringHelper*)pgm_read_word(weapon_name+currentWeapon)); //some crazy casts
    */
    // white on top
    gb.display.setColor(2);
    gb.display.cursorX = 0;
    gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight;
    gb.display.print((char*)(weapon_name[currentWeapon])); //some crazy casts

    if(nextShot>2)
      gb.display.fillRect(-2,LCDHEIGHT-2,nextShot,2);
    if(currentWeapon > 0){ //don't display the ammo of the cut
      byte xOffset = 0;
      if (ammo < 100)
        xOffset += gb.display.fontWidth;
      if (ammo < 10)
        xOffset += gb.display.fontWidth;
      gb.display.cursorX = LCDWIDTH-3*gb.display.fontWidth+xOffset;
      gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight;
      gb.display.print(ammo);
    }
    else {
      gb.display.cursorX = LCDWIDTH-3*gb.display.fontWidth;
      gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight;
      gb.display.print(("inf"));
    }
  }
}

///////////////////////////////////// SET SPLASH
void setSplash(int x, int y){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(!splash_active[thisSplash]){ //look for the first inactive splash
      splash_active[thisSplash] = true; //set it to active
      splash_x[thisSplash] = x;
      splash_y[thisSplash] = y;
      splash_dir[thisSplash] = random(0,5);
      break;
    }
  }
}

///////////////////////////////////// DRAW SPLASHES
void drawSplashes(){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(splash_active[thisSplash]){
      int x, y;
      if(screenCoord(splash_x[thisSplash], splash_y[thisSplash], x, y)){ //if the splash is in the screen
        //draw it
        gb.display.drawBitmap(x-2, y-2, splashSprite, splash_dir[thisSplash], NOFLIP);
      }
      else{ //erase it if it is out of the screen
        splash_active[thisSplash] = false;
      }
    }
  }
}


///////////////////////////////////// SPAWN CRATE
void spawnCrate(){
  boolean okay = false;
  while (okay == false){
    //pick a random location
    crate_x = random(0, WORLD_W) * 8;
    crate_y = random(0, WORLD_H) * 8;
    okay = true;
    //is that in a wall ?
    if(collideWorld(crate_x, crate_y, 8, 8)){
      okay = false;
    }
    //is that in the screen ?
    int x, y;
    if(screenCoord(crate_x, crate_y, x, y)){
      okay = false;
    }
  }
}

///////////////////////////////////// COLLIDE CRATE
void collideCrate(){
  if(gb.collideRectRect(crate_x+2, crate_y+2, 4, 4, playerX, playerY, playerW, playerH)){
    if (score <5){
      gb.popup(("Earn $5 first"), 30);
      return;
    }
    if(currentWeapon<(NUMWEAPONS-1)){
      gb.popup(("Upgraded !"), 30);
      gb.sound.playPattern(power_up,0);
    }
    else{
      gb.popup(("Refilled !"), 30);
    }
    score -= 5;
    spawnCrate();
    currentWeapon = min(NUMWEAPONS-1, currentWeapon+1); //upgrade to the next weapon
    ammo = weapon_ammo[currentWeapon];
    //gb.popup(weapon_name[currentWeapon], 30);
    //if(random(0,score/10)==0) //the higher is your score, the less life you will find in crates
    playerLife = min(playerLife+1, playerLifeMax);
    int hazefactor = (255-42*playerLife)*2/3;
    for (uint16_t k =0 ; k<16; k++) hazepalette[k] = gb.display.interpolateColor(gb.display.palette[k],gb.display.palette[4],hazefactor);
    //gb.buzz(2000,40);
  }
}

///////////////////////////////////// DRAW CRATE
void drawCrate(){
  int x, y;
  if(screenCoord(crate_x, crate_y, x, y)){
    gb.display.setColor(8);
    gb.display.drawBitmap(x, y, crateSprite);
    //gb.display.setColor(6);
    //gb.display.drawBitmap(x-1, y, crateSprite);
  }
}



///////////////////////////////////// SPAWN ONE MOB
boolean spawnMob(byte thisMob){
  boolean okay = false;
  byte timout = 0;
  mobs_size[thisMob] = mob_size;
  mobs_life[thisMob] = mob_maxLife;
  if(!boss_nextSpawn){ //spawn big mobs every 20 kills starting from 15
    boss_freq = max(boss_freq - BOSSRATE, 1);
    boss_nextSpawn = boss_freq;
    mobs_size[thisMob] = boss_size;
    mobs_life[thisMob] = boss_maxLife;
    //gb.popup("Boss spawned !", 30);
  }
  while(okay == false){ //do the following until it's okay
    //pick a random location
    mobs_x[thisMob] = random(0, WORLD_W*2) * 4;
    mobs_y[thisMob] = random(0, WORLD_H*2) * 4;
    //and check if that position is okay
    okay = true;

    if(checkMobCollisions(thisMob)){
      okay = false;
      continue;
    }
    //spawn the mobs out of the player's view
    if(wrap(mobs_x[thisMob] - cameraX, WORLD_W*8) < LCDWIDTH){
      okay = false;
      continue;
    }
    if(wrap(mobs_y[thisMob] - cameraY, WORLD_H*8) < LCDHEIGHT){
      okay = false;
      continue;
    }
  }
  mobs_dir[thisMob] = rand() % 4; //then pick a random direction
  return true;
}

///////////////////////////////////// SPAWN ALL MOBS
boolean spawnMobs(){
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //put mobs far away
    mobs_x[thisMob] = 9999;
    mobs_y[thisMob] = 9999;
  }
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){
    if(!spawnMob(thisMob)) //try to spawn a mob
        return false; //return false if an error occur
  }
  return true;
}

///////////////////////////////////// MOVE MOBS
void moveMobs(){
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //for each mob
    int x = wrap(mobs_x[thisMob] - cameraX, WORLD_W*8);
    int y = wrap(mobs_y[thisMob] - cameraY, WORLD_H*8);
    //if the mob is close to the screen
    if( (distanceBetween(mobs_x[thisMob], playerX, WORLD_W*8) < (LCDWIDTH+32)) &&  (distanceBetween(mobs_y[thisMob], playerY, WORLD_H*8) < (LCDHEIGHT+32))){
      moveXYDS(mobs_x[thisMob], mobs_y[thisMob], mobs_dir[thisMob], 1); //go forward

      //if there is a collision, move a step backward and pick a new random direction
      if(checkMobCollisions(thisMob)){
        moveXYDS(mobs_x[thisMob], mobs_y[thisMob], mobs_dir[thisMob], -1);
        mobs_dir[thisMob] = rand()%4;
        continue;
      }

      //go in a random direction
      if(random(0,32)==0){
        mobs_dir[thisMob] = rand()%4;
        continue;
      }

      //go in the direction on the player (randomly choose between X and Y axis)
      if(random(0,16)==0){
        if(random(0,2)){
          //get closer to the player on the X axis
          if((LCDWIDTH/2 - x) > 0){ //go to the left if the player is on the left
            mobs_dir[thisMob] = 3;
          }
          else{ // or go to the right if the player is on the right
            mobs_dir[thisMob] = 1;
          }
        }
        //if the distance between the player and the mob is larger on the Y axis
        else {
          //get closer to the player on the Y axis
          if((LCDHEIGHT/2 - y) > 0){ //go downward
            mobs_dir[thisMob] = 2;
          }
          else{ //go upward
            mobs_dir[thisMob] = 0;
          }
        }
      }
    }
  }
}

///////////////////////////////////// CHECK MOB COLLISIONS
boolean checkMobCollisions(byte thisMob){
  //check collision with the world
  if(collideWorld(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob]))
    return true;
  //check collision with other mobs
  if(collideOtherMobs(thisMob))
    return true;
  return false;
}


///////////////////////////////////// CHECK IF A MOB COLLIDE ANOTHER ONE
boolean collideOtherMobs(byte thisMob){
  for(byte otherMob=0; otherMob<activeMobs; otherMob++){
    if(thisMob == otherMob) //don't check collision with iself >_<'
      continue;
    if(gb.collideRectRect(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
    mobs_x[otherMob], mobs_y[otherMob], mobs_size[otherMob], mobs_size[otherMob])){
      return true;
    }
  }
  return false;
}

///////////////////////////////////// DRAW MOBS
void drawMobs(){
    gb.display.setColor(1); //8
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){
    //int x = wrap(mobs_x[thisMob] - cameraX + playerW/2, WORLD_W*8);
    //int y = wrap(mobs_y[thisMob] - cameraY + playerH/2, WORLD_H*8);
    int x, y;
    if(screenCoord(mobs_x[thisMob], mobs_y[thisMob], x, y)){
      if(mobs_size[thisMob] != boss_size)
      {
        gb.display.setColor(8);
        gb.display.drawBitmap(x-2, y-2, mobSprite, mobs_dir[thisMob], NOFLIP);
      } else
      { gb.display.setColor(9);
        gb.display.drawBitmap(x-1, y-1, bossSprite, mobs_dir[thisMob], NOFLIP);
      }
      //gb.fillRect(x, y, mobs_size[thisMob], mobs_size[thisMob], BLACK);
    }
  }
}

///////////////////////////////////// DAMAGE MOB
void damageMob(byte thisMob, byte thisBullet){
  mobs_life[thisMob] -= weapon_damage[bullets_weapon[thisBullet]];
  //recoil
  byte recoil = weapon_ennemyRecoil[bullets_weapon[thisBullet]];
  if(mobs_size[thisMob] == boss_size)
    recoil /= 4;
  moveXYDS(mobs_x[thisMob], mobs_y[thisMob], bullets_dir[thisBullet], recoil);
  if(checkMobCollisions(thisMob))
    moveXYDS(mobs_x[thisMob], mobs_y[thisMob], bullets_dir[thisBullet], -recoil);
  mobs_dir[thisMob] = (bullets_dir[thisBullet] + 2) % 4;
  //gb.buzz(1200,10);
  if(mobs_life[thisMob] <= 0){ //the mob dies
    score++;
    kills++;
    boss_nextSpawn--;
    if(bullets_weapon[thisBullet]!=3){ //if it's no the RPG
      gb.sound.playPattern(mob_death_sound,0);
    }
    if(mobs_size[thisMob] == boss_size)
      score += 4;
    setSplash(mobs_x[thisMob], mobs_y[thisMob]);
    int x, y;
    if(screenCoord(mobs_x[thisMob], mobs_y[thisMob], x, y)){
      gb.display.fillRect(x-1, y-1, mobs_size[thisMob]+1, mobs_size[thisMob]+1);
    }
    //gb.buzz(1400,20);
    spawnMob(thisMob);
    if(activeMobs < NUMMOBS){ //if the max isn't reached
      if(activeMobs < (kills/MOBSRATE)+INITNUMMOBS){ //every 8 mobs killed
        activeMobs++; //add a mob
        spawnMob(activeMobs-1); //spawn the mob added
      }
    }
  }
  else { //the mob survives
  }
}



#define PAUSEMENULENGTH 5
const char strPlay[] PROGMEM = "Play";
const char strRestart[] PROGMEM = "Restart";
const char strHighScores[] PROGMEM = "High scores";
const char strSystemInfo[] PROGMEM = "System Info";
const char strMainMenu[] PROGMEM = "Main Menu";


const char* const pauseMenu[PAUSEMENULENGTH] PROGMEM = {
  strPlay,
  strRestart,
  strHighScores,
  strSystemInfo,
  strMainMenu
};

///////////////////////////////////// PAUSE
void pausem(){
  while(gb.isRunning()){
    if(gb.update()){
      switch(gb.menu(pauseMenu, PAUSEMENULENGTH)){
      case 0: //resume
        gb.wait(100);
        gb.display.setFont(font3x5);
        play();
        gb.display.setFont(font5x7);
        gb.battery.show = true;
        break;
      case 1: //restart
        initGame();
        gb.display.setFont(font3x5);
        play();
        gb.display.setFont(font5x7);
        gb.battery.show = true;
        return;
      case 2: //high scores
        displayHighScores();
        break;
      case 3: //System info
        gb.display.setFont(font3x5);
        while (1) {
          if (gb.update()) {
            if (gb.buttons.pressed(BTN_C)) {
              gb.display.setFont(font5x7);
              gb.sound.playCancel();
              break;
            }
            //gb.display.setCursor(0, 0);
            gb.display.print(("Bat:"));
            gb.display.print(gb.battery.voltage);
            gb.display.println(("mV"));

            gb.display.print(("Bat lvl:"));
            gb.display.print(gb.battery.level);
            gb.display.println(("/4"));

            gb.display.print(("Light:"));
            gb.display.println(gb.backlight.ambientLight);

            gb.display.print(("Backlight:"));
            gb.display.println(gb.backlight.backlightValue);

            gb.display.print(("Volume:"));
            gb.display.print(gb.sound.getVolume());
            //gb.display.print(F("/"));
            //gb.display.println(gb.sound.volumeMax);

            gb.display.print("Mobs:");
            gb.display.print(activeMobs);
            gb.display.print("/");
            gb.display.println(NUMMOBS);

            gb.display.print("Killed:");
            gb.display.println(kills);
          }
        }
        break;
      case 4: //change game
        //gb.changeGame();
        gb.titleScreen(logo);
        break;
      default:
        return;
      }
    }
  }
}

///////////////////////////////////// DISPLAY HIGHSCORES
void displayHighScores(){
  while(true){
    if(gb.update()){
      gb.display.cursorX = 9+random(0,2);
      gb.display.cursorY = 0+random(0,2);
      gb.display.println(("HIGH SCORES"));
      gb.display.textWrap = false;
      gb.display.cursorX = 0;
      gb.display.cursorY = gb.display.fontHeight;
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
        if(highscore[thisScore]==0)
          gb.display.print('-');
        else
          gb.display.print(name[thisScore]);
        gb.display.cursorX = LCDWIDTH-3*gb.display.fontWidth;
        gb.display.cursorY = gb.display.fontHeight+gb.display.fontHeight*thisScore;
        gb.display.println(highscore[thisScore]);
      }
      if(gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
        gb.sound.playOK();
        break;
      }
    }
  }
}




///////////////////////////////////// INIT GAME
void initGame(){
  //lastScore = score;
  if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
    saveHighscore();
  }
  //scoreDisplayTimeLeft = 64;
  score = 0;
  kills = 0;
  currentWeapon = 0; //magnum
  ammo = 9999;
  nextShot = 0;
  shake_timeLeft = 0;
  playerLife = playerLifeMax;
  boss_freq = BOSSFREQ;
  boss_nextSpawn = boss_freq;
  activeMobs = INITNUMMOBS; //6 initial mobs
  do{
    do{
      playerX = random(0, WORLD_W) * 8;
      playerY = random(0, WORLD_H) * 8;
    }
    while(collideWorld(playerX, playerY, playerW, playerH));
    cameraX = playerX - LCDWIDTH/2 + playerW/2;
    cameraY = playerY - LCDHEIGHT/2 + playerW/2;
  }
  while(!spawnMobs()); //do that until mobs are spawned without error
  //spawn crate
  spawnCrate();
  //reset bullets
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    bullets_active[thisBullet] = false;
  }
  //reset splashes
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    splash_active[thisSplash] = false;
  }
  blast_lifespan = 0; //reset explosion
}

///////////////////////////////////// PLAY
void play(){
  gb.battery.show = false;
  byte i = 0;
  while(i < 10 && gb.isRunning()){
    if(gb.update()){
      gb.display.fontSize = 2;
      gb.display.cursorX = 6;
      gb.display.cursorY = 16;
      gb.display.print(("LET'S GO!"));
      i++;
    }
  }
  gb.display.fontSize = 1;
  gb.popup(("\x15:shoot \x16:run"), 60);

  while(gb.isRunning()){
    if(gb.update()){
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        return;
        //gb.battery.show = true;
        //pause();
        //gb.battery.show = false;
      }
      boolean moved = false;
      if(gb.buttons.repeat(BTN_RIGHT, 1)){
        playerDir = 3;
        moved = true;
      }
      else{
        if(gb.buttons.repeat(BTN_LEFT, 1)){
          playerDir = 1;
          moved = true;
        }
      }
      if(gb.buttons.repeat(BTN_DOWN, 1)){
        playerDir = 2;
        moved = true;
      }
      else{
        if(gb.buttons.repeat(BTN_UP, 1)){
          playerDir = 0;
          moved = true;
        }
      }
      if(moved){
        moveXYDS(playerX, playerY, playerDir, playerSpeed);
        if(collideWorld(playerX, playerY, playerW, playerH))
          moveXYDS(playerX, playerY, playerDir, -playerSpeed);
      }
      cameraX = playerX + playerW/2 - LCDWIDTH/2;
      cameraY = playerY + playerH/2 - LCDHEIGHT/2;
      shakeScreen();
      /*
      gb.display.setColor(7); // shrub shadow
      drawWorld(cameraX-3, cameraY-3);
      gb.display.setColor(5); // shrubs
      drawWorld(cameraX, cameraY);
      */
      int x, y;
      screenCoord(playerX, playerY, x, y);
      //gb.display.setColor(10); //shadow
      int8_t ox=0,oy=0;
      ox--;
      //gb.display.drawBitmap(x+ox, y+oy, playerSprite, playerDir, NOFLIP);
      gb.display.setColor(3); //magenta
      gb.display.drawBitmap(x, y, playerSprite, playerDir, NOFLIP);
      byte thisSprite = 0;
      moveMobs();
      drawMobs();
      if(nextShot)
        nextShot--;
      if(gb.buttons.repeat(BTN_A, 1) && !gb.buttons.repeat(BTN_B, 1)){
        shoot();
      }
      if (gb.buttons.repeat(BTN_B, 1)){
        playerSpeed = 2;
      }
      else {
        playerSpeed = 1;
      }
      moveBullets();
      drawBullets();
      explode();
      gb.display.setColor(4);
      drawSplashes();
      collideCrate();
      drawCrate();
      /** DRAW WORLD**/
      //gb.display.setColor(7); // shrub shadow
      //drawWorld(cameraX-2, cameraY-2);
      gb.display.setColor(5); // shrubs
      drawWorld(cameraX, cameraY);
      //life remaining
      for(byte i=0; i<=playerLifeMax/2; i+=1){
        if((i*2)<=playerLife){
          gb.display.setColor(4);
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
        }
        else{
          gb.display.setColor(0,0);
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
          gb.display.setColor(4,0);
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, emptyHeart);
        }
      }
      if(!playerLife){
        if((gb.frameCount%2)==0){
          shake_magnitude = 2;
          shake_timeLeft = 1;
        }
      }
      else{
        if(playerLife == 1){
          shake_magnitude = 1;
          shake_timeLeft = 1;
        }
      }

      if(playerLife%2){ //odd number
        gb.display.setColor(4,0);
        gb.display.drawBitmap(LCDWIDTH-(playerLife/2+1)*9+2, 0, halfHeart);
      }

      drawAmmoOverlay();

      displayScore();
      for(byte thisMob=0; thisMob<activeMobs; thisMob++){
        if(gb.collideRectRect(mobs_x[thisMob],mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
        playerX, playerY, playerW, playerH)){
          playerLife--;
          //pokConsoleAddMessage(MSG_PRINT,V_INT16,playerLife);
          shake_magnitude = 2;
          shake_timeLeft = 4;
          if(mobs_size[thisMob] == boss_size){
            playerLife--;
            shake_magnitude = 3;
            shake_timeLeft = 4;
          }
          //int hazefactor = (255-42*playerLife)*2/3;
          //for (uint16_t k =0 ; k<16; k++) hazepalette[k] = pokInterpolateColor(palette[k],palette[4],hazefactor);
          gb.sound.playPattern(player_damage_sound, 0);
          spawnMob(thisMob);
          if(playerLife < 0){
            byte timer=0;
            while(1){
              if(gb.update()){
                //gb.display.setColor(1);
                drawMobs();
                drawBullets();
                drawSplashes();
                drawCrate();
                drawAmmoOverlay();
                displayScore();
                drawWorld(cameraX, cameraY);
                gb.display.drawBitmap(x-1, y-1, playerSprite, playerDir, NOFLIP);
                gb.display.setColor(WHITE);
                gb.display.fillRect(0,0,timer*2,LCDHEIGHT);
                gb.display.fillRect(LCDWIDTH-timer*2,0,timer*2,LCDHEIGHT);
                gb.display.setColor(BLACK, WHITE);
                gb.display.cursorX = 12;
                gb.display.cursorY = 1;
                gb.display.print(("GAME OVER!"));
                timer++;
                if(timer==((LCDWIDTH/4)+10))
                  break;
              }
              //int hazefactor = 0;
              //for (uint16_t k =0 ; k<16; k++) hazepalette[k] = pokInterpolateColor(palette[k],palette[4],hazefactor);
            }
            while(1){
              if(gb.update()){
                if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
                  gb.display.cursorX = 2+rand()%2;
                  gb.display.cursorY = 0+rand()%2;
                  gb.display.print(("NEW HIGHSCORE"));
                }
                else{
                  gb.display.cursorX = 12;
                  gb.display.cursorY = 1;
                  gb.display.print(("GAME OVER!"));
                }
                gb.display.cursorX = 0;
                gb.display.cursorY = 12;
                gb.display.print(("You made $"));
                gb.display.print(score);
                gb.display.print(("\nby killing\n"));
                gb.display.print(kills);
                gb.display.print((" crabs."));
                gb.display.cursorX = 0;
                gb.display.cursorY = 40;
                gb.display.print(("\x15:accept"));
                if(gb.buttons.pressed(BTN_A)){
                  gb.sound.playOK();
                  break;
                }
              }
            }
            initGame();
            break;
          }
        }
      }

    }
  }
}

///////////////////////////////////// DISPLAY SCORE
void displayScore(){
  // blue on bottom
  /* gb.display.setColor(10);
  gb.display.cursorX = -1;
  gb.display.cursorY = -1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 0;
  gb.display.cursorY = -1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 1;
  gb.display.cursorY = -1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = -1;
  gb.display.cursorY = 0;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 0;
  gb.display.cursorY = 1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = -1;
  gb.display.cursorY = 1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 0;
  gb.display.cursorY = 1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 1;
  gb.display.cursorY = 1;
  gb.display.print('$');
  gb.display.println(score);
  gb.display.cursorX = 1;
  gb.display.cursorY = 0;
  gb.display.print('$');
  gb.display.println(score);*/
  // white on top
  gb.display.setColor(8);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print('$');
  gb.display.println(score);
}

///////////////////////////////////// SHAKE SCREEN
void shakeScreen(){
  if(shake_timeLeft){
    shake_timeLeft--;
    cameraX += random(-shake_magnitude,shake_magnitude+1);
    cameraY += random(-shake_magnitude,shake_magnitude+1);
    byte backlightStep = gb.backlight.backlightMax / 4;
    gb.backlight.set(gb.backlight.backlightValue-random(0,backlightStep*shake_magnitude));
  }
}

///////////////////////////////////// LOAD HIGHSCORE
void loadHighscore(){
  /*
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      name[thisScore][i] = EEPROM.read(i + thisScore*(NAMELENGTH+2));
    }
    highscore[thisScore] = EEPROM.read(NAMELENGTH + thisScore*(NAMELENGTH+2)) & 0x00FF; //LSB
    highscore[thisScore] += (EEPROM.read(NAMELENGTH+1 + thisScore*(NAMELENGTH+2)) << 8) & 0xFF00; //MSB
    highscore[thisScore] = (highscore[thisScore]==0xFFFF) ? 0 : highscore[thisScore];
  }*/ //jonnehw
}

///////////////////////////////////// SAVE HIGHSCORE
void saveHighscore(){
  //gb.getDefaultName(name[RANKMAX-1]);
  gb.display.setFont(font5x7);
  gb.keyboard(name[RANKMAX-1], NAMELENGTH+1);
  highscore[RANKMAX-1] = score;
  for(byte i=RANKMAX-1; i>0; i--){ //bubble sorting FTW
    if(highscore[i-1] < highscore[i]){
      char tempName[NAMELENGTH];
      strcpy(tempName, name[i-1]);
      strcpy(name[i-1], name[i]);
      strcpy(name[i], tempName);
      unsigned int tempScore;
      tempScore = highscore[i-1];
      highscore[i-1] = highscore[i];
      highscore[i] = tempScore;
    }
    else{
      break;
    }
  }
  /*
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      EEPROM.write(i + thisScore*(NAMELENGTH+2), name[thisScore][i]);
    }
    EEPROM.write(NAMELENGTH + thisScore*(NAMELENGTH+2), highscore[thisScore] & 0x00FF); //LSB
    EEPROM.write(NAMELENGTH+1 + thisScore*(NAMELENGTH+2), (highscore[thisScore] >> 8) & 0x00FF); //MSB
  }*/// jonnehw
  displayHighScores();
}



const byte PROGMEM logo[] =
{
  64,36, //width and height
  B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
  B10000000, B00000000, B00000000, B00000000, B00000000, B00000001, B01000001, B01000001,
  B10000000, B00000000, B00000001, B10000000, B00000110, B00000100, B00010100, B00010001,
  B10000000, B00000000, B00000001, B10000000, B00000110, B00000000, B10000000, B10000001,
  B10000000, B00000000, B00000001, B10000000, B00000110, B00000000, B00000000, B01000001,
  B10001111, B00111111, B01111001, B10111001, B11101111, B10011110, B01111110, B00010001,
  B10011001, B10111011, B11001101, B11111011, B00110110, B00110011, B01110110, B00100001,
  B10011001, B10110011, B00001101, B10011000, B00110110, B00110011, B01100110, B01000101,

  B10011000, B00110000, B01111101, B10011001, B11110110, B00110011, B01100000, B00000011,
  B10011000, B00110000, B11001101, B10011011, B00110110, B00110011, B01100001, B01000001,
  B10011000, B00110000, B11001101, B10011011, B00110110, B00110011, B01100100, B00010101,
  B10011001, B10110000, B11011101, B10011011, B01110110, B00110011, B01100000, B10001001,
  B11001111, B00110000, B11111101, B11110011, B11110011, B10011110, B01100000, B01000001,
  B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000101, B00010101,
  B10100000, B00000000, B00000000, B00000000, B00000000, B00100000, B10000000, B10100001,
  B11000000, B00000000, B00011000, B00001100, B00000000, B00000000, B00001111, B11110101,

  B10010000, B00000000, B00100100, B01010010, B10000000, B11110000, B00001000, B00010011,
  B11010000, B00000010, B10101100, B01010110, B10000001, B11111000, B00001111, B11110001,
  B10011001, B01000011, B00110100, B11011010, B11100001, B11111000, B00001010, B00110101,
  B10010101, B01000010, B00100101, B01010010, B10000001, B11111000, B00001100, B01011001,
  B11011101, B11000010, B00011001, B11001100, B01100001, B01101000, B00001111, B11110001,
  B10000000, B01000000, B00000000, B00000000, B00000000, B11110000, B00001000, B00010101,
  B10000000, B10000000, B00000000, B00000000, B00000000, B00000000, B00001111, B11110001,
  B11000000, B00000000, B00000000, B00000000, B00000110, B00000000, B00000000, B00000101,

  B10000000, B00000000, B00000000, B00000000, B00001111, B00000000, B00000000, B00000011,
  B11000000, B00000000, B00000000, B00000000, B00000110, B00000000, B00000000, B00000001,
  B10010000, B00000000, B00000000, B00000000, B00001001, B00000000, B00000000, B00000101,
  B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001001,
  B11000001, B01000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001,
  B10010011, B11010000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000101,
  B10100011, B11100000, B00000000, B00000000, B00000000, B00000110, B00000000, B00000001,
  B10000011, B11100000, B00000101, B01000101, B01000000, B00001111, B00000101, B01000101,

  B10000011, B11010000, B00000010, B00000010, B00000000, B00000110, B00000010, B00000011,
  B10000001, B01000000, B00000001, B01000001, B01000000, B00001001, B00000001, B01000001,
  B10000000, B00000000, B00000100, B00010100, B00010000, B00000000, B00000100, B00010101,
  B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
};

const byte PROGMEM world[]=
{
  B11001111,B11001111,
  B10000000,B01000000,
  B00000001,B11111100,
  B00111001,B11100000,
  B00001100,B00000110,
  B10000111,B00011111,
  B11000011,B11000001,
  B10000001,B00000001,
  B00111100,B00110011,
  B00000111,B11100000,
  B10000100,B00001001,
  B11000001,B11001111,
};
const byte PROGMEM tiles[]=
{
  8,8,
  B01010100,
  B00100000,
  B00010100,
  B01000001,
  B10001000,
  B00000100,
  B01010001,
  B00001010,
};
const byte PROGMEM mobSprite[]=
{
  8,8,
  B00000000,
  B00000000,
  B00011000,
  B00111100,
  B00011000,
  B00100100,
  B00000000,
  B00000000,
};
const byte PROGMEM bossSprite[]=
{
  8,8,
  B00000000,
  B00100100,
  B00011000,
  B01111110,
  B00111100,
  B01111110,
  B00111100,
  B00000000,
};
const byte PROGMEM playerSprite[]=
{
  8,8,
  B00000000,
  B00111100,
  B01011010,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00000000,
};
const byte PROGMEM splashSprite[]=
{
  8,8,
  B00000000,
  B00001000,
  B00000000,
  B00011000,
  B00111000,
  B10110100,
  B00000000,
  B00010000,
};
const byte PROGMEM crateSprite[]=
{
  8,8,
  B11111111,
  B10000001,
  B11111111,
  B10100011,
  B11000101,
  B11111111,
  B10000001,
  B11111111,
};
const byte PROGMEM fullHeart[]=
{
  8,8,
  B01101100,
  B11111110,
  B11111110,
  B01111100,
  B00111000,
  B00010000,
  B00000000,
  B00000000,
};
const byte PROGMEM halfHeart[]=
{
  8,8,
  B00000000,
  B00001100,
  B00011100,
  B00011000,
  B00010000,
  B00000000,
  B00000000,
  B00000000,
};
const byte PROGMEM emptyHeart[]=
{
  8,8,
  B01101100,
  B10010010,
  B10000010,
  B01000100,
  B00101000,
  B00010000,
  B00000000,
  B00000000,
};



///////////////////////////////////// GET TILE
boolean getTile(uint8_t i, uint8_t j){

  uint8_t test;
  test = (pgm_read_byte(world + (j%WORLD_H)*byteWidth + (i%WORLD_W)/8) & (B10000000 >> (i % 8)));
  if(test)
    return true;
  else
    return false;
}

///////////////////////////////////// DRAW WORLD
void drawWorld(int16_t x, int16_t y){
  int8_t i, j,
  w = WORLD_W,
  h = WORLD_H;
  x = wrap(x,w*8);
  y = wrap(y,h*8);
  for(j=y/8; j < (LCDHEIGHT/8 + y/8 + 1); j++) {
    for(i=x/8; i < (LCDWIDTH/8 + x/8 + 1); i++ ) {
      if(getTile(i, j)) {
        gb.display.drawBitmap(i*8 - x, j*8 - y, tiles);
      }
    }
  }
}

///////////////////////////////////// COLLIDE WORLD
boolean collideWorld(int16_t x, int16_t y, uint8_t w, uint8_t h){
  if(getTile(x/8, y/8))
    return true;
  if(getTile((x+w-1)/8, y/8))
    return true;
  if(getTile((x+w-1)/8, (y+h-1)/8))
    return true;
  if(getTile(x/8, (y+h-1)/8))
    return true;
  return false;
}

