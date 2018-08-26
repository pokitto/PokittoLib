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

// Might be needed for memcpy_P
#include <avr/pgmspace.h>

namespace Details
{
	// Pay no attention to the man behind the curtain

	template< typename T, size_t Size = sizeof(T) >
	struct ProgmemReadHelper;

	template< typename T, size_t Size >
	struct ProgmemReadHelper
	{
		inline static T ProgmemRead(T const * const progmem)
		{
			T t;
			memcpy_P(&t, progmem, sizeof(T));
			return t;
		}
	};

	template< typename T >
	struct ProgmemReadHelper<T, 1>
	{
		constexpr inline static T ProgmemRead(T const * const progmem)
		{
			return static_cast<T>(pgm_read_byte(progmem));
		}
	};

	template< typename T >
	struct ProgmemReadHelper<T, 2>
	{
		constexpr inline static T ProgmemRead(T const * const progmem)
		{
			return static_cast<T>(pgm_read_word(progmem));
		}
	};

	template< typename T >
	struct ProgmemReadHelper<T*, 2>
	{
		constexpr inline static T * ProgmemRead(T const * const * const progmem)
		{
			return reinterpret_cast<T *>(pgm_read_word(progmem));
		}
	};
}

//
// ProgmemRead
//

// As if by magic
template< typename T >
inline T ProgmemRead(const T * const progmem)
{
	return Details::ProgmemReadHelper<T>::ProgmemRead(progmem);
}

//
// ProgmemMemoryCopy overloads
//

template< typename T >
inline T * ProgmemMemoryCopy(T * destination, const T & source)
{
	return reinterpret_cast<T *>(memcpy_P(destination, &source, sizeof(T)));
}

template< typename T >
inline T * ProgmemMemoryCopy(T & destination, const T * source)
{
	return reinterpret_cast<T *>(memcpy_P(&destination, source, sizeof(T)));
}

template< typename T >
inline T * ProgmemMemoryCopy(T * destination, const T * source)
{
	return reinterpret_cast<T *>(memcpy_P(destination, source, sizeof(T)));
}

template< typename T >
inline T * ProgmemMemoryCopy(T & destination, const T & source, size_t n)
{
	return reinterpret_cast<T *>(memcpy_P(&destination, &source, sizeof(T) * n));
}

template< typename T >
inline T * ProgmemMemoryCopy(T * destination, const T & source, size_t n)
{
	return reinterpret_cast<T *>(memcpy_P(destination, &source, sizeof(T) * n));
}

template< typename T >
inline T * ProgmemMemoryCopy(T & destination, const T * source, size_t n)
{
	return reinterpret_cast<T *>(memcpy_P(&destination, source, sizeof(T) * n));
}

template< typename T >
inline T * ProgmemMemoryCopy(T * destination, const T * source, size_t n)
{
	return reinterpret_cast<T *>(memcpy_P(destination, source, sizeof(T) * n));
}

template< typename T >
inline T * ProgmemMemoryCopy(T & destination, const T & source)
{
	return reinterpret_cast<T *>(memcpy_P(&destination, &source, sizeof(T)));
}
