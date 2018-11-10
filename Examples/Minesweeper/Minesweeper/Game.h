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
#include "GameStateType.h"
#include "GameState.h"
#include "GameStateMachine.h"

#include "Utils.h"

#include "SplashscreenState.h"
#include "TitlescreenState.h"
#include "GameplayState.h"
#include "CreditsState.h"
#include "StatsState.h"
#include "ThemesState.h"
#if !DISABLE_SAVE_CHECK
#include "SaveCheckState.h"
#endif

class Game : public GameStateMachine<GameContext, GameStateType>
{
private:
	// This union would be ill formed if it was instantiated
	union StateData
	{
		SplashScreenState splashScreenState;
		TitleScreenState titleScreenState;
		GameplayState gameplayState;
		CreditsState creditsState;
		StatsState statsState;
		ThemesState themesState;
		#if !DISABLE_SAVE_CHECK
		SaveCheckState saveCheckState;
		#endif
	};

private:
	Context context;
	StateId currentStateId;
	StateId nextStateId;
	bool changePending;
	State * currentState;
	char stateData[sizeof(StateData)];
	
public:
	void setup(void);
	void loop(void);

	Context & getContext(void) override;
	const Context & getContext(void) const override;

	void changeState(const StateId & stateId) override;

private:
	State * createState(const StateId & stateType);

};