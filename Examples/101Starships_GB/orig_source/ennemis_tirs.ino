void newTirEnnemi(int x, int y, int vx, int vy)
{
  byte i = 0;
 while(tirs_ennemis[i].on == 1)
  {
 i++;
  } 
  if(i>=MAX_TIRS_ENNEMIS){
    i = 0;
  }
  tirs_ennemis[i].on = 1;
  tirs_ennemis[i].x = x*10;
  tirs_ennemis[i].y = y*10;
  tirs_ennemis[i].xvit = vx;
  tirs_ennemis[i].yvit = vy;
}

void updateTirsEnnemis()
{
 byte i = 0;
char xdiff = 0;
char ydiff = 0;
 while(i<MAX_TIRS_ENNEMIS)
 {
   if(tirs_ennemis[i].on == 1){
     EndGame = 1;
     tirs_ennemis[i].x += tirs_ennemis[i].xvit;
     tirs_ennemis[i].y += tirs_ennemis[i].yvit;
     gb.display.drawBitmap(tirs_ennemis[i].x/10-1,tirs_ennemis[i].y/10-1,shot);
     
     
     if(perso.destroy == 0){
       xdiff = perso.x - tirs_ennemis[i].x/10;
       ydiff = perso.y - tirs_ennemis[i].y/10;
       if(xdiff <= 2 && xdiff >= -1 && ydiff <= 1 && ydiff >= -1 && perso.repop <= 0){
         perso.destroy = 1;
        gb.sound.playPattern(player_destroy,0);
       }
     }
     
     if(tirs_ennemis[i].x/10 < -2 || tirs_ennemis[i].x/10 > LCDWIDTH + 2 || tirs_ennemis[i].y/10 < -2 || tirs_ennemis[i].y/10 > LCDHEIGHT -6)
     {
       tirs_ennemis[i].on = 0;
     }
   }
    i++;
 } 
}
