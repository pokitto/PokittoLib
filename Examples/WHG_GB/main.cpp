#include "Pokitto.h"

int main()
{

    setup();

    while(Pokitto::Core::isRunning())
    {
        loop();
    }
}



