#include "pokitto.h"
#include "CHIP8Player.h"

Pokitto::Core game;
CHIP8Player chip8Player;

int main () {

game.begin();
game.display.setFont(font5x7);
pokInitSD();
game.setFrameRate(30);
game.display.bgcolor = 8;

chip8Player.init(&game);

while (game.isRunning()) {

    if (game.update()) {

        chip8Player.update();
    }
}

return 1;
}
