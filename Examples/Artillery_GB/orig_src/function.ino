//##################################################################
//##################################################################
void fnctn_checkbuttons() {
  switch(gamestatus)
  {
     case SELECT_MAP :
          if(gb.buttons.pressed(BTN_DOWN)  && gamelevel+3<=nbAvailableLevel){gamelevel = gamelevel + 3;}
          if(gb.buttons.pressed(BTN_UP)    && gamelevel-3>=0)               {gamelevel = gamelevel - 3;}
          if(gb.buttons.pressed(BTN_RIGHT) && gamelevel+1<=nbAvailableLevel){gamelevel = gamelevel + 1;}
          if(gb.buttons.pressed(BTN_LEFT)  && gamelevel-1>=0)               {gamelevel = gamelevel - 1;}
          if(gb.buttons.pressed(BTN_A)){
            if(gamelevel==0){
              gamestatus=OPTIONS;
              setting=0;
            }else{
              randomVal = random(0,21);
              gamestatus=NEW_LEVEL;
            }
          }
          if(gb.buttons.pressed(BTN_C)){
            gb.titleScreen(gamelogo);
            gb.battery.show = false;
          }
          break;
     
     case OPTIONS :
          if(gb.buttons.pressed(BTN_DOWN)){
            if(setting < 5) setting=setting+1; else setting = 0;
          }
          if(gb.buttons.pressed(BTN_UP)){
            if(setting > 0) setting=setting-1; else setting = 5;
          }
          if(gb.buttons.pressed(BTN_RIGHT)){
            switch(setting){
              case 0:
                   if(nbTeam<4) nbTeam=nbTeam+1; else nbTeam = 2;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 1:
                   if(nbPlayer<4) nbPlayer=nbPlayer+1; else nbPlayer = 1;
                   break;
              case 2:
                   if(unitLife<3) unitLife=unitLife+1; else unitLife = 1;
                   break;
              case 3:
                   if(nbCpuTeam<nbTeam-1) nbCpuTeam=nbCpuTeam+1; else nbCpuTeam = 0;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 4:
                   if(gravity<3) gravity=gravity+1; else gravity = 1;
                   break;
            }
          }
          if(gb.buttons.pressed(BTN_LEFT)){
            switch(setting){
              case 0:
                   if(nbTeam>2) nbTeam=nbTeam-1; else nbTeam = 4;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 1:
                   if(nbPlayer>1) nbPlayer=nbPlayer-1; else nbPlayer = 4;
                   break;
              case 2:
                   if(unitLife>1) unitLife=unitLife-1; else unitLife = 3;
                   break;
              case 3:
                   if(nbCpuTeam>0) nbCpuTeam=nbCpuTeam-1;  else nbCpuTeam = nbTeam;
                   if(nbCpuTeam==nbTeam) nbCpuTeam=nbCpuTeam-1;
                   break;
              case 4:
                   if(gravity>1) gravity=gravity-1; else gravity = 3;
                   break;
            }
          }
          if(gb.buttons.pressed(BTN_B) || (gb.buttons.pressed(BTN_A) && setting==5)){
            gamestatus=SELECT_MAP;
          }
          if(gb.buttons.pressed(BTN_C)){
            gb.titleScreen(gamelogo);
            gb.battery.show = false;
          }
          break;
          
     case PAUSE :
          if(gb.buttons.pressed(BTN_DOWN)){
            if(setting < 1) setting=setting+1; else setting = 0;
          }
          if(gb.buttons.pressed(BTN_UP)){
            if(setting > 0) setting=setting-1; else setting = 1;
          }
          if(gb.buttons.pressed(BTN_B)) gamestatus = RUNNING;
          if(gb.buttons.released(BTN_A)){
            switch(setting){
              case 0:
                   gamestatus=RUNNING;
                   break;
                   
              case 1:
                   gamestatus=SELECT_MAP;
                   break;
            } 
          }
          if(gb.buttons.pressed(BTN_C)){
            gamestatus=SELECT_MAP;
          }
          break;
          
           
     case GAMEOVER :
          if(gb.buttons.pressed(BTN_A)){
            gamestatus=SELECT_MAP;
          }
          break;
          
     case SELECT_UNIT :
          if(gb.buttons.pressed(BTN_LEFT) || gb.buttons.pressed(BTN_RIGHT) || gb.buttons.pressed(BTN_B)){allPlayer[currentPlayer].timer=0;}
          break;
     
     case RUNNING :
          if(gb.buttons.repeat(BTN_A,1) && power<10){
            power = power+1;
          }else{
            if(power>0 ){
              //define original position of rocket
              rocket.x = allPlayer[currentPlayer].x+(1+(allPlayer[currentPlayer].dir))+(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)));
              rocket.y = (allPlayer[currentPlayer].y+((trajParamY[angle]/10))+1);
              rocket.x_traj = trajParamX[angle]/10*(power/2)*((allPlayer[currentPlayer].dir*2)-1);
              rocket.y_traj = trajParamY[angle]/10*(power/2);
              gamestatus=ANIMFIRE;
            }else{
              if(gb.buttons.repeat(BTN_UP,1)   || gb.buttons.pressed(BTN_UP))     {if(angle>0){angle = angle-1;}}
              if(gb.buttons.repeat(BTN_DOWN,1) || gb.buttons.pressed(BTN_DOWN))   {if(angle<8){angle = angle+1;}}
              
              if(gb.buttons.pressed(BTN_RIGHT)){
                allPlayer[currentPlayer].dir = 1;
              }else{
                if(gb.buttons.repeat(BTN_RIGHT,1)){
                  if(gb.display.getPixel(allPlayer[currentPlayer].x+4,allPlayer[currentPlayer].y+3)==0){
                    allPlayer[currentPlayer].x = allPlayer[currentPlayer].x + 1;
              } } }
              
              if(gb.buttons.pressed(BTN_LEFT)){
                allPlayer[currentPlayer].dir = 0;
              }else{
                if(gb.buttons.repeat(BTN_LEFT,1)){
                  if(gb.display.getPixel(allPlayer[currentPlayer].x-1,allPlayer[currentPlayer].y+3)==0){
                    allPlayer[currentPlayer].x = allPlayer[currentPlayer].x - 1;
              } } }
              
              if(gb.buttons.pressed(BTN_B)){
                if(jumpStatus==0 && power==0 
                    && gb.display.getPixel(allPlayer[currentPlayer].x,allPlayer[currentPlayer].y-3)==0 
                    && gb.display.getPixel(allPlayer[currentPlayer].x+3,allPlayer[currentPlayer].y-3)==0)
                { 
                  outpt_soundfx(5);
                  jumpStatus=6;
                }
              }
              if(gb.buttons.pressed(BTN_C)){
                setting=0;
                gamestatus=PAUSE;
              }
            }
          }
          break;
  } // end switch
}



