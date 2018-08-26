#include "SplashscreenState.h"

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

#include "Settings.h"
#include "Images.h"
#include "Languages.h"
#include "Credits.h"

#include "Utils.h"

void SplashScreenState::activate(StateMachine & machine)
{
	auto & context = machine.getContext();
	context.arduboy.initRandomSeed();
}

void SplashScreenState::update(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;

	if (arduboy.justPressed(Arduboy::ButtonA) || arduboy.justPressed(Arduboy::ButtonB))
	{
#if DISABLE_SAVE_CHECK
		if (Context::SaveData::verifyChecksum())
		{
			Context::stats = Context::SaveData::loadData();
		}
		else
		{
			Context::stats = GameStats();
			Context::SaveData::initialise(Context::stats);
		}
		
		machine.changeState(GameStateType::TitleScreen);
#else
		machine.changeState(GameStateType::SaveCheck);
#endif
		return;
	}

	// TODO: fix this god-awful mess
	if (blinkDelay > 0)
	{
		if (arduboy.everyXFrames(60))
		{
			--this->blinkDelay;
			if (blinkDelay == 0)
				this->blinkInvert = !this->blinkInvert;
		}
	}
	else if (arduboy.everyXFrames(2))
	{
		if (this->blinkInvert)
		{
			if (this->blinkTick > BlinkTickMin)
				--this->blinkTick;
			else
				this->blinkDelay = BlinkDelayDefault + (static_cast<uint8_t>(random()) % (BlinkDelayVariance + 1));
		}
		else
		{
			if (this->blinkTick < BlinkTickMax)
				++this->blinkTick;
			else
				this->blinkInvert = !this->blinkInvert;
		}
	}
}

void SplashScreenState::render(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;
	
	{
		constexpr const uint8_t x = CalculateCentreX(Images::PharapLogoWidth);
		constexpr const uint8_t y = CalculateCentreY(Images::PharapLogoHeight);

		Sprites::drawOverwrite(x, y, Images::PharapLogo, 0);
		arduboy.fillRect(x + 14, y + 15, 20, this->blinkTick, Arduboy::ColourWhite);
		arduboy.fillRect(x + 14, (y + 27) - this->blinkTick, 20, this->blinkTick, Arduboy::ColourWhite);
	}

	/*{
		constexpr const uint8_t width = StringWidth(StringProgrammer0);
		constexpr const uint8_t height = FontCharHeight;

		constexpr const uint8_t x = CalculateCentreX(width);

		auto & arduboy = machine.getContext().arduboy;

		arduboy.setCursor(x, y + PharapLogoHeight + 4);
		arduboy.print(AsFlashString(StringPharap));
	}*/
}