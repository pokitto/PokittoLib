/**************************************************************************/
/*!
    @file     miniprint.h
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement

    Modifications to TVPrint.h Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This file (Print.h) is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Original copyright and permissions stated below.
*/
/**************************************************************************/

/*
TVPrint.h - Base class that provides print() and println()
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

#ifndef MINIPRINT_H
#define MINIPRINT_H

#include <stdint.h>

/** PRINT **/

extern void print_char(unsigned char x, unsigned char y, unsigned char c);
extern void set_cursor(unsigned char, unsigned char);
extern void select_font(const unsigned char * f);

extern void write(char);
extern void write(const char *str);
extern void write(const unsigned char *buffer, unsigned char size);

extern void print(const char[]);
extern void print(char, int base = 0);
extern void print(unsigned char, int base = 0);
extern void print(int, int base = 10);
extern void print(unsigned int, int base = 10);
extern void print(long, int base = 10);
extern void print(unsigned long, int base = 10);
extern void print(double, int base = 2);

extern void print(unsigned char, unsigned char, const char[]);
extern void print(unsigned char, unsigned char, char, int = 0);
extern void print(unsigned char, unsigned char, unsigned char, int = 0);
extern void print(unsigned char, unsigned char, int, int = 10);
extern void print(unsigned char, unsigned char, unsigned int, int = 10);
extern void print(unsigned char, unsigned char, long, int = 10);
extern void print(unsigned char, unsigned char, unsigned long, int = 10);
extern void print(unsigned char, unsigned char, double, int = 2);

/*
extern void println(unsigned char, unsigned char, const char[]);
extern void println(unsigned char, unsigned char, char, int = 0);
extern void println(unsigned char, unsigned char, unsigned char, int = 0);
extern void println(unsigned char, unsigned char, int, int = 10);
extern void println(unsigned char, unsigned char, unsigned int, int = 10);
extern void println(unsigned char, unsigned char, long, int = 10);
extern void println(unsigned char, unsigned char, unsigned long, int = 10);
extern void println(unsigned char, unsigned char, double, int = 2);
extern void println(unsigned char, unsigned char);

extern void println(const char[]);
extern void println(char, int = 0);
extern void println(unsigned char, int = 0);
extern void println(int, int = 10);
extern void println(unsigned int, int = 10);
extern void println(long, int = 10 );
extern void println(unsigned long, int = 10);
extern void println(double, int = 2);
extern void println(void);
*/
/** PRINT **/
extern unsigned char cursor_x,cursor_y;
extern const unsigned char * font;

extern void inc_txtline();
extern void printNumber(unsigned long, unsigned char);
extern void printFloat(double, unsigned char);

#endif //MINIPRINT_H
