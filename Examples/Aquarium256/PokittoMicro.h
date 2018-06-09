#ifndef _POKITTO_MICRO
#define _POKITTO_MICRO

#define PROJ_TILEDMODE 1
#define POK_BITFRAME 0
// #define PROJ_DEVELOPER_MODE 1
#define PROJ_STREAMING_MUSIC 0
// #define PROJ_GBSOUND 1
#define PROJ_HIRES 0
#define DISABLE_SOUND_WARNING
#define SCROLL_TEXT 1
#define PROJ_ENABLE_SOUNDS 0

namespace PokittoMicro {

  enum class BlitOp {
    NORMAL=0,
    FLIP_HORIZONTAL=1,
    FLIP_VERTICAL=2
  };

  struct Update;
  typedef Update (*UpdateFunc)( bool wasInit );

  struct Update
  {

    UpdateFunc p;

    Update( UpdateFunc pp ) : p( pp ) { }
    operator UpdateFunc() { return p; }

  };  

  extern unsigned char map[];

  
  void begin( unsigned int frameRate=60 );
  void load8BitPalette( const unsigned short *pal );
  void setTransparentColor( unsigned char color );
  void setTileImage( unsigned char id, const unsigned char *tile );
  void blit( int x, int y, const unsigned char *sprite, BlitOp op );
  void update( UpdateFunc );
  
}

#endif
