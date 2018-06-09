#include "Pokitto.h"
#include <vector>
#include <stdint.h>
#include "sprites.h"
#include "bear_sprites.h"

Pokitto::Core game;
Pokitto::Display disp;
Pokitto::Buttons btn;

#define WHITE 13

#define SW 110
#define SH 88

#define MAP_WIDTH 48
#define MAP_HEIGHT 48

#define ID_BEAR 0
#define ID_PRESENT 7
#define ID_PENGUIN 33
#define ID_PORCH 35

bool game_won = false;
bool given_hat = false;
bool debug_console = false;
int present_count = 0;

const uint8_t logo[] =
{
32,52,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,35,50,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,61,211,34,34,34,34,34,34,34,34,34,34,
34,34,34,35,221,221,50,34,34,34,34,34,34,34,34,34,
34,34,34,35,221,221,50,34,34,34,34,34,34,34,34,34,
34,34,35,51,221,221,51,50,34,34,34,34,34,34,34,34,
34,34,61,221,29,209,221,211,34,34,34,34,34,34,34,34,
34,35,221,221,209,29,221,221,50,34,34,34,34,34,34,34,
34,35,221,221,209,29,221,221,50,34,34,34,34,34,34,34,
34,35,29,221,29,209,221,209,50,34,35,50,34,34,34,34,
34,35,17,17,221,221,17,17,50,34,61,99,34,34,34,34,
34,34,49,17,221,221,17,19,34,35,214,102,50,34,34,34,
34,34,35,51,221,221,51,50,34,35,102,102,50,34,34,34,
34,34,34,35,29,209,50,34,35,51,38,98,51,50,34,34,
34,34,34,35,17,17,50,34,61,243,34,34,61,83,34,34,
34,34,34,34,49,19,34,35,223,255,50,35,213,85,50,34,
34,34,34,34,35,50,34,35,255,255,51,51,85,85,50,34,
34,34,34,34,34,34,34,35,175,250,51,51,69,84,50,34,
34,34,34,34,34,34,34,35,170,170,61,131,68,68,50,34,
34,34,34,34,34,34,34,34,58,163,216,136,52,67,34,34,
34,34,34,34,34,34,34,34,35,51,136,136,51,50,34,34,
34,34,34,34,34,34,34,34,34,35,152,137,50,34,34,34,
34,34,34,34,34,34,34,34,34,35,153,153,50,34,34,34,
34,34,34,34,34,34,34,34,34,34,57,147,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,35,50,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,45,221,221,45,221,210,45,221,221,34,34,34,34,
34,34,34,33,29,17,45,17,29,45,17,17,34,34,34,34,
34,34,34,34,45,34,45,34,45,45,221,34,34,34,34,34,
34,34,34,34,45,34,45,221,209,45,17,34,34,34,34,34,
34,34,34,34,45,34,45,17,29,45,221,221,34,34,34,34,
34,34,34,34,33,34,33,34,33,33,17,17,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,34,34,210,34,34,221,221,210,34,34,34,34,34,
34,34,34,34,34,210,34,34,209,17,18,34,34,34,34,34,
34,34,34,34,34,210,34,34,221,210,34,34,34,34,34,34,
34,34,34,34,34,210,34,34,209,18,34,34,34,34,34,34,
34,34,34,34,34,221,221,210,221,221,210,34,34,34,34,34,
34,34,34,34,34,17,17,18,17,17,18,34,34,34,34,34,
34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
34,34,34,45,34,45,34,221,210,45,221,210,34,34,34,34,
34,34,34,45,210,221,45,17,29,45,17,29,34,34,34,34,
34,34,34,45,29,29,45,34,45,45,34,45,34,34,34,34,
34,34,34,45,33,45,45,221,221,45,221,209,34,34,34,34,
34,34,34,45,34,45,45,17,29,45,17,29,34,34,34,34,
34,34,34,33,34,33,33,34,33,33,34,33,34,34,34,34,
};

