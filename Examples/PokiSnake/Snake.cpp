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

#include "Snake.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Snake part class
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t initedsnakes=0;
bool border=false;
uint8_t debounce=55;

SnakePart::SnakePart() { }

SnakePart::SnakePart(uint8_t positionX, uint8_t positionY) {

  x = positionX;
  y = positionY;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Snake class
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
Snake::Snake() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Init snake
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::init(Pokitto::Core *pokittoCore, uint8_t gameState) {

  direction = random(0,4);//SNAKEDIR_RIGHT;

  foodEaten = 0;

  isDead = false;

  game = pokittoCore;

  // Init the snake for the game screen
  if (gameState == GAMESTATE_GAME) {

    x = 16+4*random(20);//4+initedsnakes*8;//(random(60)+20)&0xF8;//72;
    y = 16+4*random(15);//16*(initedsnakes+1);//(random(50)+15)&0xF0;//32;
    //x = (random(60)+20)&0xF8;//72;
    //y = (random(50)+15)&0xF0;//32;
    //if (initedsnakes==0) direction = SNAKEDIR_RIGHT;
    //else direction = SNAKEDIR_LEFT;
    initedsnakes++;

    if (initedsnakes==numplayers) initedsnakes=0;
    partsCount = 5;
    partsStart = 0;

    for (int i=0;i<5;i++) {
    parts[i] = SnakePart(x-(4-i)*SNAKE_PART_SIZE, y);
    }

    speed = 1;
    game->setFrameRate(speed);

    food.init(game);

    food.generate();
  }

  // Init the snake for the main menu
  else {

    x = 96;
    y = 0;

    partsCount = 25;
    partsStart = 0;

    for (uint8_t i=0; i<partsCount; i++) {

      parts[i] = SnakePart(i * SNAKE_PART_SIZE, 0);
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Move the snake on the game screen
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::move() {

  if (isDead) return;
  // Update snake move depending of its orientation
  if (direction == SNAKEDIR_UP) {

    y -= SNAKE_PART_SIZE;
  }
  else if (direction == SNAKEDIR_DOWN) {

    y += SNAKE_PART_SIZE;
  }
  else if (direction == SNAKEDIR_LEFT) {

    x -= SNAKE_PART_SIZE;
  }
  else {

    x += SNAKE_PART_SIZE;
  }

  if (checkCollisionWithBorder()) {
    if (border) isDead = true;
    else {
            if (x < 0) x=104;
            else if (x > 104) x=0;
            else if (y < 8) y=84;
            else y=8;
    }
  }
  else if (checkCollisionWithItself()) {

    isDead = true;

    // Make a game over sound
    /*if (canMakeSound) {

      arduboy->tunes.tone(1100, 80);
      delay(100);
      arduboy->tunes.tone(1000, 80);
      delay(100);
      arduboy->tunes.tone(500, 500);
    }*/
  }

  if (checkCollisionWithFood()) {

    // Grow the snake
    parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
    partsCount++;

    foodEaten++;

    // Rumble
    joyhat.Rumble(0.5f);

     // Update snake speed
    if (foodEaten % 3 == 0 && speed < 13) {

      speed++;
      game->setFrameRate(speed);
    }

    food.generate();

    // Make a victory sound
    /*if (canMakeSound) {

      arduboy->tunes.tone(500, 50);
      delay(80);
      arduboy->tunes.tone(800, 50);
      delay(80);
      arduboy->tunes.tone(1100, 100);
    }*/
  }
  else {

    // Move the snake
    parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
    partsStart = (partsStart + 1) % MAX_SNAKE_PARTS;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Move the snake on the main menu
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::moveOnMainMenu() {

  uint8_t randomDir = 0;

  // Update snake move depending of its orientation
  if (x == 104 && y == 0) {

    if (direction == SNAKEDIR_RIGHT) {

      direction = SNAKEDIR_DOWN;
      y += SNAKE_PART_SIZE;
    }
    else {

      direction = SNAKEDIR_LEFT;
       x -= SNAKE_PART_SIZE;
    }
  }
  else if (x == 104 && y == 32) {

    if (direction == SNAKEDIR_RIGHT) {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
    }
    else if (direction == SNAKEDIR_DOWN) {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_LEFT;
        x -= SNAKE_PART_SIZE;
      }
    }
    else {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_LEFT;
        x -= SNAKE_PART_SIZE;
      }
    }
  }
  else if (x == 104 && y == 84) {

    if (direction == SNAKEDIR_DOWN) {

      direction = SNAKEDIR_LEFT;
      x -= SNAKE_PART_SIZE;
    }
    else {

      direction = SNAKEDIR_UP;
      y -= SNAKE_PART_SIZE;
    }
  }
  else if (x == 0 && y == 84) {

    if (direction == SNAKEDIR_LEFT) {

      direction = SNAKEDIR_UP;
      y -= SNAKE_PART_SIZE;
    }
    else {

      direction = SNAKEDIR_RIGHT;
      x += SNAKE_PART_SIZE;
    }
  }
  else if (x == 0 && y == 32) {

    if (direction == SNAKEDIR_LEFT) {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
    }
    else if (direction == SNAKEDIR_DOWN) {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_RIGHT;
        x += SNAKE_PART_SIZE;
      }
    }
    else {

      randomDir = random(0, 2);

      if (randomDir) {

        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
      else {

        direction = SNAKEDIR_RIGHT;
        x += SNAKE_PART_SIZE;
      }
    }
  }
  else if (x == 0 && y == 0) {

    if (direction == SNAKEDIR_LEFT) {

      direction = SNAKEDIR_DOWN;
      y += SNAKE_PART_SIZE;
    }
    else {

      direction = SNAKEDIR_RIGHT;
      x += SNAKE_PART_SIZE;
    }
  }
  else {

    if (direction == SNAKEDIR_UP) {

      y -= SNAKE_PART_SIZE;
    }
    else if (direction == SNAKEDIR_DOWN) {

      y += SNAKE_PART_SIZE;
    }
    else if (direction == SNAKEDIR_LEFT) {

      x -= SNAKE_PART_SIZE;
    }
    else {

      x += SNAKE_PART_SIZE;
    }
  }

  // Move the snake
  parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
  partsStart = (partsStart + 1) % MAX_SNAKE_PARTS;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Display the snake
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::draw() {

  uint8_t currentPart;

  if (isDead) return;
  // For each snake part
  for (uint8_t i=0; i<partsCount; i++) {

    currentPart = (partsStart + i) % MAX_SNAKE_PARTS;

    // If the part is the snake head
    if (i == (partsCount - 1)) {

      if (direction == SNAKEDIR_UP) {

        game->display.drawRect(parts[currentPart].x, parts[currentPart].y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
      }
      else if (direction == SNAKEDIR_DOWN) {

        game->display.drawRect(parts[currentPart].x, parts[currentPart].y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
      }
      else if (direction == SNAKEDIR_LEFT) {

        game->display.drawRect(parts[currentPart].x, parts[currentPart].y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
      }
      else {

        game->display.drawRect(parts[currentPart].x, parts[currentPart].y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
      }
    }

    // If the part is a snake body part
    else {

      game->display.drawRect(parts[currentPart].x, parts[currentPart].y, SNAKE_PART_SIZE-1, SNAKE_PART_SIZE-1);
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Check snake collision with food
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithFood() {

  if (x == food.x && y == food.y) return true;
  else return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Check snake collision with the border
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithBorder() {

  if (border) {
    if (x == 0 || x == 104 || y == 0 || y == 84) return true;
  } else {
    if (x < 0 || x > 104 || y <= 8 || y >= 84) return true;
  }
  return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Check snake collision with itself
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithItself() {

  uint8_t currentPart;

  // For each snake part
  for (uint8_t i=0; i<partsCount; i++) {

    currentPart = (partsStart + i) % MAX_SNAKE_PARTS;

    if (x == parts[currentPart].x && y == parts[currentPart].y) {

      return true;
    }
  }

  return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Check snake collision with other snakes
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithOther(Snake* othersnake) {

  uint8_t currentPart;

  // For each snake part of other snake
  for (uint8_t i=0; i<partsCount; i++) {

    currentPart = (partsStart + i) % MAX_SNAKE_PARTS;

    if (x == othersnake->parts[currentPart].x && y == othersnake->parts[currentPart].y) {

      return true;
    }
  }

  return false;
}
