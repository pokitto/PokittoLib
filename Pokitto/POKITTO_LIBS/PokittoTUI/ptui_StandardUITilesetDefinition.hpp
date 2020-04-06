#ifndef POKITTOTUI_PTUI_STANDARDUITILESETDEFINITION_HPP
#	define POKITTOTUI_PTUI_STANDARDUITILESETDEFINITION_HPP


namespace ptui
{
    // Defines which indexes to use to get tileset's special tiles (texts, widgets, etc).
    // - You can actually use a completely different tileset layout by changing UITileMap's TilesetDefinition parameter to a similar class!
    // - However there are some restrictions:
    //    - The 0 tile is ALWAYS used as the null tile.
    //    - Gauge's parts' Empty and Full defines a contiguous range of filling - i.e. everything between them will be considered as "partly filled".
    //    - Text is always contiguous as well, starting from `asciiSpace`.
    struct UITilesetDefinition
    {
        static constexpr auto gaugeLeftEmpty = 5;
        static constexpr auto gaugeLeftFull = 9;
        static constexpr auto gaugeMiddleEmpty = 10;
        static constexpr auto gaugeMiddleFull = 16;
        static constexpr auto gaugeRightEmpty = 17;
        static constexpr auto gaugeRightFull = 21;
        
        static constexpr auto checkboxUnchecked = 22;
        static constexpr auto checkboxChecked = 23;
        
        static constexpr auto asciiSpace = 32;
        
        static constexpr auto boxTopLeft = 24;
        static constexpr auto boxTop = 25;
        static constexpr auto boxTopRight = 26;
        static constexpr auto boxRight = 27;
        static constexpr auto boxBottomRight = 28;
        static constexpr auto boxBottom = 29;
        static constexpr auto boxBottomLeft = 30;
        static constexpr auto boxLeft = 31;
        static constexpr auto boxMiddle = asciiSpace;
        
        static constexpr auto tabColumns = 4;
		
        static constexpr auto rightArrow = 1;
        static constexpr auto leftArrow = 2;
        static constexpr auto upArrow = 3;
        static constexpr auto downArrow = 4;
    };
}


#endif // POKITTOTUI_PTUI_STANDARDUITILESETDEFINITION_HPP