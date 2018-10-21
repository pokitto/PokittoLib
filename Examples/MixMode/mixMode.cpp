
#include "Pokitto.h"
#include "bitmaps.h"
#include "HWSound.h"

Pokitto::Core game;

#include "drawing.h"


void clearScreen(){
    // clear the screen
    for(uint16_t t=0; t<110*88; t++){
        game.display.screenbuffer[t]=0;
    }
}

int mock_y;
int mock_dir;
void setupDemo(){
    load8bitPalette(seein_angel_pal,0,255);
    load2bitPalette(mockup_pal,0,4);
    mock_y=0;
    mock_dir=1;
    game.display.setInvisibleColor(-1);
    clearScreen();
}

void demoScreen(){
    drawBitmapLines(0, mock_y-2 ,mock_y-2 , 2, seein_angel, 8);

    game.display.setInvisibleColor(-1);

    drawBitmapLines(0, (mock_y*2),0 , 176-(mock_y*2), mockup, 2);

    for(int t=0; t<mock_y; t++){
        game.display.scanType[t]=0; // 8bit
    }
    for(int t=mock_y; t<88; t++){
        game.display.scanType[t]=2; // 8bit
    }

    mock_y+=mock_dir;
    if(mock_y>87 || mock_y<1) mock_dir = -mock_dir;;
}


 int main(){
    game.begin();
    game.display.persistence=1;
    game.setFrameRate(255); // max out the frame rate so that it doesn't get used
    srand(game.getTime());

    setupDemo();

    while (game.isRunning()) {

        if(game.update()){
            demoScreen();
        }

    }

return 1;
}






/*
    Credits etc.

    intro/complete music- jayenkai

    Joe speech using 'SAM' a C64 speech synth
    https://simulationcorner.net/index.php?page=sam
    sam -mouth 150 -throat 200 -pitch 58 -speed 120 -wav gameover.wav "Game Over!"




*/
