//
// PokiSnake : A Pokitto port of the Arduboy game "ArduSnake" I made for the french magazine "Programmez!"
//
// Copyright (C) 2017 Jerome Perrot (Initgraph)
//
// Version : 0.1
//
// Licence :
//
//   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// For any remarks or questions you can contact me on Twitter (@InitgraphDotCom) or by email (contact@initgraph.com).
// You can also visit my website : http://www.initgraph.com
//
// Enjoy !
//

#include "Pokitto.h"
#include "Constants.h"
#include "Snake.h"


Pokitto::Core game;
Snake snakes[MAXSNAKES];
uint8_t numplayers=1;

uint8_t gameState;
char scoreText[4];
uint8_t highScore = 0;
char highScoreText[4];
uint8_t blinkingTextState = 0;
uint8_t lastdead=0;

int main () {

  // ------------------------------------------------------------------------------------------------------------------------------------------------------
  // Init game
  // ------------------------------------------------------------------------------------------------------------------------------------------------------
  game.begin();

  // Load HighScore
  /*highScore = EEPROM.read(EEPROM_ADDRESS);
  if (highScore == 255) {

    highScore = 0;
    EEPROM.update(EEPROM_ADDRESS, highScore);
  }*/
  highScore = 0;
  while(game.buttons.aBtn()) game.wait(1); //prevent jumping straight into oneplayer
  gameState = GAMESTATE_MAINMENU;
  for (int i=0;i<numplayers;i++) snakes[i].init(&game, GAMESTATE_MAINMENU);
  game.display.setFont(fontKoubit);
  game.display.textWrap = false; // prevent automatic line scroll


  // ------------------------------------------------------------------------------------------------------------------------------------------------------
  // Main loop
  // ------------------------------------------------------------------------------------------------------------------------------------------------------
  while (game.isRunning()) {
    if (game.update()) {

      // ===== Game over screen ==================================================
      if (gameState == GAMESTATE_GAMEOVER) {

        game.setFrameRate(1);

        // If the highscore is beaten
        for (int i=0;i<numplayers;i++) {
        if (snakes[i].foodEaten > highScore) {

          // Save the new highscore
          highScore = snakes[i].foodEaten;
          //EEPROM.update(EEPROM_ADDRESS, snake.foodEaten);
        }}

        // Read keys
        if (game.buttons.bBtn()||joyhat.Button1()||joyhat.Button2()) {

          // Show main menu
          gameState = GAMESTATE_MAINMENU;
          for (int i=0;i<numplayers;i++) snakes[i].init(&game, GAMESTATE_MAINMENU);
        }

        game.display.clear();

        // Display the scores
        game.display.color=1;
        // Display the "GAME OVER" title

        game.display.flipFontVertical=false;
        game.display.print(29, 44, "GAME OVER");
        game.display.flipFontVertical=true;
        game.display.print(55+27, 33, "GAME OVER");
        game.display.flipFontVertical=false;

        // Display a blinking "Press B to continue" message
        blinkingTextState = (blinkingTextState + 1) % 6;
        if (blinkingTextState < 3) {
          game.display.flipFontVertical=true;
          game.display.print(102, 0, "Press to continue");
          game.display.flipFontVertical=false;
          game.display.print(1, 80, "Press B to continue");
        }
        // score right way around
        game.display.setCursor(0,52+numplayers*2);
        int highestscore=0, winner=-1;
        for (int i=0;i<numplayers;i++) {
                if (snakes[i].foodEaten>highestscore) {highestscore=snakes[i].foodEaten;winner=i;}
                else if (snakes[i].foodEaten==highestscore && lastdead==i) winner=i;
        }
        if (highestscore==0) winner=-1; //no victory for winning without points!

        for (int i=0;i<numplayers;i++) {
        game.display.color = i+2;
        game.display.print("Player ");
        game.display.print(i);
        if (blinkingTextState < 3 && i==winner) {
               game.display.println(" Wins!");
        } else {
        game.display.print(" score:");
        sprintf(scoreText, "%02u", snakes[i].foodEaten);
        game.display.println(scoreText);
        }
        }
        // flipped scores
        game.display.flipFontVertical = true;

        for (int i=0;i<numplayers;i++) {
        game.display.setCursor(102,23-numplayers*2-i*8);
        game.display.color = i+2;
        game.display.print("Player ");
        game.display.print(i);
        if (blinkingTextState < 3 && i==winner) {
               game.display.println(" Wins!");
        } else {
        game.display.print(" score:");
        sprintf(scoreText, "%02u", snakes[i].foodEaten);
        game.display.println(scoreText);
        }
        }
        game.display.flipFontVertical = false;
      }

      // ===== Game screen ==================================================
      else if (gameState == GAMESTATE_GAME) {

        // Read keys
        if (game.buttons.leftBtn() && snakes[0].direction!=SNAKEDIR_RIGHT) {
          snakes[0].direction = SNAKEDIR_LEFT;
          //snakes[0].direction--;
          //snakes[0].direction&=3;
        }
        else if (game.buttons.rightBtn() && snakes[0].direction!=SNAKEDIR_LEFT) {

          snakes[0].direction = SNAKEDIR_RIGHT;
          //snakes[0].direction++;
          //snakes[0].direction&=3;
        }
        else if (game.buttons.upBtn() && snakes[0].direction!=SNAKEDIR_DOWN) {

          snakes[0].direction = SNAKEDIR_UP;
          //snakes[0].direction++;
          //snakes[0].direction&=3;
        }
        else if (game.buttons.downBtn() && snakes[0].direction!=SNAKEDIR_UP) {

          snakes[0].direction = SNAKEDIR_DOWN;
          //snakes[0].direction++;
          //snakes[0].direction&=3;
        }

        else if (game.buttons.aBtn()) {

          // Activate sound or not
          /*arduboy.setTextSize(1);
          if (snake.canMakeSound) {

            snake.canMakeSound = false;
            arduboy.setCursor(28, 0);
            arduboy.print("            ");
            arduboy.setCursor(34, 0);
            arduboy.print("Sound: OFF");
          }
          else {

            snake.canMakeSound = true;
            arduboy.setCursor(31, 0);
            arduboy.print("           ");
            arduboy.setCursor(37, 0);
            arduboy.print("Sound: ON");
            arduboy.tunes.tone(1000, 200);
          }
          arduboy.display();
          delay(1000);*/
        }
        else if (game.buttons.cBtn()) {

          gameState = GAMESTATE_PAUSED;
        }

        /*
        for (int i=0;i<numplayers*2;i+=2) {
            if (xtrabuttons[i]) snakes[i/2].direction--;
            if (xtrabuttons[i+1]) snakes[i/2].direction++;
            snakes[i/2].direction &= 3; // same as %4 but faster
        }*/

        if (numplayers>1) {
                if (joyhat.JoyY()<10 && snakes[1].direction!=SNAKEDIR_UP) snakes[1].direction=SNAKEDIR_UP;
                else if (joyhat.JoyY()>80 && snakes[1].direction!=SNAKEDIR_DOWN) snakes[1].direction=SNAKEDIR_DOWN;
                else if (joyhat.JoyX()>80 && snakes[1].direction!=SNAKEDIR_RIGHT) snakes[1].direction=SNAKEDIR_LEFT;
                else if (joyhat.JoyX()<10 && snakes[1].direction!=SNAKEDIR_LEFT) snakes[1].direction=SNAKEDIR_RIGHT;
        }

        for (int i=0; i< numplayers; i++) {
                snakes[i].move();
                for (int j=0; j< numplayers; j++) if (i!=j) if(snakes[i].checkCollisionWithOther(&snakes[j])) snakes[i].isDead=true;
        }

        int deadsnakes=0;
        for (int i=0; i< numplayers; i++) {
        if (snakes[i].isDead) deadsnakes++;
        else lastdead=i;
        }
        //if (deadsnakes==numplayers) {
        if (deadsnakes) {
          gameState = GAMESTATE_GAMEOVER;
        }

        game.display.clear();

        // Draw the border
        if (border) game.display.drawRectangle(2, 2, 103, 83);

        // Display score
        for (int i=0; i<numplayers; i++) {
        game.display.flipFontVertical=false;
        game.display.setCursor((110/numplayers)*(i)+8, 0);
        if (i==1) {
            game.display.flipFontVertical=true;
            game.display.setCursor(110-8, 80);
        }
        game.display.color=i+2;
        sprintf(scoreText, "%03u", snakes[i].foodEaten);
        //sprintf(scoreText, "x:%04u, y:%04u", joyhat.JoyX(), joyhat.JoyY());
        game.display.print(scoreText);
        snakes[i].draw();
        game.display.color=1;
        snakes[i].food.draw();
        game.display.flipFontVertical=false;
        }
      }
      // ===== Pause Screen ==================================================
      else if (gameState == GAMESTATE_PAUSED) {

        // Read keys
        if (game.buttons.cBtn()) {

          gameState = GAMESTATE_GAME;
        }

        // Show "PAUSED" title
        game.display.print(38, 38, "PAUSED");
      }

      // ===== Main menu screen ==================================================
      else {

        game.setFrameRate(60);

        snakes[0].moveOnMainMenu();

        //int players=-1;
        //for (int i=0; i<MAXSNAKES*2;i+=1) {
                //if (xtrabuttons[i]) players=i/2;
        //}
        //players++;
        int players = 0;
        if (joyhat.Button1() || joyhat.Button2()) players=2;
        else if (game.buttons.released(BTN_A)) players = 1;

        if (game.buttons.bBtn()||players) {
          // Run game
          gameState = GAMESTATE_GAME;
          numplayers = players;
          for (int i=0; i<numplayers;i++) snakes[i].init(&game, GAMESTATE_GAME);
          game.wait(100);
        }



        game.display.clear();

        // Show title
        game.display.flipFontVertical = false;
        game.display.print(28, 56, "POKISNAKE");
        game.display.flipFontVertical = true;
        game.display.print(28+7*7+2, 14+7, "POKISNAKE");
        game.display.flipFontVertical = false;

        // Show a blinking "Press B to start" message
        blinkingTextState = (blinkingTextState + 1) % 30;
        if (blinkingTextState < 15) {
          game.display.flipFontVertical = false;
          game.display.print(7, 66, "Start One Player");
          game.display.flipFontVertical = true;
          game.display.print(100, 10, "Start Two Player");
          game.display.flipFontVertical = false;
        }
        game.display.color=2;
        snakes[0].draw();
        game.display.color=1;
      }
    }
  }
  return 1;
}

