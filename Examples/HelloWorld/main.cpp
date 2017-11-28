#include "Pokitto.h"
#include "gfxdata.h"
Pokitto::Core mygame;

struct MySprite {
    int16_t x,y,w,h,vx,vy;
};

uint8_t spriteCount = 1;
MySprite sprites[8];
int16_t speedX = 3, speedY = 3;

// Palettes
const uint16_t *palettes[8] = {
    sprite1_pal,
    sprite2_pal,
    sprite3_pal,
    sprite4_pal,
    sprite5_pal,
    sprite6_pal,
    sprite7_pal,
    sprite8_pal
};

int main () {

    // Init
    int16_t startX = 0, startY = 0;
    for (int i=0; i < spriteCount; i++) {

        // Set my sprite data.
        sprites[i].x = startX + i*21;
        sprites[i].y = startY + i*21;
        sprites[i].w = 32; sprites[i].h = 32;
        sprites[i].vx = 5;
        sprites[i].vy = 5;

        // Set sprite
        mygame.display.setSpriteBitmap(i, sprite_bmp, palettes[i], startX, startY);
    }

    // Do not clear the background!
    mygame.display.persistence = 1;

    // Set the palette for the screen buffer
    mygame.display.palette[0] = background_pal[0];
    mygame.display.palette[1] = background_pal[1];
    mygame.display.palette[2] = background_pal[2];
    mygame.display.palette[3] = background_pal[3];

    mygame.begin();

    mygame.setFrameRate(100);

    uint8_t isFirstRounds = 2;
    uint32_t frameNum = 0;
    uint8_t autoRunStartIndex = 0;
    while (mygame.isRunning()) {

        if (isFirstRounds>0) {
            if (mygame.update(false)) {
                // Draw the background only in the first round
                mygame.display.drawBitmap(0, 0, background_bmp);
                isFirstRounds--;
            }
        }
        else {
            if (mygame.update(true)) {

                // Read key
                if (mygame.buttons.held(BTN_B, 1) && spriteCount < 8) {

                    ++spriteCount;
                    uint8_t i = spriteCount-1;

                    // Set my sprite data.
                    sprites[i].x = 110;
                    sprites[i].y = 88;
                    sprites[i].w = 32; sprites[i].h = 32;
                    sprites[i].vx = 5;
                    sprites[i].vy = 5;

                    // Set sprite
                    mygame.display.setSpriteBitmap(i, sprite_bmp, palettes[i], startX, startY);
                }

                if(autoRunStartIndex==1) {
                    sprites[0].vx = 0;
                    sprites[0].vy = 0;
                }

                if( spriteCount > 0 ) {
                   if(mygame.buttons.repeat(BTN_LEFT,0)) {
                        sprites[0].vx = -speedX;
                        autoRunStartIndex = 1;
                    }
                    if(mygame.buttons.repeat(BTN_RIGHT,0)) {
                        sprites[0].vx = speedX;
                        autoRunStartIndex = 1;
                   }
                    if(mygame.buttons.repeat(BTN_DOWN,0)) {
                        sprites[0].vy = speedY;
                        autoRunStartIndex = 1;
                   }
                    if(mygame.buttons.repeat(BTN_UP,0)) {
                        sprites[0].vy = -speedY;
                        autoRunStartIndex = 1;
                   }
                }

                // Move sprites
                for (int i=0; i < spriteCount; i++) {

                    int16_t x = sprites[i].x, y = sprites[i].y;
                    int16_t w = sprites[i].w, h = sprites[i].h;

                    // Advance x and y.
                    x += sprites[i].vx; y += sprites[i].vy;

                    if( i >= autoRunStartIndex ) {
                        // Check for collision to screen edges
                        if (x < -32) {
                            x = -32;
                            sprites[i].vx = speedX;
                        }
                        else if (x > 219 - w + 32) {
                            x = 219 - w + 32;
                            sprites[i].vx = -speedX;
                        }
                        if (y < -32) {
                            y = -32;
                            sprites[i].vy = speedY;
                        }
                         else if (y > 175 - h + 32) {
                            y = 175 - h + 32;
                            sprites[i].vy = -speedY;
                        }
                    }

                    // Set sprite
                    mygame.display.setSpritePos(i, x, y);

                    //
                    sprites[i].x = x; sprites[i].y = y;
                }
                frameNum++;
            }
        }
    }
}
