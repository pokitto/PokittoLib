/**************************************************************************/
/*!
    @file     Pokitto_extport.cpp
    @author   Jonne Valola

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#include "Pokitto.h"

using namespace mbed; //needed for interruptin

#if POK_USE_EXT > 0
//#if POK_BOARDREV == 1
 /** 2-layer board rev 1.3 **/
//#else
 /** 4-layer board rev 2.1 **/
 //InterruptIn ext0(P1_19);
 AnalogIn ext0(P1_19);
 DigitalIn ext1(P0_11, PullUp);
 //DigitalOut ext0(P1_19);
 //DigitalOut ext1(P0_11);
 DigitalOut ext2(P0_12);
 DigitalOut ext3(P0_13);
 DigitalOut ext4(P0_14);
 DigitalOut ext5(P0_17);
 DigitalOut ext6(P0_18);
 DigitalOut ext7(P0_19);
 DigitalOut ext8(P1_20);
 DigitalOut ext9(P1_21);
 DigitalOut ext10(P1_22);
 DigitalOut ext11(P1_23);
 DigitalOut ext12(P1_5);
 DigitalOut ext13(P1_6);
 DigitalOut ext14(P1_8);
 DigitalOut ext15(P1_26);
 DigitalOut ext16(P1_27);
 DigitalOut ext17(P0_16);
//#endif // POK_BOARDREV
#endif // POK_USE_EXT



void ext_write(uint32_t value) {
    #if POK_USE_EXT > 0
    if (value & 1) SET_EXT0 else CLR_EXT0;
    value >>= 1;
    if (value & 1) SET_EXT1 else CLR_EXT1;
    value >>= 1;
    if (value & 1) SET_EXT2 else CLR_EXT2;
    value >>= 1;
    if (value & 1) SET_EXT3 else CLR_EXT3;
    value >>= 1;
    if (value & 1) SET_EXT4 else CLR_EXT4;
    value >>= 1;
    if (value & 1) SET_EXT5 else CLR_EXT5;
    value >>= 1;
    if (value & 1) SET_EXT6 else CLR_EXT6;
    value >>= 1;
    if (value & 1) SET_EXT7 else CLR_EXT7;
    value >>= 1;
    if (value & 1) SET_EXT8 else CLR_EXT8;
    value >>= 1;
    if (value & 1) SET_EXT9 else CLR_EXT9;
    value >>= 1;
    if (value & 1) SET_EXT10 else CLR_EXT10;
    value >>= 1;
    if (value & 1) SET_EXT11 else CLR_EXT11;
    value >>= 1;
    if (value & 1) SET_EXT12 else CLR_EXT12;
    value >>= 1;
    if (value & 1) SET_EXT13 else CLR_EXT13;
    value >>= 1;
    if (value & 1) SET_EXT14 else CLR_EXT14;
    value >>= 1;
    if (value & 1) SET_EXT15 else CLR_EXT15;
    value >>= 1;
    if (value & 1) SET_EXT16 else CLR_EXT16;
    value >>= 1;
    if (value & 1) SET_EXT17 else CLR_EXT17;
    #endif
}

