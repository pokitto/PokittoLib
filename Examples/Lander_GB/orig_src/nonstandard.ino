//----------------------------------------------------------------------------
void newgame() {
  score=0;
  lives=3;
  gravity=0;
  gamestatus="selectlevel";
}
//----------------------------------------------------------------------------
void newlevel() {
  
  // create landscape array from level bitmap  
  screen=gamelevel%(maxlevel+1);
  if (gamelevel>maxlevel) {
    gravity=1;
  }
  
  gb.display.setColor(0);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(1);
  gb.display.drawBitmap(0,0,levels[screen]);

  for (int y=0; y<12; y++) {
    for (int x=0; x<21; x++) {

      int l0 = gb.display.getPixel(x,y);
      int l1=1;
      if (x>0 and y>0) { l1 = gb.display.getPixel(x-1,y-1); }
      int l2=1;
      if (y>0) { l2 = gb.display.getPixel(x,y-1); }
      int l3=1;
      if (x<20 and y>0) { l3 = gb.display.getPixel(x+1,y-1); }
      int l4=1;
      if (x>0) { l4 = gb.display.getPixel(x-1,y); }
      int l5=1;
      if (x<20) { l5 = gb.display.getPixel(x+1,y); }
      int l6=1;
      if (x>0 and y<11) { l6 = gb.display.getPixel(x-1,y+1); }
      int l7=1;
      if (y<11) { l7 = gb.display.getPixel(x,y+1); }
      int l8=1;
      if (x<20 and y<11) { l8 = gb.display.getPixel(x+1,y+1); }

      int l=0;
      if (l0==1) {
        l=1;
        if (l4==0) { l=2 + random(4); }
        if (l5==0) { l=6 + random(4); }
        if (l2==0) { l=10 + random(4); }
        if (l7==0) { l=14 + random(4); }
        if (l2==0 and l4==0) { l=18 + random(4); }
        if (l2==0 and l5==0) { l=22 + random(4); }
        if (l4==0 and l7==0) { l=26 + random(4); }
        if (l5==0 and l7==0) { l=30 + random(4); }
        if (l2==0 and l7==0) { l=50 + random(4); }
        if (l4==0 and l5==0) { l=54 + random(4); }
        if (l2==0 and l4==0 and l7==0) { l=34 + random(4); }
        if (l2==0 and l5==0 and l7==0) { l=38 + random(4); }
        if (l2==0 and l4==0 and l5==0) { l=42 + random(4); }
        if (l4==0 and l5==0 and l7==0) { l=46 + random(4); }
        if (l2==0 and l4==0 and l5==0 and l7==0) { l=58; }
      }
      landscape[x][y]=l;
    }
  }

  gb.display.setColor(0);
  gb.display.fillRect(0,0,21,12);
  gb.display.setColor(1);
  gamestatus="newlife";
}
//----------------------------------------------------------------------------
void newlife() {
  shipx=8;
  shipy=0;
  shipxspeed=10;
  shipyspeed=10;
  deadcounter=-1;
  landed=0;
  yeahtimer=0;
  fuel=500;
  gamestatus="running";    
}
//----------------------------------------------------------------------------
void showscore() {
  if (shipy/10>10) {
    gb.display.setColor(0);
    gb.display.fillRect(0,0,84,6);
    gb.display.setColor(1);
    gb.display.cursorY=0;
    gb.display.cursorX=81-4*(score>9)-4*(score>99)-4*(score>999)-4*(score>9999);
    gb.display.print(score);
    gb.display.cursorX=0;
    gb.display.print("F");
    gb.display.drawFastHLine(4,2,fuel/10);
  }
}
//----------------------------------------------------------------------------
void nextlevelcheck() {
  // increment timer after landed
  if (landed==1) {
    yeahtimer=++yeahtimer;
    if (fuel>9) {
      fuel=fuel-10;
      score=score+10;
      if (score%30 == 0) { playsoundfx(3,1); }
    }
    gb.display.setColor(0);
    gb.display.fillRect(5,17,74,7);
    gb.display.setColor(1);
    gb.display.cursorX=6;
    gb.display.cursorY=18;
    gb.display.print("READY FOR LEVEL");
    gb.display.cursorX=70;
    gb.display.print(gamelevel+2);
    if (yeahtimer>=50) {
      gamelevel=++gamelevel;
       gamestatus="newlevel";
       playsoundfx(2,1);
    }          
  }
}
//----------------------------------------------------------------------------
void handledeath() {
  if (deadcounter != -1) {
    deadcounter=--deadcounter;
    gb.display.setColor(0);
    gb.display.fillRect(17,19,50,7);
    gb.display.setColor(1);
    gb.display.cursorX=18;
    gb.display.cursorY=20;
    gb.display.print(lives-1);
    gb.display.cursorX=26;
    gb.display.print("SHIPS LEFT");
    if (deadcounter == 0) {
      deadcounter=-1;
      lives=--lives;  
      if (lives == 0) {
        gamestatus = "gameover";
      } else {
        gamestatus = "newlife";
      }      
    }
  }
}
