// music - 8bit archade4 from www.dl-sounds.com

#include "pokitto.h"
#include "tetris_gfx.h"
#include "easing.h"

#define REPSPEED 12

Pokitto::Core game;

#define HELD 0
#define NEW 1
#define RELEASE 2
byte CompletePad, ExPad, TempPad, myPad;
bool _A[3], _B[3], _C[3], _Up[3], _Down[3], _Left[3], _Right[3];
bool bgNum = 1;
char musicName[] = "music/ozzed.snd";
byte palNum = 0;

int topLine;
byte unlockedPal=0;
bool removeLine[19];
byte linesToRemove=0;
byte animCount;
bool splodeOK=0;
byte animSplode=0;

void UPDATEPAD(int pad, int var) {
  _C[pad] = (var >> 1)&1;
  _B[pad] = (var >> 2)&1;
  _A[pad] = (var >> 3)&1;
  _Down[pad] = (var >> 4)&1;
  _Left[pad] = (var >> 5)&1;
  _Right[pad] = (var >> 6)&1;
  _Up[pad] = (var >> 7)&1;
}

void UpdatePad(int joy_code){
  ExPad = CompletePad;
  CompletePad = joy_code;
  UPDATEPAD(HELD, CompletePad); // held
  UPDATEPAD(RELEASE, (ExPad & (~CompletePad))); // released
  UPDATEPAD(NEW, (CompletePad & (~ExPad))); // newpress
}

byte updateButtons(byte var){
   var = 0;
   if (game.buttons.cBtn()) var |= (1<<1);
   if (game.buttons.bBtn()) var |= (1<<2);
   if (game.buttons.aBtn()) var |= (1<<3);
   if (game.buttons.downBtn()) var |= (1<<4);
   if (game.buttons.leftBtn()) var |= (1<<5);
   if (game.buttons.rightBtn()) var |= (1<<6);
   if (game.buttons.upBtn()) var |= (1<<7);

   return var;
}



// some globals...
long int frameNumber = 0;
long int myDelay;
long int tempTime;
int dropTime = 0;
int slideTime = 0;
byte gameMode = 0;
bool paused = 0;
byte menuItem;
bool bgmusic = 1;

byte px, py, ps, pr; // player x,y,shape,rotation
byte nextTile;
byte lineCount, level;
int score, lines;
bool okToContinue = 0;
byte slideSpeed = 10;


void loadPal(char num){
    unsigned short curPal[4];
    curPal[0] = pallet[(num*4)];
    curPal[1] = pallet[(num*4)+1];
    curPal[2] = pallet[(num*4)+2];
    curPal[3] = pallet[(num*4)+3];
    game.display.load565Palette(curPal);
}

void drawShape(byte x1, signed char y1, byte shape, byte frame) {
  for (char y = 0; y < 4; y++) {
    if (y1 + y > 1) {
      for (char x = 0; x < 4; x++) {
        byte mt = pgm_read_byte(shapeMap + (x + 4 * y) + shape * 64 + (frame * 16));
        if (mt > 1) {
          game.display.drawBitmap((x1 + x)*8, (y1 + y)*8, tile_gfx[(mt-1)+shape*6]);
        }
      }
    }
  }
}

bool check(signed char x1, signed char y1, char rot) {
    byte ret=0;
  for (char y = 0; y < 4; y++) {
    if (y1 + y >= 0) {
      for (char x = 0; x < 4; x++) {
        byte mt = pgm_read_byte(shapeMap + (x + 4 * y) + ps * 64 + (rot * 16));
        if (mt > 1) {
          if ( playfield[(x1 + x) + 16 * (y1 + y)] != 0) {
            ret= 1;
          }
        }
      }
    }
  }
  return ret;
}

void stamp(signed char x1, signed char y1, byte shape, byte rot) {
  for (char y = 0; y < 4; y++) {
    if (y1 + y >= 0) {
      for (char x = 0; x < 4; x++) {
        byte mt = pgm_read_byte(shapeMap + (x + 4 * y) + shape * 64 + (rot * 16));
        if (mt > 1) {
          playfield[(x1 + x) + 16 * (y1 + y)] = (mt-1)+shape*6;
        }
      }
    }
  }
}


