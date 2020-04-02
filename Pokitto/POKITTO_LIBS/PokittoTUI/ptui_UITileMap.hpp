#ifndef POKITTOTUI_PTUI_UITILEMAP_HPP
#   define POKITTOTUI_PTUI_UITILEMAP_HPP

#   include <ptui_TileMap.hpp>
#   include <ptui_StandardUITilesetDefinition.hpp>


namespace ptui
{    
    // An extension of BaseTileMap revolving around a UI TileSet.
    // - TilesetDefinition is used to know where to get the relevant tiles in the underlying Tileset.
    // Provides the following services:
    // - Text printing with bounds and bounded autoscroll. [Terminal]
    // - Box, checkbox, gauge drawing. [Widgets]
    template<unsigned columnsP, unsigned rowsP,
             unsigned tileWidthP, unsigned tileHeightP,
             unsigned lineWidthP,
             bool tilesWithDeltasP, unsigned clutSizeP,
             typename TilesetDefinition = UITilesetDefinition>
    class UITileMap:
        public TileMap<columnsP, rowsP,
                           tileWidthP, tileHeightP,
                           lineWidthP,
                           tilesWithDeltasP, clutSizeP>
    {
    public: // Types.
        TilesetDefinition definition;
        using Delta = typename TileMap<columnsP, rowsP,
                                       tileWidthP, tileHeightP,
                                       lineWidthP,
                                       tilesWithDeltasP, clutSizeP>::Delta;
        
        
    public: // Widgets.
        // Draws a gauge filled with the given amount, extending from `firstColumn` to `lastColumn` at the given row.
        // - If `current` is negative or 0, the gauge will be empty.
        // - If `current` is `max`, the gauge will be full.
        // - If the gauge is reversed or if `max` is negative, nothing happens.
        // - The drawing will be clipped as needed.
        // - Doesn't change the delta.
        void drawGauge(int firstColumn, int lastColumn, int row,
                       int current, int max) noexcept
        {
            constexpr auto gaugeLeftCapacity = TilesetDefinition::gaugeLeftFull - TilesetDefinition::gaugeLeftEmpty;
            constexpr auto gaugeMiddleCapacity = TilesetDefinition::gaugeMiddleFull - TilesetDefinition::gaugeMiddleEmpty;
            constexpr auto gaugeRightCapacity = TilesetDefinition::gaugeRightFull - TilesetDefinition::gaugeRightEmpty;
            
            static_assert(gaugeLeftCapacity >= 0, "Gauge Left Part is invalid.");
            static_assert(gaugeMiddleCapacity >= 0, "Gauge Middle Part is invalid.");
            static_assert(gaugeRightCapacity >= 0, "Gauge Right Part is invalid.");
            
            current = std::max(0, std::min(current, max));
            if (firstColumn == lastColumn)
            {
                // It's an edge case. We'll use the Middle part.
                if (gaugeMiddleCapacity == 0)
                    this->setTile(firstColumn, row, TilesetDefinition::gaugeMiddleFull); // No distinction between empty or full = no value.
                else
                {
                    int filling = (current * gaugeMiddleCapacity + max / 2) / max;
                    
                    this->setTile(firstColumn, row, TilesetDefinition::gaugeMiddleEmpty + filling);
                }
            }
            else if (firstColumn < lastColumn)
            {
                int gaugeWidth = lastColumn - firstColumn + 1;
                int gaugeMiddleWidth = gaugeWidth - 2;
                int gaugeCapacity = gaugeLeftCapacity + gaugeMiddleWidth * gaugeMiddleCapacity + gaugeRightCapacity;
                int filling = (current * gaugeCapacity + max / 2) / max;
                
                // Left part.
                this->setTile(firstColumn, row, TilesetDefinition::gaugeLeftEmpty + std::min(filling, gaugeLeftCapacity));
                filling -= gaugeLeftCapacity;
                
                // Middle parts.
                for (int middleColumn = firstColumn + 1; middleColumn < lastColumn; middleColumn++)
                {
                    this->setTile(middleColumn, row, TilesetDefinition::gaugeMiddleEmpty + std::max(0, std::min(filling, gaugeMiddleCapacity)));
                    filling -= gaugeMiddleCapacity;
                }
                
                // Right parts.
                this->setTile(lastColumn, row, TilesetDefinition::gaugeRightEmpty + std::max(0, std::min(filling, gaugeRightCapacity)));
            }
        }
        
        // Draws a checkbox at `column`, `row` with the given checked state.
        // - Doesn't change the delta.
        void drawCheckbox(int column, int row, bool checked) noexcept
        {
            this->setTile(column, row, checked ? TilesetDefinition::checkboxChecked : TilesetDefinition::checkboxUnchecked);
        }
        
        // Draws a Box.
        // - `firstColumn`, `firstRow`, `lastColumn` and `lastRow` will not be clamped, but clipped instead.
        // - `lastColumn` and `lastRow` are included.
        // - Empty or negative boxes won't be drawn.
        // - Due to the limitation of tileset, 1-column and 1-row boxes will be rendered as Spaces.
        // - Doesn't change the delta.
        void drawBox(int firstColumn, int firstRow, int lastColumn, int lastRow) noexcept
        {
            if ((firstColumn > lastColumn) || (firstRow > firstRow))
                return ;
            if ((firstColumn == lastColumn) || (firstRow == lastRow))
            {
                this->fillRectTiles(firstColumn, firstRow, lastColumn, lastRow, TilesetDefinition::boxMiddle);
                return ;
            }
            
            // Corners.
            this->setTile(firstColumn, firstRow, TilesetDefinition::boxTopLeft);
            this->setTile(firstColumn, lastRow, TilesetDefinition::boxBottomLeft);
            this->setTile(lastColumn, firstRow, TilesetDefinition::boxTopRight);
            this->setTile(lastColumn, lastRow, TilesetDefinition::boxBottomRight);
            
            // Borders.
            this->fillRectTiles(firstColumn + 1, firstRow, lastColumn - 1, firstRow, TilesetDefinition::boxTop);
            this->fillRectTiles(firstColumn + 1, lastRow, lastColumn - 1, lastRow, TilesetDefinition::boxBottom);
            
            this->fillRectTiles(firstColumn, firstRow + 1, firstColumn, lastRow - 1, TilesetDefinition::boxLeft);
            this->fillRectTiles(lastColumn, firstRow + 1, lastColumn, lastRow - 1, TilesetDefinition::boxRight);
            
            // Inside.
            this->fillRectTiles(firstColumn + 1, firstRow + 1, lastColumn - 1, lastRow - 1, TilesetDefinition::boxMiddle);
        }
        
        
    public: // Terminal.
        // Sets where to draw the next character.
        // - Clamped to the Cursor Bounding Box.
        void setCursor(int cursorColumn, int cursorRow) noexcept
        {
            _cursorColumn = std::max<int>(_cursorFirstColumn, std::min<int>(cursorColumn, _cursorLastColumn));
            _cursorRow = std::max<int>(_cursorFirstRow, std::min<int>(cursorRow, _cursorLastRow));
        }
        
        // Sets the delta to apply for each printed tile after this call.
        // This usually change the color, depending on the main palette and the CLUT (if enabled).
        void setCursorDelta(Delta delta) noexcept
        {
            if (tilesWithDeltasP)
                _cursorDelta = delta;
        }
        
        // Changes the cursor's bounding box.
        // - Cursor's position will be updated to fit inside.
        // - Negative boxes
        void setCursorBoundingBox(int cursorFirstColumn, int cursorFirstRow, int cursorLastColumn, int cursorLastRow) noexcept
        {
            _cursorFirstColumn = cursorFirstColumn;
            _cursorFirstRow = cursorFirstRow;
            _cursorLastColumn = std::max<int>(cursorFirstColumn, cursorLastColumn);
            _cursorLastRow = std::max<int>(cursorFirstRow, cursorLastRow);
            setCursor(_cursorColumn, _cursorRow);
        }
        
        // Changes the Cursor Bounding Box back to the default.
        // - Cursor's position will be updated to fit inside.
        void resetCursorBoundingBox() noexcept
        {
            _cursorFirstColumn = 0;
            _cursorFirstRow = 0;
            _cursorLastColumn = this->columns - 1;
            _cursorLastRow = this->rows - 1;
            setCursor(_cursorColumn, _cursorRow);
        }
    
        // Prints a single character.
        // - All the Control characters are ignored, besides newlines and tabs.
        // - Tabs are based on the TilesetDefinition's `tabColumns` characters wide columns, starting from the bound's firstColumn.
        // - If printing such a char would go above the last column, then a newline is generated before.
        //   - If `c` was a space, it won't be printed and transformed instead into the said newline.
        // - Note: This is not suitable for setting a tile to a given value. Use `set()` instead.
        void printChar(char c) noexcept
        {
            if (c == '\n')
            {
                // Newlines gets the cursor back to the first column, and to the next row.
                _cursorColumn = _cursorFirstColumn;
                _cursorRow++;
                if (_cursorRow > _cursorLastRow)
                {
                    // Autoscroll.
                    this->shift(_cursorFirstColumn, _cursorFirstRow, _cursorLastColumn, _cursorLastRow,
                          0, -1);
                    this->fillRectTilesAndDeltas(_cursorFirstColumn, _cursorLastRow, _cursorLastColumn, _cursorLastRow, TilesetDefinition::boxMiddle, tilesWithDeltasP ? _cursorDelta : 0);
                    _cursorRow = _cursorLastRow;
                }
            }
            else if (c == '\t')
            {
                int nextColumn = _cursorFirstColumn + ((_cursorColumn - _cursorFirstColumn) / TilesetDefinition::tabColumns + 1) * TilesetDefinition::tabColumns;
                
                if (nextColumn > _cursorLastColumn)
                    printChar('\n');
                else while (_cursorColumn < nextColumn)
                    printChar(' ');
            }
            else if (c >= ' ')
            {
                if (_cursorColumn > _cursorLastColumn)
                {
                    printChar('\n');
                    // Spaces are transformed into the generated newline.
                    if (c == ' ')
                        return ;
                }
                this->setTileAndDelta(_cursorColumn, _cursorRow, c - ' ' + TilesetDefinition::asciiSpace, tilesWithDeltasP ? _cursorDelta : 0);
                _cursorColumn++;
            }
        }
        
        // Prints a string.
        void printString(const char* string) noexcept
        {
            for (auto charP = string; *charP != 0; charP++)
                printChar(*charP);
        }
        
        // Prints a string, but truncated up to limit.
        void printString(const char* string, int limit) noexcept
        {
            for (auto charP = string; (*charP != 0) && (limit > 0); charP++, limit--)
                printChar(*charP);
        }
        
        // Prints a number with the given radix.
        void printInteger(int number, int padding = 0, char paddingChar = ' ', int radix = 10) noexcept
        {
            padding--;
            if (std::abs(number) >= radix)
            {
                auto upperNumber = number / radix;
                
                number = std::abs(number - upperNumber * radix);
                printInteger(upperNumber, padding, paddingChar, radix);
                padding = 0;
            }
            if (number < 0)
                padding--;
            for (; padding > 0; padding--)
                printChar(paddingChar);
            if (number < 0)
            {
                printChar('-');
                number = -number;
            }
            if (number < 10)
                printChar('0' + number);
            else
                printChar('a' + number - 10);
        }
        
        // Similar to printString, but it'll try to preserve "words" (a subchain of characters comprised of anything but whitespaces and control characters) together and generate newlines early before.
        void printText(const char* text) noexcept
        {
            printText(text, std::numeric_limits<int>::max());
        }
        
        // Same than above, but only a limited number of characters are printed out of `text`.
        void printText(const char* text, int limit) noexcept
        {
            while ((*text != 0) && (limit > 0))
            {
                // Prints all the special characters before.
                for (; (*text != 0) && (limit > 0) && (*text <= 32); text++, limit--)
                    printChar(*text);
                
                if (limit > 0)
                {
                    auto wordLength = 0;
                    auto wordEnd = text;
                    
                    // Gets the length of the next wordGets all the visible characters.
                    for (; (*wordEnd != 0) && (*wordEnd > 32); wordEnd++)
                        wordLength++;
                    if (_cursorColumn + wordLength > _cursorLastColumn)
                        printChar('\n');
                    for (; (text != wordEnd) && (limit > 0); text++, limit--)
                        printChar(*text);
                }
            }
        }
        
        
    private:
        // Cursor position.
        short _cursorColumn = 0;
        short _cursorRow = 0;
        
        // Cursor bounding box.
        short _cursorFirstColumn = 0;
        short _cursorFirstRow = 0;
        short _cursorLastColumn = this->columns - 1;
        short _cursorLastRow = this->rows - 1;
        
        // TODO: There might be a way to remove this field when `tilesWithDeltasP` is `false`, but I'm not sure how.
        // Cursor "color".
        Delta _cursorDelta = 0;
    };
}


#endif // POKITTOTUI_PTUI_UITILEMAP_HPP