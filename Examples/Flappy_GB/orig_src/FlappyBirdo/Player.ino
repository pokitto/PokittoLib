//UPDATE Player position.
void playerMove() {
  //REGULAR GRAVITY
  //gravity += 0.5; //regular down.
  gravity += 0.18;
  gravity *= 0.95; //increasing friction.
  player_y += gravity; //update the player's Y position.

  //PRESS "A" BUTTON TO JUMP
  if (player_y > 0) {
    if (gb.buttons.pressed(BTN_A) || (gb.buttons.pressed(BTN_RIGHT)) || (gb.buttons.pressed(BTN_DOWN)) || (gb.buttons.pressed(BTN_LEFT)) || (gb.buttons.pressed(BTN_UP))) {
      if (gravity > -0.5) {//limit the maximum...
        gravity -= 2; //update the player's Y position.
      }
      playsoundfx(0, 0); //sound FX
      player_animation = 1; //wings animation.
    }
  }
  //COLLISION WITH TOP
  if (player_y < 0) {
    player_y = 1;
    gravity = 0;
  }
  //DEADFUL COLLISION...
  for (byte cc = 0; cc < 3; cc++) { //loop for pipes.
    if ( ((player_y + PLAYERH) > (LCDHEIGHT - GROUNDH))//...with GROUND
         || (gb.collideBitmapBitmap(PLAYERX, player_y, bird1Bitmap, pipe[cc].x, pipe[cc].y, pipeBitmap) ) //...with BOTTOM pipes.
         || (gb.collideBitmapBitmap(PLAYERX, player_y, bird1Bitmap, pipe[cc].x, pipe[cc].y - PIPEH - PIPEGAPV, pipeBitmap))) { //...with TOP pipes.
      playsoundfx(2, 0);
      player_death = true;
    }
  }
}
//Animation
void playerAnimation() {
  if ((player_animation > 0) && (player_animation < 4)) {
    player_animation++;
  } else player_animation = 0;
}

//DRAW animated player.
void drawPlayerAlive() {
  playerAnimation();
  //DRAW player sprites
  gb.display.setColor(WHITE); //white mask placed under the outline bitmap.
  if (player_animation < 2) {
    gb.display.drawBitmap(PLAYERX, player_y, bird1MaskBitmap);
  }
  else gb.display.drawBitmap(PLAYERX, player_y, bird2MaskBitmap);
  gb.display.setColor(BLACK); //outline
  if (player_animation > 2) {
    gb.display.drawBitmap(PLAYERX, player_y, bird1Bitmap);
  }
  else gb.display.drawBitmap(PLAYERX, player_y, bird2Bitmap);
}

//DRAW DEAD player
void drawPlayerEnd() {
  if ((player_death == true) && (player_y < (LCDHEIGHT - GROUNDH - PLAYERH))) player_y += 4; //the image move to the top.
  gb.display.setColor(WHITE); //white mask placed under the outline bitmap.
  gb.display.drawBitmap(PLAYERX, player_y, bird1MaskBitmap, ROTCW, NOFLIP);
  gb.display.setColor(BLACK); //outline
  gb.display.drawBitmap(PLAYERX, player_y, bird1Bitmap, ROTCW, NOFLIP);
}