// ##################################################################################################################################
// ################## PLAYER RELATED FUNCTIONS ######################################################################################
// ##################################################################################################################################


//##################################################################
//##### DEFINE PLAYERS POSITION ####################################
void fnctn_definePlayer(){
  boolean check;
  byte tmp;
  byte tmp2;
  byte randm[21];
  
  for(fct_countr=0;fct_countr<21;fct_countr++){randm[fct_countr] = (fct_countr*4)%21;}

  tmp = random(0,21);
  for(fct_countr=0;fct_countr<21-tmp;fct_countr++){
    tmp2 = randm[fct_countr];
    randm[fct_countr] = randm[fct_countr+tmp];
    randm[fct_countr+tmp] = tmp2;
  }

  
  for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
    allPlayer[fct_countr].dead = 0;
    allPlayer[fct_countr].life = unitLife;
    allPlayer[fct_countr].team = fct_countr / nbPlayer;
    allPlayer[fct_countr].fall=0;
    allPlayer[fct_countr].isIA = 0; 
    if(allPlayer[fct_countr].team>=nbTeam-nbCpuTeam){
      allPlayer[fct_countr].isIA=1;
    }
    allPlayer[fct_countr].timer = 0;
 
    
    //Define position
    allPlayer[fct_countr].x = randm[fct_countr]*4;

    switch(randm[fct_countr]%2){
      case 0:
           allPlayer[fct_countr].y = 0;
           check=0;
           while(check==0 && allPlayer[fct_countr].y<11){
             if(gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1)==0 && gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1+1)==1){
               allPlayer[fct_countr].y = allPlayer[fct_countr].y*4;
               check=1;
             }else{
               allPlayer[fct_countr].y = allPlayer[fct_countr].y+1;
             }
           }
           if(check==0){
              allPlayer[fct_countr].y = ((randm[fct_countr])%11)*4;
              if(gb.display.getPixel(randm[fct_countr]+1,(randm[fct_countr]%11)+1)==0){
                gb.display.setColor(BLACK);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1)+1);
              }else{
                gb.display.setColor(WHITE);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1));
                gb.display.setColor(BLACK);
              }
           }
           break;
      
      case 1:
           allPlayer[fct_countr].y = 10;
           check=0;
           while(check==0 && allPlayer[fct_countr].y>0){
             if(gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1)==0 && gb.display.getPixel(randm[fct_countr]+1,allPlayer[fct_countr].y+1+1)==1){
               allPlayer[fct_countr].y = allPlayer[fct_countr].y*4;
               check=1;
             }else{
               allPlayer[fct_countr].y = allPlayer[fct_countr].y-1;
             }
           }
           
           if(check==0){

              allPlayer[fct_countr].y = ((randm[fct_countr])%11)*4;
              if(gb.display.getPixel(randm[fct_countr]+1,(randm[fct_countr]%11)+1)==0){
                gb.display.setColor(BLACK);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1)+1);
              }else{
                gb.display.setColor(WHITE);
                gb.display.drawPixel(randm[fct_countr]+1,((randm[fct_countr]%11)+1));
                gb.display.setColor(BLACK);
              }
           }
           break;
    }

    //initial direction (1:right, -1:left)
    if(allPlayer[fct_countr].x>40){allPlayer[fct_countr].dir = 0;}
    else{allPlayer[fct_countr].dir = 1;}
  }
}