void clearPlayfield() {
  for (char y = 0; y < 19; y++) {
    for (char x = 3; x < 13; x++) {
      playfield[x + 16 * y] = 0;
    }
  }
}

void drawBackground(){
  for (char y = 0; y < 22; y++) {
    for (char x = 0; x < 28; x++) {
      byte mt = bg_map[x + 28 * y + (616*bgNum)];
      game.display.drawBitmap(x*8, y*8, bg_gfx[mt]);
    }
  }
}

void drawPlayfield() {
    drawBackground();

  for (char y = 1; y < 19; y++) {
    for (char x = 3; x < 13; x++) {
      byte mt = playfield[x + 16 * y];
      game.display.drawBitmap((x + 5)*8, (y+1 )*8, tile_gfx[mt]);
    }
  }

    if(animSplode==0){
        // current shape
        drawShape(px+5, py+1, ps, pr);
    }
    // next shape?
    drawShape(19, 14, nextTile, 0);

    char text[] = "        ";
    sprintf(text, "%05d", score);
    game.display.setCursor(152,56);
    game.display.color=3;
    game.display.print(text);
    sprintf(text, "%5d", level);
    game.display.setCursor(16,96);
    game.display.print(text);
    sprintf(text, "%5d", lines);
    game.display.setCursor(16,48);
    game.display.print(text);

}



void checkLine() {

    for(char t=0; t<19; t++){
        removeLine[t]=0;
    }

  if (py <= 0) {
    loadPal(1); // default green palette
    gameMode = 3;  // gameOver if off top of screen
    frameNumber=0;
    return;
  }

  score++; // increase score here as it's called whenever a tile drops
  topLine = 0;

  for (char y = 0; y < 19; y++) {
    char line = 0;
    for (char x = 3; x < 13; x++) {
      line += playfield[x + 16 * y] != 0 ? 1 : 0;
    }
    if (line == 10) { // remove line
        removeLine[y]=1;
        linesToRemove++;
        if(linesToRemove==4){splodeOK=1;}
        lineCount++;
        if (lineCount == 10) {
            lineCount = 0;
            level++;
        }
        lines++;
        score += 10;
        }
    }

  for (char y = 0; y < 10; y++) {
    char line = 0;
    for (char x = 3; x < 13; x++) {
      topLine += playfield[x + 16 * y] != 0 ? 1 : 0;
    }
  }


    // mess with the palette

      int percent = 100;
      int diff = topLine;

      unsigned short *p;
      palNum = 0;//level & 31;
      p=pallet+(palNum*4);
      unsigned short curPal[4];

      curPal[0] = pallet[(palNum*4)];
      curPal[1] = pallet[(palNum*4)+1];
      curPal[2] = pallet[(palNum*4)+2];
      curPal[3] = pallet[(palNum*4)+3];

     int greyPal[] = {0xF800,0xF8000,0xF800,0xF800}; // it's actually RED for danger!

    unsigned short red[4], green[4], blue[4], red1[4], green1[4], blue1[4], red2[4], green2[4], blue2[4];

    for(char t=0; t<4; t++){
        red1[t] = (curPal[t]>>11) & 31;
        red2[t] = (greyPal[t]>> 11) & 31;
        green1[t] = (curPal[t]>> 5) & 63;
        green2[t] = (greyPal[t]>> 5) & 63;
        blue1[t] = curPal[t] & 31;
        blue2[t] = greyPal[t] & 31;

        red[t] = red1[t]+((red2[t]-red1[t])*diff/percent);
        green[t] = green1[t]+((green2[t]-green1[t])*diff/percent);
        blue[t] = blue1[t]+((blue2[t]-blue1[t])*diff/percent);

        curPal[t] = (red[t]<<11)+(green[t]<<5)+blue[t];

    }

    game.display.load565Palette(curPal);

}


