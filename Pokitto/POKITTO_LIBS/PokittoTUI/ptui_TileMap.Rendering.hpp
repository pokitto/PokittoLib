#ifndef POKITTOTUI_PTUI_TILEMAP_RENDERING_HPP
#   define POKITTOTUI_PTUI_TILEMAP_RENDERING_HPP

#   include "ptui_TileMap.hpp"


namespace ptui
{
	// Rendering implementation for ptui::TileMap and its inheriting classes.

	// Non-Transparent Pixel Output.
    template<bool transparentZeroColor>
    struct PixelOutput
    {
        using Color8 = std::uint8_t;

        static void output(Color8& output, Color8 source) noexcept
        {
            output = source;
        }
    };

	// Transparent Pixel Output.
    template<>
    struct PixelOutput<true>
    {
        using Color8 = std::uint8_t;

        static void output(Color8& output, Color8 source) noexcept
        {
            if (source != 0)
                output = source;
        }
    };

	// Direct Absolute Tile Line Output.
    template<bool transparentZeroColor, bool colorLookUp, bool colorDelta, int deltaIndexOffset>
    struct TileLineOutput
    {
        using Color8 = std::uint8_t;
        using Tile = std::uint8_t;

        static void output(const Tile* tileP, const Color8* originalColorLUT,
                           const Color8* tileImageP, const Color8* tileImagePEnd,
                           Color8*& pixelP) noexcept
        {
            for (; tileImageP != tileImagePEnd; pixelP++, tileImageP++)
            {
                auto tilePixel = *tileImageP;

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }

        template<int tileWidth>
        static void outputFixed(const Tile* tileP, const Color8* originalColorLUT,
                                const Color8* tileImageP,
                                Color8*& pixelP) noexcept
        {
            for (auto i = tileWidth; i > 0; i--, pixelP++, tileImageP++)
            {
                auto tilePixel = *tileImageP;

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }
    };

	// Direct with Added Deltas Tile Line Output.
    template<bool transparentZeroColor, int deltaIndexOffset>
    struct TileLineOutput<transparentZeroColor, false, true, deltaIndexOffset>
    {
        using Color8 = std::uint8_t;
        using Tile = std::uint8_t;

        static void output(const Tile* tileP, const Color8* originalColorLUT,
                           const Color8* tileImageP, const Color8* tileImagePEnd,
                           Color8*& pixelP) noexcept
        {
            auto tileColorOffset = tileP[deltaIndexOffset];

            for (; tileImageP != tileImagePEnd; pixelP++, tileImageP++)
            {
                auto tilePixel = *tileImageP + tileColorOffset;

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }

        template<int tileWidth>
        static void outputFixed(const Tile* tileP, const Color8* originalColorLUT,
                                const Color8* tileImageP,
                                Color8*& pixelP) noexcept
        {
            auto tileColorOffset = tileP[deltaIndexOffset];

            for (auto i = tileWidth; i > 0; i--, pixelP++, tileImageP++)
            {
                auto tilePixel = *tileImageP + tileColorOffset;

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }
    };

	// CLUT'd Tile Line Output.
    template<bool transparentZeroColor, int deltaIndexOffset>
    struct TileLineOutput<transparentZeroColor, true, false, deltaIndexOffset>
    {
        using Color8 = std::uint8_t;
        using Tile = std::uint8_t;

        static void output(const Tile* tileP, const Color8* originalColorLUT,
                           const Color8* tileImageP, const Color8* tileImagePEnd,
                           Color8*& pixelP) noexcept
        {
            for (; tileImageP != tileImagePEnd; pixelP++, tileImageP++)
            {
                auto tilePixel = originalColorLUT[*tileImageP];

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }

        template<int tileWidth>
        static void outputFixed(const Tile* tileP, const Color8* originalColorLUT,
                                const Color8* tileImageP,
                                Color8*& pixelP) noexcept
        {
            auto tileColorOffset = tileP[deltaIndexOffset];

            for (auto i = tileWidth; i > 0; i--, pixelP++, tileImageP++)
            {
                auto tilePixel = originalColorLUT[*tileImageP];

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }
    };

	// CLUT'd with Added Deltas Tile Line Output.
    template<bool transparentZeroColor, int deltaIndexOffset>
    struct TileLineOutput<transparentZeroColor, true, true, deltaIndexOffset>
    {
        using Color8 = std::uint8_t;
        using Tile = std::uint8_t;

        static void output(const Tile* tileP, const Color8* originalColorLUT,
                           const Color8* tileImageP, const Color8* tileImagePEnd,
                           Color8*& pixelP) noexcept
        {
            // Which palette to use.
            auto colorLut = originalColorLUT + tileP[deltaIndexOffset];

            for (; tileImageP != tileImagePEnd; pixelP++, tileImageP++)
            {
                auto tilePixel = colorLut[*tileImageP];

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }

        template<int tileWidth>
        static void outputFixed(const Tile* tileP, const Color8* originalColorLUT,
                                const Color8* tileImageP,
                                Color8*& pixelP) noexcept
        {
            // Which palette to use.
            auto colorLut = originalColorLUT + tileP[deltaIndexOffset];

            for (auto i = tileWidth; i > 0; i--, pixelP++, tileImageP++)
            {
                auto tilePixel = colorLut[*tileImageP];

                PixelOutput<transparentZeroColor>::output(*pixelP, tilePixel);
            }
        }
    };


    // TileMap - Rendering.

    template<unsigned columnsP, unsigned rowsP,
             unsigned tileWidthP, unsigned tileHeightP,
             unsigned lineWidthP,
             bool enableTilesWithDeltasP, unsigned clutSizeP>
    template<bool transparentZeroColor, bool colorLookUp, bool colorOffset>
    void TileMap<columnsP, rowsP, tileWidthP, tileHeightP, lineWidthP, enableTilesWithDeltasP, clutSizeP>
		::renderIntoLineBuffer(BufferPixel* lineBuffer, int y, bool skip) noexcept
    {
        // Row initialization / change.

        if (y == 0)
        {
            _tileY = _tileYStart;
            _tileSubY = _tileSubYStart;
            _tileImageRowBase = _tilesetImage + _tileSubY * tileWidth;
        }
        else
        {
            if (_tileSubY == tileHeight - 1)
            {
                // Onto the next row!
                _tileSubY = 0;
                _tileY++;
                _tileImageRowBase = _tilesetImage;
            }
            else
            {
                // Onto the next pixel line of the same tile row!
                _tileSubY++;
                _tileImageRowBase += tileWidth;
            }
        }
        if ((skip) || (_tileY < 0) || (_tileY >= static_cast<int>(rows)) || (_indexStart >= _indexEnd))
            return ;

        // Scanline rendition.

        // Local access is faster than field access.
        auto tileImageRowBase = _tileImageRowBase;
        // Current pixel pointer.
        BufferPixel* pixelP = lineBuffer + _indexStart;
        // Last pixel pointer.
        BufferPixel* pixelPEnd = lineBuffer + _indexEnd;
        // Current tile pointer.
        const Tile* tileP = &_tiles[_tileIndex(_tileXStart, _tileY)];

        // Is the first tile cut in half?
        if (_tileSubXStart != 0)
        {
            // Is the tile empty?
            if (*tileP == 0)
                pixelP += tileWidth - _tileSubXStart;
            else
            {
                auto tileImagePStart = tileImageRowBase + *tileP * tileSize;
                // Let's render it.
                // Points to the current pixel in the tile.
                const TilesetPixel* tileImageP = tileImagePStart + _tileSubXStart;
                // Points right after the last pixel in the tile's row.
                // The line buffer is guaranteed to be greater than a single tile's width by the static_assert on top of the class.
                const TilesetPixel* tileImagePEnd = tileImagePStart + tileWidth;

                TileLineOutput<transparentZeroColor, colorLookUp && clutIsEnabled, colorOffset && enableTilesWithDeltasP,
                               deltaIndexOffset>::output(tileP, _colorLUT,
                                                         tileImageP, tileImagePEnd,
                                                         pixelP);
            }
            tileP++;
        }

        // Let's render the middle tiles.
        while (pixelP + tileWidth <= pixelPEnd)
        {
            // Let's skip any empty middle tiles.
            if (*tileP != 0)
            {
                // Middle tile!
                // Let's render it.
                // Points to the current pixel in the tile.
                const TilesetPixel* tileImageP = tileImageRowBase + *tileP * tileSize;

                TileLineOutput<transparentZeroColor, colorLookUp && clutIsEnabled, colorOffset && enableTilesWithDeltasP,
                               deltaIndexOffset>:: template outputFixed<tileWidth>(tileP, _colorLUT,
                                                                                   tileImageP,
                                                                                   pixelP);
            }
            else
                pixelP += tileWidth;
            tileP++;
        }

        // Let's render the last tile, if there is one.
        if ((*tileP != 0) && (pixelP < pixelPEnd))
        {
            // Last tile!
            // Let's render it.
            // Points to the current pixel in the tile.
            const TilesetPixel* tileImageP = tileImageRowBase + *tileP * tileSize;
            // Points right after the last pixel in the tile's row.
            // The line buffer is guaranteed to be greater than a single tile's width by the static_assert on top of the class.
            const TilesetPixel* tileImagePEnd = tileImageP + (pixelPEnd - pixelP);

            TileLineOutput<transparentZeroColor, colorLookUp && clutIsEnabled, colorOffset && enableTilesWithDeltasP,
                           deltaIndexOffset>::output(tileP, _colorLUT,
                                                     tileImageP, tileImagePEnd,
                                                     pixelP);
            tileP++;
        }
    }
}


#endif // POKITTOTUI_PTUI_CUTETILEMAP_RENDERING_HPP
