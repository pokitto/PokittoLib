void drawBackground() {
  gb.display.setColor(GRAY);
  gb.display.fillRect(0, 0, LCDWIDTH, 17); //the top of the sky is gray.

  for (byte i = 0; i < (256 / 16); i++) { //loop...
    gb.display.setColor(WHITE);
    gb.display.drawBitmap((i * 16), 13, skyMaskBitmap); //display clouds mask.
    gb.display.setColor(BLACK);
    gb.display.drawBitmap((i * 16), 13, skyBitmap); //display clouds.
    gb.display.setColor(GRAY);
    gb.display.drawBitmap((i * 16), 23, cityBitmap); //display city.
  }
}

void drawGround() {
  gb.display.setColor(BLACK); //1st top black line.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH), LCDWIDTH, 1);
  gb.display.setColor(GRAY); //gray background.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH + 1), LCDWIDTH, (GROUNDH - 1));
  gb.display.setColor(WHITE); // white highlight.
  gb.display.fillRect(0, (LCDHEIGHT - GROUNDH + 1), LCDWIDTH, 1);
  gb.display.setColor(BLACK); //shadow on the bottom.
  gb.display.fillRect(0, (LCDHEIGHT - 1), LCDWIDTH, 1);
  for (byte j = 0; j < LCDWIDTH / 2; j++) { //loop to display small shadow "dents".
    gb.display.fillRect( (j * 4), (LCDHEIGHT - 2 ), 2, 1);
  }
}
