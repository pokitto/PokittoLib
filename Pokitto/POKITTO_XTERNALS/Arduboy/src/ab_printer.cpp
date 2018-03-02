#include "Arduboy.h"
#include "ab_printer.h"
#include "glcdfont.c"

AbPrinter::AbPrinter(Arduboy &ab)
{
  arduboy = &ab;

  // font rendering
  cursorX = 0;
  cursorY = 0;
  size = 1;
}

size_t AbPrinter::write(uint8_t c)
{
  if (c == '\n')
  {
    cursorY += size*8;
    cursorX = 0;
  }
  else if (c == '\r')
  {
    // skip em
  }
  else
  {
    arduboy->drawChar(cursorX, cursorY, c, 1, 0, size);
    cursorX += size*6;
    if (wrap && (cursorX > (WIDTH - size*6)))
    {
      // calling ourselves recursively for 'newline' is
      // 12 bytes smaller than doing the same math here
      write('\n');
    }
  }
}

void AbPrinter::setCursor(int16_t x, int8_t y)
{
  cursorX = x;
  cursorY = y;
}

void AbPrinter::setSize(uint8_t s)
{
  // size must always be 1 or higher
  size = max(1,s);
}

void AbPrinter::setWrap(boolean w)
{
  wrap = w;
}
