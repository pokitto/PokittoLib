#include "Pokitto.h"

#if PROJ_SCREENMODE == TASMODE

using namespace Pokitto;

enum class SpriteMode : uint8_t {
    copy,
    mirror,
    flip,
    flipmirror
};

struct Sprite { 
    int16_t x, y; 
    SpriteMode mode; 
    uint8_t height;
    uint8_t recolor;
    const uint8_t *data;
};

Sprite spriteBuffer[PROJ_MAX_SPRITES];
uint32_t spriteBufferPos = 0;

using Tilemap = uint8_t const * const *;
constexpr uint32_t tileW = POK_TILE_W;
constexpr uint32_t tileH = POK_TILE_W;
constexpr uint32_t mapW = POK_LCD_W / tileW + 1;
constexpr uint32_t mapH = POK_LCD_H / tileH + 1;
const uint8_t *tilemap[mapH * mapW];
uint32_t cameraX;
uint32_t cameraY;

void Display::shiftTilemap(int x, int y){
    cameraX = x % tileW;
    cameraY = y % tileH;
}

void Display::drawTile(uint32_t x, uint32_t y, const uint8_t *data){
    if(x >= mapW || y >= mapH)
        return;
    tilemap[y*mapW + x] = data;
}

void Display::drawSprite(int x, int y, const uint8_t *data, bool flipped, bool mirrored, uint8_t recolor){
    if(y >= 176 || y + data[1] < 0 || x >= 220 || x + data[0] < 0) return;
    if(spriteBufferPos == PROJ_MAX_SPRITES) spriteBufferPos = 0;
    auto mode = flipped ?
        (mirrored ? SpriteMode::flipmirror : SpriteMode::flip):
        (mirrored ? SpriteMode::mirror     : SpriteMode::copy);
    spriteBuffer[spriteBufferPos++] = Sprite{x, y, mode, y + data[1], recolor, data};
}

void blit(uint8_t *line, Sprite &s, int y){
    int w = s.data[0];
    const uint8_t *src = s.data + 2 + y * w;
    if(s.x < 0){
        src -= s.x;
        w += s.x;
    }else if(s.x > 0){
        line += s.x;
    }
    if(s.x + w >= 220){
        w = 220 - s.x;
    }
    /* */
    pixelCopy(line, src, w, s.recolor);
    /*/
    while(w--){
        if(*src)
            *line = *src;
        line++;
        src++;
    }
    /* */
}

void blitMirror(uint8_t *line, Sprite &s, int y){
    int w = s.data[0];
    const uint8_t *src = s.data + 2 + y * w + w - 1;
    if(s.x < 0){
        w += s.x;
    }else if(s.x > 0){
        line += s.x;
    }
    if(s.x + w >= 220){
        src -= 220 - (s.x + w);
        w = 220 - s.x;
    }
    /* */
    pixelCopyMirror(line, src - w + 1, w, s.recolor);
    /*/
    while(w--){
        if(*src)
            *line = *src;
        line++;
        src--;
    }
    /* */
}

void blitFlip(uint8_t *line, Sprite &s, int y){
    int w = s.data[0];
    int h = s.data[1];
    const uint8_t *src = s.data + 2 + (h - 1 - y) * w;
    if(s.x < 0){
        src -= s.x;
        w += s.x;
    }else if(s.x > 0){
        line += s.x;
    }
    if(s.x + w >= 220){
        w = 220 - s.x;
    }
    pixelCopy(line, src, w, s.recolor);
    /*
    while(w--){
        if(*src)
            *line = *src;
        line++;
        src++;
    }
    */
}

void blitFlipMirror(uint8_t *line, Sprite &s, int y){
    int w = s.data[0];
    int h = s.data[1];
    const uint8_t *src = s.data + 2 + (h - 1 - y) * w + w - 1;
    if(s.x < 0){
        w += s.x;
    }else if(s.x > 0){
        line += s.x;
    }
    if(s.x + w >= 220){
        src -= 220 - (s.x + w);
        w = 220 - s.x;
    }
    /* */
    pixelCopyMirror(line, src - w + 1, w, s.recolor);
    /*/
    while(w--){
        if(*src)
            *line = *src;
        line++;
        src--;
    }
    /* */
}

using Blitter = void (*)(uint8_t *, Sprite &sprite, int y );

const Blitter blitters[] = {
    blit,
    blitMirror,
    blitFlip,
    blitFlipMirror
};

void drawSprites(int y, uint8_t *line, int max){
    if(!max) return;
    for(uint32_t i=0; i<spriteBufferPos; ++i){
        auto& s = spriteBuffer[i];
        if( s.y > y || s.height <= y ) continue;
        auto blitter = blitters[static_cast<uint32_t>(s.mode)];
        blitter(line, s, y - s.y);
        if(!--max) break;
    }
}

namespace Pokitto {

void lcdRefreshTASMode(uint8_t *line, const uint16_t* palette){
    uint8_t spritesPerLine[176];
    for(int y=0; y<176; ++y) spritesPerLine[y] = 0;

    for(uint32_t i=0; i<spriteBufferPos; ++i){
        auto& s = spriteBuffer[i];
        for(int y=s.y; y<s.height; ++y){
            spritesPerLine[y]++;
        }
    }

    Tilemap  tileWindow = tilemap - 1;
    uint32_t tileY = tileH;
    uint32_t tileX;
    uint32_t tileIndex = cameraY * tileW;
    uint32_t tilesToDraw = (220 / tileW) + 1;
    
    for(uint32_t y=0; y<176; ++y, tileIndex += tileW){
        if(!tileY--){
            tileIndex = 0;
            tileY = tileH - 1;
            tileWindow += mapW;
        }
        tileX = tileW - cameraX;

        uint32_t tile = 0;
        for(uint32_t i=0; i<220;){
            auto tileData = tileWindow[tile] + tileX + tileIndex;
            int iter = std::min(tileW - tileX, 220 - i);
            /* */
            pixelCopySolid(line+i, tileData, iter);
            i += iter;
            /*/
            while(iter--){
                line[i++] = *tileData++;
            }
            /* */
            tileX = 0;
            tile++;
        }

        drawSprites(y, line, spritesPerLine[y]);
        flushLine(palette, line);
    }
    
    spriteBufferPos = 0;
}

}

#endif
