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

#include "Clock.h"
#include "PokittoCore.h"

Pokitto::Core* co;

Clock::Clock( )
{
  // Setup initial times
  Start( );
  Stop( );
}

Clock::~Clock( )
{
}

// Records current time in start variable
void Clock::Start( void )
{
  m_start = co->getTime();
}

// Records current time in stop variable
void Clock::Stop( void )
{
  m_stop = co->getTime();
}

// Get current time from previous Start call
uint32_t Clock::Elapsed( void )
{
  m_current = co->getTime();
  if (m_current<m_start)
  return m_current - m_start;
}

// Time between last Start and Stop calls
uint32_t Clock::Difference( void )
{
  return m_stop - m_start;
}

// Get the current clock count
uint32_t Clock::Current( void )
{
  m_current = co->getTime();
  return m_current;
}