// transparent 2bit bitmap with mask
void drawMyBitmap(int16_t x, int16_t y, const uint8_t* bitmap, const uint8_t* mask)
{
    int16_t w = *bitmap;
	int16_t h = *(bitmap + 1);
    bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
    /** visibility check */
    if (y<-h || y>game.display.height) return; //invisible
    if (x<-w || x>game.display.width) return;  //invisible

    /** 2 bpp mode */
    int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
    for (i = 0; i < w; i++) {
        byteNum = i / 4;
        bitNum = (i % 4)<<1;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            uint8_t source2 = *(mask + j * byteWidth + byteNum+2);
            uint8_t output = (source & (0xC0 >> bitNum));
            output >>= (6-bitNum);

            uint8_t output2 = (source2 & (0xC0 >> bitNum));
            output2 >>= (6-bitNum);

            if (output2 != 0) {
                game.display.setColor(output);
                game.display.drawPixel(x + i, y + j);
            }
        }

    //return;
    }
}



void titleScreen(){

  // background
  for (char y = 0; y < 22; y++) {
    for (char x = 0; x < 28; x++) {
      byte mt = bg_map[x + 28 * y];
      game.display.drawBitmap(x*8, y*8, bg_gfx[mt]);
    }
  }

int y=48;
    if(frameNumber<=64){
                        // time, start, distance, duration
        #ifdef POK_SIM
        y = easeOutBounce(frameNumber, -48, 48+48, 64);
        #else
        y = easeOutBounce(frameNumber*4, -48, 48+48, 64);
        if (y>48) y=48;
        #endif
    }
    drawMyBitmap(16, y, title_bitmap, title_mask);


    char text[] = " Press A to Start ";
    game.display.setCursor(40,120);
    game.display.color=3;
    game.display.print(text);

  if(_A[NEW]){
    // make sure the playfield is clear!
      for (char y = 18; y > 0; y--) {
        for (char x = 3; x < 13; x++) {
          playfield[x + 16 * y] = 0;
        }
      }

    loadPal(0); // default green palette
    gameMode = 1;
  }

}

void gameOver(){
  // background
  for (char y = 0; y < 22; y++) {
    for (char x = 0; x < 28; x++) {
      byte mt = bg_map[x + 28 * y];
      game.display.drawBitmap(x*8, y*8, bg_gfx[mt]);
    }
  }

    int y=48;
    if(frameNumber<=64){
                        // time, start, distance, duration
        y = easeOutBounce(frameNumber, -48, 48+48, 64);
    }
    drawMyBitmap(1, y, gameover_bitmap, gameover_mask);

    char text[] = " Press A ";
    game.display.setCursor(62,120);
    game.display.color=3;
    game.display.print(text);

  if(_A[NEW]){
    gameMode = 0;
    frameNumber = 0;
    score=0;
    lines=0;
    level=0;
    splodeOK=0;
    animSplode=0;
  }

}

