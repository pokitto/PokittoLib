#ifndef POKITTOTUI_TASUI_UI_HPP
#	define POKITTOTUI_TASUI_UI_HPP

#	include <tasui>

#	include <Pokitto.h>
#	include <TASMODE.h>
#	include "tasui_Settings.hpp"
#	include "ptui_UITileMap.hpp"

#	ifdef TASUI

namespace tasui
{
	
	// A convenient wrapper for the TASUI.
	class UI
	{
	public: // Types & Constants.
		using Color = std::uint8_t;
		using Tile = std::uint8_t;
		using Delta = std::uint8_t;
		using Symbol = ptui::Symbol;
		static constexpr int tileWidth = TASUI_TILE_WIDTH;
		static constexpr int tileHeight = TASUI_TILE_HEIGHT;
		static constexpr int mapColumns = TASUI_COLUMNS;
		static constexpr int mapRows = TASUI_ROWS;
		static constexpr bool tilesCanBeTransparent = (TASUI_TILE_TRANSPARENCY == 1);
		static constexpr bool colorsCanBeRemapped = (TASUI_CLUT == 1);
		static constexpr int clutSize = colorsCanBeRemapped ? 256 : 0;
		static constexpr bool tilesCanBeRecolored = (TASUI_TILE_DELTA == 1);
		using TASUITileMap = ptui::UITileMap<mapColumns, mapRows, tileWidth, tileHeight, PROJ_LCDWIDTH, tilesCanBeRecolored, clutSize>;
	
	
	public: // Bridges (aka init).
		// Note: `show*` methods won't do anything if the Tileset wasn't configured *before*.
	
		// Hides the UI, whereever layer it is on.
		static void hide() noexcept;
		// Resets the rendering order to TileMap, Sprites, UI.
		static void showTileMapSpritesUI() noexcept;
		// Resets the rendering order to TileMap, UI, Sprites.
		static void showTileMapUISprites() noexcept;
		// Shows the UI on top of everything else.
		static void showOnTop() noexcept;
		
	
    public: // Tilesets.
		// Changes the tileset.
		// - Only 6x6, 8bpp tiles are supported.
		// - Compatible tilesets can be found in the `puits` namespace.
		// - Accepts `nullptr`, which will also remove the UI from rendition.
		static void setTilesetImage(const std::uint8_t* tilesetImage) noexcept;
		
	
    public: // Offset.
        // Changes the origin to the top-left of the display, in pixel units.
        // - 0, 0 means the top-left pixel of the UI will be the same than the top-left pixel of the display.
        static void setOffset(int offsetX, int offsetY) noexcept;
        static int offsetX() noexcept;
        static int offsetY() noexcept;
        
        
    public: // Tiles Access.
        // Changes a given tile in this map.
		// - Nothing happens if trying to write outside the map.
        static void setTile(int column, int row, Tile tile) noexcept;
        
        // Changes the targeted tile's delta.
		// - Nothing happens if trying to write outside the map.
        static void setDelta(int column, int row, Delta delta) noexcept;
        
        // Changes the targeted tile and its delta.
		// - Nothing happens if trying to write outside the map.
        static void setTileAndDelta(int column, int row, Tile tile, Delta delta) noexcept;
        
        // Returns a given tile in this map.
		// - `outsideTile` is returned if reading outside the map.
        static Tile getTile(int column, int row, Tile outsideTile = 0) noexcept;
        
        // Returns the Delta for a given location.
		// - `outsideDelta` is returned if reading outside the map.
        static Delta getDelta(int column, int row, Delta outsideDelta = 0) noexcept;
        
    
    public: // Mass Tiles Access.
    
        // Clears out the whole tilemap, tiles and deltas.
        static void clear(Tile tile = 0, Delta delta = 0) noexcept;
    
        // Sets the whole map with `tile`.
        static void clearTiles(Tile tile) noexcept;
        // Sets the whole map's deltas to `delta`.
        static void clearDeltas(Delta delta) noexcept;
        
        // Same than above, on a defined area.
        // - `firstColumn`, `firstRow`, `lastColumn` and `lastRow` will be clamped.
        // - `lastColumn` and `lastRow` are included.
        // - Negative and reversed boxes are considered as empty ones.
        static void fillRectTiles(int firstColumn, int firstRow, int lastColumn, int lastRow, Tile tile) noexcept;
        
        // Same than above, but for the tile deltas.
        static void fillRectDeltas(int firstColumn, int firstRow, int lastColumn, int lastRow, Delta delta) noexcept;
        
        // Both fillRectTiles and fillRectDeltas.
        static void fillRectTilesAndDeltas(int firstColumn, int firstRow, int lastColumn, int lastRow, Tile tile, Delta delta) noexcept;
        
        // Shift the whole map by `shiftedColumns` columns and `shiftedRows` rows.
        // - "Introduced" Tiles will be *left as is*.
        // - If Deltas are supported, they're shifted as well.
        static void shift(int shiftedColumns, int shiftedRows) noexcept;
        
        // Same than above, on a given area.
        // - `firstColumn`, `firstRow`, `lastColumn` and `lastRow` will be clamped.
        // - `lastColumn` and `lastRow` are included.
        // - Negative and reversed boxes are considered as empty ones.
        static void shift(int firstColumn, int firstRow, int lastColumn, int lastRow,
						  int shiftedColumns, int shiftedRows) noexcept;
        
        
    public: // CLUT.
        // Remaps a color to a new one.
        // - That is, when `color` is encountered in the Tileset, `newColor` will be returned instead.
        // - Using `0` as `newColor` will make the color transparent (if transparency is enabled)!
        static void mapColor(Color color, Color newColor) noexcept;
		
