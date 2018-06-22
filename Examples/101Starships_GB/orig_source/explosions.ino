void newExplosion(byte x, byte y)
{
    byte i = 0;
 while(explosions[i].on == 1)
  {
 i++;
  } 
  if(i>=MAX_EXPLOSIONS){
    i = 0;
  }
  explosions[i].on = 1;
  explosions[i].x = x;
  explosions[i].y = y;
  explosions[i].frame = 0;
}

void updateExplosions()
{
 byte i = 0;
 while(i<MAX_EXPLOSIONS)
 {
   if(explosions[i].on == 1){
    
     switch(explosions[i].frame)
      {
        case 5: { 
          gb.display.drawPixel(explosions[i].x, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+6, explosions[i].y+6);
          gb.display.drawPixel(explosions[i].x-6, explosions[i].y-6);
          gb.display.drawPixel(explosions[i].x+6, explosions[i].y-6);
          gb.display.drawPixel(explosions[i].x-6, explosions[i].y+6);
        }
        break;
        
        case 0: { 
          gb.display.drawPixel(explosions[i].x, explosions[i].y);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
        }
        case 4 : {
          
           gb.display.drawPixel(explosions[i].x+3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y-3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y-3);
          
          gb.display.drawPixel(explosions[i].x+5, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x+5, explosions[i].y-5);
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y-5);
          
          
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-4);
        }
        break;
       case 1: { 
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-3);
          
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y-2);
        }
        case 3: {
          gb.display.drawPixel(explosions[i].x-5, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+5, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x, explosions[i].y+5);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-5);
          
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y-4);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y-4);
        }
        break;
       case 2: { 
          gb.display.drawPixel(explosions[i].x, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y+1);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y-1);
          gb.display.drawPixel(explosions[i].x-1, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+1, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x-2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x-4, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+2, explosions[i].y);
          gb.display.drawPixel(explosions[i].x+4, explosions[i].y);
          
          gb.display.drawPixel(explosions[i].x, explosions[i].y+2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y+4);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-2);
          gb.display.drawPixel(explosions[i].x, explosions[i].y-4);
          
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y+3);
          gb.display.drawPixel(explosions[i].x+3, explosions[i].y-3);
          gb.display.drawPixel(explosions[i].x-3, explosions[i].y-3);
        }
        break;
      } 
     
     
     
     
     
     explosions[i].frame++;
     if(explosions[i].frame >= 6){
       explosions[i].on = 0;
     }


     
     
   }
    i++;
 } 
}
