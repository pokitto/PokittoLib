#include "Pokitto.h"

#if PROJ_SCREENMODE == TASMODE

using namespace Pokitto;

struct Sprite;

using draw_t = void (*)(uint8_t *line, Sprite &sprite, int y);

struct Sprite { 
    int16_t x, y;
    const void *data;
    draw_t draw;
    uint8_t maxY;
    uint8_t b1, b2, b3;
};

void blit(uint8_t *line, Sprite &s, int y){
    auto data = static_cast<const uint8_t*>(s.data);
    int w = s.b2;
    const uint8_t *src = data + y * w;
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
    pixelCopy(line, src, w, s.b1);
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
    auto data = static_cast<const uint8_t*>(s.data);

    int w = s.b2;
    const uint8_t *src = data + y * w + w - 1;
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
    pixelCopyMirror(line, src - w + 1, w, s.b1);
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
    auto data = static_cast<const uint8_t*>(s.data);

    int w = s.b2;
    int h = data[1];
    const uint8_t *src = data + (h - 1 - y) * w;
    if(s.x < 0){
        src -= s.x;
        w += s.x;
    }else if(s.x > 0){
        line += s.x;
    }
    if(s.x + w >= 220){
        w = 220 - s.x;
    }
    pixelCopy(line, src, w, s.b1);
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
    auto data = static_cast<const uint8_t*>(s.data);

    int w = s.b2;
    int h = data[1];
    const uint8_t *src = data + (h - 1 - y) * w + w - 1;
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
    pixelCopyMirror(line, src - w + 1, w, s.b1);
    /*/
    while(w--){
        if(*src)
            *line = *src;
        line++;
        src--;
    }
    /* */
}

Sprite spriteBuffer[PROJ_MAX_SPRITES];
uint32_t spriteBufferPos = 0;

void addSprite(const Sprite& s){
    if(spriteBufferPos >= PROJ_MAX_SPRITES) spriteBufferPos = 0;
    spriteBuffer[spriteBufferPos] = s;
    auto& sb = spriteBuffer[spriteBufferPos];
    int32_t maxY = sb.maxY;
    maxY += sb.y;
    if(maxY < 0) return;
    if(maxY > 176) maxY = 176;
    sb.maxY = maxY;
    spriteBufferPos++;
}

using Tilemap = uint8_t const * const *;
constexpr uint32_t tileW = POK_TILE_W;
constexpr uint32_t tileH = POK_TILE_H;
constexpr uint32_t mapW = POK_LCD_W / tileW + 2;
constexpr uint32_t mapH = POK_LCD_H / tileH + 2;
const uint8_t *tilemap[mapH * mapW];
uint32_t cameraX;
uint32_t cameraY;

void pixelCopySolid4BPP(uint8_t *line, const uint8_t *src, uint32_t w, uint32_t sx){
    while(w--){
        auto b = (sx&1)?
            src[sx>>1]&0xF:
            src[sx>>1]>>4;
        if(b) *line = b;
        line++;
        sx++;
    }
}

void Display::shiftTilemap(int x, int y){
    if(x<0) x = 0;
    if(x>=tileW) x=tileW-1;
    if(y<0) y = 0;
    if(y>=tileH) y=tileH-1;
    
    cameraX = x % tileW;
    cameraY = y % tileH;
}

void Display::drawColumn(int x, int sy, int ey){
    if(x < 0 || x >= 220) return;
    if(sy < 0) sy = 0;
    if(ey > 176) ey = 176;
    if(ey <= sy) return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   line[s.x] = s.b1;
               };    
    addSprite(Sprite{x, sy, nullptr, f, ey - sy, Display::color});
}

void Display::drawRow(int sx, int ex, int y){
    if(y < 0 || y >= 176) return;
    if(sx < 0) sx = 0;
    if(ex > 220) ex = 220;
    if(ex <= sx) return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   int c = s.b1;
                   int w = s.b2;
                   line += s.x;
                   while(w--){
                       *line++ = c;
                   }
               };    
    addSprite(Sprite{sx, y, nullptr, f, 1, Display::color, ex - sx});
}

void Display::drawRectangle(int x, int y, int w, int h) {
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto c = s.b1;
                   int w = s.b2;
                   int sx = 0;
                   if(s.x < 0){
                       sx = -s.x;
                       w += s.x;
                   }else if(s.x >= 0){
                       line += s.x;
                       line[0] = c;
                   }
                   if(s.x + w >= 220){
                       w = 220 - s.x;
                   }else{
                       line[w-1] = c;
                   }
                   if(y == 0 || y == s.b3){
                       while(w--){
                           *line++ = c;
                       }
                   }
               };

    addSprite(Sprite{x, y, nullptr, f, h, Display::color, w, h-1});
}