//##################################################################
//##################################################################
void fnctn_checkJump(){
  switch(jumpStatus){
    case 6:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-3;
         break;
         
    case 5:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-2;
         break;
         
    case 4:
         allPlayer[currentPlayer].y=allPlayer[currentPlayer].y-1;
         break;
  }
  if(jumpStatus>0){jumpStatus=jumpStatus-1;}
}

//##################################################################
//##################################################################
void fnctn_checkPlayerPos(){
  //check all players

  for(fct_countr2=0;fct_countr2<nbPlayer*nbTeam;fct_countr2++){
    //s'il n'y a rien
    
    if(gb.display.getPixel(allPlayer[fct_countr2].x,allPlayer[fct_countr2].y+4)==0 && gb.display.getPixel(allPlayer[fct_countr2].x+3,allPlayer[fct_countr2].y+4)==0){
      
      for(fct_countr3=0; fct_countr3<allPlayer[fct_countr2].fall+1;fct_countr3++){
        if(gb.display.getPixel(allPlayer[fct_countr2].x,allPlayer[fct_countr2].y+4)==0 && gb.display.getPixel(allPlayer[fct_countr2].x+3,allPlayer[fct_countr2].y+4)==0){
          if(allPlayer[fct_countr2].y>48 && allPlayer[fct_countr2].y<200){
            if(allPlayer[fct_countr2].dead==0){
              teamInfo[allPlayer[fct_countr2].team].nbAlive = teamInfo[allPlayer[fct_countr2].team].nbAlive-1;
              power = 0;
              allPlayer[fct_countr2].life=0;
              allPlayer[fct_countr2].fall=0;
              allPlayer[fct_countr2].dead=1;
            }
          }else{
            allPlayer[fct_countr2].y=allPlayer[fct_countr2].y+1;
          }
        }
      }
      if(allPlayer[fct_countr2].fall<4 && allPlayer[fct_countr2].dead==0){allPlayer[fct_countr2].fall=allPlayer[fct_countr2].fall + 1;}
    }else{
      allPlayer[fct_countr2].fall=0;
    }
    if(gb.display.getPixel(allPlayer[fct_countr2].x+2,allPlayer[fct_countr2].y+3)==1 
      || gb.display.getPixel(allPlayer[fct_countr2].x+1,allPlayer[fct_countr2].y+3)==1 
      && allPlayer[fct_countr2].dead==0){
      allPlayer[fct_countr2].y=allPlayer[fct_countr2].y-1;
      allPlayer[fct_countr2].fall = 0;
    }
  }
}

