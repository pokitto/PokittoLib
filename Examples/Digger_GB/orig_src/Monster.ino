void monster_die(byte x, byte y) {
  if (   world[x - 1][y - 1] == PLAYER
         || world[x - 1][y] == PLAYER
         || world[x - 1][y + 1] == PLAYER
         || world[x][y - 1] == PLAYER
         || world[x][y + 1] == PLAYER
         || world[x + 1][y - 1] == PLAYER
         || world[x + 1][y] == PLAYER
         || world[x + 1][y + 1] == PLAYER
     ) die();
  sIND(x - 1, y + 1, EMPTY);
  sIND(x  , y + 1, EMPTY);
  sIND(x + 1, y + 1, EMPTY);
  if (gamemode == EASY) {
    world[x][y] = DIAMOND;
    sIND(x - 1, y - 1, DIAMOND);
    sIND(x - 1, y  , DIAMOND);
    sIND(x  , y - 1, DIAMOND);
    sIND(x + 1, y - 1, DIAMOND);
    sIND(x + 1, y  , DIAMOND);
  } else {
    world[x][y] = EMPTY;
    sIND(x - 1, y - 1, EMPTY);
    sIND(x - 1, y  , EMPTY);
    sIND(x  , y - 1, EMPTY);
    sIND(x + 1, y - 1, EMPTY);
    sIND(x + 1, y  , EMPTY);
  }

}

void moveMonster(byte x, byte y, byte sprite) {
  if (gb.frameCount % 10 == 0) {
    if (world[x - 1][y] > EMPTY && world[x + 1][y] > EMPTY && world[x][y - 1] > EMPTY && world[x][y + 1] > EMPTY) return; //no way out

    //straight forward preferred, if not: search new direction
    if (sprite == MONSTER_D) {
      switch (world[x][y + 1] & SPRITEMASK) {
        case EMPTY: world[x][y + 1] = world[x][y] | MOVINGSTATE; world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_U)) {
      switch (world[x][y - 1] & SPRITEMASK ) {
        case EMPTY: world[x][y - 1] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_L)) {
      switch (world[x - 1][y] & SPRITEMASK ) {
        case EMPTY: world[x - 1][y] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }
    if ((sprite == MONSTER_R)) {
      switch (world[x + 1][y] & SPRITEMASK ) {
        case EMPTY: world[x + 1][y] = (world[x][y] | MOVINGSTATE); world[x][y] = EMPTY ; break;
        case PLAYER: die(); break;
        default: world[x][y] = getNewDirection (x, y); break;
      }
    }

  }
}
void sIND(byte x, byte y, byte sprite) { //setIfNotWorldBorder
  if (x > 0 && x < WORLD_W - 1 && y > 0 && y < WORLD_H - 1)   world[x][y] = sprite;
}

byte getNewDirection(byte x, byte y) {
  byte type = world[x][y] & TYPEMASK;
  byte sprite = world[x][y] & SPRITEMASK;
  switch (type) {
    case TYPE_UD:
      sprite = (sprite == MONSTER_D) ? MONSTER_U : MONSTER_D;
      break;
    case TYPE_LR:
      sprite = (sprite == MONSTER_R) ? MONSTER_L : MONSTER_R;
      break;
    case TYPE_RT:
      while (!testMove(x, y, sprite)) {
        sprite++;
        if (sprite > MONSTER_R) sprite = MONSTER_D;
      }
      break;
    case TYPE_LT:
      while (!testMove(x, y, sprite)) {
        sprite--;
        if (sprite < MONSTER_D) sprite = MONSTER_R;
      }
      break;
  }
  return sprite + type;
}

bool testMove(byte x, byte y, byte monster) {
  switch (monster) {
    case MONSTER_U:
      if (world[x][y - 1] == PLAYER) die();
      return (world[x][y - 1] == EMPTY) ? true : false; break;

    case MONSTER_L:
      if (world[x - 1][y] == PLAYER) die();
      return (world[x - 1][y] == EMPTY) ? true : false; break;
    case MONSTER_D:
      if (world[x][y + 1] == PLAYER) die();
      return (world[x][y + 1] == EMPTY) ? true : false; break;
    case MONSTER_R:
      if (world[x + 1][y] == PLAYER) die();
      return (world[x + 1][y] == EMPTY) ? true : false; break;
    default:
      return true; break;
  }
}
