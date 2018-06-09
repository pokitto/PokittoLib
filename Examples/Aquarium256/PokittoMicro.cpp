#include "PokittoMicro.h"

#include "Pokitto.h"

using namespace Pokitto;

void write_command_16(uint16_t data);
void write_data_16(uint16_t data);

static void lcdrefresh();

const int MAX_SPRITES = 11;

static uint32_t *palette = reinterpret_cast<uint32_t *>(0x20000000);
const unsigned char ** tiles = reinterpret_cast<const unsigned char **>(0x20000000+256*4);

namespace PokittoMicro {

  const uint8_t *cmds[220][MAX_SPRITES];
  uint8_t map[15*12] = {0};
  uint8_t transparentColor;

  inline const uint8_t *packptr( const uint8_t *ptr, uint8_t data, uint8_t flags ){
 
    union {
      const uint8_t *ptr;
      uint32_t u32;
      uint8_t u8[4];
    } u;

    u.ptr = ptr;
    u.u8[2] |= flags<<4;
    u.u32 = (u.u32<<8) + data;

    return u.ptr;
    
  }

  inline uint8_t getdata( const uint8_t *ptr ){
    union {
      const uint8_t *ptr;
      uint8_t u8[4];
    } u;
    u.ptr = ptr;
    return u.u8[0];
  }

  inline void unpack( const uint8_t *ptr, int32_t &data, int32_t &flags, const uint8_t *&raw ){

    union {
      const uint8_t *ptr;
      uint32_t u32;
      uint8_t u8[4];
    } u;

    u.ptr = ptr;
    
    data = u.u8[0];

    flags = u.u8[3]>>4;

    u.u8[3] &= 0x0F;
    
    u.u32 >>= 8;
    raw = u.ptr;
    
  }

  void clear(){
    const uint8_t *ptr = packptr( nullptr, 176, 0 );
    for( int i=0; i<220; ++i ){
      for( int j=0; j<10; ++j )
	cmds[i][j] = ptr;
    }
  }

  void setTransparentColor( unsigned char color ){
    transparentColor = color;
  }  
  
  void load8BitPalette( const unsigned short *pal ){
    uint32_t *out = palette;
    for( int i=0; i<256; ++i ){
      *out++ = uint32_t(*pal++)<<3;
    }
  }

  void setTileImage( unsigned char id, const unsigned char *tile ){
    tiles[id] = tile;
  }

  uint32_t startTime;
  uint32_t frameEnd;
  uint32_t frameTime;
  
  void begin( unsigned int frameRate ){
    
    Core::begin();
    
    *reinterpret_cast<uint32_t *>(0x40048080) |= 3 << 26;

    Display::enableDirectPrinting(true);
    
    clear();


    frameTime = 1000 / frameRate;
    
    setWindow( 0, 0, POK_LCD_H-1, POK_LCD_W-1 );

    startTime = Core::getTime();
    frameEnd = 0;
    
  }

  uint32_t frameCount = 0;
  char timeText[10] = {0};

  void update( UpdateFunc func ){
    bool wasInit = false;
    
    while( func ){

      uint32_t now = Core::getTime();

      if( now < frameEnd )
	continue;

      frameEnd = now + frameTime;
      
      auto newFunc = (*func)( wasInit );
      wasInit = newFunc == func;
      func = newFunc;

      lcdrefresh();
      
      clear();

      frameCount++;
      if( frameCount == 100 ){
	frameCount = 0;
	sprintf( timeText, "%u", static_cast<unsigned int>(100000/(now - startTime)) );
	startTime = Core::getTime();
      }

      Display::setCursor( 0, 0 );
      Display::print(timeText);

    }

  }