void playGame(){
    #ifdef POK_SIM
    #define SLIDECOUNT 6
    #define DROPCOUNT 20
    #else
    #define SLIDECOUNT 1
    #define DROPCOUNT 2
    #endif

    if(linesToRemove==0 && animSplode==0){

        if (_Left[NEW]) {
          if (check(px - 1, py, pr) == 0) {
            px--;
            slideTime = 0;
          }
        }
        if (_Right[NEW]) {
          if (check(px + 1, py, pr) == 0) {
            px++;
            slideTime = 0;
          }
        }
        if (_Left[HELD] && slideTime++ > SLIDECOUNT) {
          if (check(px - 1, py, pr) == 0) {
            px--;
            slideTime = 12;
          }
        }
        if (_Right[HELD] && slideTime++ > SLIDECOUNT) {
          if (check(px + 1, py, pr) == 0) {
            px++;
            slideTime = 12;
          }
        }

        if ((_Down[HELD]) || (dropTime++ > DROPCOUNT - (level * 2))) {
          dropTime = 0;
          if (check(px, py+1, pr) == 0) {
            py++;
          } else {
            // place shape and create new one
            stamp(px, py, ps, pr);
            checkLine();
            py = 0; px = 6; ps = nextTile; nextTile = random(6); pr = 0;
          }
        }
        if (_Up[NEW] && splodeOK==1) {
                splodeOK=0;
                animSplode=1;
        }

        if (_A[NEW]) {
          if (check(px, py, (pr - 1) & 3) == 0) {
            pr--;
          } else if (check(px - 1, py, (pr - 1) & 3) == 0) {
            pr--; px--;
          } else if (check(px + 1, py, (pr - 1) & 3) == 0) {
            pr--; px++;
          } else if (check(px - 2, py, (pr - 1) & 3) == 0) {
            pr--; px -= 2;
          } else if (check(px + 2, py, (pr - 1) & 3) == 0) {
            pr--; px += 2;
          }
          pr &= 3;
        }

        if (_B[NEW]) {
          if (check(px, py, (pr + 1) & 3) == 0) {
            pr++;
          } else if (check(px - 1, py, (pr + 1) & 3) == 0) {
            pr++; px--;
          } else if (check(px + 1, py, (pr + 1) & 3) == 0) {
            pr++; px++;
          } else if (check(px - 2, py, (pr + 1) & 3) == 0) {
            pr++; px -= 2;
          } else if (check(px + 2, py, (pr + 1) & 3) == 0) {
            pr++; px += 2;
          }
          pr &= 3;
        }

        animCount=0;
    }

        if(linesToRemove!=0){
            // remove some lines
            for(byte t=0; t<19; t++){
                if(removeLine[t]==1){
                    if(animCount<5){
                        for (char x = 3; x < 13; x++) {
                            playfield[x + 16 * t] = 5+animCount*6;
                        } // x
                    }else{
                        removeLine[t]=0;
                        linesToRemove--;
                        for (char y1 = t; y1 > 0; y1--) {
                            for (char x = 3; x < 13; x++) {
                                playfield[x + 16 * y1] = playfield[x + 16 * (y1 - 1)];
                            }
                        }
                    }
                }
            }
            animCount++;
        }

        if(animSplode!=0){

            if(animSplode<6){
                for (char y = 0; y < 4; y++) {
                    if (py + y >= 0) {
                        for (char x = 0; x < 4; x++) {
                            byte mt = pgm_read_byte(shapeMap + (x + 4 * y) + ps * 64 + (pr * 16));
                            if (mt > 1) {
                                playfield[(px + x) + 16 * (py + y)] = 5+animSplode*6;
                            }
                        }
                    }
                }
                animSplode++;
            }else{

                for (char y = 0; y < 4; y++) {
                    if (py + y >= 0) {
                        for (char x = 0; x < 4; x++) {
                            byte mt = pgm_read_byte(shapeMap + (x + 4 * y) + ps * 64 + (pr * 16));
                            if (mt > 1) {
                                playfield[(px + x) + 16 * (py + y)] = 0;
                            }
                        }
                    }
                }
                py = 0; px = 6; ps = nextTile;
                nextTile = random(6); pr = 0;
                animSplode=0;
            }
        }

    // render screen
    drawPlayfield();
}




int main(){
    game.initRandom();
    game.sound.playMusicStream(musicName); //load the file to buffers before game.begin
	game.begin();
	game.sound.playMusicStream(); //use without parameters to trigger
	game.display.width = 220; // full size
	game.display.height = 176;
    game.display.setFont(fontC64);
    game.display.adjustCharStep = 0; //needed for the non-proportional C64 font (normal value=1)
    game.display.fixedWidthFont = true;

    loadPal(1); // default green palette

    px=6;
    gameMode=0;
    char oldMode = 50;
    myDelay=40;

    frameNumber=0;
    animCount=0;
    splodeOK=0;
    animSplode=0;


   while (game.isRunning()) {

        // if it is time to update the screen
        if (game.update()){

            frameNumber++;
            game.sound.updateStream();
            game.buttons.update();
            myPad = updateButtons(myPad);
            UpdatePad(myPad);

            switch (gameMode) {
            case 0:
              titleScreen();
                break;
            case 1:
                if (paused) {
            //        pauseMenu();
                } else {
                    playGame();
                }
                break;
            case 3:
              gameOver();
              break;
            }

        }
    }
    return 0;
}