//##################################################################
//##################################################################
void fnctn_nextPlayer(){
  do{
    currentTeam = (currentTeam + 1)%nbTeam;
  }while(teamInfo[currentTeam].nbAlive==0);
  
  do{
    currentPlayer = (currentTeam*nbPlayer)+(teamInfo[currentTeam].lastPlayer+1)%nbPlayer;
    teamInfo[currentTeam].lastPlayer = (teamInfo[currentTeam].lastPlayer+1)%nbPlayer;
  }while(allPlayer[currentPlayer].dead==1);
  allPlayer[currentPlayer].timer = consttimer;
  power = 0;
  angle = 4;
  ia.angle = 2;
  ia.power = 10;
  ia.targetlocked=0;
  gamestatus = SELECT_UNIT;
  fnctn_gameOver();
}


//##################################################################
//##################################################################
void fnctn_selectUnit(){
  out_countr3=0;
  outpt_selectUnit();
  if(out_countr3==0){gamestatus=RUNNING;}
}


//##################################################################
//##################################################################
void fnctn_checkDead(){
  //only check if killed by rocket
  for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
    //if player is still alive
    if(allPlayer[fct_countr].dead==0){
      //if player is hitten by bullet
      if(allPlayer[fct_countr].x <= rocket.x && allPlayer[fct_countr].x+4 >= rocket.x && allPlayer[fct_countr].y <= rocket.y && allPlayer[fct_countr].y+4 >= rocket.y ){
        allPlayer[fct_countr].life = allPlayer[fct_countr].life-1;
        if(allPlayer[fct_countr].life<=0){
          power = 0;
          allPlayer[fct_countr].dead=1;
          teamInfo[allPlayer[fct_countr].team].nbAlive = teamInfo[allPlayer[fct_countr].team].nbAlive-1;
        }else{
          allPlayer[fct_countr].timer = consttimer;
        }
      } 
    }    
  }
}

//##################################################################
//##################################################################
void fnctn_gameOver(){
  byte nbAliveTeam = 0;
  //gamestatus=GAMEOVER;
  for(fct_countr=0;fct_countr<nbTeam;fct_countr++){
    if(teamInfo[fct_countr].nbAlive>0){nbAliveTeam=nbAliveTeam+1;}
  }
  if(nbAliveTeam<=1){
    gamestatus = GAMEOVER;
    //gamestatus = DAMAGE;
  }
}





// ##################################################################################################################################
// ################## BULLET RELATED FUNCTIONS ######################################################################################
// ##################################################################################################################################

//##################################################################
//##################################################################
void fn_nextProjPosition(){
  rocket.x = rocket.x + (rocket.x_traj/3);
  rocket.y = rocket.y + (rocket.y_traj/3);
  rocket.x_traj = rocket.x_traj + 0; //Wind
  rocket.y_traj = rocket.y_traj + gravity; //Gravity
  if(abs(rocket.x_traj)>12){rocket.x_traj=12*rocket.x_traj/abs(rocket.x_traj);}
  if(abs(rocket.y_traj)>12){rocket.y_traj=12*rocket.y_traj/abs(rocket.y_traj);}
}

