/**************************************************************************/
/*!
    @file     Pokitto_extport.h
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

#ifndef POKITTO_EXTPORT_H
#define POKITTO_EXTPORT_H

#include "mbed.h"

using namespace mbed; //interruptin

#if POK_BOARDREV == 1
 /** 2-layer board rev 1.3 **/
 #define EXT0_PORT   1
 #define EXT0_PIN    19
 #define EXT1_PORT   0
 #define EXT1_PIN    11
 #define EXT2_PORT   0
 #define EXT2_PIN    12
 #define EXT3_PORT   0
 #define EXT3_PIN    13
 #define EXT4_PORT   0
 #define EXT4_PIN    14
 #define EXT5_PORT   0
 #define EXT5_PIN    17
 #define EXT6_PORT   0
 #define EXT6_PIN    18
 #define EXT7_PORT   0
 #define EXT7_PIN    19
 #define EXT8_PORT   1
 #define EXT8_PIN    20
 #define EXT9_PORT   1
 #define EXT9_PIN    21
 #define EXT10_PORT   1
 #define EXT10_PIN    22
 #define EXT11_PORT   1
 #define EXT11_PIN    23
 #define EXT12_PORT   1
 #define EXT12_PIN    5
 #define EXT13_PORT   1
 #define EXT13_PIN    6
 #define EXT14_PORT   1
 #define EXT14_PIN    8
 #define EXT15_PORT   0
 #define EXT15_PIN    19
 #define EXT16_PORT   0
 #define EXT16_PIN    13
 #define EXT17_PORT   0
 #define EXT17_PIN    16
#else
 /** 4-layer board rev 2.1 **/
 #define EXT0_PORT   1
 #define EXT0_PIN    19
 #define EXT1_PORT   0
 #define EXT1_PIN    11
 #define EXT2_PORT   0
 #define EXT2_PIN    12
 #define EXT3_PORT   0
 #define EXT3_PIN    13
 #define EXT4_PORT   0
 #define EXT4_PIN    14
 #define EXT5_PORT   0
 #define EXT5_PIN    17
 #define EXT6_PORT   0
 #define EXT6_PIN    18
 #define EXT7_PORT   0
 #define EXT7_PIN    19
 #define EXT8_PORT   1
 #define EXT8_PIN    20
 #define EXT9_PORT   1
 #define EXT9_PIN    21
 #define EXT10_PORT   1
 #define EXT10_PIN    22
 #define EXT11_PORT   1
 #define EXT11_PIN    23
 #define EXT12_PORT   1
 #define EXT12_PIN    5
 #define EXT13_PORT   1
 #define EXT13_PIN    6
 #define EXT14_PORT   1
 #define EXT14_PIN    8
 #define EXT15_PORT   0
 #define EXT15_PIN    19
 #define EXT16_PORT   0
 #define EXT16_PIN    13
 #define EXT17_PORT   0
 #define EXT17_PIN    16

 #define EXT0 	P1_19
 #define EXT1	P0_11
 #define EXT2	P0_12
 #define EXT3	P0_13
 #define EXT4	P0_14
 #define EXT5	P0_17
 #define EXT6	P0_18
 #define EXT7	P0_19
 #define EXT8	P1_20
 #define EXT9	P1_21
 #define EXT10	P1_22
 #define EXT11	P1_23
 #define EXT12	P1_5
 #define EXT13	P1_6
 #define EXT14	P1_8
 #define EXT15	P1_26
 #define EXT16	P1_27
 #define EXT17	P0_16

#endif // POK_BOARDREV

