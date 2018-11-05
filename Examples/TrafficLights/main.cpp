#include "Pokitto.h"

Pokitto::Core mygame;

Ticker LightTimer;
DigitalOut red1(EXT3);
DigitalOut yellow1(EXT2);
DigitalOut green1(EXT5);
DigitalOut red2(EXT4);
DigitalOut yellow2(EXT9);
DigitalOut green2(EXT8);


enum LightStates {
    red = 0,
    rednyellow = 1,
    green = 2,
    yellow = 3
};

LightStates lights1,lights2;

void change() {
    switch (lights1) {
    case red:
        lights1 = rednyellow;
        lights2 = yellow;
        break;
    case rednyellow:
        lights1 = green;
        lights2 = red;
        break;
    case green:
        lights1 = yellow;
        lights2 = red;
        break;
    case yellow:
        lights1 = red;
        lights2 = rednyellow;
        break;
    }
}

int main ()
{
	mygame.begin();
    lights1 = red;
    lights2 = yellow;
    //LightTimer.attach(&change, 2.0);
	while (mygame.isRunning())
	{
		if (mygame.update())
		{
			if (mygame.buttons.released(BTN_A)) change();
			mygame.display.println("Traffic Lights demo");
			switch (lights1) {
            case red:
                  mygame.display.println("red");
                  red1 = 1 ; yellow1 = 0; green1 = 0 ;
                  red2 = 0 ; yellow2 = 0; green2 = 1 ;
                  break;
            case rednyellow:
                  mygame.display.println("rednyellow");
                  red1 = 1 ; yellow1 = 1; green1 = 0 ;
                  red2 = 0 ; yellow2 = 1; green2 = 0 ;
                  break;
            case green:
                  mygame.display.println("green");
                  red1 = 0 ; yellow1 = 0; green1 = 1 ;
                  red2 = 1 ; yellow2 = 0; green2 = 0 ;
                  break;
            case yellow:
                  mygame.display.println("yellow");
                  red1 = 0 ; yellow1 = 1; green1 = 0 ;
                  red2 = 1 ; yellow2 = 1; green2 = 0 ;
                  break;
			}
		}
	}
}
