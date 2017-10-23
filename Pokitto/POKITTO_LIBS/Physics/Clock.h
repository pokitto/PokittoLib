/*
    Copyright (c) 2013 Randy Gaul http://RandyGaul.net

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
*/

/* MODIFICATIONS FOR POKITTO LIBRARY (C) by JONNE VALOLA 2016. See LICENSE.TXT for Pokitto Library*/

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

class Clock
{
public:
  Clock( );
  ~Clock( );

  // Records current time in start variable
  void Start( void );

  // Records current time in stop variable
  void Stop( void );

  // Time since last Start call
  uint32_t Elapsed( void );


  // Time between last Start and Stop calls
  uint32_t Difference( void );

  // Get the current clock count
  uint32_t Current( void );

private:
   uint32_t m_start;
   uint32_t m_stop;
   uint32_t m_current;

};

#endif // CLOCK_H