using namespace std;

float lerp(float a, float b, float t) {
    return (1-t)*a + t*b;
}

float clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}
int clip(int n, int lower, int upper) {
  return std::max(lower, std::min(n, upper));
}

float dist(float x1, float y1, float x2, float y2) {
    float x = x1 - x2;
    float y = y1 - y2;
    float dist;
    dist = pow(x,2)+pow(y,2);
    dist = sqrt(dist);
    return dist;
}
float angle(float x1, float y1, float x2, float y2) {
    return atan2(y1 - y2, x1 - x2);
}

void splashScreen(char * name, char bg_color, char text_color) {
    disp.persistence = true;
    disp.bgcolor = bg_color;
    disp.clear();
    disp.textWrap = true;
    disp.drawBitmap((SW/2)-16,(SH/2)-28, logo);
    disp.setColor(text_color);
    disp.print(SW/2- ((strlen(name)*4)/2)-2,SH-7,name);
    //game.wait(time);
    for (int c=0; c<16;c++) {
        disp.palette[c] = 0x00;
    }
    disp.update();

    int frame = 0;

    while (frame<180&&!btn.cBtn()) {
        if (frame<60) {
            for (int c = 0; c<16; c++) {
                disp.palette[c] = disp.interpolateColor(disp.palette[c], sprites_pal[c], (255/60)*frame);
            }
        }
        else if (frame<120) {
            disp.load565Palette(sprites_pal);
        }
        else {
            for (int c = 0; c<16; c++) {
                disp.palette[c] = disp.interpolateColor(disp.palette[c], 0x00, (255/60)*(frame-120));
            }
        }
        disp.update();
        frame++;
    }

    disp.persistence = false;
}

char dialogs[][88] {
    {"Oh no! Santa's sleigh had a\nhole in the floor and all the presents fell from the sky!"},
    {"Do you think you can find all of them before Christmas\nday?"},
    {"If you find a present, pick it up and bring it back to the workshop!"},
    {"Just set them on the porch here. Be careful though,\n it's icy out there!"},
    {"You may carry more than one present at a time,\nbut they're pretty HEAVY!"},

    {"You did it! You saved Christmas! Here, have a hat for your reward."},
    {"There's not much left to do anymore. Continue to roam around if you'd like."}

};

bool dialog_active = false;
char active_dialog = 0;
char dialog_index = 0;
int dialog_draw_y = -42;
char dialog_target_y = 2;

void drawDialog(int y, int collected_presents) {

    if (!dialog_active) return;

    disp.setColor(3);
    int height = disp.fontHeight*3;
    disp.fillRectangle(0,y,110,height);
    disp.setColor(2);
    disp.drawLine(0, y+height, 110, y + height);
    disp.setColor(1);
    disp.drawLine(0, y+height+1, 110, y + height+1);
    disp.setColor(0);
    disp.setCursor(0, y+1);
    disp.print(" ");
    for (char i = 0; i<dialog_index; i++) {
        disp.print(dialogs[active_dialog][i]);
        if (disp.cursorX==0) disp.cursorX = 3;
    }

    if (game.frameCount%1==0) dialog_index = min(dialog_index + 1, 88);
    if (dialog_index>44) {
        disp.drawBitmap(100, y + 20, sprites[31 + ((game.frameCount / 4) % 2)]);
        if (btn.pressed(BTN_A)) {
            active_dialog++;
            if (active_dialog==6&&game_won) {
                given_hat = true;
            }
            if ((active_dialog>4&&!game_won)||(active_dialog>6&&game_won)) {
                dialog_target_y = -42;
                dialog_active = false;
            }
            dialog_index=0;
        }
    }
    dialog_draw_y = lerp(dialog_draw_y, dialog_target_y, 0.2);

    if (dialog_target_y == -42 && dialog_draw_y < -40) dialog_active = false;
}


