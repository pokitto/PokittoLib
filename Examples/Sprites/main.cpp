#include "Pokitto.h"
#include "gfxdata.h"
Pokitto::Core mygame;

// MySprite
struct MySprite {
    int16_t x,y,w,h,vx,vy;
};

const int16_t speed = 4;
const uint8_t spriteCount = 4;
const uint8_t spriteW = 32, spriteH = 32;
MySprite mySprites[spriteCount];

// Palettes
const uint16_t *palettes[spriteCount] = {
    sprite1_pal,
    sprite2_pal,
    sprite3_pal,
    sprite4_pal,
};

int main () {

    // Init
    int16_t startX = 0, startY = 0;
    for(int i=0; i < spriteCount; i++) {

        // Set my sprite data.
        mySprites[i].x = startX + i*21;
        mySprites[i].y = startY + i*21;
        mySprites[i].w = spriteW; mySprites[i].h = spriteH;
        mySprites[i].vx = speed; mySprites[i].vy = speed;

        // Set sprite
        mygame.display.setSpriteBitmap(i, sprite_bmp, palettes[i], mySprites[i].x, mySprites[i].y);
    }

    // Do not clear the background!
    mygame.display.persistence = 1;

    // Set the palette for the screen buffer
    mygame.display.palette[0] = background_pal[0];
    mygame.display.palette[1] = background_pal[1];
    mygame.display.palette[2] = background_pal[2];
    mygame.display.palette[3] = background_pal[3];

    mygame.begin();

    mygame.setFrameRate(100);  // No limits!

    // Game loop
    uint8_t isFirstRounds = 2;
    while(mygame.isRunning()) {

        // Draw background
        if(isFirstRounds > 0) {
            if(mygame.update(false)) { // Update the screen buffer
                // Draw the background only in the first rounds.
                mygame.display.drawBitmap(0, 0, background_bmp);
                isFirstRounds--;
            }
        }
        // Draw mySprites
        else {
            if (mygame.update(true)) { // Do not update the screen buffer
                // Move mySprites
                for (int i=0; i < spriteCount; i++) {

                    int16_t x = mySprites[i].x, y = mySprites[i].y;
                    int16_t w = mySprites[i].w, h = mySprites[i].h;

                    // Advance x and y.
                    x += mySprites[i].vx; y += mySprites[i].vy;

                    // Bounce from hidden edges
                    if (x < -spriteW) {
                        x = -spriteW;
                        mySprites[i].vx = speed;
                    }
                    else if (x >= mygame.display.getWidth() - w + spriteW) {
                        x = mygame.display.getWidth() - 1 - w + spriteW;
                        mySprites[i].vx = -speed;
                    }
                    if (y < -spriteH) {
                        y = -spriteH;
                        mySprites[i].vy = speed;
                    }
                     else if (y >= mygame.display.getHeight() - h + spriteH) {
                        y = mygame.display.getHeight() - 1 - h + spriteH;
                        mySprites[i].vy = -speed;
                    }

                    // Set sprite
                    mygame.display.setSpritePos(i, x, y);

                    //
                    mySprites[i].x = x; mySprites[i].y = y;
                }
            }
        }
    }
    return 0;
}