void Display::fillRectangle(int x, int y, int w, int h) {
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;
    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto c = s.b1;
                   int w = s.b2;
                   int sx = 0;
                   if(s.x < 0){
                       sx = -s.x;
                       w += s.x;
                   }else if(s.x > 0){
                       line += s.x;
                   }
                   if(s.x + w >= 220){
                       w = 220 - s.x;
                   }
                   while(w--){
                       *line++ = c;
                   }
               };

    addSprite(Sprite{x, y, nullptr, f, h, Display::color, w});
}

int Display::bufferChar(int16_t x, int16_t y, uint16_t index){
    const uint8_t* bitmap = font;
    uint8_t w = *bitmap;
    uint8_t h = *(bitmap + 1);

    uint8_t hbytes = ((h>>3) + ((h != 8) && (h != 16)));

    // add an offset to the pointer (fonts !)
    bitmap = bitmap + 4 + index * (w * hbytes + 1);
    uint32_t numBytes = *bitmap++; //first byte of char is the width in bytes

    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return numBytes + adjustCharStep;

    draw_t f;
    if( fontSize != 2 ){
        fontSize = 1;
        f = [](uint8_t *line, Sprite &s, int y){
                int h = s.maxY - s.y;
                auto bitmap = static_cast<const uint8_t *>(s.data);
                int x = s.x;
                uint8_t fg = s.b3;
                uint8_t bg = s.b2;
                int numBytes = s.b1;
                if( s.b1 + x > 220 )
                    numBytes = 220 - x;

                uint8_t hbytes = ((h>>3) + ((h != 8) && (h != 16))) == 2;                   
                for (int i = 0; i < numBytes; i++) {
                    uint32_t bitcolumn = *bitmap++;
                    if (hbytes)
                        bitcolumn |= (*bitmap++)<<8;
                    uint8_t c = bitcolumn & (1<<y) ? bg : fg;
                    if( c != Display::invisiblecolor )
                        line[x+i] = c;
                }
            };
    } else {
        f = [](uint8_t *line, Sprite &s, int y){
                int h = (s.maxY - s.y)>>1;
                y >>= 1;
                auto bitmap = static_cast<const uint8_t *>(s.data);
                int x = s.x;
                uint8_t fg = s.b3;
                uint8_t bg = s.b2;
                int numBytes = s.b1;
                if( (s.b1<<1) + x > 220 )
                    numBytes = (220 - x) >> 1;

                uint8_t hbytes = ((h>>3) + ((h != 8) && (h != 16))) == 2;

                for (int i = 0; i < numBytes; i++) {
                    uint32_t bitcolumn = *bitmap++;
                    if (hbytes)
                        bitcolumn |= (*bitmap++)<<8;
                    uint8_t c = bitcolumn & (1<<y) ? bg : fg;
                    if( c != Display::invisiblecolor ){
                        line[x+(i<<1)] = c;
                        line[x+(i<<1)+1] = c;
                    }
                }
            };
        h *= 2;
    }

    addSprite(Sprite{
            x, y,
            bitmap, f,
            h, numBytes,
            Display::color, Display::bgcolor
        });

    return numBytes*fontSize+adjustCharStep;
}

void Display::drawBitmapData2BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto w = s.b2;
                   auto src = static_cast<const uint8_t*>(s.data) + y * (w>>2);
                   int sx = 0;
                   if(s.x < 0){
                       sx = -s.x;
                       w += s.x;
                   }else if(s.x > 0){
                       line += s.x;
                   }
                   if(s.x + w >= 220){
                       w = 220 - s.x;
                   }
                   while(w--){
                       auto b = (src[sx>>2] >> ((sx&3)<<1)) & 0x3;
                       if(b) *line = b;
                       line++;
                       sx++;
                   }
               };

    addSprite(Sprite{x, y, bitmap, f, h, 0, w});
}

void Display::drawBitmapDataXFlipped2BPP(int x, int y, int w, int h, const uint8_t* bitmap){
}

void Display::drawBitmapDataXFlipped8BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0) return;
    addSprite(Sprite{x, y, bitmap, blitMirror, h, 0, w});
}

void Display::drawBitmapDataYFlipped(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap) {
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto h = s.maxY - s.y;
                   auto w = s.b2;
                   auto src = static_cast<const uint8_t*>(s.data) + (h - 1 - y) * (w>>1);
                   int sx = 0;
                   if(s.x < 0){
                       sx = -s.x;
                       w += s.x;
                   }else if(s.x > 0){
                       line += s.x;
                   }
                   if(s.x + w >= 220){
                       w = 220 - s.x;
                   }
                   while(w--){
                       auto b = (sx&1)?
                           src[sx>>1]&0xF:
                           src[sx>>1]>>4;
                       if(b) *line = b;
                       line++;
                       sx++;
                   }
               };

    addSprite(Sprite{x, y, bitmap, f, h, 0, w});
}

