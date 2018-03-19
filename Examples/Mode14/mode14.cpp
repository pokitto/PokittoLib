
/**


THIS IS WORK IN PROGRESS ! IT DOES NOT WORK YET !!!


**/

#include "Pokitto.h"
#include "lemm.h"

Pokitto::Core mygame;

DigitalOut p(EXT0); //P1_19

#define TEST_PORT          1
#define TEST_PIN           19
#define CLR_TEST LPC_GPIO_PORT->CLR[TEST_PORT] = 1 << TEST_PIN;
#define SET_TEST LPC_GPIO_PORT->SET[TEST_PORT] = 1 << TEST_PIN;

int s=0;

int main () {
    mygame.begin();
    int c = 1;
    while (mygame.run_state) {
        if (mygame.update()) {
        if (s) {SET_TEST}
        else {CLR_TEST}
        s=1-s;
        //mygame.display.update();

            //mygame.display.bgcolor = c++;
            //if (c==8) c=0;
            //mygame.display.clear();
            //mygame.display.setCursor(55,80);
            //mygame.display.print("Hello World!");
            mygame.display.drawBitmap(0,0,lemm);
        }
        }

}

