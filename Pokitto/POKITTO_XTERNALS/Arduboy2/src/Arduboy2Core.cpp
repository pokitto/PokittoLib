/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

#include "Arduboy2.h"
#include "PokittoCore.h"

// need to redeclare these here since we declare them static in .h
volatile uint8_t *Arduboy2Core::csport, *Arduboy2Core::dcport;
uint8_t Arduboy2Core::cspinmask, Arduboy2Core::dcpinmask;

const uint8_t PROGMEM pinBootProgram[] = {

};

const uint8_t PROGMEM lcdBootProgram[] = {

};


Arduboy2Core::Arduboy2Core() {}

void Arduboy2Core::boot()
{

}

#ifdef ARDUBOY_SET_CPU_8MHZ
// If we're compiling for 8MHz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz.
// We also need to readjust the PLL prescaler because the Arduino USB code
// likely will have incorrectly set it for an 8MHz hardware clock.
void Arduboy2Core::setCPUSpeed8MHz()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  PLLCSR = _BV(PINDIV); // dissable the PLL and set prescale for 16MHz)
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  PLLCSR = _BV(PLLE) | _BV(PINDIV); // enable the PLL (with 16MHz prescale)
  SREG = oldSREG;       // restore interrupts
}
#endif

void Arduboy2Core::bootPins()
{
  uint8_t pin, mode;
  const uint8_t *i = pinBootProgram;

  while(true) {
    pin = pgm_read_byte(i++);
    mode = pgm_read_byte(i++);
    if (pin==0) break;
    pinMode(pin, mode);
  }

  digitalWrite(RST, HIGH);
  delay(1);           // VDD (3.3V) goes high at start, lets just chill for a ms
  digitalWrite(RST, LOW);   // bring reset low
  delay(10);          // wait 10ms
  digitalWrite(RST, HIGH);  // bring out of reset
}

void Arduboy2Core::bootOLED()
{

}

void Arduboy2Core::LCDDataMode()
{

}

void Arduboy2Core::LCDCommandMode()
{

}



void Arduboy2Core::safeMode()
{
  blank(); // too avoid random gibberish
  while (true) {
    asm volatile("nop \n");
  }
}


/* Power Management */

void Arduboy2Core::idle()
{
/*  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();*/
}

void Arduboy2Core::bootPowerSaving()
{
 /* power_adc_disable();
  power_usart0_disable();
  power_twi_disable();
  // timer 0 is for millis()
  // timers 1 and 3 are for music and sounds
  power_timer2_disable();
  power_usart1_disable();
  // we need USB, for now (to allow triggered reboots to reprogram)
  // power_usb_disable()*/
}

uint8_t Arduboy2Core::width() { return WIDTH; }

uint8_t Arduboy2Core::height() { return HEIGHT; }


/* Drawing */

void Arduboy2Core::paint8Pixels(uint8_t pixels)
{
  if (pixels&1) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&2) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&4) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&8) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&0x10) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&0x20) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&0x40) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
  if (pixels&0x80) Pokitto::blitWord(_c.display.color);
  else Pokitto::blitWord(_c.display.bgcolor);
}

void Arduboy2Core::paintScreen(const uint8_t *image)
{
  _c.display.lcdRefresh((unsigned char*)image);
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
    _c.display.lcdRefresh((unsigned char*)image);
    //if (clear) _c.display.clear();
}

void Arduboy2Core::blank()
{
  _c.display.clearLCD();
}

void Arduboy2Core::sendLCDCommand(uint8_t command)
{

}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse)
{
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on)
{
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
}

// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
}

/* RGB LED */

void Arduboy2Core::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10 // RGB, all the pretty colors
  // inversion is necessary because these are common annode LEDs
  /*analogWrite(RED_LED, 255 - red);
  analogWrite(GREEN_LED, 255 - green);
  analogWrite(BLUE_LED, 255 - blue);
#elif defined(AB_DEVKIT)
  // only blue on devkit
  digitalWrite(BLUE_LED, ~blue);*/
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10
//  digitalWrite(RED_LED, red);
 // digitalWrite(GREEN_LED, green);
 // digitalWrite(BLUE_LED, blue);
#elif defined(AB_DEVKIT)
  //digitalWrite(BLUE_LED, blue);
#endif
}

/* Buttons */

uint8_t Arduboy2Core::buttonsState()
{
uint8_t buttons=0; // jonne added this, do not initialize empty

  #ifdef POK_SIM
    _c.buttons.pollButtons();
  #endif // POK_SIM
  if (_c.buttons.downBtn()) buttons |= DOWN_BUTTON;
  if (_c.buttons.upBtn()) buttons |= UP_BUTTON;
  if (_c.buttons.leftBtn()) buttons |= LEFT_BUTTON;
  if (_c.buttons.rightBtn()) buttons |= RIGHT_BUTTON;
  if (_c.buttons.aBtn()) buttons |= A_BUTTON;
  if (_c.buttons.bBtn()) buttons |= B_BUTTON;

  return buttons;
}
