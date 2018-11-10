#include "Pokitto.h"

Pokitto::Core mygame;

int main ()
{
	mygame.begin();

	while (mygame.isRunning())
	{
		if (mygame.update())
		{
			mygame.display.adjustLineStep = 5;
			mygame.display.println("Hello World!");
			/* Below is an example of printf, the ability print formatted strings */
			/* Remove the two slashes '//' to uncomment it and see it in action */
			/* Add the // back in the beginning of the line to comment it out again */
			mygame.display.printf("\nA random\nnumber:%d",random(100));
		}
	}
}
