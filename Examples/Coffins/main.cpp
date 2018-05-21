#include "Pokitto.h"
#include <vector>
#include <string>
#include "sprites.h"

#include "mapgen.c"
#include "classes.cpp"

#define SEED 344 //Change this value for unique map every time. Hoping for RTC on Hardware!

bool solids[255];

char dungeon[MAPSIZE][MAPSIZE];
int dungeonSize = 16;//Starting dungeon size, which increases each level
int dungeonDepth = 1;//Which floor the player is on

bool solid(char map[][MAPSIZE],int x, int y){
    return solids[map[y][x]];
}

void init_solids(){
    for( int i = 1; i <= ID_COFFIN_OPEN_BOTTOM; i++ )
        solids[i]=true;
}

Pokitto::Core game;

int playerX = 1;
int playerY = 2;


void Ent::draw(){ //Classes are not implemented yet. This wont work either
    game.display.drawBitmap(playerX-x*14,playerY-y*14,sprites[id]);
}

#define MAP_TILES 0
#define ITEM_TILES (MAP_TILES + 34)
#define ENTITY_TILES (ITEM_TILES + 36)

#define ID_PLAYER            ENTITY_TILES + 2

#define ID_STAIRS_DOWN       MAP_TILES + 19
#define ID_GOBLIN_WARRIOR    ENTITY_TILES + 5
#define ID_GOBLIN_MAGE       ENTITY_TILES + 6
#define ID_SKELETON_MAGE     ENTITY_TILES + 7
#define ID_SKELETON_ARCHER   ENTITY_TILES + 8
#define ID_SKELETON_WARIOR   ENTITY_TILES + 9
#define ID_BLOOD_SKELETON    ENTITY_TILES + 10
#define ID_BLOOD             ENTITY_TILES + 11
#define ID_RAT               ENTITY_TILES + 4
#define ID_SCROLL            ITEM_TILES + 3
#define ID_CHEST             ENTITY_TILES + 12
#define ID_CHEST_OPEN        ENTITY_TILES + 13
#define ID_MIMIC             ENTITY_TILES + 14
#define ID_COIN              ITEM_TILES
#define ID_COINS             ITEM_TILES + 1
#define ID_BAG               ITEM_TILES + 2

#define StateGame  0
#define StateMenu  1
#define StateIntro 2
#define StateDead  3

void addDescent(char map[][MAPSIZE]){
    int x=random(1,dungeonSize-1);
    int y=random(1,dungeonSize-1);
    if (!solid(map,x,y) && !solid(map,x-1,y) && !solid(map,x+1,y) && !solid(map,x,y-1) && !solid(map,x,y+1)) {
        map[y][x] = ID_STAIRS_DOWN;
    }
    else {
        addDescent(map);
    }
}

//globals
//std::vector<entity> entities(entities_size);
struct entity{
    uint8_t x;
    uint8_t y;
    int8_t hp;
    uint8_t id;
};
#define ESIZE 2
std::vector<entity> entities(ESIZE);
std::vector<std::string> inventory;

void removeEntity(int i){

        using std::swap;
         std::swap(entities[i], entities.back());
    entities.pop_back();

}

void spawner(int amount){
    entities.clear();
    for(int i = 0; i < amount; i++ ){
        entity spawn;
        bool l = true;
        int sx, sy;
        while(l){
            sx = rand()%dungeonSize;
            sy = rand()%dungeonSize;
            if(dungeon[sy][sx] == 0){
                spawn.id = ENTITY_TILES+4+(rand()%16);//rand()%8+ENTITY_TILES+4;//Skip first few entities for now
                spawn.x = sx;
                spawn.y = sy;
                spawn.hp = rand()%20;
                entities.push_back(spawn);
                l = false;
            }
        }
    }
}


char printer[40] = "";
int playerGold = 0;
int playerHP = 100;
uint8_t GameState = StateIntro;
uint8_t MenuSelector = 0;
#include "gui.h"
#include "crapai.h"
using namespace std;


