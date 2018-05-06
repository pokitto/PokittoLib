#include "Pokitto.h"

Pokitto::Core mygame;


int main () {
    mygame.begin();
    int a = random(100);
    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.println(a);
            mygame.display.println((int)Pokitto::Battery::level);
            mygame.display.print("Hello World!");

        }
        }

}