#define CLR_EXT0 LPC_GPIO_PORT->CLR[EXT0_PORT] = 1 << EXT0_PIN;
#define SET_EXT0 LPC_GPIO_PORT->SET[EXT0_PORT] = 1 << EXT0_PIN;
#define CLR_EXT1 LPC_GPIO_PORT->CLR[EXT1_PORT] = 1 << EXT1_PIN;
#define SET_EXT1 LPC_GPIO_PORT->SET[EXT1_PORT] = 1 << EXT1_PIN;
#define CLR_EXT2 LPC_GPIO_PORT->CLR[EXT2_PORT] = 1 << EXT2_PIN;
#define SET_EXT2 LPC_GPIO_PORT->SET[EXT2_PORT] = 1 << EXT2_PIN;
#define CLR_EXT3 LPC_GPIO_PORT->CLR[EXT3_PORT] = 1 << EXT3_PIN;
#define SET_EXT3 LPC_GPIO_PORT->SET[EXT3_PORT] = 1 << EXT3_PIN;
#define CLR_EXT4 LPC_GPIO_PORT->CLR[EXT4_PORT] = 1 << EXT4_PIN;
#define SET_EXT4 LPC_GPIO_PORT->SET[EXT4_PORT] = 1 << EXT4_PIN;
#define CLR_EXT5 LPC_GPIO_PORT->CLR[EXT5_PORT] = 1 << EXT5_PIN;
#define SET_EXT5 LPC_GPIO_PORT->SET[EXT5_PORT] = 1 << EXT5_PIN;
#define CLR_EXT6 LPC_GPIO_PORT->CLR[EXT6_PORT] = 1 << EXT6_PIN;
#define SET_EXT6 LPC_GPIO_PORT->SET[EXT6_PORT] = 1 << EXT6_PIN;
#define CLR_EXT7 LPC_GPIO_PORT->CLR[EXT7_PORT] = 1 << EXT7_PIN;
#define SET_EXT7 LPC_GPIO_PORT->SET[EXT7_PORT] = 1 << EXT7_PIN;
#define CLR_EXT8 LPC_GPIO_PORT->CLR[EXT8_PORT] = 1 << EXT8_PIN;
#define SET_EXT8 LPC_GPIO_PORT->SET[EXT8_PORT] = 1 << EXT8_PIN;
#define CLR_EXT9 LPC_GPIO_PORT->CLR[EXT9_PORT] = 1 << EXT9_PIN;
#define SET_EXT9 LPC_GPIO_PORT->SET[EXT9_PORT] = 1 << EXT9_PIN;
#define CLR_EXT10 LPC_GPIO_PORT->CLR[EXT10_PORT] = 1 << EXT10_PIN;
#define SET_EXT10 LPC_GPIO_PORT->SET[EXT10_PORT] = 1 << EXT10_PIN;
#define CLR_EXT11 LPC_GPIO_PORT->CLR[EXT11_PORT] = 1 << EXT11_PIN;
#define SET_EXT11 LPC_GPIO_PORT->SET[EXT11_PORT] = 1 << EXT11_PIN;
#define CLR_EXT12 LPC_GPIO_PORT->CLR[EXT12_PORT] = 1 << EXT12_PIN;
#define SET_EXT12 LPC_GPIO_PORT->SET[EXT12_PORT] = 1 << EXT12_PIN;
#define CLR_EXT13 LPC_GPIO_PORT->CLR[EXT13_PORT] = 1 << EXT13_PIN;
#define SET_EXT13 LPC_GPIO_PORT->SET[EXT13_PORT] = 1 << EXT13_PIN;
#define CLR_EXT14 LPC_GPIO_PORT->CLR[EXT14_PORT] = 1 << EXT14_PIN;
#define SET_EXT14 LPC_GPIO_PORT->SET[EXT14_PORT] = 1 << EXT14_PIN;
#define CLR_EXT15 LPC_GPIO_PORT->CLR[EXT15_PORT] = 1 << EXT15_PIN;
#define SET_EXT15 LPC_GPIO_PORT->SET[EXT15_PORT] = 1 << EXT15_PIN;
#define CLR_EXT16 LPC_GPIO_PORT->CLR[EXT16_PORT] = 1 << EXT16_PIN;
#define SET_EXT16 LPC_GPIO_PORT->SET[EXT16_PORT] = 1 << EXT16_PIN;
#define CLR_EXT17 LPC_GPIO_PORT->CLR[EXT17_PORT] = 1 << EXT17_PIN;
#define SET_EXT17 LPC_GPIO_PORT->SET[EXT17_PORT] = 1 << EXT17_PIN;

#if POK_USE_EXT > 0
//#if POK_BOARDREV == 1
 /** 2-layer board rev 1.3 **/
//#else
 /** 4-layer board rev 2.1 **/
 //extern InterruptIn ext0;
 extern AnalogIn ext0;
 extern DigitalIn ext1;
 //DigitalOut ext0;
 //DigitalOut ext1;
 extern DigitalOut ext2;
 extern DigitalOut ext3;
 extern DigitalOut ext4;
 extern DigitalOut ext5;
 extern DigitalOut ext6;
 extern DigitalOut ext7;
 extern DigitalOut ext8;
 extern DigitalOut ext9;
 extern DigitalOut ext10;
 extern DigitalOut ext11;
 extern DigitalOut ext12;
 extern DigitalOut ext13;
 extern DigitalOut ext14;
 extern DigitalOut ext15;
 extern DigitalOut ext16;
 extern DigitalOut ext17;
//#endif // POK_BOARDREV
#endif // POK_USE_EXT

extern int turn;
#endif // POKITTO_EXTPORT_H

