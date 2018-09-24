#include "Pokitto.h"

Pokitto::Core mygame;

int main ()
{
	mygame.begin();
	mygame.display.setFont(fontKoubit);

    while (mygame.isRunning()) {
        int laskuri=10;
        do {
            mygame.display.print(50,37,laskuri);
            mygame.display.update(); //pakota ruudun uudelleenpiirto
            mygame.wait(500); //odota 500 millisekuntia
            mygame.display.clear(); //näyttö puhtaaksi
            mygame.display.update(); //pakota näytön päivitys
            mygame.wait(200); //odota
            laskuri--; //lasketaan alas 10, 9, 8, jne.
        } while(laskuri);

        //lähtölaskenta on ohi

        mygame.display.print(27,37,"GAME OVER!");
        mygame.display.update();
        mygame.wait(1000); //odota 1 sek
        mygame.display.clear();

    }
}
