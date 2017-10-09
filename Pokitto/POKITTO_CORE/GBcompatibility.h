#ifndef GBCOMPATIBILITY_H
#define GBCOMPATIBILITY_H

/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */



#ifndef SETTINGS_C
#define	SETTINGS_C

#include "binary.h"

#define _BV(bit) (1 << (bit)) //jonne

#define NOROT 0
#define ROTCCW 1
#define ROT180 2
#define ROTCW 3

//colors
#define WHITE 0
#define BLACK 1
#define GRAY 2
#define INVERT 255

//for extended bitmap function :
#define NOROT 0
#define ROTCCW 1
#define ROT180 2
#define ROTCW 3
#define NOFLIP 0
#define FLIPH 1
#define FLIPV 2
#define FLIPVH 3




//SETTINGS YOU CAN EDIT

#define NUM_CHANNELS 1 //number of sound channels, between 0 and 4
#define DISPLAY_ROT NOROT //set to NOROT, ROTCCW, ROT180 or ROTCW. Can be used to play in portrait mode.
#define ENABLE_GUI 1 //enable menu, keyboard, pop-up, volume adjust functions
#define ENABLE_BITMAPS 1 //will replace bitmaps with rectangles if disabled
#define ENABLE_GRAYSCALE 1 //allows the use of the GRAY color
#define EXTENDED_NOTE_RANGE 0 //allows the use of notes above A 5... please avoid that they sound really bad

//not really useful
#define ENABLE_BATTERY 1 //disable battery monitoring
#define ENABLE_BACKLIGHT 1 //disable automatic back-light

//IT'S STRONGLY ADVISED TO LEAVE THE FOLLOWING SETTINGS ALONE

//defaults values of settings which can be adjusted on each Gamebuino using settings.hex
#define SCR_CONTRAST 60
#define START_MENU_TIMER 255 //40 = 40 frames (2sec) before start menu is skipped, 0 = no start menu, 255 = start menu until you press A

//addresses of settings stored in the program memory
#define SETTINGS_PAGE ((const char *)(0x7800-128))
#define SETTINGS_TOKEN 			0xC001
#define OFFSET_CURRENTGAME		2
#define OFFSET_USERNAME			11
#define USERNAME_LENGTH			10
#define OFFSET_CONTRAST			22
#define OFFSET_BACKLIGHT_MIN	23
#define OFFSET_BACKLIGHT_MAX	24
#define OFFSET_LIGHT_MIN		25
#define OFFSET_LIGHT_MAX		27
#define OFFSET_VOLUME_MAX		29
#define OFFSET_VOLUME_DEFAULT	30
#define OFFSET_START_MENU_TIMER	31
#define OFFSET_BATTERY_CRITIC	32
#define OFFSET_BATTERY_LOW		34
#define OFFSET_BATTERY_MED		36
#define OFFSET_BATTERY_FULL		38

//GUI
#define KEYBOARD_W 16
#define KEYBOARD_H 8

//sound
#define VOLUME_CHANNEL_MAX 255/NUM_CHANNELS/7/9 //7=instrument volume 9=note volume

//battery voltage monitor
#define BAT_PIN 6 //was A6
#define NUM_LVL 4
#define BAT_LVL_CRITIC  3500
#define BAT_LVL_LOW		3550
#define BAT_LVL_MED		3700
#define BAT_LVL_FULL	3900

//SD card
#define SD_CS 10

//screens back light
#define BACKLIGHT_PIN 5
//auto back-light levels
#define BACKLIGHT_MIN 0
#define BACKLIGHT_MAX 255

//ambient light sensor
#define AMBIENTLIGHT_PIN A7
//auto back-light levels
#define AMBIENTLIGHT_MIN 800 //800
#define AMBIENTLIGHT_MAX 980 //1000
#define AMBIENTLIGHT_SMOOTHING 16

//number of buttons
#define NUM_BTN         7
//buttons ID
#if DISPLAY_ROT == 0
	#define BTN_UP      1
	#define BTN_RIGHT   2
	#define BTN_DOWN    3
	#define BTN_LEFT    0
#elif DISPLAY_ROT == ROTCCW
	#define BTN_UP      2
	#define BTN_RIGHT   3
	#define BTN_DOWN    0
	#define BTN_LEFT    1
#elif DISPLAY_ROT == ROT180
	#define BTN_UP      3
	#define BTN_RIGHT   0
	#define BTN_DOWN    1
	#define BTN_LEFT    2
#elif DISPLAY_ROT == ROTCW
	#define BTN_UP      0
	#define BTN_RIGHT   1
	#define BTN_DOWN    2
	#define BTN_LEFT    3
#endif
#define BTN_A           4
#define BTN_B           5
#define BTN_C           6

#define BTN_LEFT_PIN 0
#define BTN_UP_PIN 1
#define BTN_RIGHT_PIN 2
#define BTN_DOWN_PIN 3
#define BTN_A_PIN 4
#define BTN_B_PIN 5
#define BTN_C_PIN 6



#endif /*  */

const uint16_t startupSound[] = {0x0005,0x3089,0x208,0x238,0x7849,0x1468,0x0000};

const uint8_t gamebuinoLogo[]  =
{
	84,10, //width and height
	B00000011, B11100001, B10000001, B10000110, B01111111, B00111110, B00011000, B01101101, B10000011, B00001111, B00001111,
	B00001110, B00000001, B10000011, B10000110, B01100000, B00110011, B00011000, B01101101, B11000011, B00011001, B10001111,
	B00011000, B00000011, B11000011, B10001110, B01100000, B00110011, B00011000, B01101101, B11100011, B00110000, B11001111,
	B00011000, B00000011, B11000011, B10011110, B01100000, B00110110, B00110000, B11001101, B11100011, B01100000, B11001111,
	B00110000, B00000110, B11000111, B10011110, B01111110, B00111110, B00110000, B11001101, B10110011, B01100000, B11001111,
	B00110000, B00001100, B11000110, B11110110, B01100000, B00110011, B00110000, B11011001, B10110110, B01100000, B11001111,
	B00110011, B11001111, B11001100, B11110110, B01100000, B01100001, B10110000, B11011011, B00011110, B01100000, B11001111,
	B00110000, B11011000, B01101100, B11100110, B11000000, B01100001, B10110000, B11011011, B00011110, B01100001, B10001111,
	B00011001, B10011000, B01101100, B11000110, B11000000, B01100011, B10110001, B10011011, B00001110, B00110011, B00001111,
	B00001111, B10110000, B01111000, B11000110, B11111111, B01111110, B00011111, B00011011, B00000110, B00011110, B00001111,
};

#endif // GBCOMPATIBILITY_H