//##################################################################
//##################################################################
void fn_checkCollision(){
  if(rocket.x>84 || rocket.x<0 || (rocket.y>48 && rocket.y<100)){
    fnctn_nextPlayer();
  }else{
    if(landscape[rocket.x/4][rocket.y/4]<60 && rocket.y/4>=0){gamestatus=BOOM;}
    for(fct_countr=0;fct_countr<nbPlayer*nbTeam;fct_countr++){
      if(allPlayer[fct_countr].x <= rocket.x && allPlayer[fct_countr].x+4 >= rocket.x && allPlayer[fct_countr].y <= rocket.y && allPlayer[fct_countr].y+4 >= rocket.y ){gamestatus=BOOM;}
    }
  }
}



// ##################################################################################################################################
// ################# LANDSCAPE RELATED FUNCTIONS ####################################################################################
// ##################################################################################################################################

//##################################################################
//##################################################################
void fnctn_newlevel() {
  // create landscape array from level bitmap    
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(BLACK);
  gb.display.drawRect(0, 0, 23, 14);
  gb.display.drawBitmap(1,1,levels[screen]);
 
  fnctn_definePlayer();
  fnctn_buildLandscape();
      
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,23,14);
  gb.display.setColor(BLACK);  
}

//##################################################################
//##################################################################
void fnctn_buildLandscape(){
  for (fct_countr=1; fct_countr<13; fct_countr++) {
    for (fct_countr2=1; fct_countr2<22; fct_countr2++) {
      if(gb.display.getPixel(fct_countr2,   fct_countr))  {pixelCheck.L0=0;} else {pixelCheck.L0=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr-1)){pixelCheck.L1=0;} else {pixelCheck.L1=60;}
      if(gb.display.getPixel(fct_countr2,   fct_countr-1)){pixelCheck.L2=0;} else {pixelCheck.L2=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr-1)){pixelCheck.L3=0;} else {pixelCheck.L3=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr))  {pixelCheck.L4=0;} else {pixelCheck.L4=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr))  {pixelCheck.L5=0;} else {pixelCheck.L5=60;}
      if(gb.display.getPixel(fct_countr2-1, fct_countr+1)){pixelCheck.L6=0;} else {pixelCheck.L6=60;}
      if(gb.display.getPixel(fct_countr2,   fct_countr+1)){pixelCheck.L7=0;} else {pixelCheck.L7=60;}
      if(gb.display.getPixel(fct_countr2+1, fct_countr+1)){pixelCheck.L8=0;} else {pixelCheck.L8=60;}
      fnctn_unzip(fct_countr2-1, fct_countr-1);   
    }
  }
}
//##################################################################
//##################################################################
void fnctn_rebuildMap(){

  if(rocket.x_traj>=0){
    if(rocket.y_traj>=0){
      if(landscape[(rocket.x/4)-1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)-1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)-1>=0
              && (rocket.x/4)-1<=21
              && rocket.y/4>=0
              && rocket.y/4<=12){
          landscape[(rocket.x/4)-1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
              && rocket.x/4<=21
              && (rocket.y/4)-1>=0
              && (rocket.y/4)-1<=12){
          landscape[rocket.x/4][(rocket.y/4)-1]=60;}
        }
      }else{
        if(rocket.x/4>=0
            && rocket.x/4<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }else{
      if(landscape[(rocket.x/4)-1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)+1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)-1>=0
              && (rocket.x/4)-1<=21
              && rocket.y/4>=0
              && rocket.y/4<=12){
          landscape[(rocket.x/4)-1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
              && rocket.x/4<=21
              && (rocket.y/4)+1>=0
              && (rocket.y/4)+1<=12){
          landscape[rocket.x/4][(rocket.y/4)+1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }
  }else{
    if(rocket.y_traj>=0){
      if(landscape[(rocket.x/4)+1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)-1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)+1>=0
            && (rocket.x/4)+1<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
          landscape[(rocket.x/4)+1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
            && rocket.x/4<=21
            && (rocket.y/4)-1>=0
            && (rocket.y/4)-1<=12){
          landscape[rocket.x/4][(rocket.y/4)-1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }else{
      if(landscape[(rocket.x/4)+1][rocket.y/4]<60 && landscape[rocket.x/4][(rocket.y/4)+1]<60){
        if(abs(rocket.x_traj)>abs(rocket.y_traj)/2){
          if((rocket.x/4)+1>=0
            && (rocket.x/4)+1<=21
            && rocket.y/4>=0
            && rocket.y/4<=12){
          landscape[(rocket.x/4)+1][rocket.y/4]=60;}
        }else{
          if(rocket.x/4>=0
            && rocket.x/4<=21
            && (rocket.y/4)+1>=0
            && (rocket.y/4)+1<=12){
          landscape[rocket.x/4][(rocket.y/4)+1]=60;}
        }
      }else{
        if(rocket.x/4>=0
          && rocket.x/4<=21
          && rocket.y/4>=0
          && rocket.y/4<=12){
        landscape[rocket.x/4][rocket.y/4]=60;}
      }
    }
  }

  for (fct_countr=0; fct_countr<12; fct_countr++) {
    for (fct_countr2=0; fct_countr2<21; fct_countr2++) {
      pixelCheck.L0 = landscape[fct_countr2][fct_countr];
      if (fct_countr2>0 and fct_countr>0)   { pixelCheck.L1 = landscape[fct_countr2-1][fct_countr-1]; } else { pixelCheck.L1=0; }
      if (fct_countr>0)                     { pixelCheck.L2 = landscape[fct_countr2][fct_countr-1];   } else { pixelCheck.L2=0; }
      if (fct_countr2<20 and fct_countr>0)  { pixelCheck.L3 = landscape[fct_countr2+1][fct_countr-1]; } else { pixelCheck.L3=0; }
      if (fct_countr2>0)                    { pixelCheck.L4 = landscape[fct_countr2-1][fct_countr];   } else { pixelCheck.L4=0; }
      if (fct_countr2<20)                   { pixelCheck.L5 = landscape[fct_countr2+1][fct_countr];   } else { pixelCheck.L5=0; }
      if (fct_countr2>0 and fct_countr<11)  { pixelCheck.L6 = landscape[fct_countr2-1][fct_countr+1]; } else { pixelCheck.L6=0; }
      if (fct_countr<11)                    { pixelCheck.L7 = landscape[fct_countr2][fct_countr+1];   } else { pixelCheck.L7=0; }
      if (fct_countr2<20 and fct_countr<11) { pixelCheck.L8 = landscape[fct_countr2+1][fct_countr+1]; } else { pixelCheck.L8=0; }
      
      fnctn_unzip(fct_countr2, fct_countr);
    }
  }
}

//##################################################################
//##################################################################
void fnctn_unzip(byte x, byte y){
  if (pixelCheck.L0<60) {
    landscape[x][y]=0;
    //BLACK

    //1 corner
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L5>=60){landscape[x][y]=10;}
    if(pixelCheck.L4>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=11;}
    if(pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=12;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60){landscape[x][y]=13;}

    //2 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60){landscape[x][y]=20;}
    if(pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=21;}
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=22;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=23;}    

    //3 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60){landscape[x][y]=40;}
    if(pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=41;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=42;}
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=43;}

    //4 corner
    if(pixelCheck.L1>=60 && pixelCheck.L2>=60 && pixelCheck.L3>=60 && pixelCheck.L4>=60 && pixelCheck.L5>=60 && pixelCheck.L6>=60 && pixelCheck.L7>=60 && pixelCheck.L8>=60){landscape[x][y]=50;}
    
  }else{
    landscape[x][y]=60;
    //WHITE
    
    //1 corner
    if(pixelCheck.L2<60 && pixelCheck.L5<60){landscape[x][y]=70;}
    if(pixelCheck.L4<60 && pixelCheck.L7<60){landscape[x][y]=71;}
    if(pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=72;}
    if(pixelCheck.L2<60 && pixelCheck.L4<60){landscape[x][y]=73;}

    //2 corner
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60){landscape[x][y]=80;}
    if(pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=81;}
    if(pixelCheck.L2<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=82;}
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L7<60){landscape[x][y]=83;}    

    //3 corner
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=100;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=101;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=102;}
    //if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=103;}

    //4 corner
    if(pixelCheck.L2<60 && pixelCheck.L4<60 && pixelCheck.L5<60 && pixelCheck.L7<60){landscape[x][y]=110;}

  }
}