bool SOLID_TILES[40] {
    0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
};

namespace Map {
    unsigned char data[MAP_WIDTH*MAP_HEIGHT] = {
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,3,3,3,3,3,3,3,3,3,3,0,0,6,3,3,3,3,3,3,3,3,3,3,
3,3,3,1,1,1,0,0,0,6,5,6,0,6,6,6,5,6,0,6,0,6,0,3,3,1,1,0,3,3,3,3,3,3,3,0,10,0,0,6,0,0,1,1,1,3,3,3,
3,3,1,5,1,0,1,0,6,0,6,0,0,5,6,7,6,0,6,0,0,0,0,3,1,0,0,0,0,3,3,3,3,3,6,0,0,0,0,0,0,6,6,0,6,1,3,3,
3,3,1,0,6,0,6,0,6,0,0,1,6,1,0,6,6,6,6,0,6,6,6,3,0,0,0,0,0,0,3,3,3,0,6,1,0,0,0,0,0,6,6,5,0,6,1,3,
3,3,1,5,6,0,0,6,0,0,0,0,0,0,6,0,6,1,5,1,0,6,0,3,0,0,8,0,1,0,3,3,3,3,6,6,0,6,6,6,0,0,0,6,0,0,1,3,
3,1,6,6,6,0,0,0,0,0,0,0,0,0,0,6,0,0,6,1,1,6,6,3,3,0,0,0,0,5,3,3,3,3,6,6,1,0,6,3,6,6,9,6,1,0,1,3,
3,1,0,0,0,6,5,0,0,0,0,0,0,6,0,0,0,5,6,1,1,6,6,0,3,3,3,0,0,0,3,3,3,0,6,6,0,0,6,0,6,6,0,0,6,0,6,3,
3,1,0,0,6,0,0,0,0,0,0,0,6,0,6,0,0,0,6,1,6,0,0,1,6,5,3,3,0,0,3,3,3,6,6,0,0,5,6,6,0,0,0,5,0,0,0,3,
3,1,0,0,5,0,0,0,0,0,30,30,30,30,30,5,0,1,6,6,5,6,6,0,6,5,5,3,3,0,3,3,0,6,0,0,0,6,6,0,1,6,0,0,0,0,0,3,
3,1,6,5,6,0,0,6,5,6,30,30,30,30,30,6,0,0,0,0,0,6,6,0,6,6,0,1,3,0,0,3,6,6,0,6,0,0,0,0,6,6,5,6,6,6,0,3,
3,1,6,5,6,6,6,0,6,0,16,15,15,15,18,1,6,6,0,1,0,5,0,6,0,6,6,6,6,6,6,6,0,0,6,0,0,5,0,6,6,0,6,1,6,0,3,3,
3,6,1,1,0,5,6,0,6,5,17,13,13,13,17,6,6,0,6,0,0,0,0,0,6,5,6,0,0,0,6,0,0,0,0,0,0,6,6,0,0,6,0,0,0,0,3,3,
3,0,6,6,6,6,6,6,5,36,19,20,14,12,20,36,6,5,0,0,5,0,6,1,0,0,6,0,0,0,0,6,0,1,0,6,6,1,6,0,0,6,0,0,0,6,3,3,
3,0,5,6,0,5,6,0,1,36,35,35,35,35,35,36,1,0,0,6,0,0,5,6,0,0,0,0,0,6,0,6,6,0,6,6,6,1,1,6,0,0,0,5,5,6,0,3,
3,6,1,5,0,6,5,0,0,36,35,35,35,35,35,36,0,6,5,0,1,6,0,6,0,6,6,6,0,6,0,0,6,6,6,6,1,1,1,6,6,1,6,6,0,6,0,3,
3,3,6,0,0,6,6,1,6,37,38,38,6,38,38,39,0,6,6,0,0,5,0,6,0,6,1,1,6,6,0,0,1,6,6,1,1,6,6,6,3,3,6,6,0,6,0,3,
3,3,0,6,5,0,6,6,6,1,6,6,6,33,0,0,0,6,0,6,6,6,6,6,0,1,1,5,0,6,6,6,1,1,1,1,1,1,6,6,3,0,0,0,0,0,0,3,
3,3,0,0,0,6,0,0,0,5,0,2,6,6,6,6,21,23,6,0,5,6,0,0,6,1,1,1,6,0,6,5,1,1,1,5,1,1,6,6,0,0,5,0,6,6,0,3,
3,3,3,5,0,0,6,6,6,0,0,5,6,21,22,22,25,25,23,6,0,6,6,0,5,0,1,1,5,6,6,1,1,1,1,1,5,6,0,0,6,6,0,6,0,1,6,3,
3,3,3,3,5,0,6,5,0,0,1,6,6,24,25,25,28,28,25,23,5,6,1,6,5,0,0,1,6,6,6,1,1,6,6,0,0,0,0,6,5,1,0,0,0,6,0,3,
3,3,3,3,3,3,3,0,1,6,5,6,6,27,28,29,0,1,27,29,0,0,0,5,0,6,0,5,0,1,6,6,1,1,6,0,6,5,6,5,0,0,0,6,0,0,0,3,
3,3,3,1,3,3,3,3,0,5,6,0,6,6,6,6,5,0,0,0,0,0,6,0,0,0,6,0,0,6,6,6,1,1,1,6,0,0,6,6,6,5,0,6,6,0,6,3,
3,3,1,1,1,1,3,3,6,0,0,6,0,0,5,6,5,0,6,6,1,6,6,0,6,3,3,3,3,3,5,6,6,1,1,6,6,6,6,1,1,6,0,0,0,7,0,3,
3,3,1,6,0,0,0,3,6,0,5,5,0,1,1,6,0,6,0,0,6,0,6,6,3,3,6,6,0,3,6,0,6,1,1,1,1,1,1,1,1,6,6,0,6,6,0,3,
3,3,0,5,0,5,0,3,3,0,5,5,6,6,1,1,1,0,0,6,1,0,5,0,3,5,6,5,7,3,3,3,1,1,1,6,6,1,1,6,6,6,0,5,0,0,6,3,
3,3,0,0,10,0,0,3,3,6,6,0,6,6,6,6,5,0,0,0,6,0,0,1,3,3,0,1,6,6,3,3,3,1,6,0,6,0,0,6,0,0,6,1,5,6,0,3,
3,3,0,5,0,0,0,3,5,6,6,0,6,0,6,6,0,5,6,6,6,5,5,5,0,3,3,0,5,0,3,3,3,5,0,0,9,0,0,0,6,0,6,6,6,0,0,3,
3,3,3,0,0,0,0,0,6,5,0,1,6,0,6,5,1,6,6,6,6,6,0,6,0,0,5,6,6,6,3,3,3,0,6,6,6,5,0,0,6,6,6,6,6,6,5,3,
3,3,3,3,0,5,0,0,0,6,6,6,0,6,1,6,0,1,0,6,0,0,5,6,0,0,0,6,0,3,3,3,3,3,6,1,1,1,6,1,1,5,6,6,6,6,3,3,
3,3,3,3,3,3,3,3,6,0,6,6,6,6,5,6,6,6,0,5,0,6,1,6,0,0,0,3,3,3,0,0,3,3,6,3,1,1,1,1,6,6,0,6,6,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,6,0,6,5,0,0,0,0,0,0,1,0,5,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,3,3,3,3,3,3,
3,3,3,3,3,3,1,6,6,6,6,0,0,0,6,0,0,5,0,5,0,6,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,3,3,3,3,3,3,
3,3,3,3,3,1,1,6,6,0,5,6,6,1,6,0,5,0,0,0,6,0,6,0,6,0,0,6,0,0,0,5,0,0,0,6,1,5,0,6,0,0,0,3,3,3,3,3,
3,3,3,1,1,1,0,0,0,0,5,0,0,0,0,6,0,5,1,0,0,0,3,3,0,0,0,0,6,1,0,6,0,0,6,0,0,6,6,6,0,0,0,1,3,3,3,3,
3,3,1,1,0,0,6,0,0,6,6,0,0,6,6,0,6,0,0,3,3,3,3,6,1,6,0,5,0,0,0,0,0,1,6,5,6,0,0,0,0,0,6,6,0,3,3,3,
3,1,1,0,0,0,6,0,6,0,0,6,0,0,0,6,0,3,3,3,3,3,3,0,0,0,0,0,0,0,21,22,23,0,0,6,0,6,0,1,1,5,0,0,0,1,3,3,
3,1,6,6,0,6,0,0,0,0,0,5,0,6,0,0,0,3,3,3,0,0,6,0,0,6,0,21,22,22,25,25,26,0,0,0,0,1,1,1,1,6,6,1,6,5,3,3,
3,0,6,6,1,5,0,0,0,0,6,0,0,0,0,6,0,6,0,0,0,5,0,0,0,21,22,25,25,25,25,25,29,6,0,0,0,1,1,1,6,6,0,6,0,0,3,3,
3,0,1,0,0,6,6,0,0,9,6,6,0,0,0,0,0,5,5,0,0,0,0,0,0,6,0,0,6,27,28,29,0,6,1,0,0,1,1,0,6,1,0,0,6,6,3,3,
3,0,0,0,1,1,6,0,0,0,0,0,0,0,6,6,6,0,6,0,1,0,5,0,0,0,0,0,6,6,0,1,6,5,0,6,5,0,0,0,0,0,0,5,7,0,3,3,
3,5,0,0,0,1,1,6,6,0,0,6,0,5,0,6,0,0,0,5,6,5,0,0,0,1,1,6,0,0,1,0,6,0,0,6,0,0,0,6,0,0,0,6,6,6,3,3,
3,0,0,0,0,0,1,1,5,0,0,0,0,1,1,0,5,6,0,0,0,6,6,6,6,0,6,5,6,0,6,1,0,0,0,6,6,6,0,0,6,5,0,6,0,0,6,3,
3,3,3,0,0,6,1,1,1,1,0,0,0,0,6,0,5,5,0,0,0,5,0,6,0,0,0,6,0,6,6,6,0,0,1,0,0,5,0,0,0,6,6,0,6,0,0,3,
3,3,3,6,0,0,0,1,1,1,0,0,6,6,6,6,5,6,0,6,0,6,5,0,6,0,0,0,5,6,0,0,0,0,5,0,6,6,0,0,9,0,0,6,0,1,3,3,
3,3,3,3,5,6,7,0,1,1,0,6,0,6,6,0,0,3,3,0,6,6,0,5,6,0,6,6,0,5,1,0,0,0,0,6,0,0,0,0,0,0,0,5,0,3,3,3,
3,3,3,3,3,3,0,6,3,3,3,3,3,3,3,3,3,3,3,3,3,5,6,0,0,5,0,0,0,0,0,5,6,0,0,5,6,0,0,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    };

