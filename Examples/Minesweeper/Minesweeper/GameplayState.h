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
#include "GameState.h"
#include "GameStateType.h"
#include "GameStatus.h"
#include "TileRenderingSettings.h"
#include "Tile.h"
#include "Grid.h"
#include "Point2.h"
#include "ConfirmationMenu.h"

class GameplayState : public GameState<GameContext, GameStateType>
{
private:

	using TileGrid = Grid<Tile, GameContext::Width, GameContext::Height>;

	constexpr const static uint8_t Width = TileGrid::Width;
	constexpr const static uint8_t Height = TileGrid::Height;
	constexpr const static uint16_t TileCount = TileGrid::Count;

public:
	using RenderingSettings = TileRenderingSettings;

private:

	constexpr const static uint8_t TileWidth = RenderingSettings::TileWidth;
	constexpr const static uint8_t TileHeight = RenderingSettings::TileHeight;

	constexpr const static uint8_t TileXBorder = RenderingSettings::TileXBorder;
	constexpr const static uint8_t TileYBorder = RenderingSettings::TileYBorder;

	constexpr const static uint8_t LargeTileWidth = RenderingSettings::LargeTileWidth;
	constexpr const static uint8_t LargeTileHeight = RenderingSettings::LargeTileHeight;

	constexpr const static uint8_t VisibleGridWidth = TileXBorder + (TileWidth * Width) + (TileXBorder * Width);
	constexpr const static uint8_t VisibleGridHeight = TileYBorder + (TileWidth * Height) + (TileYBorder * Height);

private:
	TileGrid tiles;
	GameStatus status;
	Point2<uint8_t> selector;
	bool isFirstMove;
	ConfirmationMenu retryMenu;
	
public:
	void activate(StateMachine & machine) override;
	
	void update(StateMachine & machine) override;
	void render(StateMachine & machine) override;
	
private:
	void updateGameplay(StateMachine & machine);
	void updateWin(StateMachine & machine);
	void updateLoss(StateMachine & machine);
	void updateRetry(StateMachine & machine);

	void renderGameplay(StateMachine & machine);
	void renderWin(Arduboy2 & arduboy);
	void renderLoss(Arduboy2 & arduboy);
	void renderRetry(Arduboy2 & arduboy);

private:
	static void revealAllMines(TileGrid & tiles);

	static void fillMineCounts(TileGrid & tiles);

	static void pickMineLocations(TileGrid & tiles, uint8_t count);

	static void floodFillReveal(TileGrid & tiles, uint8_t x, uint8_t y);
	
	static bool checkForWin(const TileGrid & tiles);
};

