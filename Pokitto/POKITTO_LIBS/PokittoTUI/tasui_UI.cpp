#include "tasui_UI.hpp"

#	ifdef TASUI

namespace tasui
{
	constexpr auto availableLineFillersCount = sizeof(Pokitto::Display::lineFillers) / sizeof(Pokitto::Display::lineFillers[0]);
	
	
	// UI - Bridges (aka init).
	
	void UI::hide() noexcept
	{
		using PDisplay = Pokitto::Display;
		
		for (auto& lineFiller: PDisplay::lineFillers)
			if (lineFiller == &UI::LineFiller)
				lineFiller = TAS::NOPFiller;
	}
	
	void UI::showTileMapSpritesUI() noexcept
	{
		using PDisplay = Pokitto::Display;
		
		// Clears everything.
		for (auto& lineFiller : PDisplay::lineFillers)
			lineFiller = TAS::NOPFiller;
		if (availableLineFillersCount > 0)
		{
			PDisplay::lineFillers[0] = TAS::BGTileFiller;
			if (availableLineFillersCount > 1)
			{
				PDisplay::lineFillers[1] = TAS::SpriteFiller;
				if (availableLineFillersCount > 2)
						PDisplay::lineFillers[2] = UI::LineFiller;
			}
		}
	}
	
	void UI::showTileMapUISprites() noexcept
	{
		using PDisplay = Pokitto::Display;
		
		// Clears everything.
		for (auto& lineFiller : PDisplay::lineFillers)
			lineFiller = TAS::NOPFiller;
		if (availableLineFillersCount > 0)
		{
			PDisplay::lineFillers[0] = TAS::BGTileFiller;
			if (availableLineFillersCount > 1)
			{
				PDisplay::lineFillers[1] = UI::LineFiller;
				if (availableLineFillersCount > 2)
					PDisplay::lineFillers[2] = TAS::SpriteFiller;
			}
		}
	}
	
	void UI::showOnTop() noexcept
	{
		using PDisplay = Pokitto::Display;
		
		hide();
		
		int lastOperatingLineFillerI = availableLineFillersCount;
		
		// Finds the last operator LineFiller's index.
		for (lastOperatingLineFillerI--; lastOperatingLineFillerI >= 0; lastOperatingLineFillerI--)
			if (PDisplay::lineFillers[lastOperatingLineFillerI] != TAS::NOPFiller)
				break;
		// Adds our LineFiller right after it, if possible.
		if (lastOperatingLineFillerI + 1 < availableLineFillersCount)
			PDisplay::lineFillers[lastOperatingLineFillerI + 1] = &UI::LineFiller;
	}
		
	
    // UI - Tilesets.
	
	void UI::setTilesetImage(const std::uint8_t* tilesetImage) noexcept
	{
		uiTileMap.setTilesetImage(tilesetImage);
	}
		
	
    // UI - Offset.
	
	void UI::setOffset(int offsetX, int offsetY) noexcept
	{
		uiTileMap.setOffset(offsetX, offsetY);
	}
	
	int UI::offsetX() noexcept
	{
		return uiTileMap.offsetX();
	}
	
	int UI::offsetY() noexcept
	{
		return uiTileMap.offsetY();
	}
        
        
    // UI - Tiles Access.
	
	void UI::setTile(int column, int row, Tile tile) noexcept
	{
		uiTileMap.setTile(column, row, tile);
	}
	
	void UI::setDelta(int column, int row, Delta delta) noexcept
	{
		uiTileMap.setDelta(column, row, delta);
	}
	
	void UI::setTileAndDelta(int column, int row, Tile tile, Delta delta) noexcept
	{
		uiTileMap.setTileAndDelta(column, row, tile, delta);
	}
	
	UI::Tile UI::getTile(int column, int row, Tile outsideTile) noexcept
	{
		return uiTileMap.getTile(column, row, outsideTile);
	}
	
	UI::Delta UI::getDelta(int column, int row, Delta outsideDelta) noexcept
	{
		return uiTileMap.getDelta(column, row, outsideDelta);
	}
        
    
    // UI - Mass Tiles Access.
    
	void UI::clear(Tile tile, Delta delta) noexcept
	{
		uiTileMap.clear(tile, delta);
	}

	void UI::clearTiles(Tile tile) noexcept
	{
		uiTileMap.clearTiles(tile);
	}
	
	void UI::clearDeltas(Delta delta) noexcept
	{
		uiTileMap.clearDeltas(delta);
	}
	
	void UI::fillRectTiles(int firstColumn, int firstRow, int lastColumn, int lastRow, Tile tile) noexcept
	{
		uiTileMap.fillRectTiles(firstColumn, firstRow, lastColumn, lastRow, tile);
	}
	
