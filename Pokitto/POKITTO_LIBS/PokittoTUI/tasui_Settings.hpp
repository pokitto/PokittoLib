#ifndef POKITTOTUI_TASUI_SETTINGS_HPP
#	define POKITTOTUI_TASUI_SETTINGS_HPP

// Configuration for TASUI.

// TASUI must be defined in order to enable the library.
// TASUI_TILE_WIDTH and TASUI_TILE_HEIGHT defines the size of an individual tile.
// - They're *NOT* configurable and forever left to 6x6.
// TASUI_COLUMNS defines the number of columns in the UI tilemap.
// - Default will be enough to cover the screen horizontally (37 in HiRes).
// TASUI_ROWS defines the number of rows in the UI tilemap.
// - Default will be enough to cover the screen vertically (30 in HiRes).
// TASUI_TILE_TRANSPARENCY tells if tiles can have transparent pixels (1) or not (0) on their 0-indexed color.
// - Enabled by default. (1)
// TASUI_CLUT tells if Tiles' colors can be recolored (1) or not (0).
// - Enabled by default. (1)
// TASUI_TILE_DELTA tells if a Delta can be associated to each tile (1) or not (0).
// - Deltas are used to recolor individual Tiles.
// - Enabled by default. (1)

#	ifdef TASUI

// TASUI requires TAS.
#		if PROJ_SCREENMODE != TASMODE
#			error TASUI requires TASMODE or TASMODELOW
#		endif

// Tile size is not overridable (yet).
#		ifdef TASUI_TILE_WIDTH
#			error TASUI_TILE_WIDTH cannot be defined.
#		endif // TASUI_TILE_WIDTH
#		define TASUI_TILE_WIDTH 6
#		ifdef TASUI_TILE_HEIGHT
#			error TASUI_TILE_HEIGHT cannot be defined.
#		endif // TASUI_TILE_HEIGHT
#		define TASUI_TILE_HEIGHT 6


#		ifndef TASUI_COLUMNS
#			define TASUI_COLUMNS (((PROJ_LCDWIDTH) + (TASUI_TILE_WIDTH) - 1) / (TASUI_TILE_WIDTH));
#		endif // TASUI_COLUMNS
#		ifndef TASUI_ROWS
#			define TASUI_ROWS (((PROJ_LCDHEIGHT) + (TASUI_TILE_HEIGHT) - 1) / (TASUI_TILE_HEIGHT));
#		endif // TASUI_ROWS

#		ifndef TASUI_TILE_TRANSPARENCY
#			define TASUI_TILE_TRANSPARENCY 1
#		endif // TASUI_TILE_TRANSPARENCY

#		ifndef TASUI_CLUT
#			define TASUI_CLUT 1
#		endif // TASUI_CLUT

#		ifndef TASUI_TILE_DELTA
#			define TASUI_TILE_DELTA 1
#		endif // TASUI_TILE_DELTA

#	endif // TASUI

#endif // POKITTOTUI_TASUI_SETTINGS_HPP
