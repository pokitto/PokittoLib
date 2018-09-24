/** @file ppp-blinky.cpp */
#include "Pokitto.h"
#include "ppp-blinky.h"

Pokitto::Core app;
Pokitto::Display d;

// PPP-Blinky - "The Most Basic Internet Thing"

int main()
{
    app.begin();
    d.persistence=1;
    d.println("Initializing PPP..."); 
    d.update();
    initializePpp(); // initialize the serial port(s) and structures
    while(app.isRunning()) {
        d.println("Waiting for connect");
        d.update();
        waitForPcConnectString(); // wait for PC to send a connect message
        d.println("Connected"); 
        d.update();
        while( connectedPpp() ) {
            waitForPppFrame(); // process PPP frames until we receive a disconnect command
        }
        d.println("Disconnected. Bye!");
        d.update();
    } 
    return 0;
}