	void UI::fillRectDeltas(int firstColumn, int firstRow, int lastColumn, int lastRow, Delta delta) noexcept
	{
		uiTileMap.fillRectDeltas(firstColumn, firstRow, lastColumn, lastRow, delta);
	}
	
	void UI::fillRectTilesAndDeltas(int firstColumn, int firstRow, int lastColumn, int lastRow, Tile tile, Delta delta) noexcept
	{
		uiTileMap.fillRectTilesAndDeltas(firstColumn, firstRow, lastColumn, lastRow, tile, delta);
	}
	
	void UI::shift(int shiftedColumns, int shiftedRows) noexcept
	{
		uiTileMap.shift(shiftedColumns, shiftedRows);
	}
	
	void UI::shift(int firstColumn, int firstRow, int lastColumn, int lastRow, int shiftedColumns, int shiftedRows) noexcept
	{
		uiTileMap.shift(firstColumn, firstRow, lastColumn, lastRow,
						shiftedColumns, shiftedRows);
	}
        
        
    // UI - CLUT.
	
	void UI::mapColor(Color color, Color newColor) noexcept
	{
		uiTileMap.mapColor(color, newColor);
	}
	UI::Color UI::mappedColor(Color color) noexcept
	{
		return uiTileMap.mappedColor(color);
	}
	
	void UI::resetCLUT() noexcept
	{
		uiTileMap.resetCLUT();
	}
		
		
    // UI - Widgets.
	
	void UI::drawGauge(int firstColumn, int lastColumn, int row, int current, int max) noexcept
	{
		uiTileMap.drawGauge(firstColumn, lastColumn, row, current, max);
	}
	
	int UI::indexForSymbol(Symbol symbol) noexcept
	{
		return uiTileMap.indexForSymbol(symbol);
	}
	
	void UI::drawSymbol(int column, int row, Symbol symbol) noexcept
	{
		uiTileMap.drawSymbol(column, row, symbol);
	}
	
	void UI::drawBox(int firstColumn, int firstRow, int lastColumn, int lastRow) noexcept
	{
		uiTileMap.drawBox(firstColumn, firstRow, lastColumn, lastRow);
	}
        
        
    // UI - Terminal.
	
	void UI::setCursor(int cursorColumn, int cursorRow) noexcept
	{
		uiTileMap.setCursor(cursorColumn, cursorRow);
	}
		
	int UI::cursorColumn() noexcept
	{
		return uiTileMap.cursorColumn();
	}
	
	int UI::cursorRow() noexcept
	{
		return uiTileMap.cursorRow();
	}
	
	void UI::setCursorDelta(Delta delta) noexcept
	{
		uiTileMap.setCursorDelta(delta);
	}
	
	UI::Delta UI::cursorDelta() noexcept
	{
		return uiTileMap.cursorDelta();
	}
	
	void UI::setCursorBoundingBox(int cursorFirstColumn, int cursorFirstRow, int cursorLastColumn, int cursorLastRow) noexcept
	{
		uiTileMap.setCursorBoundingBox(cursorFirstColumn, cursorFirstRow, cursorLastColumn, cursorLastRow);
	}
	
	void UI::resetCursorBoundingBox() noexcept
	{
		uiTileMap.resetCursorBoundingBox();
	}

	void UI::printChar(char c) noexcept
	{
		uiTileMap.printChar(c);
	}
	
	void UI::printString(const char* string) noexcept
	{
		uiTileMap.printString(string);
	}
	
	void UI::printString(const char* string, int limit) noexcept
	{
		uiTileMap.printString(string, limit);
	}
	
	void UI::printInteger(int number, int padding, char paddingChar, int radix) noexcept
	{
		uiTileMap.printInteger(number, padding, paddingChar, radix);
	}
	
	void UI::printText(const char* text) noexcept
	{
		uiTileMap.printText(text);
	}
	
	void UI::printText(const char* text, int limit) noexcept
	{
		uiTileMap.printText(text, limit);
	}
	
	void UI::printSymbol(Symbol symbol) noexcept
	{
		uiTileMap.printSymbol(symbol);
	}
		
		
	// UI - Undocumented things that probably aren't of any interest for you.

	void UI::LineFiller(std::uint8_t* line, std::uint32_t y, bool skip) noexcept
	{
		uiTileMap.renderIntoLineBuffer<tilesCanBeTransparent, true, true>(line, y, skip);
	}
	
	UI::TASUITileMap UI::uiTileMap;
}

#	endif // TASUI