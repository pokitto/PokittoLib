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

#include <Arduboy2.h>

#include "Languages.h"
#include "Utils.h"

enum class ConfirmationOption : uint8_t
{
	None, Yes, No,
};

class ConfirmationMenu
{
private:
	constexpr static const uint16_t selectionWidth = MaxValue<uint8_t, StringWidth(Strings::Selected), StringWidth(Strings::NotSelected)>();
	constexpr static const uint16_t confirmationWidth = MaxValue<uint8_t, StringWidth(Strings::Yes), StringWidth(Strings::No)>();
	constexpr static const uint8_t defaultWidth = (selectionWidth + confirmationWidth);

private:
	uint8_t optionIndex = 1;

public:
	ConfirmationMenu(void) = default;

	ConfirmationOption update(Arduboy2 & arduboy)
	{
		if (arduboy.justPressed(Arduboy::ButtonUp))
		{
			if (this->optionIndex > 0)
				--this->optionIndex;
		}

		if (arduboy.justPressed(Arduboy::ButtonDown))
		{
			if (this->optionIndex < 1)
				++this->optionIndex;
		}

		if (arduboy.justPressed(Arduboy::ButtonA))
		{
			return this->optionIndex == 0 ? ConfirmationOption::Yes : ConfirmationOption::No;
		}

		if (arduboy.justPressed(Arduboy::ButtonB))
		{
			return ConfirmationOption::No;
		}

		return ConfirmationOption::None;
	}

	void render(Arduboy2 & arduboy)
	{
		constexpr const uint8_t singleMargin = 4;
		constexpr const uint8_t doubleMargin = singleMargin * 2;
		constexpr const uint8_t extraMargin = singleMargin + 1;
		constexpr const uint8_t doubleExtraMargin = extraMargin * 2;
	
		constexpr const uint8_t width = defaultWidth;
		constexpr const uint8_t widthWithMargin = width + doubleMargin;
		constexpr const uint8_t x = CalculateCentreX((widthWithMargin < Arduboy::ScreenWidth) ? width : (Arduboy::ScreenWidth - doubleMargin));

		constexpr const uint8_t height = StringHeight(2);
		constexpr const uint8_t heightWithMargin = height + doubleMargin;

		constexpr const uint8_t yBase = CalculateCentreY((heightWithMargin < Arduboy::ScreenHeight) ? height : (Arduboy::ScreenHeight - doubleMargin));
		constexpr const uint8_t yStep = FontLineHeight;

		arduboy.fillRect(x - extraMargin, yBase - extraMargin, width + doubleExtraMargin, height + doubleExtraMargin, Arduboy::ColourBlack);

		uint8_t y = yBase;
		
		arduboy.setCursor(x, y);
		arduboy.print((this->optionIndex == 0) ? AsFlashString(Strings::Selected) : AsFlashString(Strings::NotSelected));
		arduboy.print(AsFlashString(Strings::Yes));

		y += yStep;
		arduboy.setCursor(x, y);
		arduboy.print((this->optionIndex == 1) ? AsFlashString(Strings::Selected) : AsFlashString(Strings::NotSelected));
		arduboy.print(AsFlashString(Strings::No));

		arduboy.drawRect(x - singleMargin, yBase - singleMargin, widthWithMargin, heightWithMargin, Arduboy::ColourWhite);
	}

	template< size_t Size >
	void render(Arduboy2 & arduboy, const char (&progmemString)[Size])
	{
		constexpr const uint8_t singleMargin = 4;
		constexpr const uint8_t doubleMargin = singleMargin * 2;
		constexpr const uint8_t extraMargin = singleMargin + 1;
		constexpr const uint8_t doubleExtraMargin = extraMargin * 2;
	
		constexpr const uint8_t stringWidth = StringWidth(Size - 1);
		constexpr const uint8_t width = (stringWidth > defaultWidth) ? stringWidth : defaultWidth;
		constexpr const uint8_t widthWithMargin = width + doubleMargin;
		constexpr const uint8_t x = CalculateCentreX((widthWithMargin < Arduboy::ScreenWidth) ? width : (Arduboy::ScreenWidth - doubleMargin));

		constexpr const uint8_t height = StringHeight(3);
		constexpr const uint8_t heightWithMargin = height + doubleMargin;

		constexpr const uint8_t yBase = CalculateCentreY((heightWithMargin < Arduboy::ScreenHeight) ? height : (Arduboy::ScreenHeight - doubleMargin));
		constexpr const uint8_t yStep = FontLineHeight;

		arduboy.fillRect(x - extraMargin, yBase - extraMargin, width + doubleExtraMargin, height + doubleExtraMargin, Arduboy::ColourBlack);

		uint8_t y = yBase;

		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(progmemString));

		y += yStep;
		arduboy.setCursor(x, y);
		arduboy.print((this->optionIndex == 0) ? AsFlashString(Strings::Selected) : AsFlashString(Strings::NotSelected));
		arduboy.print(AsFlashString(Strings::Yes));

		y += yStep;
		arduboy.setCursor(x, y);
		arduboy.print((this->optionIndex == 1) ? AsFlashString(Strings::Selected) : AsFlashString(Strings::NotSelected));
		arduboy.print(AsFlashString(Strings::No));

		arduboy.drawRect(x - singleMargin, yBase - singleMargin, widthWithMargin, heightWithMargin, Arduboy::ColourWhite);
	}
};