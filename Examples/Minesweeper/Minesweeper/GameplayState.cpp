#include "GameplayState.h"

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

#include "GameStatus.h"
#include "Languages.h"
#include "Utils.h"
#include "TileGridRenderer.h"
#include "Stack.h"
#include "Point2.h"

void GameplayState::activate(StateMachine & machine)
{
	auto & context = machine.getContext();
	context.arduboy.initRandomSeed();

	this->tiles.fill(Tile());

	this->isFirstMove = true;
	this->status = GameStatus::Playing;
}

void GameplayState::update(StateMachine & machine)
{
	auto & context = machine.getContext();
	auto & arduboy = context.arduboy;

	switch(this->status)
	{
		case GameStatus::Playing:
		{
			if(arduboy.justPressed(Arduboy::ButtonUp))
			{
				if (this->selector.Y > 0)
					--this->selector.Y;
			}

			if(arduboy.justPressed(Arduboy::ButtonDown))
			{
				if (this->selector.Y < Height - 1)
					++this->selector.Y;
			}

			if(arduboy.justPressed(Arduboy::ButtonLeft))
			{
				if (this->selector.X > 0)
					--this->selector.X;
			}

			if(arduboy.justPressed(Arduboy::ButtonRight))
			{
				if (this->selector.X < Width - 1)
					++this->selector.X;
			}

			Tile & tile = this->tiles.getItem(this->selector.X, this->selector.Y);
					
			if(arduboy.justPressed(Arduboy::ButtonA))
			{
				if(this->isFirstMove)
				{
					// Temporarily calling the current tile a mine is
					// a bit of a hack for the sake of cheapness
					tile.setIsMine(true);
					this->pickMineLocations(this->tiles, 6);
					
					tile.setIsMine(false);							
					this->fillMineCounts(this->tiles);
					
					if(tile.getMineCount() == 0)
						this->floodFillReveal(this->tiles, this->selector.X, this->selector.Y);
						
					this->isFirstMove = false;
				}			
				else if(!tile.isMarked())
				{
					tile.show();
					if(tile.isMine())
					{
						this->status = GameStatus::Lost;
						this->revealAllMines(this->tiles);
						++Context::stats.losses;
						if (Context::saveCheckEnabled)
							Context::SaveData::saveData(Context::stats);
					}
					else if(tile.getMineCount() == 0)
					{
						this->floodFillReveal(this->tiles, this->selector.X, this->selector.Y);
					}
				}
			}

			if(arduboy.justPressed(Arduboy::ButtonB))
			{
				tile.toggleMark();
			}
			
			if(this->checkForWin(this->tiles))
			{
				this->status = GameStatus::Won;
				this->revealAllMines(this->tiles);
				++Context::stats.wins;
				if (Context::saveCheckEnabled)
					Context::SaveData::saveData(Context::stats);
			}
		}
		break;
		case GameStatus::Retry:
		{
			const auto option = this->retryMenu.update(arduboy);

			switch (option)
			{
			case ConfirmationOption::None: break;
			case ConfirmationOption::Yes:
				machine.changeState(GameStateType::Gameplay);
				break;
			case ConfirmationOption::No:
				machine.changeState(GameStateType::TitleScreen);
				break;
			}
		}
		break;
		case GameStatus::Won:
		{
			if (arduboy.justPressed(Arduboy::ButtonA))
			{
				this->status = GameStatus::Retry;
			}
		}
		break;
		case GameStatus::Lost:
		{
			if (arduboy.justPressed(Arduboy::ButtonA))
			{
				this->status = GameStatus::Retry;
			}
		}
		break;
		default:
			break;
	}
}

