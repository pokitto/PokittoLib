//##################################################################
//##################################################################
void outpt_selectMap(){
  byte x,y,z;
  if((((gamelevel)/3)*17)+20>48){
    z=(((gamelevel-3)/3))*17;
  }else{
    z=0;
    gb.display.drawRect(3, 7, 23, 14);
    gb.display.drawBitmap(4,9,options);
  }
  for (out_countr=0; out_countr<nbAvailableLevel; out_countr++) {
    x = 4+(((out_countr+1)%3)*27);
    y = 8+(((out_countr+1)/3)*17)-z;
    if(y<100){
      gb.display.drawRect(x-1, y-1, 23, 14);
      gb.display.drawBitmap(x,y,levels[out_countr]);
      if(out_countr+1==gamelevel){gb.display.drawRect(x-2, y-2, 25, 16);}
    }
  }

  gb.display.setColor(WHITE);
  gb.display.fillRect(0, 0, 84, 6);
  gb.display.setColor(BLACK);
  if(gamelevel==0){
    gb.display.cursorX = 27;
    gb.display.print("Settings");
    gb.display.drawRect(2, 6, 25, 16);
  }else{
    gb.display.cursorX = 28;
    gb.display.print("Level ");
    gb.display.print(gamelevel);
}
}


//##################################################################
//##################################################################
void outpt_options(){
  gb.display.cursorY = 0;
  gb.display.cursorX = 28;
  gb.display.print("Settings");
  
  gb.display.cursorY = 12;
  gb.display.cursorX = 16;
  gb.display.print("Player:  <");
  gb.display.print(nbTeam);
  gb.display.print(">");
  
  gb.display.cursorY = 18;
  gb.display.cursorX = 16;
  gb.display.print("Units:   <");
  gb.display.print(nbPlayer);
  gb.display.print(">");
  
  gb.display.cursorY = 24;
  gb.display.cursorX = 16;
  gb.display.print("Life:    <");
  gb.display.print(unitLife);
  gb.display.print(">");
   
  gb.display.cursorY = 30;
  gb.display.cursorX = 16;
  gb.display.print("NbCPU:   <");
  gb.display.print(nbCpuTeam);
  gb.display.print(">");
  
  gb.display.cursorY = 36;
  gb.display.cursorX = 16;
  gb.display.print("Gravity: <");
  gb.display.print(gravity);
  gb.display.print(">");
  
  gb.display.cursorY = 42;
  gb.display.cursorX = 16;
  gb.display.print("Back");

  gb.display.drawBitmap(11,13+(setting*6),units[0]);
}

//##################################################################
//##################################################################
void outpt_landscape() {
  for (out_countr=0; out_countr<12; out_countr++) {
    for (out_countr2=0; out_countr2<21; out_countr2++) {
      /*gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10]);*/
      
      switch(landscape[out_countr2][out_countr]%10){
        case 0:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10]);
             break;
        case 1:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROT180,NOFLIP);
             break;
        case 2:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROTCW,NOFLIP);
             break;
        case 3:
             gb.display.drawBitmap(out_countr2*4,out_countr*4,landscapetiles[landscape[out_countr2][out_countr]/10],ROTCCW,NOFLIP);
             break;
      }
    }
  }
}

//##################################################################
//##################################################################
void outpt_players() {
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){    
    if(allPlayer[out_countr].dead==0){
      gb.display.drawBitmap(allPlayer[out_countr].x,allPlayer[out_countr].y,units[allPlayer[out_countr].team]);
    }else{
      gb.display.drawFastVLine(allPlayer[out_countr].x+1,allPlayer[out_countr].y,4);
      switch(allPlayer[out_countr].team){
        case 0:
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y+1,3);
          break;
        case 1:
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y,3);
          gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y+2,3);
          break;      
        case 2:
          gb.display.drawPixel(allPlayer[out_countr].x,allPlayer[out_countr].y);
          gb.display.drawPixel(allPlayer[out_countr].x+2,allPlayer[out_countr].y+1);
          break; 
        case 3:
          gb.display.drawPixel(allPlayer[out_countr].x,allPlayer[out_countr].y+1);
          gb.display.drawPixel(allPlayer[out_countr].x+2,allPlayer[out_countr].y);
          break; 
      }
    }
  }
}

