void initDifficulty() {
  //MOVE ARROW
  if (gb.buttons.pressed(BTN_UP))  {
    //gb.sound.setVolume(1, 0);
    playsoundfx(4, 0); //gb.sound.playOK();
    difficulty_level --;
    player_animation = 1 ;
  }
  if (gb.buttons.pressed(BTN_DOWN)) {
    //gb.sound.setVolume(1, 0);
    playsoundfx(4, 0); //gb.sound.playOK();
    difficulty_level ++;
    player_animation = 1 ;
  }
  if (difficulty_level < 0) difficulty_level = 2;
  if (difficulty_level > 2) difficulty_level = 0;

  gb.display.cursorX = 0;
  gb.display.cursorY = 16 + (8 * floor(difficulty_level));
  //gb.display.print(F("\20")); //arrow
  playerAnimation();
  gb.display.setColor(BLACK);
  if (player_animation > 2) {
    gb.display.drawBitmap(0, 22 + (8 * floor(difficulty_level)), bird2Bitmap);
  } else gb.display.drawBitmap(0, 22 + (8 * floor(difficulty_level)), bird1Bitmap); //bird

  if (gb.buttons.pressed(BTN_A)) { //validate level
    if (difficulty_level == 0) difficulty_level = 0.5;
    gb.sound.playOK();
    difficulty_menu = false ;
  }
  //DRAW TEXTS
  gb.display.cursorX = 0;
  gb.display.setFont(font5x7);
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print(F("DIFFICULTY:\n \n\n  SLOW\n  NORMAL\n  FAST"));
  gb.display.setFont(font3x5);
  gb.display.cursorY = 2;
  gb.display.print(F("\nUP/DOWN to choose\nA to validate"));
  //gb.display.println(difficulty_level);//test
  //HIGHSCORE
  for (int pp = 0; pp < 3; pp++) { //
    gb.display.cursorX = 49;
    gb.display.cursorY = 25 + (pp * 8);
    gb.display.print(F("HIGH:"));
    gb.display.cursorY = 25 + (pp * 8);
    gb.display.cursorX = 70;
    gb.display.print(round(pipe[pp].hs));
  }
}
