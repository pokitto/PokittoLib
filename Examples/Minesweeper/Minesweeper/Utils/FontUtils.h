#pragma once

//
// Copyright (C) 2018 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "ArrayUtils.h"
#include "FlashString.h"
//#include <avr/pgmspace.h>

constexpr const static uint8_t FontCharWidth = 5;
constexpr const static uint8_t FontCharHeight = 7;
constexpr const static uint8_t FontSpaceWidth = 1;
constexpr const static uint8_t FontSpaceHeight = 1;
constexpr const static uint8_t FontLineHeight = FontCharHeight + FontSpaceHeight;

constexpr inline uint8_t StringWidth(const uint8_t & charCount)
{
	return (charCount * FontCharWidth) + ((charCount - 1) * FontSpaceWidth);
}

constexpr inline size_t StringWidth(const size_t & charCount)
{
	return (charCount * FontCharWidth) + ((charCount - 1) * FontSpaceWidth);
}

constexpr inline size_t StringWidth(const int & charCount)
{
	return StringWidth(static_cast<size_t>(charCount));
}

constexpr inline uint8_t StringHeight(const uint8_t & lineCount)
{
	return (lineCount * FontCharHeight) + ((lineCount - 1) * FontSpaceHeight);
}

constexpr inline size_t StringHeight(const size_t & lineCount)
{
	return (lineCount * FontCharHeight) + ((lineCount - 1) * FontSpaceHeight);
}

constexpr inline size_t StringHeight(const int & lineCount)
{
	return StringHeight(static_cast<size_t>(lineCount));
}

template< typename T, size_t Size >
constexpr size_t StringWidth(T (&string)[Size])
{
	return StringWidth(StringLength(string));
}

inline size_t ProgmemStringWidth(const char * string)
{
	//return StringWidth(strlen_P(string));
	return strlen(string);
}

inline size_t ProgmemStringWidth(FlashString string)
{
	return ProgmemStringWidth(reinterpret_cast<const char *>(string));
}

inline size_t RuntimeStringWidth(const char * string)
{
	return StringWidth(strlen(string));
}

template< typename T, size_t Size >
inline size_t RuntimeStringWidth(T(&string)[Size])
{
	return StringWidth(strlen(&string[0]));
}
