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
#include <stdint.h>

#include "Settings.h"
#include "GameStats.h"
#include "SaveSystem.h"
#include "Utils.h"

class GameContext
{
public:
	using SaveData = SaveSystem<Checksum<SkullHash>, 1024, 512, GameStats>;

public:
	constexpr const static uint8_t Width = 7;
	constexpr const static uint8_t Height = 7;

	Arduboy2 arduboy;

	constexpr const static uint8_t FirstThemeIndex = 0;
	constexpr const static uint8_t LastThemeIndex = 4;

	static GameStats stats;

#if DISABLE_SAVE_CHECK
	constexpr const static bool saveCheckEnabled = true;
#else
	static bool saveCheckEnabled;
#endif
};