  void blit( int y, int x, const unsigned char *sprite, BlitOp op ){
    int32_t w = *sprite++;
    int32_t h = *sprite++;

    if( y+h < 0 || y>=220 || x >= 176 || x+w < 0 )
      return;

    const uint8_t *stripLengths = sprite;
    int32_t hdelta = 1;

    sprite += w<<1; // skip strip table
    
    if( uint32_t(op) & uint32_t(BlitOp::FLIP_HORIZONTAL) ){
      hdelta = -1;
      y += h - 1;
    }
   
    int32_t srx = 0, erx = w;
    uint32_t acc = 0;

    if( uint32_t(op) & uint32_t(BlitOp::FLIP_VERTICAL) ){
    }else{
      
      for( ; h>0; y += hdelta, --h ){

	int32_t offset = *stripLengths++;
	int32_t length = *stripLengths++;
	srx = x + offset;
	erx = min( 176, srx+length );

	if( srx < 0 ){
	  sprite -= srx;
	  length += srx;
	  srx = 0;
	}

	if( y>=0 && y<220 && length > 0 ) for( int c=0; c<MAX_SPRITES-1; ++c ){

	  uint8_t cmdyc = getdata(cmds[y][c]);
	
	  if( cmdyc < srx )
	    continue;
	  
	  int32_t erase = 0;
	  for( int j=c+1; j<MAX_SPRITES; ++j ){
	    if( getdata(cmds[y][j]) <= erx ) erase++;
	    else break;
	  }

	  int shift = 1-erase;
	
	  int32_t nextdata, nextflags;
	  const uint8_t * next;
	  unpack( cmds[y][c+erase], nextdata, nextflags, next );

	  if( nextdata <= erx ){
	  
	    if( next ){
	    
	      if( nextflags )
		next -= erx - nextdata;
	      else
		next += erx - nextdata;

	    }
	    
	    cmds[y][c+erase] = packptr( next, erx, nextflags );

	  }else{
	    shift++;
	  }
	
	  if( shift>0 ){
	  
	    for( int j=MAX_SPRITES-1; j>=c+shift; --j )
	      cmds[y][j] = cmds[y][j-shift];
	  
	  }else if( shift<0 ){

	    for( int j=c+1; j<MAX_SPRITES+shift; ++j )
	      cmds[y][j] = cmds[y][j-shift];
	  
	  }

	  cmds[y][c] = packptr( sprite, srx, 0 );

	  if( nextdata > srx+length ){
	    cmds[y][c+1] = packptr( nullptr, erx, 0 );
	  }
	
	  break;
	
	}
      
	sprite += length;

      }
      
    }
    
  }

}

#define PLOTTILE							\
  *reinterpret_cast<uint32_t *>(0xA0002188) = palette[*tile++];		\
  *reinterpret_cast<uint32_t *>(0xA0002284) = 1 << LCD_WR_PIN;		\
  *reinterpret_cast<uint32_t *>(0xA0002204) = 1 << LCD_WR_PIN;
  

static void lcdrefresh(){
  
  write_command(0x20); write_data(0);
  write_command(0x21); write_data(0);
  write_command(0x22); // write data to DRAM
  CLR_CS_SET_CD_RD_WR;

  SET_MASK_P2;
      
  int stride=1, nextstride;

  for( int y=0; y<220; ++y ){
    
    int c=0;
    const uint8_t *nextptr;
    int32_t tx, flags;
    const auto *cmd = &PokittoMicro::cmds[y][0];
    PokittoMicro::unpack( *cmd, tx, flags, nextptr );
    const uint8_t *ptr = nullptr;
    
    nextstride = flags & 1 ? -1 : 1;
    
    for( int x=0; x<176; ){
      
      if( x>=tx ){
	ptr = nextptr;
	stride = nextstride;
	c++;
	if( c>=MAX_SPRITES ){
	  ptr = nextptr = nullptr;
	  tx = 176;
	  c = 10;
	}else{
	  cmd++;
	  PokittoMicro::unpack( *cmd, tx, flags, nextptr );
	  nextstride = flags & 1 ? -1 : 1;
	}
      }


      int rep = tx - x;

      if( rep <= 0 ){
	tx = x;
	continue;
      }

      if( ptr ){
	
	// x += rep;
	
	while( rep-- ){

	  auto color = *ptr;

	  if( color == PokittoMicro::transparentColor )
	    color = tiles[ PokittoMicro::map[(x>>4)*15+(y>>4)] ][ (x&0xF) + (y&0xF)*16 ];

	  *reinterpret_cast<uint32_t *>(0xA0002188) = palette[ color ];
	  ptr += stride;	  
	  *reinterpret_cast<uint32_t *>(0xA0002284) = 1 << LCD_WR_PIN;
	  *reinterpret_cast<uint32_t *>(0xA0002204) = 1 << LCD_WR_PIN;

	  x++;

	}
	
      }else{

	while( rep>0 ){

	  int trep = min( rep, 16-(x&0xF) );

	  const uint8_t *tile = tiles[ PokittoMicro::map[(x>>4)*15+(y>>4)] ] + (x&0xF) + (y&0xF)*16;
	  x += trep;

	  rep -= trep;

	  switch( trep ){
	  default:
	  case 16: PLOTTILE  
	  case 15: PLOTTILE  
	  case 14: PLOTTILE  
	  case 13: PLOTTILE  
	  case 12: PLOTTILE  
	  case 11: PLOTTILE  
	  case 10: PLOTTILE  
	  case  9: PLOTTILE  
	  case  8: PLOTTILE  
	  case  7: PLOTTILE  
	  case  6: PLOTTILE  
	  case  5: PLOTTILE  
	  case  4: PLOTTILE  
	  case  3: PLOTTILE  
	  case  2: PLOTTILE  
	  case  1: PLOTTILE  
	  case  0: break;
	  }

	}
	
      }
      
      
    }
    
  }

  CLR_MASK_P2;

}
