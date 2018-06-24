//Update score
void updateScore() {
  if (score + 1 <= SCOREMAX) {
    score++; //+1 point.
    playsoundfx(1, 0); //sound FX.
  } else { //=score maximum!
    score = SCOREMAX;
  }
}

//DRAW score.
void drawScore() {
  gb.display.setFont(font3x5);
  if (score > 9) score_units = true; //the outline is larger is the score is 10 minimum. //no need for brackets here
  gb.display.setColor(BLACK);
  gb.display.fillRect(floor((LCDWIDTH - (3 * (1 + score_units))) / 2) - 1, 3, 5 + (4 * score_units), 7); //outline of the score.
  gb.display.cursorX = (floor(LCDWIDTH - (3 * (1 + score_units))) / 2); //X position of the score.
  gb.display.cursorY = 4; //Y position of the score.
  gb.display.setColor(WHITE);
  gb.display.println(score);//test
}

//HIGHSCORES update
void updateHighscore() {
  if ((difficulty_level == 0.5) && (score > pipe[0].hs)) pipe[0].hs = score;
  if ((difficulty_level == 1.0) && (score > pipe[1].hs)) pipe[1].hs = score;
  if ((difficulty_level == 2.0) && (score > pipe[2].hs)) pipe[2].hs = score;
}
