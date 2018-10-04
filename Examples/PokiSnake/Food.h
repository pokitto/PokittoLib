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

#ifndef FOOD_H
#define FOOD_H

#include "pokitto.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Food class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class Food {

  public:

    static uint8_t x;
    static uint8_t y;

    Food();
    static void generate();
    static void init(Pokitto::Core *pokittoCore);
    static void draw();

  private:

    static Pokitto::Core *game;
};

#endif
