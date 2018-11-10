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

template< typename T, uint8_t WidthValue, uint8_t HeightValue >
class Grid
{
public:

public:
	constexpr static const uint16_t Width = WidthValue;
	constexpr static const uint16_t Height = HeightValue;
	constexpr static const uint16_t Count = static_cast<uint16_t>(Width) * static_cast<uint16_t>(Height);

	constexpr static const uint8_t FirstX = 0;
	constexpr static const uint8_t FirstY = 0;
	constexpr static const uint8_t LastX = (Width > 0) ? (Width - 1) : 0;
	constexpr static const uint8_t LastY = (Height > 0) ? (Height - 1) : 0;

private:
	T items[Count];

	inline uint16_t FlattenIndex(const uint8_t & x, const uint8_t & y) const
	{
		return (Width * static_cast<uint16_t>(y)) + static_cast<uint16_t>(x);
	}
	
public:	
	constexpr uint16_t getCount(void) const;
	constexpr uint8_t getWidth(void) const;
	constexpr uint8_t getHeight(void) const;

	T & getItem(const uint8_t & x, const uint8_t & y);
	const T & getItem(const uint8_t & x, const uint8_t & y) const;
	
	void fill(const T & value);
};

template< typename T, uint8_t Width, uint8_t Height >
constexpr uint16_t Grid<T, Width, Height>::getCount(void) const
{
	return Grid<T, Width, Height>::Count;
}
	
template< typename T, uint8_t Width, uint8_t Height >
constexpr uint8_t Grid<T, Width, Height>::getWidth(void) const
{
	return Grid<T, Width, Height>::Width;
}
	
template< typename T, uint8_t Width, uint8_t Height >
constexpr uint8_t Grid<T, Width, Height>::getHeight(void) const
{
	return Grid<T, Width, Height>::Height;
}

template< typename T, uint8_t Width, uint8_t Height >
T & Grid<T, Width, Height>::getItem(const uint8_t & x, const uint8_t & y)
{
	return this->items[FlattenIndex(x, y)];
}

template< typename T, uint8_t Width, uint8_t Height >
const T & Grid<T, Width, Height>::getItem(const uint8_t & x, const uint8_t & y) const
{
	return this->items[FlattenIndex(x, y)];
}

template< typename T, uint8_t Width, uint8_t Height >
void Grid<T, Width, Height>::fill(const T & value)
{
	for(uint16_t i = 0; i < Count; ++i)
		items[i] = value;
}