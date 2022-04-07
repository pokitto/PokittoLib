#include "JoyHat.h"

DigitalOut* rumblepin;

mbed::DigitalOut _rumble(EXT0);
mbed::DigitalIn _b1(EXT15);
mbed::DigitalIn _b2(EXT14);
mbed::DigitalIn _b3(EXT13);

mbed::AnalogIn _joyx(_X_PIN);
mbed::AnalogIn _joyy(_Y_PIN);

mbed::Timeout _rTimer;

JoyHat joyhat;

void stoprumble() {
    rumblepin->write(0);
}