void GameplayState::render(StateMachine & machine)
{
	auto & context = machine.getContext();

	// Draw background
	{
		constexpr uint8_t x = Arduboy::ScreenWidth - Images::BackgroundRightWidth;
		
		Sprites::drawOverwrite(0, 0, Images::BackgroundLeft, Context::stats.themeIndex);
		Sprites::drawOverwrite(x, 0, Images::BackgroundRight, Context::stats.themeIndex);
	}

	const uint8_t gridX = (Arduboy::ScreenWidth - this->VisibleGridWidth) / 2;
	const uint8_t gridY = (Arduboy::ScreenHeight - this->VisibleGridHeight) / 2;

	// Draw grid
	TileGridRenderer::drawGridAt(this->tiles, gridX, gridY);

	auto & arduboy = context.arduboy;

	// Draw selector
	{
		const uint8_t x = gridX + ((TileWidth + 1) * this->selector.X);
		const uint8_t y = gridY + ((TileHeight + 1) * this->selector.Y);

		arduboy.drawRect(x, y, (TileWidth + TileXBorder * 2), (TileHeight + TileYBorder * 2), Arduboy::ColourWhite);
	}

	// Draw large tile
	{
		constexpr uint8_t largeTileOffsetX = 9;
		constexpr uint8_t largeTileOffsetY = 6;

		const Tile & selectedTile = this->tiles.getItem(this->selector.X, this->selector.Y);
		const uint8_t x = gridX + this->VisibleGridWidth + largeTileOffsetX;
		const uint8_t y = gridY + largeTileOffsetY;
		TileGridRenderer::drawLargeTileAt(selectedTile, x, y);
	}

	switch (this->status)
	{
		case GameStatus::Playing: break;
		case GameStatus::Won: this->renderWin(arduboy); break;
		case GameStatus::Lost: this->renderLoss(arduboy); break;
		case GameStatus::Retry: this->renderRetry(arduboy); break;
	}
}

void GameplayState::renderWin(Arduboy2 & arduboy)
{
	constexpr const uint8_t singleMargin = 4;
	constexpr const uint8_t doubleMargin = singleMargin * 2;
	constexpr const uint8_t extraMargin = singleMargin + 1;
	constexpr const uint8_t doubleExtraMargin = extraMargin * 2;
		
	constexpr const uint8_t width = StringWidth(Strings::YouWin);
	constexpr const uint8_t height = StringHeight(1);

	constexpr const uint8_t x = CalculateCentreX(width);
	constexpr const uint8_t y = CalculateCentreY(height);

	arduboy.fillRect(x - extraMargin, y - extraMargin, width + doubleExtraMargin, height + doubleExtraMargin, Arduboy::ColourBlack);
	arduboy.setCursor(x, y);
	arduboy.print(AsFlashString(Strings::YouWin));
	arduboy.drawRect(x - singleMargin, y - singleMargin, width + doubleMargin, height + doubleMargin, Arduboy::ColourWhite);
}

void GameplayState::renderLoss(Arduboy2 & arduboy)
{
	constexpr const uint8_t singleMargin = 4;
	constexpr const uint8_t doubleMargin = singleMargin * 2;
	constexpr const uint8_t extraMargin = singleMargin + 1;
	constexpr const uint8_t doubleExtraMargin = extraMargin * 2;
		
	constexpr const uint8_t width = StringWidth(Strings::YouLose);
	constexpr const uint8_t height = StringHeight(1);

	constexpr const uint8_t x = CalculateCentreX(width);
	constexpr const uint8_t y = CalculateCentreY(height);

	arduboy.fillRect(x - extraMargin, y - extraMargin, width + doubleExtraMargin, height + doubleExtraMargin, Arduboy::ColourBlack);
	arduboy.setCursor(x, y);
	arduboy.print(AsFlashString(Strings::YouLose));
	arduboy.drawRect(x - singleMargin, y - singleMargin, width + doubleMargin, height + doubleMargin, Arduboy::ColourWhite);
}

void GameplayState::renderRetry(Arduboy2 & arduboy)
{
	this->retryMenu.render(arduboy, Strings::PlayAgain);
}

