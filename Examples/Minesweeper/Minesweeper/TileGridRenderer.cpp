#include "TileGridRenderer.h"

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

void TileGridRenderer::drawTileAt(const Tile & tile, const uint8_t & x, const uint8_t & y)
{	
	uint8_t index = 0;

	if (tile.isVisible())
	{
		if (tile.isMine())
			index = (12 + GameContext::stats.themeIndex);
		else
			index = tile.getMineCount();
	}
	else
	{
		if (tile.isMarked())
			index = 11;
		else
			index = 10;
	}

	Sprites::drawOverwrite(x, y, Images::Tiles, index);
}

void TileGridRenderer::drawLargeTileAt(const Tile & tile, const uint8_t & x, const uint8_t & y)
{
	uint8_t index = 0;

	if (tile.isVisible())
	{
		if (tile.isMine())
			index = (12 + GameContext::stats.themeIndex);
		else
			index = tile.getMineCount();
	}
	else
	{
		if (tile.isMarked())
			index = 11;
		else
			index = 10;
	}

	Sprites::drawOverwrite(x, y, Images::LargeTiles, index);
}