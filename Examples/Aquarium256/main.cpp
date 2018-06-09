
#include "PokittoMicro.h"

#include "Pokitto.h"

#include "bitmaps.h"

using namespace PokittoMicro;

uint32_t noise( uint32_t jsr ){
  jsr ^= jsr << 17;
  jsr ^= jsr >> 13;
  jsr ^= jsr << 5;
  return jsr;
}

int seed = 0xF00DBEEF;

const int flyCount = 10;

struct Fly {
  float x, y, sx, sy;
  const uint8_t *sprite;
} flies[ flyCount ];

Update stateSplash( bool wasInit ){

  for( int i=0; i<flyCount; ++i ){
    auto &fly = flies[i];

    fly.x += fly.sx;
    fly.y += fly.sy;

    int d = (fly.x - 110) * (fly.x - 110) + (fly.y - 88) * (fly.y - 88);

    if( d > 800 ){

      if( fly.x < 110 ) fly.sx+=0.1;
      else if( fly.x > 110 ) fly.sx-=0.1;
      if( fly.y < 88 ) fly.sy+=0.1;
      else if( fly.y > 88 ) fly.sy-=0.1;

    }else{

      fly.sx += random(0, 200) / 100.0f - 1.0f;
      fly.sy += random(0, 200) / 100.0f - 1.0f;

    }

    fly.sx *= 0.98;
    fly.sy *= 0.98;

    blit( fly.x, fly.y, fly.sprite, (fly.sx < 0)?BlitOp::FLIP_HORIZONTAL:BlitOp::NORMAL );

  }

  return stateSplash;
}

int main () {

  begin(60);

  load8BitPalette(palette);

  setTileImage( 0, fish1_tile8 );
  setTileImage( 1, fish2_tile8 );
  setTileImage( 2, fish3_tile8 );
  setTileImage( 3, fish4_tile8 );
  setTileImage( 4, fish5_tile8 );
  setTileImage( 5, fish13_tile8 );

  for( int y=0; y<12; ++y ){
    for( int x=0; x<15; ++x ){
      map[y*15+x] = y < 7 ? 5 : ( y > 7 ? ( (noise(seed*(x*47+y*16))&0xFF)>200?3+(x&1):0) : ( 1+(x&1) ) );
    }
  }

  setTransparentColor( 0 );

  const uint8_t *sprites[] = {
    fish6_sprite8,
    fish7_sprite8,
    fish8_sprite8,
    fish9_sprite8,
    fish10_sprite8,
    fish11_sprite8,
    fish12_sprite8
  };

  for( int i=0; i<flyCount; ++i ){
    flies[i].x = random(0, 220);
    flies[i].y = random(0, 176);
    flies[i].sx = flies[i].sy = 0;
    flies[i].sprite = sprites[i%7];
  }

  update( stateSplash );

}

