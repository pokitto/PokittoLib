void updatePerso(){

  boolean bt_right;
  boolean bt_left;
  boolean bt_up;
  boolean bt_down;


  if(perso.wait_end < 10){
    bt_right = gb.buttons.repeat(BTN_RIGHT, 1);
    bt_left = gb.buttons.repeat(BTN_LEFT, 1);
    bt_up = gb.buttons.repeat(BTN_UP, 1);
    bt_down = gb.buttons.repeat(BTN_DOWN, 1);
  }
  else{  //Prise en main des contrôles pour la fin de partie
    bt_right = 0;
    bt_left = 0;
    bt_up = 0;
    bt_down = 0;
    if(perso.y > 23){
      bt_up = 1;
    }
    if(perso.y < 20){
      bt_down = 1;
    }
    if(perso.x > 11){
      bt_left = 1;
    }
    if(perso.x < 8){
      bt_right = 1;
    }
    if(perso.x >= 8 && perso.x <= 11){
      perso.x = 10;
    }
    if(perso.wait_end == 10){
      gb.sound.stopTrack(1);
      gb.sound.stopTrack(2);
    }
  }



  if(perso.destroy == 0){



    //Mouvement
    if(perso.wait_end < 50){
      if(bt_right && !bt_left){
        if(bt_down){
          perso.y += PLAYER_SPD2;
          perso.x += PLAYER_SPD2;
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD2);
          perso.x += PLAYER_SPD2;
        }
        else if(!bt_down){
          perso.x += PLAYER_SPD1;
        }
      }
      if(bt_left && !bt_right){
        if(bt_down){
          perso.y += PLAYER_SPD2;
          perso.x = max(0,perso.x-PLAYER_SPD2);
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD2);
          perso.x = max(0,perso.x-PLAYER_SPD2);
        }
        else if(!bt_down){
          perso.x = max(0,perso.x-PLAYER_SPD1);
        }
      }
      if((!bt_left && !bt_right) || (bt_left && bt_right)){
        if(bt_down){
          perso.y += PLAYER_SPD1;
        }
        else if(bt_up){
          perso.y = max(0,perso.y-PLAYER_SPD1);
        }
      }

      if(perso.x < 2)
      {
        perso.x = 2; 
      }
      if(perso.y < 2)
      {
        perso.y = 2; 
      }
      if(perso.x > LCDWIDTH - 3)
      {
        perso.x = LCDWIDTH - 3; 
      }
      if(perso.y > LCDHEIGHT - 9)
      {
        perso.y = LCDHEIGHT - 9; 
      }
    }
    else{
      perso.x = min(100,10+85.0*((perso.wait_end - 50.0)/70.0)*((perso.wait_end - 50.0)/70.0));
      if(perso.x > 11 && perso.wait_end < 120){
        starspeed += 1 + (perso.x-11) / 10;
      }
      if(perso.wait_end >= 120){
        starspeed = max(0,5-(perso.wait_end - 120)/5);
      } 
    }


    if(perso.repop >= 20){
      perso.y = LCDHEIGHT/2 - 3;
      float repop2 = ((((float)perso.repop) - 30.0)/15.0)*((((float)perso.repop) - 30.0)/15.0);
      perso.x = 25.0-30.0*repop2 ;
      if(perso.repop < 52){
        for(byte j=0;j<MAX_TIRS_ENNEMIS;j++)
        {
          tirs_ennemis[j].on = 0;
        }
      }

    }
    if(perso.repop > 0){
      perso.repop -= 1;
      perso.wait_end = 0; 
    }

    byte xorig = perso.x-2;
    byte yorig = perso.y-2;
    if(perso.repop % 2 == 0 || (perso.vies == 3 && continu == 0)){
      if(perso.repop < 20 && perso.wait_end < 80){
        gb.display.drawBitmap(xorig,yorig,vaisseau);
      }
      if(perso.repop >= 20){
        if((perso.repop-20) % 8 < 2){
          gb.display.drawBitmap(xorig,yorig,vaisseau);
        }
        else{
          if((perso.repop-20) % 8 == 4 || (perso.repop-20) % 8 == 5){
            gb.display.drawBitmap(xorig,yorig,vaisseauXS);
          }
          else{
            gb.display.drawBitmap(xorig,yorig,vaisseauS);
          }
        }
      }

      if(perso.wait_end >= 80){
        if((perso.wait_end-80) % 8 < 2){
          gb.display.drawBitmap(xorig,yorig,vaisseau);
        }
        else{
          if((perso.wait_end-80) % 8 == 4 || (perso.wait_end-80) % 8 == 5){
            gb.display.drawBitmap(xorig,yorig,vaisseauXS);
          }
          else{
            gb.display.drawBitmap(xorig,yorig,vaisseauS);
          }
        }
      }
    }


    //Super Tir
    if(perso.repop > 0){
      perso.supershot = 0;
      perso.supershot_nb = 0;
    }
    if(perso.supershot == 30 && gb.buttons.repeat(BTN_A, 1) && perso.wait_end < 10){
      perso.supershot_nb = 5;
      perso.supershot = 0;
    }
    if(perso.supershot == 29 ){
      gb.sound.playPattern(player_super,0);
    }
    if(perso.supershot_nb == 5 || perso.supershot_nb == 3 || perso.supershot_nb == 1){
      newTir(45,-5,-1);
      newTir(45,5,1);
      newTir(55,0,0);
      newTir(50,0,-1);
      newTir(50,0,1);
    }
    if(perso.supershot_nb > 0){
      perso.supershot_nb -= 1;
      perso.supershot = 0;
    }

    //Tir normal, chargement du super tir
    if(gb.buttons.repeat(BTN_A, 1) && perso.cadencetir == 0 && perso.repop < 20 && perso.supershot_nb == 0 && perso.wait_end < 10){
      newTir(50,0,-1);
      newTir(50,0,1);
      perso.cadencetir = 6;
    }
    if(perso.cadencetir > 0){
      perso.cadencetir -= 1;
      perso.supershot = 0;
    }
    if(perso.cadencetir == 0 && perso.supershot < 30){
      perso.supershot += 1;
    }
    if(perso.supershot >= 10 && readennemi < totennemis){
      score += 5;
    }

  }
  else { //Si le joueur vient de se faire détuire
    perso.supershot = 0;
    if(perso.vies > 1){
      perso.repop = 60;
      perso.vies -= 1; 
      perso.destroy = 0;
      newExplosion(perso.x, perso.y);
    }
    else{
      if(perso.vies != 0){
        newExplosion(perso.x, perso.y); 
        perso.wait_gameover = 0;
      }
      perso.vies = 0; 
      gb.sound.stopTrack(1);
      gb.sound.stopTrack(2);

    }
  }


  if(perso.vies == 0){
    perso.wait_gameover += 1; 

    if(perso.wait_gameover >= 10){


      gb.display.setColor(WHITE);
      gb.display.fillRect(20, 10, LCDWIDTH-40, 10);
      gb.display.fillRect(20, 29, LCDWIDTH-40, 12);
      gb.display.setColor(BLACK);

      displayText("CONTINUE?",22,10,1);
      displayInt(min(9,(210-perso.wait_gameover)/20),60,10,1);
      displayText("\25/\27 Continue",20,29);
      displayText("\26 End game",24,36);
      if(perso.wait_gameover % 20 == 10){
        gb.sound.playTick(); 
      }
      if(gb.buttons.pressed(BTN_B)){
        perso.wait_gameover = 230;
        gb.sound.playCancel();
      }

      if(gb.buttons.pressed(BTN_C) || gb.buttons.pressed(BTN_A)){
        gb.sound.playOK();
        perso.vies = 3;
        perso.repop = 50;
        score = 0;
        scoreaff = 0;
        perso.destroy = 0;
        continu = 1;
        readennemi = checkpoint;
        nextennemi = nbframes + 50;
        phase = 0;
        ships_destroyed = 0;
        EndGame = 0;
        bossframe = 0;
        boss_shots1 = 0;
        boss_shots2 = 0;
        boss_shots3 = 0;

        for(byte j=0;j<MAX_ENNEMIS;j++)
        {
          ennemis[j].on = 0;
        }
        for(byte j=0;j<MAX_TIRS_ENNEMIS;j++)
        {
          tirs_ennemis[j].on = 0;
        }
        for(byte j=0;j<STARS;j++){
          star[j*2] = 100+random(0,LCDWIDTH/STARS+3)+j*(LCDWIDTH/STARS); 
          star[j*2+1] = random(0,LCDHEIGHT); 
        }
      }
    }


  }
  if(EndGame == 0 && readennemi >= totennemis){
    perso.wait_end += 1; 

  }




}

void initPerso(){
  perso.x = 10;
  perso.y = LCDHEIGHT/2.0;
  perso.repop = 50;
  perso.vies = 3;
  perso.destroy = 0;
  perso.wait_end = 0;
  perso.wait_gameover = 0;
}







