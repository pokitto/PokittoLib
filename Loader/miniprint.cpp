/**************************************************************************/
/*!
    @file     minibuttons.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

/*
TVPrint.cpp - Base class that provides print() and println()
Copyright (c) 2008 David A. Mellis.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Modified 23 November 2006 by David A. Mellis
*/

#include <math.h>
#include "miniGFX.h"
#include "Pokitto_settings.h"

/** PRINT **/
    unsigned char cursor_x,cursor_y;
	const unsigned char * font;

	void inc_txtline();
    void printNumber(unsigned long, unsigned char);
    void printFloat(double, unsigned char);


void select_font(const unsigned char * f) {
	font = f;
}

/*
 * print an 8x8 char c at x,y
 * x must be a multiple of 8
 */
void print_char(unsigned char x, unsigned char y, unsigned char c) {
	c -= font[2];
	pokDirectChar(x,y,font,c);
}

void inc_txtline() {
	if (cursor_y >= (POK_LCD_H - font[1]))
		cursor_y = 0;
	else
		cursor_y += font[1];
}

/* default implementation: may be overridden */
void write(const char *str)
{
  while (*str)
    write(*str++);
}

/* default implementation: may be overridden */
void write(const unsigned char *buffer, unsigned char size)
{
  while (size--)
    write(*buffer++);
}

void write(char c) {
	switch(c) {
		case '\0':			//null
			break;
		case '\n':			//line feed
			cursor_x = 0;
			inc_txtline();
			break;
		case 8:				//backspace
			cursor_x -= font[0];
			print_char(cursor_x,cursor_y,' ');
			break;
		case 13:			//carriage return !?!?!?!VT!?!??!?!
			cursor_x = 0;
			break;
		case 14:			//form feed new page(clear screen)
			//clear_screen();
			break;
		default:
			if (cursor_x >= (POK_LCD_W - font[0])) {
				cursor_x = 0;
				inc_txtline();
				print_char(cursor_x,cursor_y,c);
			}
			else
				print_char(cursor_x,cursor_y,c);
			cursor_x += font[0];
	}
}

void print(const char str[])
{
  write(str);
}

void print(char c, int base)
{
  print((long) c, base);
}

void print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void print(int n, int base)
{
  print((long) n, base);
}

void print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void print(long n, int base)
{
  if (base == 0) {
    write(n);
  } else if (base == 10) {
    if (n < 0) {
      print('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void print(double n, int digits)
{
  printFloat(n, digits);
}

void println(void)
{
  print('\r');
  print('\n');
}

void println(const char c[])
{
  print(c);
  println();
}

void println(char c, int base)
{
  print(c, base);
  println();
}

void println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void println(int n, int base)
{
  print(n, base);
  println();
}

void println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void println(long n, int base)
{
  print(n, base);
  println();
}

void println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void println(double n, int digits)
{
  print(n, digits);
  println();
}

void set_cursor(unsigned char x, unsigned char y) {
	cursor_x = x;
	cursor_y = y;
}

void print(unsigned char x, unsigned char y, const char str[]) {
	cursor_x = x;
	cursor_y = y;
	write(str);

}
void print(uint8_t x, uint8_t y, char c, int base) {
	cursor_x = x;
	cursor_y = y;
	print((long) c, base);
}
void print(uint8_t x, uint8_t y, unsigned char b, int base) {
	cursor_x = x;
	cursor_y = y;
	print((unsigned long) b, base);
}
void print(uint8_t x, uint8_t y, int n, int base) {
	cursor_x = x;
	cursor_y = y;
	print((long) n, base);
}
void print(uint8_t x, uint8_t y, unsigned int n, int base) {
	cursor_x = x;
	cursor_y = y;
	print((unsigned long) n, base);
}
void print(uint8_t x, uint8_t y, long n, int base) {
	cursor_x = x;
	cursor_y = y;
	print(n,base);
}
void print(uint8_t x, uint8_t y, unsigned long n, int base) {
	cursor_x = x;
	cursor_y = y;
	print(n,base);
}
void print(uint8_t x, uint8_t y, double n, int digits) {
	cursor_x = x;
	cursor_y = y;
	print(n,digits);
}

void println(uint8_t x, uint8_t y, const char c[])
{
	cursor_x = x;
	cursor_y = y;
	print(c);
	println();
}

void println(uint8_t x, uint8_t y, char c, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(c, base);
	println();
}

void println(uint8_t x, uint8_t y, unsigned char b, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(b, base);
	println();
}

void println(uint8_t x, uint8_t y, int n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void println(uint8_t x, uint8_t y, unsigned int n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void println(uint8_t x, uint8_t y, long n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void println(uint8_t x, uint8_t y, unsigned long n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void println(uint8_t x, uint8_t y, double n, int digits)
{
	cursor_x = x;
	cursor_y = y;
	print(n, digits);
	println();
}

void printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars.
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  }

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void printFloat(double number, uint8_t digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint;
  }
}

