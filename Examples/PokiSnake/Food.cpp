//
// PokiSnake : A Pokitto port of the Arduboy game "ArduSnake" I made for the french magazine "Programmez!"
//
// Copyright (C) 2017 Jerome Perrot (Initgraph)
//
// Version : 0.1
//
// Licence :
//
//   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// For any remarks or questions you can contact me on Twitter (@InitgraphDotCom) or by email (contact@initgraph.com).
// You can also visit my website : http://www.initgraph.com
//
// Enjoy !
//

#include "Constants.h"
#include "Food.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
Food::Food() { }

uint8_t Food::x;
uint8_t Food::y;
Pokitto::Core* Food::game;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Init snake food
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Food::init(Pokitto::Core *pokittoCore) {

  game = pokittoCore;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Generate random food coordinates
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Food::generate() {

  x = random(1, 25) * SNAKE_PART_SIZE;
  y = random(2, 20) * SNAKE_PART_SIZE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Draw the food
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Food::draw() {

  game->display.drawRect(x, y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
}
