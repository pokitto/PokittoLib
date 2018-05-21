void drawHP(int hp){
    //gui
        game.display.drawBitmap(212,0,UI1);
        game.display.drawFastVLine(219,3,158);
        game.display.drawFastVLine(211,3,158);
        game.display.drawBitmap(212,162,UI2);
        int UI_hp = hp * 0.78;

        if(hp >= 0){
            for(int i = 0; i < UI_hp; i++){
                game.display.drawBitmap(213,159-(i*2),UI4);
            }
            game.display.drawBitmap(213,160-((UI_hp+1)*2),UI3);
            game.display.drawBitmap(213,161,UI5);
        }
}

bool isInventory = false;
void drawInv(int x,int y){
    game.display.setFont(fontAdventurer);


        game.display.setColor(0);
        game.display.fillRect(x,y,100,inventory.size()*17+4); //90
        game.display.setColor(1);
        game.display.drawRectangle(x,y,100,inventory.size()*17+4);

        game.display.drawBitmap(x+2,y+6+(MenuSelector*17),UIS);


        game.display.setCursor(x+8,y+2);
        game.display.color = 1;
        //game.display.print("item 1\n               item 2\n     item 3\n  item 4\n  item 5");

        game.display.print(inventory[0].c_str());

        for ( int i = 0; i < inventory.size(); i++ ) {
            game.display.setCursor(x+8,y+2+(i*17));
            game.display.print(inventory[i].c_str());
            game.display.drawBitmap(x+86,y+2+(i*17),sprites[i+34]);
        }

        if (game.buttons.held(BTN_B,0) || game.buttons.held(BTN_LEFT,0)){
            isInventory = false;
            MenuSelector = 0;
        }
        if (game.buttons.repeat(BTN_UP,4)){
                if(MenuSelector >0)
                MenuSelector --;
        }
        if (game.buttons.repeat(BTN_DOWN,4)){
                if(MenuSelector < inventory.size()-1)
               MenuSelector ++;
        }
        //inventory.size()
}
void drawMenu( int x,int y,int s, int c){
        if(!isInventory){
        if (game.buttons.held(BTN_A,0) || game.buttons.held(BTN_RIGHT,0)){
            switch(MenuSelector){
                case 0:
                    isInventory = true;
                    MenuSelector = 0;
                    break;
                case 1:

                    break;
                case 2:
                    //wait a turn
                    break;
                case 3:

                    break;
                case 4:
                    GameState = StateGame;
                    MenuSelector = 0;
                    break;
            }
        }

        if (game.buttons.held(BTN_B,0) || game.buttons.held(BTN_LEFT,0)){

        }
        if (game.buttons.repeat(BTN_UP,4)){
                if(MenuSelector >0)
                MenuSelector --;
        }
        if (game.buttons.repeat(BTN_DOWN,4)){
                if(MenuSelector <=3)
               MenuSelector ++;
        }
        }
        game.display.setFont(fontAdventurer);
        game.display.setColor(0);
        game.display.fillRect(x,y,65,90);
        game.display.setColor(1+isInventory);
        game.display.drawRectangle(x,y,65,90);


        game.display.setCursor(x+8,y+2);
        game.display.color = 1+isInventory;
        game.display.print("devs:\n  made\n  for\n  Pokitto\n  back");
        if(isInventory){
            drawInv(27,1);
        }
        else game.display.drawBitmap(x+2,y+6+(s*17),UIS);
        game.display.setFont(font5x7);


}


