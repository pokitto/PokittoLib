void movePlayer(char xoff, char yoff) {
  world[cursor_x][cursor_y] = EMPTY;
  cursor_x = cursor_x + xoff;
  cursor_y = cursor_y + yoff;
  world[cursor_x][cursor_y] = PLAYER;
}
void handlePlayer(char xoff, char yoff) {
  dir = 7 + xoff * 2 + yoff;
  timer = 5;
  byte target = world[cursor_x + xoff][cursor_y + yoff];
  target &= ~FALLINGSTATE;
  switch ( target ) {
    case EMPTY: movePlayer(xoff, yoff); break; //empty
    case WALL: gb.sound.playTick(); break; //wall
    case ROCK: moveRock(xoff, yoff); break; //ROCK
    case SOIL: movePlayer(xoff, yoff); break; //soil
    case DIAMOND: movePlayer(xoff, yoff); diamonds++; gb.sound.playOK(); break; //diamond
    case CHANGER: gb.sound.playTick(); break; //changer
    case DOOR:
      if (diams_need <= diamonds) {
        movePlayer(xoff, yoff);
        gb.sound.playOK();
        nextLevel();

      } else {
        gb.sound.playTick();
      }
      break;
    default: die(); //monsters or Player (wich shouldn't happen) :(
  }
}
void die() {
  byte x = cursor_x; 
  byte y = cursor_y;
  world[x][y] = DEATH;
  dead = true;
  lives--;
  
}

void updateCursor() {
  if (gb.buttons.repeat(BTN_A, 4) && !dead) {
    gb.sound.playOK();
    die();
  }

  if (gb.buttons.repeat(BTN_B, 4) && dead ) {
    gb.sound.playCancel();
    initWorld(curLevel);
    dead = false;
  }
  if (!dead) {
    if (gb.buttons.repeat(BTN_RIGHT, 4)) {
      handlePlayer(1, 0);
    }
    if (gb.buttons.repeat(BTN_LEFT, 4)) {
      handlePlayer(-1, 0);
    }
    if (gb.buttons.repeat(BTN_DOWN, 4)) {
      handlePlayer(0, 1);
    }
    if (gb.buttons.repeat(BTN_UP, 4)) {
      handlePlayer(0, -1);
    }
  }
  //target position of the camera for the cursor to be centered
  int camera_x_target = cursor_x * 6 - LCDWIDTH / 2 + 3;
  int camera_y_target = cursor_y * 6 - LCDHEIGHT / 2 + 3;
  //apply the target coordinate to the current coordinates with some smoothing
  camera_x = (camera_x * 3 + camera_x_target) / 4;
  camera_y = (camera_y * 3 + camera_y_target) / 4;

}

