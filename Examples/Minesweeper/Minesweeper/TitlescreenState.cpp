#include "TitlescreenState.h"

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

#include "Images.h"
#include "Languages.h"
#include "Utils.h"

const TitleScreenState::Option TitleScreenState::Options[] PROGMEM =
{
	{ GameStateType::Gameplay, Strings::Play },
	{ GameStateType::Themes, Strings::Themes },
	{ GameStateType::Credits, Strings::Credits },
	{ GameStateType::Stats, Strings::Stats },
};

void TitleScreenState::update(StateMachine & machine)
{
	constexpr const static uint8_t MinOption = 0;
	constexpr const static uint8_t OptionCount = ArrayLength(Options);
	constexpr const static uint8_t MaxOption = (MinOption + OptionCount) - 1;

	auto & arduboy = machine.getContext().arduboy;
	
	if (arduboy.justPressed(Arduboy::ButtonUp))
	{
		if (this->option > MinOption)
			--this->option;
	}

	if (arduboy.justPressed(Arduboy::ButtonDown))
	{
		if (this->option < MaxOption)
			++this->option;
	}

	if (arduboy.justPressed(Arduboy::ButtonA))
	{
		auto state = ProgmemRead(&(Options[this->option].State));
		machine.changeState(state);
	}
}

void TitleScreenState::render(StateMachine & machine)
{

	auto & arduboy = machine.getContext().arduboy;

	// Draw Title
	Sprites::drawOverwrite(0, 0, Images::Title, 0);

	// Draw options
	{
		constexpr const uint8_t maxWidth =
			MaxValue<uint8_t,
			StringWidth(Strings::Play),
			StringWidth(Strings::Credits),
			StringWidth(Strings::Stats)
			>();

		constexpr const uint8_t step = FontCharHeight + 2;
		constexpr const uint8_t x = CalculateCentreX(maxWidth);
		constexpr const uint8_t yBase = Images::TitleHeight + 4;

		constexpr const static uint8_t MinOption = 0;
		constexpr const static uint8_t OptionCount = ArrayLength(Options);

		uint8_t y = yBase;

		for (uint8_t i = MinOption; i < OptionCount; ++i, y += step)
		{
			arduboy.setCursor(x, y);
			arduboy.print(AsFlashString(ProgmemRead(&(Options[i].Text))));

			if (this->option == i)
			{
				const uint8_t index = (12 + Context::stats.themeIndex);
				Sprites::drawOverwrite(x - (2 * FontCharWidth), y, Images::Tiles, index);
			}
		}
	}
}