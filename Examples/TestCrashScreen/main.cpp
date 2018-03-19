// *** Make a stack overflow error to test CheckStack() function and Pokitto Crash Screen.

#include "Pokitto.h"

Pokitto::Core mygame;
extern "C" void CheckStack();

// Do recursion until stack is going to overflow
void RecursiveFunc (int16_t recursionLevel)
{
    // Reserve some stack in each recursion.
    char test[256];

    // Check for the stack to be near overflow. If the stack is too big we show the Pokitto Crash Screen and never return
    // from this function.
    CheckStack();

    // Make a recursive call
    if (recursionLevel>0) {
        RecursiveFunc( recursionLevel - 1 );
    } else {
        // Print the stack pointer address of the last recursion.
        mygame.display.println("Current stack pointer: ");
        mygame.display.print("0x"); mygame.display.println((uint32_t)&test, 16);
    }
}

// Main
int main () {
    int16_t maxRecursionLevel = 1;
    mygame.begin();

    while (mygame.isRunning()) {
        if (mygame.update()) {

            #ifndef POK_SIM
            mygame.display.print("Recursion level: ");
            mygame.display.println(maxRecursionLevel);
            RecursiveFunc( maxRecursionLevel );
            maxRecursionLevel++;
            #endif
        }
    }
}

