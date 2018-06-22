void updateAff(){
  gb.display.setColor(WHITE);
  gb.display.fillRect(0, 42, LCDWIDTH, 6);
  gb.display.setColor(BLACK);

  
  if(nbframes > 40){
    gb.display.fillRect(16, 44, 2*(perso.supershot-10), 3);
  
    if(score > scoreaff+50){
     scoreaff += 45; 
    }
    if(score > scoreaff+25){
     scoreaff += 25; 
    }
    if(score > scoreaff){
     scoreaff += min(5,score-scoreaff); 
    }
  
    displayInt(scoreaff,60,43,6);      
    
    gb.display.cursorY = 43;
    gb.display.cursorX = 8;
    gb.display.print(perso.vies);
    
    gb.display.drawBitmap(1,LCDHEIGHT-8,vaisseau,ROTCCW,NOFLIP);
  }
  else{
    displayText("HIGHSCORE:",1,43);
    displayInt(highscore,60,43,6);  
    
    
  }
  
}
