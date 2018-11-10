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

#ifndef SNAKE_H
#define SNAKE_H

#define MAXSNAKES 5

#include "Constants.h"
#include "Food.h"
#include "Pokitto.h"
#include "JoyHat.h"

extern bool border;
extern uint8_t numplayers;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Snake part class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class SnakePart {

  public:

    int16_t x;
    int16_t y;

    SnakePart();
    SnakePart(uint8_t positionX, uint8_t positionY);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Snake class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class Snake {

  public:

    int16_t x;
    int16_t y;
    int8_t direction;
    Food food;
    uint8_t foodEaten;
    bool isDead;
    uint8_t speed;
    bool canMakeSound = true;

    Snake();
    void init(Pokitto::Core *pokittoCore, uint8_t gameState);
    void move();
    void moveOnMainMenu();
    void draw();
    bool checkCollisionWithOther(Snake* s);

  private:

    uint8_t partsCount;
    uint8_t partsStart;
    SnakePart parts[MAX_SNAKE_PARTS];
    Pokitto::Core *game;

    bool checkCollisionWithFood();
    bool checkCollisionWithBorder();
    bool checkCollisionWithItself();
};

#endif
