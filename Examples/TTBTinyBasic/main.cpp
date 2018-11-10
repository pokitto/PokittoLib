#include "Pokitto.h"
#include "USBSerial.h"
#include "ttbasic.h"

Pokitto::Core game;

USBSerial pc;

Ticker cursor;
unsigned char blinkybox=0;
bool cursormutex=false;

#define BLINKYPERIOD 1000

void cursorticker() {
        if (cursormutex) return;
        int oX = game.display.cursorX;
        int oY = game.display.cursorY;
        blinkybox= 1 - blinkybox;
        if (blinkybox) game.display.bgcolor = 1; // reverse whitespace color
        else game.display.bgcolor = 0;
        game.display.print(" ");  // print whitespace
        game.display.bgcolor = 0; // revert bg color back to normal
        game.display.setCursor(oX,oY);
        game.buttons.update();
        game.display.update();
}

int main () {
game.display.palette[0]=COLOR_BLUE;
game.display.palette[1]=COLOR_WHITE;
game.begin();
game.display.persistence=1;
// Next 4 lines are just to show the C64 font, you can comment them out by adding // in the start of the line
game.display.setFont(fontC64);
//game.display.palette[0] = game.display.RGBto565(0x42, 0x42, 0xe7); //default background is palette[0]
//game.display.palette[1] = game.display.RGBto565(0xa5, 0xa5, 0xff); //default foreground is palette[1]
game.display.adjustCharStep = 0; //needed for the non-proportional C64 font (normal value=1)
int blinkybox = 0;
long blinkytime = game.getTime();
game.display.print("***** TTB TINYBASIC *****\n\n");
    game.display.println(" 32K RAM POKITTO SYSTEM \n");
    game.display.println("READY.");
    pc.printf("Pokitto ready\r\n");
    //c_puts("TOYOSHIKI TINY BASIC");newline();error();
    //c_putch('>');// Prompt
    //c_gets(); // Input 1 line
    inew();
    cursor.attach(&cursorticker, 0.5f);

while (game.isRunning()) {
        //if (game.update()) {
        basic();
        //}
}

return 1;
}

