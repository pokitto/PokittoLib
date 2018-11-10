#include "Game.h"

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

#include "Settings.h"
#include "SplashscreenState.h"
#include "TitlescreenState.h"
#include "GameplayState.h"
#include "CreditsState.h"
#include "StatsState.h"
#include "ThemesState.h"
#if !DISABLE_SAVE_CHECK
#include "SaveCheckState.h"
#endif

void Game::setup(void)
{
	auto & arduboy = this->context.arduboy;

	this->currentStateId = GameStateType::SplashScreen;
	this->currentState = createState(this->currentStateId);
	this->nextStateId = this->currentStateId;

	arduboy.begin();
	arduboy.initRandomSeed();

	this->currentState->activate(*this);
}

void Game::loop(void)
{
	auto & arduboy = this->context.arduboy;

	if(!arduboy.nextFrame())
		return;

	arduboy.pollButtons();

	if (this->currentState != nullptr)
		this->currentState->update(*this);
		
	arduboy.clear();

	if (this->currentState != nullptr)
		this->currentState->render(*this);

	arduboy.display();
	
	while(this->changePending)
	{
		this->currentState->deactivate(*this);

		if (this->currentStateId != this->nextStateId)
		{
			// Strictly speaking this does nothing, but I'm keeping it for the sake of correctness
			this->currentState->~GameState();
			this->currentState = this->createState(this->nextStateId);
			this->currentStateId = this->nextStateId;
		}

		this->changePending = false;
		this->currentState->activate(*this);
	}
}

Game::Context & Game::getContext(void)
{
	return this->context;
}

const Game::Context & Game::getContext(void) const
{
	return this->context;
}

void Game::changeState(const StateId & stateId)
{
	this->nextStateId = stateId;
	this->changePending = true;
}

Game::State * Game::createState(const StateId & stateType)
{
	switch (stateType)
	{
		case GameStateType::SplashScreen: return new (&this->stateData[0]) SplashScreenState();
		case GameStateType::TitleScreen: return new (&this->stateData[0]) TitleScreenState();
		case GameStateType::Gameplay: return new (&this->stateData[0]) GameplayState();
		case GameStateType::Credits: return new (&this->stateData[0]) CreditsState();
		case GameStateType::Stats: return new (&this->stateData[0]) StatsState();
		case GameStateType::Themes: return new (&this->stateData[0]) ThemesState();
		#if !DISABLE_SAVE_CHECK
		case GameStateType::SaveCheck: return new (&this->stateData[0]) SaveCheckState();
		#endif
		default: return nullptr;
	}
}