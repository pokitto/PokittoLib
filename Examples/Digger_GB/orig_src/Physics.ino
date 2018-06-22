void updatePhysics() {
  //Skip borders in Check
  if (gb.frameCount % 5 == 0) {
    for (byte y = WORLD_H - 2; y > 0; y--) {
      for (byte x = 1; x < WORLD_W - 1; x++) {
        byte sprite = world[x][y] & ~FALLINGSTATE & ~TYPEMASK;
        switch (sprite) {
          case ROCK: falling(x, y, ROCK); break; //ROCK
          case DIAMOND: falling(x, y, DIAMOND); break; //diam
          case MONSTER_D: moveMonster(x, y, MONSTER_D); break; //mon_down
          case MONSTER_U: moveMonster(x, y, MONSTER_U); break; //mon_up
          case MONSTER_L: moveMonster(x, y, MONSTER_L); break;
          case MONSTER_R: moveMonster(x, y, MONSTER_R); break;

          default: break;
        }
      }
    }
    for (byte y = WORLD_H - 2; y > 0; y--) {
      for (byte x = 1; x < WORLD_W - 1; x++) {
        world[x][y] &= ~MOVINGSTATE;
      }
    }
  }
}



void falling(byte x, byte y, byte sprite) {
  if ( (world[x][y] & FALLINGSTATE) > 0) {
    switch (world[x][y + 1] & SPRITEMASK) {
      case PLAYER: die(); break;
      case MONSTER_D: monster_die(x, y + 1);return; break;
      case MONSTER_U: monster_die(x, y + 1);return; break;
      case MONSTER_L: monster_die(x, y + 1);return; break;
      case MONSTER_R: monster_die(x, y + 1);return; break;
      default: break;
    }
  }

  world[x][y] &= ~FALLINGSTATE;
  if (world[x][y + 1] == EMPTY) {
    world[x][y + 1] = sprite;
    world[x][y] = EMPTY;
    world[x][y + 1] |= FALLINGSTATE;
    return;
  }
  if (world[x][y + 1] == ROCK || world[x][y + 1] == DIAMOND) {
    if (world[x + 1][y + 1] == EMPTY && world[x + 1][y] == EMPTY) {
      world[x + 1][y + 1] = sprite;
      world[x][y] = EMPTY;
    } else if (world[x - 1][y + 1] == EMPTY && world[x - 1][y] == EMPTY) {
      world[x - 1][y + 1] = sprite;
      world[x][y] = EMPTY;
    }
    return;
  }
  if ((sprite == ROCK) && (world[x][y + 1] == CHANGER) && (world[x][y + 2] == EMPTY)) {
    world[x][y] = EMPTY;
    world[x][y + 2] = DIAMOND | FALLINGSTATE;
  }

}
void moveRock(char xoff, char yoff) {
  if (yoff != 0) {
    gb.sound.playTick();
    return;
  }
  if (gb.buttons.held(BTN_RIGHT, 8) && world[cursor_x + 2][cursor_y] == EMPTY ) {
    world[cursor_x + 2][cursor_y] = ROCK;
    movePlayer(1, 0);
    gb.sound.playCancel();
  }
  if (gb.buttons.held(BTN_LEFT, 8) && world[cursor_x - 2][cursor_y] == EMPTY ) {
    world[cursor_x - 2][cursor_y] = ROCK;
    movePlayer(-1, 0);
    gb.sound.playCancel();
  }
}
