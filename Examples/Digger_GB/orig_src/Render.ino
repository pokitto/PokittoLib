
void drawWorld() {
  for (byte y = 0; y < WORLD_H; y++) {
    for (byte x = 0; x < WORLD_W; x++) {
      //coordinates on the screen depending on the camera position
      int x_screen = x * SPRITE_W - camera_x;
      int y_screen = y * SPRITE_H - camera_y;
      byte sprite = world[x][y];
      sprite &= SPRITEMASK;
      if (sprite == DIAMOND && gb.frameCount % 2) {
        sprite = DIAMOND2;
      }
      if (sprite == PLAYER) sprite = PLAYER + dir;
      if (x_screen < -SPRITE_W || x_screen > LCDWIDTH || y_screen < 0 || y_screen > LCDHEIGHT) {
        continue; // don't draw sprites which are out of the screen
      }
      gb.display.drawBitmap(x_screen, y_screen, sprites[sprite], 0, 0);
    }
  }
  if (timer == 0) dir = 7; timer--;
  gb.display.setColor(gamemode ? BLACK : WHITE);
  gb.display.fillRect(0, 0, LCDWIDTH, SPRITE_H);
  gb.display.setColor(gamemode ? WHITE : BLACK);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print(F("\04:"));
  gb.display.print(diamonds);
  gb.display.print(F("/"));
  gb.display.print(diams_need);
  gb.display.print(F(" \03:"));
  gb.display.print(lives);
  gb.display.print(F(" L:"));
  gb.display.print(curLevel + 1);
}

