#include "Tile.h"

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

bool Tile::isMine(void) const
{
	return (this->value & isMineMask) != 0;
}

bool Tile::isVisible(void) const
{
	return (this->value & isVisibleMask) != 0;
}

bool Tile::isMarked(void) const
{
	return (this->value & isMarkedMask) != 0;
}

uint8_t Tile::getMineCount(void) const
{
	return (this->value & MineCountMask);	
}
	
void Tile::setIsMine(const bool & isMine)
{
	this->value = (isMine) ? (this->value | isMineMask) : (this->value & ~isMineMask);
}

void Tile::setIsVisible(const bool & isVisible)
{
	this->value = (isVisible) ? (this->value | isVisibleMask) : (this->value & ~isVisibleMask);
}

void Tile::setIsMarked(const bool & isMarked)
{
	this->value = (isMarked) ? (this->value | isMarkedMask) : (this->value & ~isMarkedMask);
}

void Tile::setMineCount(const uint8_t & mineCount)
{
	this->value &= ~MineCountMask;
	this->value |= (mineCount & MineCountMask);
}

void Tile::show(void)
{
	this->value |= isVisibleMask;
}
	
void Tile::hide(void)
{
	this->value &= ~isVisibleMask;
}
	
void Tile::mark(void)
{
	this->value |= isMarkedMask;
}
	
void Tile::unmark(void)
{
	this->value &= ~isMarkedMask;
}

void Tile::toggleMark(void)
{
	this->value ^= isMarkedMask;
}