    int getID(int x, int y) {
        int addr = (y * MAP_WIDTH) + x;
        return data[addr];
    }

    bool checkSolid(int x, int y) {
        return SOLID_TILES[getID(x, y)];
    }

    float cam_x, cam_y;

    void update(int target_x, int target_y) {
        cam_x = clip(lerp(cam_x, 55 - target_x - 5, .1), (float)-((MAP_WIDTH*10)-110), (float).0);
        cam_y = clip(lerp(cam_y, 44 - target_y - 5, .1), (float)-((MAP_HEIGHT*10)-88), (float).0);
        //cam_x = clip(55 - target_x, -((MAP_WIDTH*10)-110), 0);
        //cam_y = clip(44 - target_y, -((MAP_HEIGHT*10)-88), 0);
    }
    void drawFast() {
        for (unsigned char tx = 0; tx<12; tx++) {
            for (unsigned char ty = 0; ty<10; ty++) {
                unsigned char id = getID(tx-(cam_x/10), ty-(cam_y/10));
                if (id!=0) {
                    if ((id == 33)&&(game.frameCount/8)%2==0) id = 33+1;
                    disp.drawBitmap(tx*10 + ((int)cam_x%10), ty*10 + ((int)cam_y%10), sprites[id]);
                }
            }
        }
    }
    void draw() {
        for (int i = 0; i<(MAP_WIDTH*MAP_HEIGHT); i++) {
            int x, y;
            x = i%MAP_WIDTH;
            y = (i-x)/MAP_WIDTH;
            if (data[i]!=0) disp.drawBitmap(x*10 + cam_x, y*10 + cam_y, sprites[data[i]]);
        }
    }
}

