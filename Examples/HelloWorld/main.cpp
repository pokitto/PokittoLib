#include "Pokitto.h"

Pokitto::Core mygame;

int main () {
    mygame.begin();

    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.println("Hello World!");
        }
        }

}