//##################################################################
//##################################################################
void outpt_power(){
  if(power>0){
    gb.display.setColor(0);
    gb.display.fillRect(0, 0, 12, 4);
    gb.display.setColor(1);
    gb.display.drawPixel(0,0);
    gb.display.drawPixel(0,3);
    gb.display.drawPixel(11,0);
    gb.display.drawPixel(11,3);
    
    for(out_countr=1; out_countr<power+1; out_countr++){
      gb.display.drawPixel(out_countr,1);
      gb.display.drawPixel(out_countr,2);
    }
  }
}

//################################################################## 
//##################################################################
void outpt_life(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].dead==0 && allPlayer[out_countr].fall==0){
      if(out_countr==currentPlayer && jumpStatus!=0){}
      else{
        gb.display.setColor(BLACK);
        switch(allPlayer[out_countr].life){
          case 3:
               gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y-3,4); 
               break; 
          case 2:
               gb.display.drawFastHLine(allPlayer[out_countr].x,allPlayer[out_countr].y-3,2); 
               break; 
        }
      }
    }
  }
}

//################################################################## 
//##################################################################
void outpt_team(){
  if(power==0){
    gb.display.print("P");
    gb.display.print(currentTeam+1);
  }
}

//################################################################## 
//##################################################################
void outpt_cursor(){

  gb.display.setColor(WHITE);
  gb.display.drawPixel(allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1                             
                      ,allPlayer[currentPlayer].y+1);
  gb.display.drawPixel(allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))                           
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5));

  gb.display.setColor(BLACK);
  gb.display.drawLine (allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  -1                           
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +0
                      ,allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +0                         
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         -1);
  gb.display.drawLine (allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +1                           
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +0
                      ,allPlayer[currentPlayer].x+allPlayer[currentPlayer].dir+1  +(((trajParamX[angle]/10)*((allPlayer[currentPlayer].dir*2)-1)*1.5))  +0                         
                      ,allPlayer[currentPlayer].y+1                               +((trajParamY[angle]/10)*1.5)                                         +1);

}

//##################################################################
//##################################################################
void outpt_projectile(){
  gb.display.drawPixel(rocket.x,rocket.y);
  gb.display.drawPixel(rocket.x+1,rocket.y);
  gb.display.drawPixel(rocket.x,rocket.y-1);
  gb.display.drawPixel(rocket.x+1,rocket.y-1);
}

//##################################################################
//##################################################################
void outpt_selectUnit(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].timer>0){
      out_countr3=1;
      gb.display.drawFastVLine(allPlayer[out_countr].x+1, allPlayer[out_countr].y-6-(allPlayer[out_countr].timer%4), 2);
      gb.display.drawPixel(allPlayer[out_countr].x  , allPlayer[out_countr].y-3-(allPlayer[out_countr].timer%4));
      gb.display.drawPixel(allPlayer[out_countr].x+1, allPlayer[out_countr].y-2-(allPlayer[out_countr].timer%4));
      gb.display.drawPixel(allPlayer[out_countr].x+2, allPlayer[out_countr].y-3-(allPlayer[out_countr].timer%4));
      allPlayer[out_countr].timer=allPlayer[out_countr].timer-1;
    }
  }
}

//##################################################################
//##################################################################
void outpt_damage(){
  for(out_countr=0;out_countr<nbPlayer*nbTeam;out_countr++){
    if(allPlayer[out_countr].timer%3==2){
       gb.display.setColor(WHITE);
       gb.display.fillRect(allPlayer[out_countr].x,allPlayer[out_countr].y,4,4);
       gb.display.setColor(BLACK);
    }
    if(allPlayer[out_countr].timer>0){
      allPlayer[out_countr].timer=allPlayer[out_countr].timer-1;
      out_countr3=1;
    }
  }
}

