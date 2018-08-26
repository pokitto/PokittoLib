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

#include "GameContext.h"

#include "GameState.h"
#include "GameStateType.h"

class CreditsState : public GameState<GameContext, GameStateType>
{
private:
	enum class ScrollDirection : uint8_t
	{
		None, Up, Down
	};

	struct Category
	{
		char const * Title;
		char const * const * Entries;
		uint8_t EntryCount;

		Category(void) = default;

		template< size_t Size >
		constexpr Category(char const title[], char const * const (&entries)[Size])
			: Title(title), Entries(entries), EntryCount(Size)
		{
			static_assert(Size < 256, "Category cannot exceed 255 names");
		}

		constexpr Category(char const title[], char const * const entries[], uint8_t entryCount)
			: Title(title), Entries(entries), EntryCount(entryCount)
		{
		}
	};

	static const Category Categories[];

	constexpr static const uint8_t ScrollDelay = 25;

private:
	uint8_t offset;
	ScrollDirection scrollDirection;
	uint16_t nextScrollTime;

public:
	void update(StateMachine & machine) override;
	void render(StateMachine & machine) override;
};