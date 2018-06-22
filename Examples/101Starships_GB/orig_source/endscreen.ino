void updateEndScreen(){

  if(perso.vies == 0){
    displayText("GAME OVER",8,0,2);
  }
  else{
    displayText("COMPLETE!",8,0,2);
  }

  if(wait_end_screen == 0){
    bonus_end_screen = perso.vies*2000;
    if(continu == 1){
      bonus_end_screen = 0;
    }
  }

  if(bonus_end_screen == 0 && score_saved == 0){
    score_saved = 1;
    if(score > highscore){
      highscore = score;
      EEPROM.write(0, highscore & 0x00FF); //LSB
      EEPROM.write(1, (highscore >> 8) & 0x00FF); //MSB
      new_highscore = 1;
    }

    rank_letter = 0;
    unsigned int top = 38000;
    while(rank_letter < 10 && score < top){
      rank_letter++;
      top = top *0.75;
    }  
  }





  displayText("Ships destroyed :",1,14);
  displayInt(ships_destroyed,72,14,3);  


  if(perso.vies > 0){
    displayText("Remaining lives :",1,20);
    gb.display.cursorX = 80;
    gb.display.print(perso.vies);
    if(bonus_end_screen > 0){
      displayInt(bonus_end_screen,68,26,4);  
    }

  }

  //Score Final
  if(!new_highscore || wait_end_screen <= 20){
    displayText("FINAL SCORE :",1,36);
  }
  else{
    if(wait_end_screen % 10 < 7){
      displayText("HIGHSCORE !",1,36);
    }
  }

  displayInt(score,60,36,6);  



  if(bonus_end_screen == 0){ 
    gb.display.cursorX = 1;
    gb.display.cursorY = 30;  
    gb.display.print("Rank :");
    gb.display.cursorX = 80;
    switch(rank_letter){
    case 0:
      gb.display.print('S');
      break;
    case 1:
      gb.display.cursorX = 76;
      gb.display.print("A+");
      break;
    case 2:
      gb.display.print('A');
      break;
    case 3:
      gb.display.cursorX = 76;
      gb.display.print("A-");
      break;
    case 4:
      gb.display.cursorX = 76;
      gb.display.print("B+");
      break;
    case 5:
      gb.display.print('B');
      break;
    case 6:
      gb.display.cursorX = 76;
      gb.display.print("B-");
      break;
    case 7:
      gb.display.cursorX = 76;
      gb.display.print("C+");
      break;
    case 8:
      gb.display.print('C');
      break;
    case 9:
      gb.display.cursorX = 76;
      gb.display.print("C-");
      break;
    default:
      gb.display.print('D');
      break;
    }
  }






  //Boutons
  if(bonus_end_screen == 0 && wait_end_screen > 20){
    if((wait_end_screen-20) % 60 < 30){
      displayText("\25 Retry",30,42);
    }
    else{
      displayText("\26/\27 Title Screen",12,42);
    }
  }

  if(bonus_end_screen == 0 && wait_end_screen > 20){ 
    if(gb.buttons.pressed(BTN_A)){
      gb.sound.playOK();
      initGame(1);
    }
    if(gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
      gb.sound.playCancel();
      initGame(0);
    }
  }


  if(bonus_end_screen > 0 && wait_end_screen > 20){
    int s = min(bonus_end_screen,97);
    bonus_end_screen -= s;
    gb.sound.playTick();
    score += s;
    wait_end_screen = 20;
  }

  wait_end_screen++;
  if(wait_end_screen > 80){
    wait_end_screen -= 60;
  }
}








