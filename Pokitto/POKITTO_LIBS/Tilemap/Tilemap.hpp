#pragma once

#include "Pokitto.h"
#include <cstdint>

#ifndef MAX_TILE_COUNT
#define MAX_TILE_COUNT 16
#endif

class BaseTilemap {
public:
#if PROJ_SCREENMODE == TASMODE
    using Tile = const uint8_t *;
#else
    struct Tile {
        const uint8_t *data = nullptr;
        std::uint8_t width;
        std::uint8_t height;
        // For use with image data that does not contain the image dimensions
        void set( uint8_t width, uint8_t height, const uint8_t *data ){
            this->data = data;
            this->height = height;
            this->width = width;
        }
    };
#endif

    Tile tiles[ MAX_TILE_COUNT ];

    const uint8_t *map;
    std::size_t width;
    std::size_t height;

    BaseTilemap(){
        map = nullptr;
        for( std::uint32_t i=0; i<MAX_TILE_COUNT; ++i )
            tiles[i] = {};
    }

    void set( size_t width, size_t height, const uint8_t *map ){
        this->width = width;
        this->height = height;
        this->map = map;
    }
};

#if PROJ_SCREENMODE == TASMODE

class Tilemap : public BaseTilemap {
public:

    // For use with image data that does contain the image dimensions
    void setTile(uint8_t index, const uint8_t *data ){
        this->tiles[index] = &data[2];
    }

    // For use with image data that does not contain the image dimensions
    void setTile(uint8_t index, uint8_t width, uint8_t height, const uint8_t *data ){
        this->tiles[index] = data;
    }

    void draw( std::int32_t x, std::int32_t y ){
        if( !map ) return;
    }
};

#else

void SDL_RenderCopySolid( const uint8_t *data, uint32_t width, uint32_t height, int32_t x, int32_t y );

class Tilemap : public BaseTilemap {

    int32_t blit( uint32_t id, int32_t /*OUT*/&x, int32_t y ){
        Tile &tile = tiles[id];
        if( !tile.data )
            return 0;

        if(
            x < Pokitto::Display::width &&
            y < Pokitto::Display::height &&
            (x + tile.width >= 0) &&
            (y + tile.height >= 0)
            ){
            if( POK_COLORDEPTH != 4 ){
                Pokitto::Display::drawBitmapData(
                    x, y,
                    tile.width, tile.height,
                    tile.data
                    );
            } else {
                SDL_RenderCopySolid(
                    tile.data,
                    tile.width, tile.height,
                    x, y
                    );
            }
        }

        x += tile.width;
        return tile.height;
    }

public:

    // For use with image data that does contain the image dimensions
    void setTile(uint8_t index, const uint8_t *data ){
        Tile tile;
        tile.data = &data[2];
        tile.height = data[1];
        tile.width = data[0];
        this->tiles[index] = tile;
    }

    // For use with image data that does not contain the image dimensions
    void setTile(uint8_t index, uint8_t width, uint8_t height, const uint8_t *data ){
        Tile tile;
        tile.data = data;
        tile.height = height;
        tile.width = width;
        this->tiles[index] = tile;
    }
    
    void draw( std::int32_t x, std::int32_t y ){

        if( !map ) return;

        int32_t cx = x, cy = y;
        for( uint32_t ty = 0; ty<height; ty++ ){
            cx = x;

            uint32_t h = 0;

            #if MAX_TILE_COUNT == 16

            // Note: Each 8-bit map item contains 2 tiles.
            for( uint32_t tx = 0; tx<width; tx += 2 ){
                uint32_t id = map[ (ty*width + tx)>>1 ];

                h = blit( id >> 4, /*OUT*/cx, cy );
                if( cx < Pokitto::Display::width )
                    h = blit( id & 0xF, /*OUT*/cx, cy );

                if( cx >= Pokitto::Display::width )
                    break;
            }

            #elif MAX_TILE_COUNT > 16 && MAX_TILE_COUNT <= 256

            // Note: Each 8-bit map item contains 1 tile.
            for( uint32_t tx = 0; tx<width; tx += 1 ){
                uint32_t id = map[ ty*width + tx ];
                if( cx < Pokitto::Display::width )
                    h = blit( id, /*OUT*/cx, cy );

                if( cx >= Pokitto::Display::width )
                    break;
            }

            #else
                #error "Invalid MAX_TILE_COUNT value"
            #endif

             cy += h;

        }
    }

    // Get the tile under the given x and y world coordinates.
    uint8_t GetTileId( std::int32_t x, std::int32_t y, uint8_t tileSize ) {

        // Get tile x and y
        uint32_t tx = 0;
        uint32_t ty = 0;
        if( tileSize == 8 ) {
            tx = x>>3;
            ty = y>>3;
        }
        else if( tileSize == 16 ) {
            tx = x>>4;
            ty = y>>4;
        }
        else { // any size
            tx = x/tileSize;
            ty = y/tileSize;
        }

        // Check x and y for out of bounds.
        if( tx >= width || ty >= height )
            return 0;

        // Get the tile id.
        #if MAX_TILE_COUNT == 16

        uint8_t id = map[ (ty*width + tx)>>1 ];
        if(tx&1)
            id &= 0xF;
        else
            id >>= 4;

        #elif MAX_TILE_COUNT > 16 && MAX_TILE_COUNT <= 256

        uint8_t id = map[ ty*width + tx ];

        #else
            #error "Invalid MAX_TILE_COUNT value"
        #endif

        return id;
    }

    // Get the tiles at the rect corners.
    void GetTileIds( std::int32_t tlx, std::int32_t tly, std::int32_t brx, std::int32_t bry, uint8_t tileSize,
                   /*OUT*/ uint8_t& tileIdTl, uint8_t& tileIdTr, uint8_t& tileIdBl, uint8_t& tileIdBr ){

        tileIdTl = GetTileId( tlx, tly, tileSize );
        tileIdTr = GetTileId( brx, tly, tileSize );
        tileIdBl = GetTileId( tlx, bry, tileSize );
        tileIdBr = GetTileId( brx, bry, tileSize );
    }
};

#endif
