#include <cstdint>
#include <Pokitto.h>
#include "Tilemap.hpp"

void SDL_RenderCopySolid( const uint8_t *data, uint32_t width, uint32_t height, int32_t x, int32_t y )
{
    struct rect {
        uint32_t x, y, w, h;
    };

    rect srcClipped, dstClipped;
    srcClipped.x = 0;
    srcClipped.y = 0;
    srcClipped.w = width;
    srcClipped.h = height;
    dstClipped.x = x;
    dstClipped.y = y;

    if( x < 0 )
    {
        srcClipped.x -= x;
        dstClipped.x = 0;
        srcClipped.w += x;
    }

    if( y < 0 )
    {
        srcClipped.y -= y;
        srcClipped.h += y;
        dstClipped.y = 0;
    }

    if( dstClipped.x + srcClipped.w >= Pokitto::Display::width )
    {
        int16_t w = Pokitto::Display::width - dstClipped.x;
        if( w&1) w--;
        srcClipped.w = w;
    }

    if( dstClipped.y + srcClipped.h >= Pokitto::Display::height )
    {
        srcClipped.h = Pokitto::Display::height - dstClipped.y;
    }

    if( srcClipped.x & 1 )
    {
        srcClipped.x++;
        dstClipped.x++;
        srcClipped.w--;
    }

    if( srcClipped.w <= 0 || srcClipped.h <= 0 )
        return;

    const uint8_t *pd;
    uint8_t *out;
    int offsett = 0;
    int offsetty = 0;
    int dsty = dstClipped.y * (Pokitto::Display::width >> 1);
    offsetty = srcClipped.y * width;

    for (uint32_t y=0; y < srcClipped.h; y++, offsetty += width, dsty += Pokitto::Display::width >> 1 )
    {
        offsett = offsetty + srcClipped.x;
	pd = &data[(offsett>>1)];
	out = Pokitto::Display::screenbuffer + dsty + (dstClipped.x>>1);

        if( dstClipped.x & 1 )
        {
            for (uint32_t x = 0; x < srcClipped.w-1; x+=2, out++, pd++)
            {
                out[1] = (out[1]&0x0F) | (*pd<<4);
                out[0] = (out[0]&0xF0) | (*pd>>4);
            }
        }
        else
        {
            for (uint32_t x = 0; x < srcClipped.w-1; x+=2 )
            {
                *out++ = *pd++;
            }
        }

    }

}
