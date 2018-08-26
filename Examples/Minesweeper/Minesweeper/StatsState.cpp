#include "StatsState.h"

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

void StatsState::update(StateMachine & machine)
{
	auto & context = machine.getContext();
	auto & arduboy = context.arduboy;

	if (this->deleteMenuActive)
	{
		const auto option = this->deleteMenu.update(arduboy);

		if(option == ConfirmationOption::Yes)
			if (!this->cleared)
			{
				Context::stats.wins = 0;
				Context::stats.losses = 0;
				Context::SaveData::saveData(Context::stats);
				this->cleared = true;
			}

		if (option != ConfirmationOption::None)
			this->deleteMenuActive = false;
	}
	else
	{
		if (arduboy.justPressed(Arduboy::ButtonA))
			if (Context::saveCheckEnabled)
				this->deleteMenuActive = true;

		if (arduboy.justPressed(Arduboy::ButtonB))
			machine.changeState(GameStateType::TitleScreen);
	}
}

void StatsState::render(StateMachine & machine)
{
	auto & context = machine.getContext();
	auto & arduboy = context.arduboy;

	constexpr const uint8_t step = FontCharHeight + 1;

	uint8_t y = 4;

	{
		constexpr const uint8_t width = StringWidth(Strings::Stats);
		constexpr const uint8_t x = CalculateCentreX(width);
		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(Strings::Stats));
	}

	{
		constexpr const uint8_t x = 4;

		y += step;
		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(Strings::Wins));
		arduboy.print(AsFlashString(Strings::ColonSpace));

		y += step;
		arduboy.setCursor(x, y);
		arduboy.print(Context::stats.wins);
		
		y += step;
		arduboy.setCursor(x, y);
		arduboy.print(AsFlashString(Strings::Losses));
		arduboy.print(AsFlashString(Strings::ColonSpace));

		y += step;
		arduboy.setCursor(x, y);
		arduboy.print(Context::stats.losses);

		y += step;

		if (this->cleared)
		{
			y += step;
			arduboy.setCursor(x, y);
			arduboy.print(AsFlashString(Strings::Deleted));
		}
	}

	if(this->deleteMenuActive)
		this->deleteMenu.render(arduboy, Strings::Delete);
}