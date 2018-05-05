#include <Arduino.h>
#include "PokittoCore.h"

Pokitto::Core _dc;

/** for Arduino compatibility **/
void delay(unsigned long t) {
    _dc.wait(t);
}