struct SnowFlake {
    char x, y;
    SnowFlake() {
        x = random(0,110);
        y = random(0,88);
    };
    void update() {
        y+=random(1, 2);
        y%=88;
        x+=random(-1,1) % 110;
        disp.drawPixel(x, y, WHITE);
    }
} snowflakes[20];

struct Particle {
    int x, y, vx, vy, radius;
    unsigned int time;
    unsigned char color;
    Particle(int _x, int _y, unsigned char _color, unsigned char _radius) {
        x = _x;
        y = _y;
        vx = 0;
        vy = 0;
        color = _color;
        radius = _radius;
        time = 0;
    }
    void update() {
        x+=vx;
        y+=vy;
        if (time % 2 == 0) {
            x+=random(-1, 1);
            y+=random(-1, 1);
        }
        time++;
    };
    void draw() {
        disp.setColor(color);
        disp.fillCircle(x + Map::cam_x, y + Map::cam_y, radius);
    };
};

vector<Particle> parts;


struct Entity {
    float x, y, vx, vy;
    unsigned char w, h, type, spr, flip;
    Entity * target;
    Entity(int _x, int _y, unsigned char _type) {
        type = _type;
        spr = type;
        x = _x;
        y = _y;
        vx = 0.0;
        vy = 0.0;
        w = 10;
        h = 10;
        if (type == ID_PRESENT) {
            w = 5;
            h = 6;
        }
        target = NULL;
    };
    Entity(int _x, int _y, unsigned char _type, int _spr) {
        type = _type;
        spr = type;
        x = _x;
        y = _y;
        vx = 0.0;
        vy = 0.0;
        w = 10;
        h = 10;
        if (type == ID_PRESENT) {
            w = 5;
            h = 6;
        }
        target = NULL;
        spr = _spr;
    };

