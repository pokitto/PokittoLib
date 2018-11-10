#include "ThemesState.h"

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

void ThemesState::update(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;
	
	if (arduboy.justPressed(Arduboy::ButtonA) || arduboy.justPressed(Arduboy::ButtonB))
	{
		if(Context::saveCheckEnabled)
			Context::SaveData::saveData(Context::stats);
		machine.changeState(GameStateType::TitleScreen);
	}
	
	if (arduboy.justPressed(Arduboy::ButtonLeft))
	{
		if (Context::stats.themeIndex > Context::FirstThemeIndex)
			--Context::stats.themeIndex;
	}

	if (arduboy.justPressed(Arduboy::ButtonRight))
	{
		if (Context::stats.themeIndex < Context::LastThemeIndex)
			++Context::stats.themeIndex;
	}
}

void ThemesState::render(StateMachine & machine)
{
	constexpr uint8_t singleMargin = 2;
	constexpr uint8_t doubleMargin = singleMargin * 2;

	auto & arduboy = machine.getContext().arduboy;

	// Draw the state title "themes"
	{
		constexpr uint8_t x = CalculateCentreX(StringWidth(Strings::Themes));
		constexpr uint8_t y = singleMargin;

		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(Strings::Themes));
	}

	// Draw the theme name
	{
		constexpr uint8_t y = Arduboy::ScreenHeight - (singleMargin + FontLineHeight);

		FlashString name = reinterpret_cast<FlashString>(pgm_read_ptr(&Strings::ThemeNames[Context::stats.themeIndex]));

		uint8_t x = CalculateCentreX(ProgmemStringWidth(name));

		arduboy.setCursor(x, y);
		arduboy.print(name);
	}

	// Draw mine selector
	{
		constexpr uint8_t x = CalculateCentreX(Images::IconWidth);
		constexpr uint8_t y = CalculateCentreY(Images::IconHeight);

		constexpr uint8_t arrowY = y + CalculateCentre(Images::IconHeight, Images::ArrowHeight);
		constexpr uint8_t leftArrowX = x - (Images::ArrowWidth + singleMargin);
		constexpr uint8_t rightArrowX = x + (Images::IconWidth + singleMargin);
		
		constexpr uint8_t iconY = y;
		constexpr uint8_t leftIconX = leftArrowX - (Images::IconWidth + singleMargin);
		constexpr uint8_t rightIconX = rightArrowX + (Images::ArrowWidth + singleMargin);

		if (Context::stats.themeIndex > Context::FirstThemeIndex)
		{
			Sprites::drawOverwrite(leftArrowX, arrowY, Images::Arrows, 2);
			Sprites::drawOverwrite(leftIconX, iconY, Images::Icons, Context::stats.themeIndex - 1);
		}

		if (Context::stats.themeIndex < Context::LastThemeIndex)
		{
			Sprites::drawOverwrite(rightArrowX, arrowY, Images::Arrows, 3);
			Sprites::drawOverwrite(rightIconX, iconY, Images::Icons, Context::stats.themeIndex + 1);
		}

		Sprites::drawOverwrite(x, y, Images::Icons, Context::stats.themeIndex);

		constexpr uint8_t rectWidth = Images::IconWidth + doubleMargin;
		constexpr uint8_t rectHeight = Images::IconHeight + doubleMargin;

		arduboy.drawRect(x - singleMargin, y - singleMargin, rectWidth, rectHeight);
		arduboy.drawRect(leftIconX - singleMargin, iconY - singleMargin, rectWidth, rectHeight);
		arduboy.drawRect(rightIconX - singleMargin, iconY - singleMargin, rectWidth, rectHeight);
		
		arduboy.drawFastHLine(0, y - doubleMargin, Arduboy::ScreenWidth);
		arduboy.drawFastHLine(0, y + doubleMargin + (Images::IconHeight - 1), Arduboy::ScreenWidth);
	}
}