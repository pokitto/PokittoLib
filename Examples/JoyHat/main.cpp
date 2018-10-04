#include "Pokitto.h"

Pokitto::Core mygame;
Pokitto::Display d;

mbed::DigitalOut jh_rumble(EXT0);
mbed::DigitalIn jh_b1(EXT15);
mbed::DigitalIn jh_b2(EXT14);
mbed::AnalogIn jh_x(EXT1);
mbed::AnalogIn jh_y(EXT2);

int main ()
{
	mygame.begin();

	while (mygame.isRunning())
	{
		if (mygame.update())
		{
			jh_rumble.write(mygame.buttons.aBtn());
			d.color = 1;
			d.printf("Joy X: %d\n",(int)(jh_x.read()*100));
			d.color++;
			d.printf("Joy Y: %d\n",(int)(jh_y.read()*100));
			d.color++;
			d.printf("B1: %d\n",jh_b1.read());
			d.color++;
			d.printf("B2: %d\n",jh_b2.read());
			d.color++;
            d.printf("A to Rumble!\n");
		}
	}
}