		// Returns the mapped colors.
		static Color mappedColor(Color color) noexcept;
        
        // Resets the mapping to a default x => x setting.
        static void resetCLUT() noexcept;
		
		
    public: // Widgets.
        // Draws a gauge filled with the given amount, extending from `firstColumn` to `lastColumn` at the given row.
        // - If `current` is negative or 0, the gauge will be empty.
        // - If `current` is `max`, the gauge will be full.
        // - If the gauge is reversed or if `max` is negative, nothing happens.
        // - The drawing will be clipped as needed.
        // - Doesn't change the delta.
        static void drawGauge(int firstColumn, int lastColumn, int row,
							  int current, int max) noexcept;
        
        // Returns the tile index for the given symbol.
		// - Check the Symbol enum for more information.
        static int indexForSymbol(Symbol symbol) noexcept;
		
        // Draws the given symbol at`column`, `row`.
		// - Check the Symbol enum for more information.
        // - Doesn't change the delta.
        static void drawSymbol(int column, int row, Symbol symbol) noexcept;
		
        
        // Draws a Box.
        // - `firstColumn`, `firstRow`, `lastColumn` and `lastRow` will not be clamped, but clipped instead.
        // - `lastColumn` and `lastRow` are included.
        // - Empty or negative boxes won't be drawn.
        // - Due to the limitation of tileset, 1-column and 1-row boxes will be rendered as Spaces.
        // - Doesn't change the delta. You can use fillRectDeltas with the same parameters to do so.
        static void drawBox(int firstColumn, int firstRow, int lastColumn, int lastRow) noexcept;
        
        
    public: // Terminal.
        // Sets where to draw the next character.
        // - Clamped to the Cursor Bounding Box.
        static void setCursor(int cursorColumn, int cursorRow) noexcept;
		
		// Returns the cursor's column number.
		static int cursorColumn() noexcept;
		// Returns the cursor's row number.
		static int cursorRow() noexcept;
		
        
        // Sets the delta to apply for each printed tile after this call.
        // This usually change the color, depending on the main palette and the CLUT (if enabled).
        static void setCursorDelta(Delta delta) noexcept;
        
        // Returns the delta applied for each printed tile.
        static Delta cursorDelta() noexcept;
        
        // Changes the cursor's bounding box.
        // - Cursor's position will be updated to fit inside.
		// - Printing character will make everything fit inside.
		// - When reaching `cursorLastColumn`, `cursorLastRow`, the whole box will autoscroll by 1 row.
        static void setCursorBoundingBox(int cursorFirstColumn, int cursorFirstRow, int cursorLastColumn, int cursorLastRow) noexcept;
        
        // Changes the Cursor Bounding Box back to the default, which covers the whole map.
        // - Cursor's position will be updated to fit inside.
        static void resetCursorBoundingBox() noexcept;
    
        // Prints a single character.
        // - All the Control characters are ignored, besides newlines and tabs.
        // - Tabs are based on the TilesetDefinition's `tabColumns` characters wide columns, starting from the bound's firstColumn.
        // - If printing such a char would go above the last column, then a newline is generated before.
        //   - If `c` was a space, it won't be printed and transformed instead into the said newline.
        // - Note: This is not suitable for setting a tile to a given value. Use `set()` instead.
        // - Delta is set to the cursor's delta.
        static void printChar(char c) noexcept;
        
        // Prints a string.
        // - Delta is set to the cursor's delta.
        static void printString(const char* string) noexcept;
        
        // Prints a string, but truncated up to limit.
        // - Delta is set to the cursor's delta.
        static void printString(const char* string, int limit) noexcept;
        
        // Prints a number with the given radix.
        // - Delta is set to the cursor's delta.
        static void printInteger(int number, int padding = 0, char paddingChar = ' ', int radix = 10) noexcept;
        
        // Similar to printString, but it'll try to preserve "words" (a subchain of characters comprised of anything but whitespaces and control characters) together and generate newlines early before.
        // - Delta is set to the cursor's delta.
        static void printText(const char* text) noexcept;
        
        // Same than above, but only a limited number of characters are printed out of `text`.
        // - Delta is set to the cursor's delta.
        static void printText(const char* text, int limit) noexcept;
		
        // Prints the given symbol.
		// - Symbols are various special symbol (arrows, checkbox, ...) that cannot be printed using regular characters.
		// - Check the Symbol enum for more information.
        // - Delta is set to the cursor's delta.
        static void printSymbol(Symbol symbol) noexcept;
		
		
	public: // Undocumented things that probably aren't of any interest for you.
		// Oh crap, I forgot to document this! You'd best not try, it might be dangerous, being undocumented and all. Might fry your Pokitto, who knows.
		static void LineFiller(std::uint8_t* line, std::uint32_t y, bool skip) noexcept;
	
		// Another thing left undocumented. Best not use that either!
		static TASUITileMap uiTileMap;
		
		// No complain will be accepted.
	};
}

namespace Pokitto
{
	// Convenient typedef.
	using UI = tasui::UI;
}

#	endif // TASUI

#endif // POKITTOTUI_TASUI_UI_HPP