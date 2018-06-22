void newTir(int vx, int vy, int diffy)
{
  byte i = 0;
 while(tirs[i].on == 1)
  {
 i++;
  } 
  if(i>=MAX_TIRS){
    i = 0;
  }
  tirs[i].on = 1;
  tirs[i].x = ((int)perso.x-1) *10;
  tirs[i].y = ((int)perso.y-diffy) *10+vy;
  tirs[i].xvit = vx;
  tirs[i].yvit = vy;
  gb.sound.playPattern(player_shot,0);
}

void updateTirs()
{
 byte i = 0;
 while(i<MAX_TIRS)
 {
   if(tirs[i].on == 1){
     tirs[i].x += tirs[i].xvit;
     tirs[i].y += tirs[i].yvit;
     gb.display.drawFastHLine(tirs[i].x/10, tirs[i].y/10, 3);
     if(tirs[i].x/10 > LCDWIDTH - 7 || tirs[i].y/10 < 0 || tirs[i].y/10 > LCDHEIGHT-1)
     {
       tirs[i].on = 0;
     }

     
   }
    i++;
 } 
}