void GameplayState::revealAllMines(TileGrid & tiles)
{
	for(uint8_t x = 0; x < Width; ++x)
		for(uint8_t y = 0; y < Height; ++y)
		{
			Tile & tile = tiles.getItem(x, y);
			if(tile.isMine())
				tile.show();
		}
}

void GameplayState::pickMineLocations(TileGrid & tiles, uint8_t count)
{
	for(uint8_t i = 0; i < count; ++i)
		while(true)
		{
			const uint8_t x = static_cast<uint8_t>(random()) % Width;
			const uint8_t y = static_cast<uint8_t>(random()) % Height;
			
			Tile & tile = tiles.getItem(x, y);
			
			if(tile.isMine())
				continue;

			tile.setIsMine(true);
			break;
		}
}

// It would be more efficient to make this work for selected areas
// But unless more board sizes are added,
// it's cheaper (in terms of progmem) to just fill the whole board every time
void GameplayState::fillMineCounts(TileGrid & tiles)
{
	for(uint8_t x = 0; x < Width; ++x)
	{
		const uint8_t startX = (x > 0) ? x - 1 : x;
		const uint8_t endX = (x < Width - 1) ? x + 1 : x;
		
		for(uint8_t y = 0; y < Height; ++y)
		{
			const uint8_t startY = (y > 0) ? y - 1 : y;
			const uint8_t endY = (y < Height - 1) ? y + 1 : y;

			uint8_t count = 0;
			for(uint8_t i = startX; i <= endX; ++i)
				for(uint8_t j = startY; j <= endY; ++j)
				{
					if(i == x && j == y)
					{
						continue;
					}
					
					if(tiles.getItem(i, j).isMine())
					{
						++count;
					}
				}

			tiles.getItem(x, y).setMineCount(count);
		}
	}
}

void GameplayState::floodFillReveal(TileGrid & tiles, uint8_t x, uint8_t y)
{
	using Point = Point2<uint8_t>;
	using PointStack = Stack<Point, TileCount / 2>;
	PointStack stack = PointStack();

	{
		Tile & tile = tiles.getItem(x, y);
		tile.show();
		if (tile.getMineCount() == 0)
			stack.push(Point(x, y));
	}

	while (!stack.isEmpty())
	{
		const Point current = stack.pull();

		if (current.X > 0)
		{
			const auto y = current.Y;
			const auto x = current.X - 1;
			Tile & tile = tiles.getItem(x, y);
			if (!tile.isVisible())
			{
				tile.show();
				if (tile.getMineCount() == 0)
					stack.push(Point(x, y));
			}
		}
		if (current.X < Width - 1)
		{
			const auto y = current.Y;
			const auto x = current.X + 1;
			Tile & tile = tiles.getItem(x, y);
			if (!tile.isVisible())
			{
				tile.show();
				if (tile.getMineCount() == 0)
					stack.push(Point(x, y));
			}
		}
		if (current.Y > 0)
		{
			const auto y = current.Y - 1;
			const auto x = current.X;
			Tile & tile = tiles.getItem(x, y);
			if (!tile.isVisible())
			{
				tile.show();
				if (tile.getMineCount() == 0)
					stack.push(Point(x, y));
			}
		}
		if (current.Y < Height - 1)
		{
			const auto y = current.Y + 1;
			const auto x = current.X;
			Tile & tile = tiles.getItem(x, y);
			if (!tile.isVisible())
			{
				tile.show();
				if (tile.getMineCount() == 0)
					stack.push(Point(x, y));
			}
		}
	}
}
	
bool GameplayState::checkForWin(const TileGrid & tiles)
{
	for(uint8_t x = 0; x < Width; ++x)
		for(uint8_t y = 0; y < Height; ++y)
		{
			const Tile & tile = tiles.getItem(x, y);
			if(!tile.isMine() && !tile.isVisible())
				return false;
		}
	return true;
}