/**************************************************************************/
/*!
    @file     PokittoItoa.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Jonne Valola
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

#include "PokittoItoa.h"
#include <string.h>
#include <stdint.h>


void pokItoa(uint16_t value, char *str, int base)
{
    /** pokItoa:  convert value n to characters
     * Needed to emulate AVR LibC non-standard functions, this is a very fast version of itoa
     *
     * @param value unsigned integer to be converted
     * @param *str pointer to string where function stores the result as characters
     * @param base (not implemented in this function, base is always 10)
     */

    char *p = str;
    uint8_t v;
    if (value > 499)
    {
        if (value > 799)
        {
            if (value > 999)
            {
                // Treated as a special case as there
                // are only 24 values above 999
                *p++ = '1';
                *p++ = '0';
                uint8_t v = value - 1000;

                if      (v > 19) { *p++ = '2'; v -= 20; }
                else if (v >  9) { *p++ = '1'; v -= 10; }
                else             { *p++ = '0'; }

                *p++ = v + '0';
                *p = 0;
                return;
            }

            if (value > 899) { *p++ = '9'; v = value - 900; }
            else             { *p++ = '8'; v = value - 800; }
        }
        else
        {
            if      (value > 699) { *p++ = '7'; v = value - 700; }
            else if (value > 599) { *p++ = '6'; v = value - 600; }
            else                  { *p++ = '5'; v = value - 500; }
        }
    }
    else
    {
        if (value > 299)
        {
            if (value > 399) { *p++ = '4'; v = value - 400; }
            else             { *p++ = '3'; v = value - 300; }
        }
        else
        {
            if      (value > 199) { *p++ = '2'; v = value - 200; }
            else if (value >  99) { *p++ = '1'; v = value - 100; }
            else                  {             v = value; }
        }
    }

    if (v > 49)
    {
        if (v > 69)
        {
            if      (v > 89) { *p++ = '9'; v -= 90; }
            else if (v > 79) { *p++ = '8'; v -= 80; }
            else             { *p++ = '7'; v -= 70; }
        }
        else
        {
            if (v > 59) { *p++ = '6'; v -= 60; }
            else        { *p++ = '5'; v -= 50; }
        }
    }
    else
    {
        if (v > 19)
        {
            if      (v > 39) { *p++ = '4'; v -= 40; }
            else if (v > 29) { *p++ = '3'; v -= 30; }
            else             { *p++ = '2'; v -= 20; }
        }
        else
        {
            if      (v >  9)   { *p++ = '1'; v -= 10; }
            else if (p != str) { *p++ = '0'; }
        }
    }

    *p++ = v + '0';
    *p = 0;
}

void pokUtoa(uint16_t value, char *str, int base) {
    pokItoa(value,str,base);
}

void pokLtoa(long value, char *str, int base) {
    pokItoa(value,str,base);
}

