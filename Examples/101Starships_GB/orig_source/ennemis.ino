void updateEnnemis()
{
  if(perso.vies > 0){
    EndGame = 0;
  }
  byte i = 0;
  float t = 0.0;
  while(i<MAX_ENNEMIS)
  {
    if(ennemis[i].on == 1){

      EndGame = 1; 

      //Trajectoire
      ennemis[i].bez_frame++;
      if(ennemis[i].image == 3 && ennemis[i].bez_frame > ennemis[i].bez.tot_frames){
        ennemis[i].bez_frame = ennemis[i].bez.tot_frames;
      }
      t = ((float)ennemis[i].bez_frame)/((float)ennemis[i].bez.tot_frames);
      ennemis[i].x = (1.0 - t)*(1.0 - t)*ennemis[i].bez.x1 + 2*t*(1.0 - t)*ennemis[i].bez.x2 + t*t*ennemis[i].bez.x3;
      ennemis[i].y = (1.0 - t)*(1.0 - t)*ennemis[i].bez.y1 + 2*t*(1.0 - t)*ennemis[i].bez.y2 + t*t*ennemis[i].bez.y3;

      char x = ennemis[i].x - 100;
      char y = ennemis[i].y - 100;

      //Boss
      if(t >= 1 && ennemis[i].image == 3){
        if(boss_shots3 == 0){
          bossframe++;
        }
        boss_shots1++;
        boss_shots2++;
        int tir_v = 20;
        int tir_vx = 0;
        int tir_vy = 0;
        float tir_ang = 0;
        y = 21 + 16*sin(bossframe / 6.0);
        if(boss_shots1 >= 28){ //Tirs haut et bas
          if(perso.repop < 20){
            for(byte p = 0; p < 8; p++){
              if(y < 21){
                tir_ang = (215-13*p)* 0.0174;
              }
              else{
                tir_ang = (145+13*p)* 0.0174;
              }
              tir_vx = tir_v*cos(tir_ang);
              tir_vy = tir_v*sin(tir_ang);
              if(y < 21){
                newTirEnnemi(x-4,y+4,tir_vx,tir_vy);
              }
              else{
                newTirEnnemi(x-4,y-4,tir_vx,tir_vy);
              }
            }
            boss_shots1-= 28;
          }
        }
        if(boss_shots2 >= 100 && y < 24 && y > 18){
          boss_shots3 = 1;
        }
        if(boss_shots3 > 0){
          boss_shots1 = 0;
          boss_shots2 = 0;
          boss_shots3++;
          if(boss_shots3 % 9 == 0 && perso.repop < 20){
            for(byte p = 0; p < 9; p++){
              tir_ang = (112 + 17*p+3*(boss_shots3/9))* 0.0174;
              tir_vx = tir_v*cos(tir_ang);
              tir_vy = tir_v*sin(tir_ang);
              newTirEnnemi(x-3,y,tir_vx,tir_vy);
            }
          }
          if(boss_shots3 > 50){
            boss_shots3 = 0;
          }
        }


      }

      if(ennemis[i].justhit == 0){

        char min_x = -2; //Pour que les ennemis se fassent tirer dessus
        char max_x = 3;
        char min_y = -2; 
        char max_y = 3;


        switch(ennemis[i].image)
        {
        case 2: 
          { 
            gb.display.drawBitmap(x-2,y-2,vaisseau4);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau4,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }
          }
          break;
        case 1: 
          { 
            gb.display.drawBitmap(x-2,y-2,vaisseau3);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau3,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }
          }
          break;
        case 3: 
          { 
            gb.display.drawBitmap(x-5,y-6,boss);
            min_x = -2; //Pour que les ennemis se fassent tirer dessus
            max_x = 3;
            min_y = -5; 
            max_y = 5;
            if(gb.collideBitmapBitmap(x-5,y-6,boss,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
            }
          }
          break;
        default:
          { 
            gb.display.drawBitmap(x-2,y-2,vaisseau2);
            if(gb.collideBitmapBitmap(x-2,y-2,vaisseau2,perso.x, perso.y, pt) && perso.destroy == 0 && perso.repop <= 0 && perso.destroy == 0){
              perso.destroy = 1;
              gb.sound.playPattern(player_destroy,0);
              ennemis[i].on = 0;
              newExplosion(x, y);
            }

          }
        } 





        //Détection des tirs du joueur
        for(int m = 0; m < MAX_TIRS; m = m+1)
        {
          if(ennemis[i].on == 1 && tirs[m].on == 1)
          {
            if(tirs[m].x/10 >= x + min_x-2 && tirs[m].y/10 >= y + min_y - 1 && tirs[m].x/10 <= x + max_x && tirs[m].y/10 <= y + max_y+1)
            {
              if(ennemis[i].vie > 0)
              {
                ennemis[i].vie -= 1;
                score += 50;
              }
              tirs[m].on = 0;
              ennemis[i].justhit = 1;
              gb.sound.playPattern(enn_hit,0);


            }


          }
        }

      }
      else{
        ennemis[i].justhit = 0;
      }


      //Tirs
      ennemis[i].tirs_frame++;
      if(ennemis[i].tirs_frame >= ennemis[i].tir.tot_frames)
      {
        ennemis[i].tirs_frame = 0;
        int tir_v = 17;
        int tir_vx = 0;
        int tir_vy = 0;
        float tir_ang = 0;
        float tir_coef = 0.0;
        for(int k = 0; k < ennemis[i].tir.nb; k++)
        {
          if(ennemis[i].tir.nb > 1){
            tir_coef = ((float) k)/((float) (ennemis[i].tir.nb - 1));
          }
          else{
            tir_coef = 0.5;
          }
          tir_ang = tir_coef * ennemis[i].tir.ang1*10 + (1.0 - tir_coef) * ennemis[i].tir.ang2*10;
          tir_ang = tir_ang * 0.0174;
          tir_vx = tir_v*cos(tir_ang);
          tir_vy = tir_v*sin(tir_ang);
          if(perso.repop < 20){ //L'ennemi ne tire que si le joueur est prêt
            newTirEnnemi(x,y,tir_vx,tir_vy);
            gb.sound.playPattern(enn_shot,0);
          }
        }

      }

      if(ennemis[i].vie <= 0){
        newExplosion(x, y);
        ships_destroyed++;
        gb.sound.playPattern(enn_destroy,0);
        switch(ennemis[i].image)
        {
        case 1: 
          { 
            score += 200;
          }
          break;
        case 3: 
          { 
            score += 1000;
          }
          break;
        default:
          { 
            score += 100;
          }
        } 
      }

      if(ennemis[i].vie <= 0 || (t > 1 && ennemis[i].image != 3)){
        ennemis[i].on = 0;
      }





    }
    i++;
  } 







  //Nouvel ennemi !
  while(nbframes >= nextennemi && readennemi < totennemis && perso.vies > 0){
    //Déterminer l'espace libre
    byte i = 0;
    while(ennemis[i].on == 1)
    {
      i++;
    } 
    if(i>=MAX_ENNEMIS){
      i = 0;
    }

    ennemis[i].on = 1;
    ennemis[i].x = 255;
    ennemis[i].y = 255;
    ennemis[i].bez_frame = 0;
    ennemis[i].tirs_frame = 0;

    ennemis[i].vie = pgm_read_byte_near(ennemiSet + 1 + VARS_ENNEMIS*readennemi);
    ennemis[i].image = pgm_read_byte_near(ennemiSet + 2 + VARS_ENNEMIS*readennemi);

    ennemis[i].bez.x1 = pgm_read_byte_near(ennemiSet + 3 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.x2 = pgm_read_byte_near(ennemiSet + 4 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.x3 = pgm_read_byte_near(ennemiSet + 5 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y1 = pgm_read_byte_near(ennemiSet + 6 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y2 = pgm_read_byte_near(ennemiSet + 7 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.y3 = pgm_read_byte_near(ennemiSet + 8 + VARS_ENNEMIS*readennemi);
    ennemis[i].bez.tot_frames = pgm_read_byte_near(ennemiSet + 9 + VARS_ENNEMIS*readennemi);

    ennemis[i].tir.nb = pgm_read_byte_near(ennemiSet + 10 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.ang1 = pgm_read_byte_near(ennemiSet + 11 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.ang2 = pgm_read_byte_near(ennemiSet + 12 + VARS_ENNEMIS*readennemi);
    ennemis[i].tir.tot_frames = pgm_read_byte_near(ennemiSet + 13 + VARS_ENNEMIS*readennemi);


    readennemi += 1;
    nextennemi += pgm_read_byte_near(ennemiSet + VARS_ENNEMIS*readennemi);
  }
}