    void control(vector<Entity> &ents) {
        bool walking = false;
        float speed = 1.0;
        if (btn.bBtn()) speed = 1.5;
        if (btn.leftBtn()) {
            vx = max((float)vx-.1,-(double)speed);
            flip = 1;
            walking = true;
        }
        else if (btn.rightBtn()) {
            vx = min(vx+.1,(double)speed);
            flip = 0;
            walking = true;
        }
        else {
            vx = lerp(vx, 0, 0.2);
        }

        if (btn.upBtn()) {
            vy = max((float)vy-.1,-(double)speed);
            walking = true;
        }
        else if (btn.downBtn()) {
            vy = min(vy+.1,(double)speed);
            walking = true;
        }
        else {
            vy = lerp(vy, 0, 0.2);
        }

        if (Map::getID( (x + vx ) / 10, ( y + vy ) / 10 ) == ID_PENGUIN || Map::getID( ( x + w - 1 + vx ) / 10, ( y + vy ) / 10 ) == ID_PENGUIN ) {
            if (btn.pressed(BTN_A)) {
                dialog_active = true;
                if (game_won) {
                    active_dialog = 5;
                }
                else active_dialog = 0;
                dialog_index = 0;
                dialog_draw_y = -42;
                dialog_target_y = 2;
            }
        }

        //------//--ANIMATION--------

        //--IDLE--

        if (!walking) {
            spr = (game.frameCount/8)%2;
        }

        //--WALK--
        else {
            int rate = game.frameCount/2;
            if (btn.bBtn()) {
                rate = game.frameCount;
            }
            spr = 2 + (rate)%4;
            if (game.frameCount%8==0) {
                parts.push_back(Particle(x + (w&&flip==1||0), y + h, 13, random(0,1)+.5));
            }
        }
        //------//

        if (btn.pressed(BTN_A)) {
            for (int i = 0; i<ents.size(); i++) {
                ents[i].target = NULL;
            }
        }
    };

