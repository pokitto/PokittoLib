#include "Pokitto.h"
#include "PokittoSaveblock.h"

Pokitto::Core mygame;

class saveb : public Pokitto::Saveblock {
    int val=0;
    int goo=35;
};

saveb mysave;

int main () {
    mygame.begin();
    mysave.formatKeyTable();
    mysave.begin("TESTTEST");
    int a = random(100);
    char answer[9];

    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.println(a);
            mygame.display.println((int)Pokitto::Battery::level);
            mygame.display.println("Hello World!");
            mysave.getKeyAt(0, answer);
            mygame.display.println(answer);
            mysave.getKeyAt(1, answer);
            mygame.display.println(answer);
            mysave.getKeyAt(2, answer);
            mygame.display.println(answer);
            mygame.display.println(sizeof(mysave));
        }
        }

}