void Display::drawBitmapDataXFlipped4BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto w = s.b2;
                   auto src = static_cast<const uint8_t*>(s.data) + y * (w>>1) + (w>>1);
                   if(s.x < 0){
                       w += s.x;
                   }else if(s.x > 0){
                       line += s.x;
                   }
                   if(s.x + w >= 220){
                       src -= (220 - (s.x + w)) >> 1;
                       w = 220 - s.x;
                   }
                   int sx = -1;
                   while(w--){
                       auto b = (sx&1)?
                           src[sx>>1]&0xF:
                           src[sx>>1]>>4;
                       if(b) *line = b;
                       line++;
                       sx--;
                   }
               };

    addSprite(Sprite{x, y, bitmap, f, h, 0, w});
}

void Display::drawBitmapData4BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0)
        return;

    draw_t f = [](uint8_t *line, Sprite &s, int y){
                   auto w = s.b2;
                   auto src = static_cast<const uint8_t*>(s.data) + y * (w>>1);
                   int sx = 0;
                   if(s.x < 0){
                       sx = -s.x;
                       w += s.x;
                   }else if(s.x > 0){
                       line += s.x;
                   }
                   if(s.x + w >= 220){
                       w = 220 - s.x;
                   }
                   while(w--){
                       auto b = (sx&1)?
                           src[sx>>1]&0xF:
                           src[sx>>1]>>4;
                       if(b) *line = b;
                       line++;
                       sx++;
                   }
               };

    addSprite(Sprite{x, y, bitmap, f, h, 0, w});
}

void Display::drawBitmapData8BPP(int x, int y, int w, int h, const uint8_t* bitmap) {
    if(y >= 176 || y + h < 0 || x >= 220 || x + w < 0) return;
    addSprite(Sprite{x, y, bitmap, blit, h, 0, w});
}

void Display::drawColorTile(uint32_t x, uint32_t y, uint8_t color){
    if(x >= mapW || y >= mapH)
        return;
    uint32_t color32 = color;
    tilemap[y*mapW + x] = reinterpret_cast<const uint8_t*>(color32);
}


void Display::drawTile(uint32_t x, uint32_t y, const uint8_t *data){
    if(x >= mapW || y >= mapH)
        return;
    tilemap[y*mapW + x] = data;
}

void Display::drawSprite(int x, int y, const uint8_t *data, bool flipped, bool mirrored, uint8_t recolor){
    if(y >= 176 || y + data[1] < 0 || x >= 220 || x + data[0] < 0) return;
    auto mode = flipped ?
        (mirrored ? blitFlipMirror : blitFlip):
        (mirrored ? blitMirror     : blit);
    addSprite(Sprite{x, y, data+2, mode, data[1], recolor, data[0]});
}

void drawSprites(int16_t y, uint8_t *line, int max){
    if(!max) return;
    for(uint32_t i=0; i<spriteBufferPos; ++i){
        auto& s = spriteBuffer[i];
        if( s.y > y ) continue;

        int smaxY = s.maxY;
        if( smaxY <= y ){
            /* makes no difference? * /
            for(uint32_t j=i+1; j<spriteBufferPos; ++j){
                spriteBuffer[j-1] = spriteBuffer[j];
            }
            --i;
            --spriteBufferPos;
            /* */
            continue;
        }

        s.draw(line, s, y - s.y);
        if(!--max) break;
    }
}

namespace Pokitto {

void lcdRefreshTASMode(uint8_t *line, const uint16_t* palette){
    uint8_t spritesPerLine[176];
    for(int y=0; y<176; ++y) spritesPerLine[y] = 0;

    for(uint32_t i=0; i<spriteBufferPos; ++i){
        auto& s = spriteBuffer[i];
        for(int y=std::max(0, static_cast<int>(s.y)), my=std::min(176, static_cast<int>(s.maxY)); y<my; ++y){
            spritesPerLine[y]++;
        }
    }

    Tilemap  tileWindow = tilemap;
    uint32_t tileY = tileH - cameraY;
    uint32_t tileX = cameraX;
    uint32_t tileIndex = cameraY * tileW;
    
    for(uint32_t y=0; y<176; ++y, tileIndex += tileW){
        if(!tileY--){
            tileIndex = 0;
            tileY = tileH - 1;
            tileWindow += mapW;
        }

        tileX = cameraX;
        uint32_t tile = 0;
        for(uint32_t i=0; i<220;){
            int iter = std::min(tileW - tileX, 220 - i);

            auto tileColor = reinterpret_cast<uintptr_t>(tileWindow[tile]);
            if( tileColor < 256 ){
                while(iter--){
                    line[i++] = tileColor;
                }
            }else if(Display::m_colordepth == 8){
                auto tileData = tileWindow[tile] + tileX + tileIndex;
                pixelCopySolid(line+i, tileData, iter);
                i += iter;
            }else if(Display::m_colordepth == 4){
                auto tileData = tileWindow[tile] + (tileIndex>>1);
                pixelCopySolid4BPP(line+i, tileData, iter, tileX);
                i += iter;
            }
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