//##################################################################
//##################################################################
void outpt_pause() {
  gb.display.cursorY = 0;
  gb.display.cursorX = 28;
  gb.display.print("Pause");
  gb.display.cursorY = 15;
  gb.display.cursorX = 16;
  gb.display.print("Back to Game");
  gb.display.cursorY = 25;
  gb.display.cursorX = 16;
  gb.display.print("Quit to New Map");

  gb.display.drawBitmap(11,16+(setting*10),units[0]);
}

//##################################################################
//##################################################################
void outpt_boom() {
  switch(timer){
    case  0:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);          
          break;
    
    case  1:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);          
          break;    
    
    case  2:
          gb.display.setColor(WHITE);
          gb.display.drawPixel(rocket.x-1,rocket.y-1);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-2,rocket.y-1 ,rocket.x-1 ,rocket.y-2);
          gb.display.drawLine(rocket.x  ,rocket.y-1 ,rocket.x-1 ,rocket.y  );         
          break;
    
    case  3:
          gb.display.setColor(WHITE);
          gb.display.fillRect(rocket.x-1,rocket.y-1, 2, 2);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-1 ,rocket.y-2 ,rocket.x   ,rocket.y-2);
          gb.display.drawLine(rocket.x+1 ,rocket.y-1 ,rocket.x+1 ,rocket.y  );
          gb.display.drawLine(rocket.x-2 ,rocket.y-1 ,rocket.x-2 ,rocket.y  );
          gb.display.drawLine(rocket.x-1 ,rocket.y+1 ,rocket.x   ,rocket.y+1);
          break;
    
    case  5:
          gb.display.setColor(WHITE);
          gb.display.drawPixel(rocket.x,rocket.y);
          gb.display.setColor(INVERT);
          gb.display.drawLine(rocket.x-1,rocket.y   ,rocket.x   ,rocket.y-1);
          gb.display.drawLine(rocket.x  ,rocket.y+1 ,rocket.x+1 ,rocket.y  );
          break;
    
    case  6:
          gb.display.setColor(INVERT);
          gb.display.drawPixel(rocket.x,rocket.y);
          break;
    
  }
}


//##################################################################
//##################################################################
void outpt_gameOver(){
    gb.display.setColor(WHITE);
    gb.display.fillRect(4,5,76,24);
    gb.display.setColor(BLACK);
    gb.display.drawRect(5,6,74,22);
    gb.display.cursorY = 11;
    gb.display.cursorX = 17;
    if(allPlayer[currentPlayer].isIA==0){
      gb.display.print("Player ");
    }else{
      gb.display.print("  CPU ");
    }
    gb.display.print(currentTeam+1);
    gb.display.println(" win!");
    gb.display.cursorX = 7;
    switch(randomVal%10){
      case 0: gb.display.println("Others are losers!");break;
      
      case 1: gb.display.println("    Congrats!     ");break;
      
      case 2: gb.display.println(" Better next time ");break;
      
      case 3: gb.display.println("   Frtzz Gzzuit!  ");break;
      
      case 4: gb.display.println(" Game by Frakasss ");break;
      
      case 5: gb.display.println("  Not so bad...   ");break;
      
      case 6: gb.display.println("  WHO'S THE BEST? ");break;
      
      case 7: gb.display.println("Not really fair...");break;
      
      case 8: gb.display.println("Quite boring game.");break;
      
      case 9: gb.display.println(" Just dust in eye ");break;
      
    }
}

//##################################################################
//##################################################################
void outpt_title() {
  gb.titleScreen(gamelogo);
}

//##################################################################
//##################################################################
void outpt_soundfx(byte fxno) {
  gb.sound.command(0,soundfx[fxno][6],0,0); // set volume
  gb.sound.command(1,soundfx[fxno][0],0,0); // set waveform
  gb.sound.command(2,soundfx[fxno][5],-soundfx[fxno][4],0); // set volume slide
  gb.sound.command(3,soundfx[fxno][3],soundfx[fxno][2]-58,0); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1],soundfx[fxno][7],0); // play note
}