    void updateBody(vector<Entity> * ents, int index) {

        for (int i = 0; i<ents->size();i++) {
            Entity * other = &ents->at(i);
            if (i!=index&&game.collideRectRect(x + vx, y + vy, w-1, h-1, other->x + other->vx, other->y + other->vy, other->w-1, other->h-1)) {
                if (i == 0) {
                    target = other;
                }
            }
            if (i!=index&&game.collideRectRect(x + vx, y + vy, w, h, other->x, other->y, other->w, other->h)) {
                if (x+vx<other->x) {
                    vx-=.1;
                }
                else vx+=.1;

                if (y+vy<other->y) {
                    vy-=.1;
                }
                else vy+=.1;
            }
        }

        if (index>=1&&target!=NULL) {
            float distance = dist(x, y, target->x, target->y);
            if (distance>=16) {
                float ang = angle(target->x, target->y, x, y);
                vx = lerp(vx, vx + cos(ang), distance/100);
                vy = lerp(vy, vy + sin(ang), distance/100);
                target->vx = lerp(target->vx, target->vx + (-cos(ang)), distance/200);
                target->vy = lerp(target->vy, target->vy + (-sin(ang)), distance/200);
            }
            else {
                vx = lerp(vx, 0, 0.05);
                vy = lerp(vy, 0, 0.05);
            }

        }
        else if (index>=1) {
                vx = lerp(vx, 0, 0.05);
                vy = lerp(vy, 0, 0.05);
        }


        char x_offset = 0;
        if (vx>0) {x_offset+=(w-1);}
        if (Map::checkSolid((x + vx + x_offset) / 10, y / 10)
            ||
            Map::checkSolid((x + vx + x_offset)/10, (y + h - 1) / 10)
            )
        {
            vx = 0;
        }
        char y_offset = 0;
        if (vy>0) {y_offset += (h - 1);}
        if (Map::checkSolid(x / 10, (y + vy + y_offset) / 10)
            ||
            Map::checkSolid((x + w - 1) / 10, (y + vy + y_offset) / 10)
            )
        {
            vy = 0;
        }

        x+=vx;
        y+=vy;
    };
    void drawShadow() {
        disp.setColor(11);
        int dx, dy;
        dx = x + Map::cam_x;
        dy = y + Map::cam_y;
        disp.drawLine(dx+1, dy + h, dx + w-1, dy + h);
        disp.drawLine(dx+2, dy + h+1, dx + w-2, dy + h+1);
    }
    void drawRope() {
        disp.setColor(15);
        if (target) {
            disp.setColor(3);
            disp.drawLine(x+ Map::cam_x + w/2, y + Map::cam_y + h/2, target->x + Map::cam_x + target->w/2, target->y + Map::cam_y + target->h/2);
        }
    };
    void draw() {
        if (type==ID_BEAR) {
            disp.drawBitmap(x + Map::cam_x, y + Map::cam_y, bear[spr], 0, flip);
            if (given_hat) {
                disp.drawBitmap(x + Map::cam_x, y + Map::cam_y-1, sprites[40], 0, flip);
            }
        }
        else {
            int ox = 0;
            #ifdef POK_SIM
            //not really sure why the sprite draws at the wrong coords in the sim. draws as expected on HW.
                ox = -4;
            #endif // POK_SIM
            disp.drawBitmap(x + Map::cam_x + ox, y + Map::cam_y + (cos(game.frameCount*.2)), sprites[spr], 0, flip);
        }

    };

};
vector<Entity> ents;

