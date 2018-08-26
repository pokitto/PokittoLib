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

#include "Tile.h"
#include "Grid.h"

#include "Images.h"
#include "TileRenderingSettings.h"
//#include "Utils/ProgmemUtils.h"

//
// Declaration
//

class TileGridRenderer
{
public:
	constexpr const static uint8_t TileXBorder = TileRenderingSettings::TileXBorder;
	constexpr const static uint8_t TileYBorder = TileRenderingSettings::TileYBorder;

	constexpr const static uint8_t TileWidth = TileRenderingSettings::TileWidth;
	constexpr const static uint8_t TileHeight = TileRenderingSettings::TileHeight;

	constexpr const static uint8_t LargeTileWidth = TileRenderingSettings::LargeTileWidth;
	constexpr const static uint8_t LargeTileHeight = TileRenderingSettings::LargeTileHeight;

public:
	template< uint8_t Width, uint8_t Height >
	inline static void drawGridAt(const Grid<Tile, Width, Height> & grid, const uint8_t & x, const uint8_t & y);

	static void drawTileAt(const Tile & tile, const uint8_t & x, const uint8_t & y);

	static void drawLargeTileAt(const Tile & tile, const uint8_t & x, const uint8_t & y);
};

//
// Definition
//

template< uint8_t Width, uint8_t Height >
inline void TileGridRenderer::drawGridAt(const Grid<Tile, Width, Height> & grid, const uint8_t & x, const uint8_t & y)
{
	for(uint8_t xi = 0; xi < Width; ++xi)
		for(uint8_t yi = 0; yi < Height; ++yi)
		{
			const Tile & tile = grid.getItem(xi, yi);
			const uint8_t xPos = x + (TileXBorder + ((TileWidth + TileXBorder) * xi));
			const uint8_t yPos = y + (TileYBorder + ((TileHeight + TileYBorder) * yi));
			drawTileAt(tile, xPos, yPos);
		}
}
