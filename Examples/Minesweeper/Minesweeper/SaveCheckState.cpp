#include "SaveCheckState.h"

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

void SaveCheckState::activate(StateMachine & machine)
{
	if (Context::SaveData::verifyChecksum())
	{
		Context::stats = Context::SaveData::loadData();
		if(Context::stats.themeIndex > Context::LastThemeIndex)
			Context::stats.themeIndex = Context::LastThemeIndex;
		machine.changeState(GameStateType::TitleScreen);
	}
}

void SaveCheckState::update(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;
	const auto option = this->menu.update(arduboy);

	if (option != ConfirmationOption::None)
	{
		if (option == ConfirmationOption::Yes)
		{
			Context::stats = GameStats();
			Context::SaveData::initialise(Context::stats);
			Context::saveCheckEnabled = true;
		}

		if (option == ConfirmationOption::No)
		{
			Context::saveCheckEnabled = false;
		}

		machine.changeState(GameStateType::TitleScreen);
	}
}

void SaveCheckState::render(StateMachine & machine)
{
	auto & arduboy = machine.getContext().arduboy;
	arduboy.print(AsFlashString(Strings::CreateSave));
	this->menu.render(arduboy);
}