#include "CreditsState.h"

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
#include "Credits.h"

#include "Utils.h"

const CreditsState::Category CreditsState::Categories[] PROGMEM =
{
	Category(Strings::Programming, Strings::Programmers),
	Category(Strings::Graphics, Strings::Artists),
	Category(Strings::Testing, Strings::Testers),
	Category(Strings::Localisation, Strings::Translators),
};

void CreditsState::update(StateMachine & machine)
{
	constexpr static const uint8_t CategoryCount = ArrayLength(CreditsState::Categories);
	// To do: find a way to make this automatic
	constexpr static const uint8_t ScrollMaxLines = (CategoryCount * 2) +
	ArrayLength(Strings::Programmers) + ArrayLength(Strings::Artists) +
	ArrayLength(Strings::Testers) + ArrayLength(Strings::Translators);

	constexpr static const uint8_t ScrollMax = (ScrollMaxLines * FontLineHeight);
	
	auto & arduboy = machine.getContext().arduboy;
	
	if (arduboy.justPressed(Arduboy::ButtonB))
	{
		machine.changeState(GameStateType::TitleScreen);
	}
	
	if (arduboy.justPressed(Arduboy::ButtonUp))
	{
		this->scrollDirection = ScrollDirection::Up;
	}
	else if (arduboy.justReleased(Arduboy::ButtonUp))
	{
		if (this->scrollDirection == ScrollDirection::Up)
			this->scrollDirection = ScrollDirection::None;
	}
	
	if (arduboy.justPressed(Arduboy::ButtonDown))
	{
		this->scrollDirection = ScrollDirection::Down;
	}
	else if (arduboy.justReleased(Arduboy::ButtonDown))
	{
		if (this->scrollDirection == ScrollDirection::Down)
			this->scrollDirection = ScrollDirection::None;
	}
	
	if (this->scrollDirection != ScrollDirection::None)
	{
		auto time = millis();
		if (time > this->nextScrollTime)
		{
			if (this->scrollDirection == ScrollDirection::Up)
			{
				if(this->offset > 0)
					--this->offset;
			}
			else if (this->scrollDirection == ScrollDirection::Down)
			{
				if (this->offset < ScrollMax)
					++this->offset;
			}
			this->nextScrollTime = time + ScrollDelay;
		}
	}
}

void CreditsState::render(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;
	
	constexpr const uint8_t border = 4;
	constexpr const uint8_t indent = FontCharWidth + 1;
	constexpr const uint8_t step = FontCharHeight + 1;

	int16_t y = border - this->offset;

	// Draw the title
	{
		constexpr const uint16_t x = CalculateCentreX(StringWidth(Strings::Credits));
		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(Strings::Credits));
		y += step + border;
	}

	// Draw the logo
	{
		constexpr const uint16_t x = CalculateCentreX(Images::PharapLogoWidth);
		Sprites::drawOverwrite(x, y, Images::PharapLogo, 0);
		y += Images::PharapLogoHeight + border;
	}

	// Draw the credits
	{
		constexpr const uint8_t x = border;
				
		for (uint8_t i = 0; i < ArrayLength(Categories) && (y < ScreenHeight); ++i, y += step)
		{
			Category category;
			ProgmemMemoryCopy(category, Categories[i]);

			arduboy.setCursor(x, y);
			arduboy.print(AsFlashString(category.Title));

			y += step;
			for (uint8_t j = 0; j < category.EntryCount; ++j, y += step)
			{
				arduboy.setCursor(x + indent, y);
				arduboy.print(AsFlashString(ProgmemRead(&(category.Entries[j]))));
			}
		}
	}

	// Draw the arrows
	{
		constexpr static const uint8_t CategoryCount = ArrayLength(CreditsState::Categories);
		// To do: find a way to make this automatic
		constexpr static const uint8_t ScrollMaxLines = (CategoryCount * 2) +
		ArrayLength(Strings::Programmers) + ArrayLength(Strings::Artists) +
		ArrayLength(Strings::Testers) + ArrayLength(Strings::Translators);

		constexpr static const uint8_t ScrollMax = (ScrollMaxLines * FontLineHeight);

		constexpr const uint8_t x = Arduboy::ScreenWidth - Images::ArrowWidth;

		if (this->offset > 0)
		{
			Sprites::drawOverwrite(x, 0, Images::Arrows, 0);
		}

		if (this->offset < ScrollMax)
		{
			constexpr const uint8_t y = Arduboy::ScreenHeight - Images::ArrowHeight;
			Sprites::drawOverwrite(x, y, Images::Arrows, 1);
		}
	}
}