//##################################################################
//##################################################################
void fnctn_ia(){
  //this function can run only if landscape + player (only) are displayed (no cursor, no lifebar, no power, no level)
  if(timer > 0){
    timer = timer -1;
  }else{
    if(ia.targetlocked==0){
      //direction
      for(fct_countr=0;fct_countr<2;fct_countr++){
        //angle
        for(fct_countr2=8;fct_countr2>0;fct_countr2--){
          //power
          for(fct_countr3=0;fct_countr3<10;fct_countr3++){
            check = 0; 
            rocket.x = allPlayer[currentPlayer].x+(1+(fct_countr))+(((trajParamY[fct_countr2]/10)*((fct_countr*2)-1)));
            rocket.y = (allPlayer[currentPlayer].y+((trajParamY[fct_countr2]/10))+1);
            rocket.x_traj = trajParamX[fct_countr2]/10*(fct_countr3/2)*((fct_countr*2)-1);
            rocket.y_traj = trajParamY[fct_countr2]/10*(fct_countr3/2);
            while(check == 0){
              for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
                if(    allPlayer[out_countr].x-4 <= rocket.x 
                    && allPlayer[out_countr].x+8 >= rocket.x 
                    && allPlayer[out_countr].y-4 <= rocket.y 
                    && allPlayer[out_countr].y+8 >= rocket.y 
                    && allPlayer[out_countr].dead == 0
                    && allPlayer[out_countr].team != allPlayer[currentPlayer].team
                    ){
                      if(ia.targetlocked==0){
                        allPlayer[currentPlayer].dir = fct_countr;
                        ia.angle = fct_countr2;
                        ia.power = fct_countr3;
                        if(allPlayer[out_countr].x-2 <= rocket.x 
                          && allPlayer[out_countr].x+6 >= rocket.x 
                          && allPlayer[out_countr].y-2 <= rocket.y 
                          && allPlayer[out_countr].y+6 >= rocket.y 
                          && allPlayer[out_countr].dead == 0
                          && allPlayer[out_countr].team != allPlayer[currentPlayer].team)
                      {
                        ia.targetlocked=1;
                        timer = 20;
                      } 
                    }
                    check=1;
                }
                if(rocket.x>84)                         {check=1;}
                if(rocket.x<0)                          {check=1;}
                if(rocket.y>48)                         {check=1;}
                if(landscape[rocket.x/4][rocket.y/4]<60){check=1;}
                fn_nextProjPosition();
              }
            }
          }
        }
      }
      if(ia.targetlocked==0){
        ia.targetlocked=1;
        timer = 20;
      }
    }else{
      //animation
      if(angle!=ia.angle){
        if(angle>ia.angle){angle = angle -1;}
        else {
          angle = angle +1;
          if(angle==ia.angle){timer = 20;}
        }
      }else{
        if(power<ia.power){
          power = power + 1;
        }else{
          rocket.x = allPlayer[currentPlayer].x+(1+(allPlayer[currentPlayer].dir))+(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)));
          rocket.y = (allPlayer[currentPlayer].y+((trajParamY[angle]/10))+1);
          rocket.x_traj = trajParamX[angle]/10*(power/2)*((allPlayer[currentPlayer].dir*2)-1);
          rocket.y_traj = trajParamY[angle]/10*(power/2);
          gamestatus = ANIMFIRE;
        }
      }
    }
  }  
}

