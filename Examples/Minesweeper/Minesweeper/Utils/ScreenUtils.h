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

#include "Arduboy2PrettificationUtils.h"
#include "Type.h"

constexpr const static uint8_t ScreenWidth = Arduboy::ScreenWidth;
constexpr const static uint8_t ScreenHeight = Arduboy::ScreenHeight;
constexpr const static uint8_t HalfScreenWidth = (ScreenWidth / 2);
constexpr const static uint8_t HalfScreenHeight = (ScreenHeight / 2);

template< typename Type >
constexpr Type intRound(const Type & value)
{
	return ((value % 2 == 0) ? value : value + 1);
}

template< typename Type >
constexpr Type div2Round(const Type & value)
{
	return intRound(value) / 2;
}

template< typename Type >
constexpr Type CalculateCentre(const Type & maxDimension, const Type & dimension)
{
	return div2Round(maxDimension) - div2Round(dimension);
}

template< typename T, typename U >
constexpr LargerType<T, U> CalculateCentre(const T & maxDimension, const U & dimension)
{
	using Result = LargerType<T, U>;
	return CalculateCentre<Result>(static_cast<Result>(maxDimension), static_cast<Result>(dimension));
}

constexpr int16_t CalculateCentreX(const uint8_t & width)
{
	return div2Round(ScreenWidth) - div2Round(width);
}

constexpr int16_t CalculateCentreY(const uint8_t & height)
{
	return div2Round(ScreenHeight) - div2Round(height);
}

constexpr int16_t CalculateCentreX(const uint8_t & x, const uint8_t & maxWidth, const uint8_t & width)
{
	return (x + div2Round(maxWidth - x)) - div2Round(width);
}

constexpr int16_t CalculateCentreY(const uint8_t & y, const uint8_t & maxHeight, const uint8_t & height)
{
	return (y + div2Round(maxHeight - y)) - div2Round(height);
}