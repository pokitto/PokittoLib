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

#include <stdint.h>

class Tile
{
private:
	constexpr const static uint8_t isMineMask = 1 << 4;
	constexpr const static uint8_t isVisibleMask = 1 << 5;
	constexpr const static uint8_t isMarkedMask = 1 << 6;
	constexpr const static uint8_t MineCountMask = 0x0F;

private:
	uint8_t value;

public:
	bool isMine(void) const;
	bool isVisible(void) const;
	bool isMarked(void) const;
	uint8_t getMineCount(void) const;
	
	void setIsMine(const bool & isMine);
	void setIsVisible(const bool & isVisible);
	void setIsMarked(const bool & isMarked);
	void setMineCount(const uint8_t & mineCount);

	void show(void);
	void hide(void);
	void mark(void);
	void unmark(void);
	
	void toggleMark(void);
};