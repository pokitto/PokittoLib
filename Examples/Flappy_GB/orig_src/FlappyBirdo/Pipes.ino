void pipesStart() {
  for (byte bb = 0; bb < 3; bb++) {
    pipe[bb].x = LCDWIDTH + (PIPEGAPH * bb); //pipe X position
    pipe[bb].y = random(2, 17) + PIPEGAPV; //pipe Y position. 2-->17
  }
}

//Update the Pipes positions.
void updatePipes() {
  for (byte z = 0; z < 3; z++) {
    pipe[z].x -= difficulty_level; //the pipes move to the left.
    if (pipe[z].x <= -PIPEW) pipe[z].x = (LCDWIDTH ); //if a pipe is too far left, the pipe is relocated to the right.
    if (pipe[z].x >= LCDWIDTH) pipe[z].y = random(2, 17) + PIPEGAPV; //new random height.
    if (pipe[z].x == PLAYERX) updateScore(); //when 1 point is obtained.
  }
}

void drawPipes() {
  for (byte a = 0; a < 3; a++) {
    gb.display.setColor(GRAY);
    gb.display.fillRect(pipe[a].x, pipe[a].y, PIPEW, 24); //bottom pipe
    gb.display.fillRect(pipe[a].x, (pipe[a].y - PIPEH - PIPEGAPV), PIPEW, 24); //top pipe
    //highlights
    gb.display.setColor(WHITE);
    gb.display.fillRect(pipe[a].x, (pipe[a].y + 1), PIPEW, 1); //bottom pipe
    gb.display.fillRect(pipe[a].x + 2, pipe[a].y, 2, 24); //bottom pipe

    gb.display.fillRect(pipe[a].x, (pipe[a].y - PIPEGAPV - 1), PIPEW, 1); //top pipe
    gb.display.fillRect(pipe[a].x + 2, (pipe[a].y - PIPEH - PIPEGAPV), 2, 24); //top pipe
    //outline
    gb.display.setColor(BLACK);
    gb.display.drawBitmap(pipe[a].x, pipe[a].y, pipeBitmap); //bottom pipe
    gb.display.drawBitmap(pipe[a].x, (pipe[a].y - PIPEH - PIPEGAPV), pipeBitmap, NOROT, FLIPV); //top pipe
  }
}
