//----------------------------------------------------------------------------
void checkbuttons() {
  byte changed=0;
  if (gb.buttons.repeat(BTN_LEFT,1) and deadcounter==-1 and yeahtimer==0) {
    shipxspeed=--shipxspeed;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10+6,shipy/10+1,thrust);
    if (shipxspeed==255) { shipxspeed=0; }
  }      
  if (gb.buttons.repeat(BTN_RIGHT,1) and deadcounter==-1 and yeahtimer==0) {
    shipxspeed=++shipxspeed;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10-4,shipy/10+1,thrust);
    if (shipxspeed>20) { shipxspeed=20; }
  }
  if (gb.buttons.repeat(BTN_A,0) and deadcounter==-1 and yeahtimer==0) {
    shipyspeed=shipyspeed-2;
    fuel=--fuel;
    playsoundfx(0,0);
    if (fuel<50) { playsoundfx(3,1); }
    gb.display.drawBitmap(shipx/10+1,shipy/10+5,thrust);
    if (shipyspeed>250) { shipyspeed=0; }
  }      

  if (gb.buttons.pressed(BTN_C)) {
    gb.titleScreen(F("    Yoda's"),gamelogo);
    gamestatus="title";
  }

  if (fuel<0) { fuel=0; }
  gravitycounter=++gravitycounter;
  if (gravitycounter>2-gravity) {
    shipyspeed=++shipyspeed;
    gravitycounter=0;
    if (shipyspeed>20) { shipyspeed=20; }
  }
  
}
//----------------------------------------------------------------------------
void drawlandscape() {
  for (int y=0; y<12; y++) {
    for (int x=0; x<21; x++) {
      if (landscape[x][y] != 0) {
        gb.display.drawBitmap(x*4,y*4,landscapetiles[landscape[x][y]-1]);
      }
    }
  }
}
//----------------------------------------------------------------------------
void drawspaceship() {
  if (deadcounter==-1 and yeahtimer==0) {
    shipx=shipx+(shipxspeed-10);
    shipy=shipy+(shipyspeed-10);
    if (shipx>780) {
      shipx=780;
    }
    if (shipx<0) {
      shipx=0;
    }
    if (shipy<0) {
      shipy=0;
    }
  }
  
  if (deadcounter==-1) {
    if ((shipyspeed>=11 and shipyspeed<=12 and shipxspeed>8 and shipxspeed<12) or yeahtimer !=0 ) {
      gb.display.drawBitmap(shipx/10,shipy/10,spaceship[1]);
      // check if landed
      if (shipx/10>=goalx[screen]*4 and shipx/10+5<=goalx[screen]*4+11 and shipy/10+6>=goaly[screen]*4 and yeahtimer == 0) {
        landed=1;
      }
    } else {
      gb.display.drawBitmap(shipx/10,shipy/10,spaceship[0]);    
    }
  } else {
    gb.display.drawBitmap(shipx/10-10+deadcounter/4,shipy/10-10+deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10-10+deadcounter/4,shipy/10+10-deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10+10-deadcounter/4,shipy/10-10+deadcounter/4,spaceship[5-deadcounter/10]);
    gb.display.drawBitmap(shipx/10+10-deadcounter/4,shipy/10+10-deadcounter/4,spaceship[5-deadcounter/10]);
    handledeath();
  }

  // draw fuel boxes

  // draw landing platform
  gb.display.setColor(0);
  gb.display.drawFastHLine(goalx[screen]*4,goaly[screen]*4+1,12);
  gb.display.setColor(1);
  gb.display.drawFastHLine(goalx[screen]*4,goaly[screen]*4,12);

}
//----------------------------------------------------------------------------
void checkpickup() {

}
//----------------------------------------------------------------------------
void selectlevel() {
  gb.display.cursorY=18;
  gb.display.cursorX=10;
  gb.display.print("SELECT LEVEL:");
  gb.display.cursorX=66;
  gb.display.print(gamelevel+1);
  gb.display.cursorY=36;
  gb.display.cursorX=2;
  gb.display.print("LEFT/RIGHT TO SELECT");
  gb.display.cursorX=18;
  gb.display.cursorY=42;
  gb.display.print("B TO CONFIRM");
  if (gb.buttons.repeat(BTN_LEFT,2) and gamelevel>0) {
    gamelevel=--gamelevel;
    playsoundfx(4,1);
  }
  if (gb.buttons.repeat(BTN_RIGHT,2) and gamelevel<maxlevel) {
    gamelevel=++gamelevel;
    playsoundfx(4,1);
  }
  if (gb.buttons.repeat(BTN_B,1)) {
    gamestatus="newlevel";
    playsoundfx(4,1);
  }
}
//----------------------------------------------------------------------------
void checkcollission() {
  if (deadcounter == -1 and yeahtimer == 0) {
    int x=shipx/10;
    int y=shipy/10;
    int l0 = gb.display.getPixel(x+1,y)+gb.display.getPixel(x+2,y+1)+gb.display.getPixel(x+3,y+1)+gb.display.getPixel(x+4,y);
    int l1 = gb.display.getPixel(x+1,y+4)+gb.display.getPixel(x+2,y+3)+gb.display.getPixel(x+3,y+3)+gb.display.getPixel(x+4,y+4);
    int l2 = gb.display.getPixel(x+1,y+2)+gb.display.getPixel(x+4,y+2);
    int l3 = 0;
    if (shipyspeed>=11 and shipyspeed<=12 and shipxspeed>8 and shipxspeed<12) {
      l3 = gb.display.getPixel(x,y+5)+gb.display.getPixel(x+5,y+5);
    }
    if (l0+l1+l2+l3 != 0) {
      deadcounter=30;
      playsoundfx(1,1);
    }
  }
}
