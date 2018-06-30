
#include "Pokitto.h"

extern "C" {
#include "drv.h"
}

using namespace Pokitto;

extern "C" int zboymain(int argc, char **argv);

extern uint8_t *framebuffer;

/* initialization of the I/O subsystem. This is called by zBoy once, when the
 * emulator starts. screenwidth and screenheight must contain the size of
 * requested virtual screen, joyid is either the id of the joystick that have
 * to be polled (0..x) or -1 if no joystick support is required. */
extern "C" int drv_init(int screenwidth, int screenheight, int joyid){
  framebuffer = Pokitto::Display::screenbuffer;
  return 0;
}

/* renders the screen from the memory buffer into the actual screen */
extern "C" void drv_refreshscreen(void){
  Pokitto::Display::update();
}

uint8_t reported = 0, changes = 0;
/* returns the next input event in queue */
int drv_keypoll(void){
  
  if( (changes & (1<<UPBIT)) && !(reported & (1<<UPBIT)) ){
    reported |= 1<<UPBIT;
    if( Buttons::buttons_state & (1<<UPBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_UP;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_UP;
  }
  
  if( (changes & (1<<DOWNBIT)) && !(reported & (1<<DOWNBIT)) ){
    reported |= 1<<DOWNBIT;
    if( Buttons::buttons_state & (1<<DOWNBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_DOWN;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_DOWN;
  }

  if( (changes & (1<<LEFTBIT)) && !(reported & (1<<LEFTBIT)) ){
    reported |= 1<<LEFTBIT;
    if( Buttons::buttons_state & (1<<LEFTBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LEFT;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LEFT;
  }

  if( (changes & (1<<RIGHTBIT)) && !(reported & (1<<RIGHTBIT)) ){
    reported |= 1<<RIGHTBIT;
    if( Buttons::buttons_state & (1<<RIGHTBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RIGHT;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RIGHT;
  }

  if( (changes & (1<<CBIT)) && !(reported & (1<<CBIT)) ){
    reported |= 1<<CBIT;
    if( Buttons::buttons_state & (1<<CBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RET;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_RET;
  }

  if( (changes & (1<<ABIT)) && !(reported & (1<<ABIT)) ){
    reported |= 1<<ABIT;
    if( Buttons::buttons_state & (1<<ABIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LCTRL;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LCTRL;
  }

  if( (changes & (1<<BBIT)) && !(reported & (1<<BBIT)) ){
    reported |= 1<<BBIT;
    if( Buttons::buttons_state & (1<<BBIT) )
      return DRV_INPUT_KEYDOWN | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LALT;
    else 
      return DRV_INPUT_KEYUP | DRV_INPUT_KEYBOARD | DRV_INPUT_KEY_LALT;
  }

  reported = 0;
  Buttons::pollButtons();    
  changes = Buttons::buttons_state ^ (Buttons::buttons_held | Buttons::buttons_released);
  return DRV_INPUT_NONE;
  
}

/* loads a palette of colors into zBoy. *palette must be an array of at least
 * 256 color values written in 32bits each as RGB triplets */
int drv_loadpal(uint32_t *palette){
  return 0;
}

/* returns a monotonously increasing counter that increases by 1 every
 * milisecond */
unsigned long drv_getticks(void){
  return Pokitto::Core::getTime();
}

extern "C" void indexRAM();

int main () {
  
  Core::begin();

  char *args[] = {""};
  *reinterpret_cast<uint32_t *>(0x40048080) |= 3 << 26;

  /* * Grayscale /
  Display::palette[0] = Display::RGBto565( 0xFF, 0xFF, 0xFF );
  Display::palette[1] = Display::RGBto565( 0xAA, 0xAA, 0xAA );
  Display::palette[2] = Display::RGBto565( 0x55, 0x55, 0x55 );
  Display::palette[3] = Display::RGBto565( 0x00, 0x00, 0x00 );
  /*/

  /* * Green Tint /
  Display::palette[3] = Display::RGBto565( 0x24, 0x31, 0x37 );
  Display::palette[2] = Display::RGBto565( 0x3f, 0x50, 0x3f );
  Display::palette[1] = Display::RGBto565( 0x76, 0x84, 0x48 );
  Display::palette[0] = Display::RGBto565( 0xac, 0xb5, 0x6b );
  /*/

  Display::palette[3] = Display::RGBto565( 0x33,0x2c,0x50 );
  Display::palette[2] = Display::RGBto565( 0x46,0x87,0x8f );
  Display::palette[1] = Display::RGBto565( 0x94,0xe3,0x44 );
  Display::palette[0] = Display::RGBto565( 0xe2,0xf3,0xe4 );
  






  /* */
  indexRAM();

  Pokitto::lcdClear();
  Display::clear();
  zboymain(0, args);
  
}

