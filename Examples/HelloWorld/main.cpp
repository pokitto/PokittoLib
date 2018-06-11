#include "Pokitto.h"
#include "PokittoCookie.h"

Pokitto::Core mygame;

class saveb : public Pokitto::Cookie {
    int val=0;
    int goo=35;
};

saveb mysave;

int main () {
    mygame.begin();
    mysave.formatKeytable();
    mysave.begin("TESTTEST",sizeof(mysave));
    int a = random(100);
    char answer[9];

    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.println(a);
            mygame.display.println((int)Pokitto::Battery::level);
            mygame.display.println("Hello World!");
            mysave.readKeytableEntry(0, answer);
            mygame.display.println(answer);
            mysave.readKeytableEntry(1, answer);
            mygame.display.println(answer);
            mysave.readKeytableEntry(2, answer);
            mygame.display.println(answer);
            mygame.display.println(sizeof(mysave));
        }
        }

}
