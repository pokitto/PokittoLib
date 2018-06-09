#include "Pokitto.h"
//#include "Gamebuino.h"

Pokitto::Core g;

int main()
{

    //pokInitSD();
    /*if (!isThisFileOpen("WOLF.SND")) {
        fileClose(); // close any open files
        fileOpen("WOLF.SND",FILE_MODE_OVERWRITE | FILE_MODE_BINARY);
    }*/

    setup();

    while(g.isRunning())
    {
        loop();
    }
}



