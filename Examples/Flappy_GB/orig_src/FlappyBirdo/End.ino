void gameOver() { //= DEATH. When the player hits a pipe or the ground.
  if (gameover_y > floor(LCDHEIGHT / 3)) gameover_y -= 4; //update Y position.
  //update ending timer.
  if (gameover_timer < LCDHEIGHT / 2) {
    gameover_timer += 2;
  } else initVariables(); //simple death = simple reset.

  //DRAW game over logo.
  gb.display.setColor(WHITE);
  gb.display.drawBitmap(GAMEOVERX, gameover_y, gameoverMaskBitmap); //white mask placed under the outline bitmap.
  gb.display.setColor(BLACK);
  gb.display.drawBitmap(GAMEOVERX, gameover_y, gameoverBitmap); //outline
}

void gameWin() { //==WIN
  if (score_maxreached <= 1)  playsoundfx(3, 0); //start the WINNING melody.
  score_maxreached++;
  if (score_maxreached > (LCDHEIGHT / 1)) initGame(); //END = RESET + go back to titlescreen.

  //DRAW TROPHY.
  if (gameover_y > floor(LCDHEIGHT / 3) - 2 ) gameover_y -= 2; //update Y position.
  gb.display.setColor(WHITE);
  gb.display.drawBitmap(TROPHYX, gameover_y, trophyMaskBitmap); //white mask placed under the outline bitmap.
  gb.display.setColor(BLACK);
  gb.display.drawBitmap(TROPHYX, gameover_y, trophyBitmap); //outline
}

//PRESS "C" BUTTON TO RESET GAME & RETURN TO THE TITLESCREEN.
void resetGame() {
  if (gb.buttons.pressed(BTN_C)) {
    initGame();
  }
}