int main () {
init_solids();
srand(SEED);
mapinit(dungeon,dungeonSize,dungeonSize);
mapgen(dungeon,dungeonSize,dungeonSize,0,0,dungeonSize-1,dungeonSize-1);
mappretty(dungeon,dungeonSize,dungeonSize);
addDescent(dungeon);
std::vector<Ent> ents;
inventory.push_back("adekto");
inventory.push_back("trelemar");
inventory.push_back("VonBednar");
inventory.push_back("wuuff");
Ent Etemp(3,3);
ents.push_back(Etemp);
pokInitSD(); // before game.begin if streaming
game.sound.playMusicStream("coffins/coffins.snd"); // before game.begin if streaming
game.begin();
game.sound.playMusicStream(); //after game.begin so that sound buffer is ready
game.display.setFont(font5x7);

//mapgen(0,0,0,20,20);

game.display.loadRGBPalette(paletteCGA);
//game.display.setFont(fontAdventurer);
//game.display.persistence = true;
game.display.setInvisibleColor(0);

spawner(ESIZE);
/*entities[0].id = 12;
entities[0].x = 5;
entities[0].y = 5;
entities[0].hp = rand()%20;*/

uint8_t introspinner=0xFF;

while (game.isRunning()) {

    if (game.update()) {

        if( GameState == StateIntro){
            if (introspinner<85) {
                game.display.setFont(fontAdventurer);
                game.display.setCursor(game.display.getWidth()/4-16,36);
                game.display.print("Columns & Coffins \n \n         A Pokitto Roguelike \n \n          PRESS A(z)");
                game.display.setFont(font5x7);
            } else if (introspinner>160) {
                game.display.load565Palette(cnctitle_pal);
                game.display.drawBitmap(0,0,cnctitle);
            } else {
                game.display.loadRGBPalette(paletteCGA);
                game.display.drawBitmap(33,70,pokitteam);
            }
            if( game.buttons.held(BTN_A,0) ){
                game.display.loadRGBPalette(paletteCGA);
                GameState = StateGame;
            }
            introspinner--;
            continue;
        }

        if( GameState == StateDead){
            game.display.setFont(fontAdventurer);
            game.display.setCursor(game.display.getWidth()/4,32);
            char over[60];

            sprintf(over,"Game Over \n \n   You died on floor %i \n \n   with %i gold",dungeonDepth,playerGold);
            game.display.print(over);
            //game.display.print(dungeonDepth);
            //game.display.print("\n \n   You made it to year: ");
            //game.display.print("ERR");//Not implemented
            game.display.print("\n \n   PRESS A");
            if( game.buttons.held(BTN_A,0) ){
                GameState = StateIntro;
                dungeonSize = 16;
                dungeonDepth = 1;
                playerHP = 100;
                playerX = 1;
                playerY = 2;
                mapinit(dungeon,dungeonSize,dungeonSize);
                mapgen(dungeon,dungeonSize,dungeonSize,0,0,dungeonSize-1,dungeonSize-1);
                mappretty(dungeon,dungeonSize,dungeonSize);
                addDescent(dungeon);
                spawner(ESIZE);
            }
            game.display.setFont(font5x7);
            continue;
        }

        //If the player is standing on stairs down, generate a new bigger map
        if( dungeon[playerY][playerX] == ID_STAIRS_DOWN ){
            if( dungeonSize + 2 < MAPSIZE ){ //As long as we aren't at maximum size
                dungeonSize += 2;//Increase map x and y by 2
            }
            dungeonDepth++;
            playerX = 1;
            playerY = 2;
            mapinit(dungeon,dungeonSize,dungeonSize);
            mapgen(dungeon,dungeonSize,dungeonSize,0,0,dungeonSize-1,dungeonSize-1);
            mappretty(dungeon,dungeonSize,dungeonSize);
            addDescent(dungeon);
            spawner(ESIZE);
        }

        if (game.buttons.held(BTN_C,0)){
            //doing it this way since more context may happen
            if(GameState == StateGame){
                //game.display.rotatePalette(1);
                GameState = StateMenu;
            }
            else if(GameState == StateMenu){
              //game.display.rotatePalette(-1);
              GameState = StateGame;
              MenuSelector = 0;
              isInventory = false;
            }
        }
        if(GameState == StateGame){
            if( playerHP <= 0){
                GameState = StateDead;
                continue;
            }

            if (game.buttons.repeat(BTN_UP,4)){
                if (!solids[dungeon[playerY-1][playerX]]){
                    if(entitiesLogic( playerX, playerY-1)) playerY --;
                }
            }
            if (game.buttons.repeat(BTN_DOWN,4)){
                if (!solids[dungeon[playerY+1][playerX]]){
                    if(entitiesLogic( playerX, playerY+1)) playerY ++;
                }
            }
            if (game.buttons.repeat(BTN_LEFT,4)){
                if (!solids[dungeon[playerY][playerX-1]]){
                    if(entitiesLogic( playerX-1, playerY))playerX --;
                }
            }
            if (game.buttons.repeat(BTN_RIGHT,4)){
                if (!solids[dungeon[playerY][playerX+1]]){
                    if(entitiesLogic( playerX+1, playerY))playerX ++;
                }
            }
        }


        for(int x =playerX-7; x<playerX+8; x++){ //7
            for(int y =playerY-6; y<playerY+6; y++){
                if(x >= 0 && y >= 0 && x < dungeonSize && y < dungeonSize){
                    game.display.drawBitmap(14*(x-playerX+7),14*(y-playerY+6),sprites[dungeon[y][x]]);
                }
            }
        }

        for(int i=0; i<entities.size(); ++i){
            game.display.color = 0; //remove before release
            game.display.fillRect(14*(entities[i].x-playerX+7),14*(entities[i].y-playerY+6),14,14);//remove and fix before release
            game.display.drawBitmap(14*(entities[i].x-playerX+7),14*(entities[i].y-playerY+6),sprites[entities[i].id]);
        }

        game.display.setCursor(0,168);
        game.display.color = 1;
        game.display.print(printer);



        drawHP( playerHP);
        //ents[0].draw();

        game.display.drawBitmap(14*(7),14*(6),sprites[ID_PLAYER]);

        if(GameState == StateMenu){
            drawMenu( 1,1, MenuSelector,1);
        }
    }

}

return 1;
}

