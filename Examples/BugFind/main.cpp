#include "PokittoCore.h"
#include "PokittoPalettes.h"
#include "tiles.h"

Pokitto::Core gb;

#define VEL_MAX (2)
#define FRICTION 1

#define WORLD_SIZE 128
uint8_t world[WORLD_SIZE][WORLD_SIZE];

int16_t vx = 0;
int16_t camx = 32-3;
int16_t camy = 8;

void world_step();
void world_draw(int16_t x, int16_t y);

void setup(){
  gb.begin();
  gb.display.loadRGBPalette(palettePico);
  gb.display.setColor(0,0);
  gb.display.setInvisibleColor(0);
  gb.setFrameRate(30);
  for( uint8_t i = 0; i < WORLD_SIZE; i++ ){
    for( uint8_t j = 0; j < WORLD_SIZE; j++ ){
      world[i][j] = 1;
    }
  }
}

void loop(){
  if( gb.update() ){
    gb.display.clear();
    gb.display.setColor(12);
    gb.display.fillRect(0,0,110,88);
    gb.buttons.pollButtons();
    world_step();
    world_draw(camx,camy);
  }
}

void world_step(){
  if( vx > 0 ) vx -= FRICTION;
  if( vx < 0 ) vx += FRICTION;
  if( gb.buttons.repeat(BTN_LEFT,1) ){
    vx -= 2;
    if( vx < -VEL_MAX ) vx = -VEL_MAX;
  }else if( gb.buttons.repeat(BTN_RIGHT,1) ){
    vx += 2;
    if( vx > VEL_MAX ) vx = VEL_MAX;
  }

  camx += vx;
}

void world_draw(int16_t x, int16_t y){
    int16_t w,h;
    for( h = (y/8); h < (y/8)+16; h++ ){
      for( w = (x/8); w < (x/8)+16; w++ ){
      //for( w = (x/8)+13; w < (x/8)+16; w++ ){
        if( h >=0 && h < WORLD_SIZE ){
          if( w < 0 )
            gb.display.drawBitmap( (w-x/8)*8-x%8, (h-y/8)*8-y%8, &tiles[world[h][w+WORLD_SIZE]*34]);
          else
            gb.display.drawBitmap( (w-x/8)*8-x%8, (h-y/8)*8-y%8, &tiles[world[h][w%WORLD_SIZE]*34]);
        }
      }
    }
}

int main(int argc, char** argv){
  setup();
  while( gb.isRunning() ){
    loop();
  }
  return 0;
}