void initEntities() {
    for (int i = 0; i<(MAP_WIDTH*MAP_HEIGHT); i++) {
        int sx, sy;
        sx = i%MAP_WIDTH;
        sy = (i-sx)/MAP_WIDTH;
        if (Map::data[i]>=ID_PRESENT&&Map::data[i]<=ID_PRESENT+3) {
            ents.push_back(Entity(sx*10, sy*10, ID_PRESENT, Map::data[i]));
            Map::data[i] = 0;
            present_count++;
        }
        if (Map::data[i]==2) {
            Map::data[i]=0;
        }
    }
}

int main() {
    game.sound.playMusicStream("music/xmas.snd"); //before game.begin
    game.begin();
    game.sound.pauseMusicStream();
    game.setFrameRate(30);
    disp.setFont(fontTight);
    game.wait(10);
    splashScreen("@trelemar", 2, 1);
    disp.bgcolor = 12;
    disp.setInvisibleColor(12);
    //disp.load565Palette(sprites_pal);
    ents.push_back(Entity(110, 180, ID_BEAR));
    initEntities();
    Entity * player = &ents[0];
    Map::cam_x = 55-player->x;
    Map::cam_y = 88-player->y;

    game.sound.playMusicStream(); //trigger playback
    while (game.isRunning()) {
        if (game.update()) {
            if (game.frameCount<200) {
                for (int c = 0; c<16; c++) {
                    disp.palette[c] = disp.interpolateColor(disp.palette[c], sprites_pal[c], 100);
                }
            }

            int contained_presents = 0;

            Map::update(player->x, player->y);
            Map::drawFast();

            player->control(ents);
            for (int i = 0; i<ents.size(); i++) {
                ents[i].drawShadow();
                ents[i].drawRope();
            }
            for (int i = 0; i<ents.size(); i++) {
                Entity * ent = &ents.at(i);
                ent->updateBody(&ents, i);
                ent->draw();
                if (ent->type==ID_PRESENT) {
                    if (Map::getID(ent->x/10, ent->y/10)==ID_PORCH) {
                        contained_presents++;
                    }
                }
            }
            if (contained_presents==present_count) {
                game_won = true;
            }
            for (int i = 0; i<parts.size(); i++) {
                parts[i].update();
                parts[i].draw();
                if (parts[i].time>20) parts.erase(parts.begin()+i);
            }
            for (int i = 0; i<20; i++) {
                snowflakes[i].update();
            }
            if (btn.held(BTN_C, 50)) {
                debug_console = ! debug_console;
            }
            if (debug_console) {
                disp.setColor(10);
                disp.print("X ");
                disp.print(player->x);
                disp.print("  Y ");
                disp.println(player->y);
                disp.print("CAMx ");
                disp.println(Map::cam_x);
                disp.print("CAMy ");
                disp.println(Map::cam_y);
                disp.print("#ENTS ");
                disp.println(ents.size()+parts.size());
                disp.print("presents ");
                disp.println(present_count);

            }
            //Present counter for returned ones
            disp.setColor(3);
            disp.fillRoundRect(1, 1, 28, 7, 2);
            disp.drawBitmap(2, 2, sprites[ID_PRESENT]);
            disp.setColor(13);
            disp.print(9, 2, contained_presents);
            disp.print("/");
            disp.print(present_count);

            drawDialog(dialog_draw_y, contained_presents);
        }
    }
    return 1;
}

