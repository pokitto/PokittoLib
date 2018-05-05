#include "Pokitto.h"
#include "Pixonia.h"
#include "Synth.h"

Pokitto::Core gb;

#define YOFF 4
#define XOFF 8

#define WORLDSIZE 50
#define TIMEINCREMENT 16

#define SUBDIVS 4
#define XSPEED 2<<4//2
#define YSPEED 1<<4//1
#define PILLARHEIGHT 2
#define GROUNDLIFT 1

#define MAXSTEP 2
#define MAXWOLFSTEP 3
#define WOLFENERGY 0x26F
#define MONSTERMORNING 75
#define MONSTERNIGHT 130
//#define DEBUG

#define MAXELEVATIONDIFF 1
#define DIGMAX 2

#define MAXHUMANS 10
#define MAXWOLVES 10
#define TREESNROCKS 1
#define HUTPROB 0x01
#define STONEPROB 0x24
#define TREEPROB 0x30
#define GOLDPROB 0x08 //was 7
#define MAXGOLD 50
#define GOLDMULT 3
#define WOLFMULT 1
#define MAPMULT 10
#define GLIMMERPROB 0x20
#define WOLFPROB 0x2000



#define DIR_NE 0
#define DIR_E 1
#define DIR_SE 2
#define DIR_S 3
#define DIR_SW 4
#define DIR_W  5
#define DIR_NW 6
#define DIR_N 7


#define TER_UNDEFINED 0
#define TER_WATER 31
#define TER_PLAIN 1
#define TER_HUT 2
#define TER_TREE 3
#define TER_STONE 4
#define TER_BUMP_NW 5
#define TER_BUMP_SE 6
#define TER_BUMP_NE 7
#define TER_BUMP_SW 8
#define TER_BUMP_N 9
#define TER_BUMP_S 10
#define TER_BUMP_E 11
#define TER_BUMP_W 12
#define TER_GOLD 13
#define TER_ROCK 14
#define TER_BEACHSW 29
#define TER_BEACHSE 30

enum walkDir {
    NE,
    SE,
    SW,
    NW
};

enum humanTasks {
    IDLE,
    CHOPPING,
    RAISING,
    LOWERING,
    PLANTING,
    MOVING,
    HUTBUILDING
};

enum AImodes {
    HOMING,
    INTERCEPT,
    CHASE,
    TURNCW,
    GOTO
};

enum Gamestates {
    TITLE,
    NEXTLEVEL,
    INTRO,
    GAME,
    GAMEOVER,
    LEVELPASSED
};

Gamestates gamestate = TITLE;
int gamelevel=1;
uint16_t worldsize=10;


int32_t wx=-8,wy=0,cx=10,cy=10;
uint16_t prevtile=0, playertile=0, focustile=0, centertile=0,oldcenter, tween_palette[16], golds=0, playergolds=0;
uint16_t timeofday=0; //value>>8 0 is full daytime, 127 is full night
uint8_t wave=0, oldtime=0,wolvestick=0,watertick=0;
int8_t wading=1;
bool focusing=false;
bool inHut = false;
int16_t tweenfactor=0,numwolves=0;

/** function declarations **/
bool raiseTile(uint16_t);
bool lowerTile(uint16_t);
bool caught=false;
int8_t getTileHeightToTile(uint16_t, uint16_t);
uint8_t digCount=0, monsterdrain=1;
uint16_t workingOnTile=0;



struct coords {
    int16_t x;
    int16_t y;
};

struct creature {
    int16_t energy;
    bool visible;
    walkDir facing;
    uint8_t animframe;
    uint16_t tile;
    coords position;
    coords goingto;
    AImodes ai;
};

creature humans[MAXHUMANS];
creature werewolves[MAXWOLVES];

coords player,oldplayer,focus,goldnugget;
uint8_t oldplayercount=0, goldnuggetcount;

uint8_t world[WORLDSIZE*WORLDSIZE];

int getTileHeight(uint16_t i) {
    return (world[i]>>5);
}

int getTileType(uint16_t i) {
    return (world[i]&0x1F);
}

void setTileHeight(uint16_t i, int8_t h) {
    world[i] &= 0x1F; //save tile type, wipe height bits
    world[i] |= (h<<5); //store height in highest 3 bits
}

void setTileType(uint16_t i, uint8_t t) {
    world[i] &= 0xE0; // wipe bottom 5 bits (tile type)
    world[i] |= (t&0x1F); // set bottom 5 bits to type
}

uint16_t tileFromCoords (int16_t x,int16_t y) {
    if (x<0) x=0;
    if (y<0) y=0;
    uint16_t tile = (x/SUBDIVS)+(y/SUBDIVS)*gamelevel*MAPMULT;
    return tile;
}

coords tileToScreenXY (uint16_t tile) {
    coords s;
    uint16_t x = tile % worldsize;
    uint16_t y = tile / worldsize;
    s.y = y*YOFF+wy+(3+x)*YOFF+2+getTileHeight(tile);
    s.x = x*XOFF+wx+2*XOFF+2;
    return s;
}

uint16_t screenToTile (int16_t x, int16_t y) {
    /* tile 0 is at wx,wy in screen space */
    int16_t distx = x-wx;
    int16_t disty = y-wy;
    uint16_t tilesx,tilesy,tx,ty;
    if (distx>0) {
            // point is to the right of 0 tile
            tilesx = distx/XOFF; //number of tiles moved right, XOFF = half width of tile
            tilesy = disty/YOFF; //num tiles downwards
            tx = tilesx + tilesy; // this actually gives on an isometric grid, the x row of tiles !

    } else {
            // point is to the left of 0 tile
            distx = -distx;
            tilesx = distx/XOFF; //number of tiles moved right
            tilesy = disty/YOFF; //num tiles downwards
    }
    /* calculate how many tiles fit in between */


    int32_t tilex = (x - wx - 2 - XOFF)/XOFF;
    int32_t tiley = (y-wy-2);
    tiley -=(3+tilex)*YOFF;
    tiley /= YOFF;
    return tilex+tiley*worldsize;
}

uint16_t worldToTile (uint16_t x, uint16_t y) {
    return (x/SUBDIVS)+(y/SUBDIVS)*worldsize;
}

coords worldToScreen(uint16_t x, uint16_t y) {
    uint16_t scry =(wy+y/SUBDIVS*4)+(x/SUBDIVS+1)*YOFF;
    uint16_t scrx = wx+(x/SUBDIVS+2)*XOFF-y/SUBDIVS*8;
    int16_t oy,ox;
    int16_t offsety,offsetx;
    offsety = y%SUBDIVS;
    offsetx = x%SUBDIVS;
    oy = -offsetx*YOFF/SUBDIVS - offsety*YOFF/SUBDIVS;
    ox =  offsetx*XOFF/SUBDIVS - offsety*8/SUBDIVS;
    coords t;
    t.x = scrx+ox; t.y = scry-oy;
    return t;
}

bool onScreen(coords p) {
    coords t = worldToScreen(p.x,p.y);
    if (t.x>=0 && t.x<110 && t.y > 0 && t.y <88) return true;
    return false;
}

bool focusOnTile(uint16_t t) {
    if (!focusing || t!= focustile) {
        focusing = true;
        focustile = t;
        focus.x = t % worldsize;
        focus.y = t / worldsize;
    }
    uint16_t currentfocus = centertile;//screenToTile(55,44);
    if (currentfocus > focustile-3 && currentfocus <focustile+3) {
            focusing = false;
            return true;
    }
    int16_t x = currentfocus % worldsize;
    int16_t y = currentfocus / worldsize;
    int16_t dx = x - focus.x;
    int16_t dy = y - focus.y;
    if (dx<0) dx = -dx;
    if (dy<0) dy = -dy;
    if (dx+dy>20) dx = XSPEED*8;
    else if (dx+dy>5) dx = XSPEED*2;
    else dx=XSPEED;
    if (x<focus.x) wx-=dx;
    else if (x>focus.x) wx += dx;
    if (y<focus.y) wy-= dx;
    else if (y>focus.y) wy+= dx;
    return false;
    /*if (gb.buttons.upBtn()) {
                wy+=YSPEED;wx+=XSPEED;}
        if (gb.buttons.downBtn()) {wy-=YSPEED;wx-=XSPEED;}
        if (gb.buttons.leftBtn()) {wy-=YSPEED;wx+=XSPEED;}
        if (gb.buttons.rightBtn()) {wy+=YSPEED;wx-=XSPEED;}*/
}

void drawToTile(uint16_t tile, const uint8_t* bitmap, int16_t offsetx, int16_t offsety, uint8_t flip) {
    uint16_t tilex,tiley;
    tiley = tile/worldsize;
    tilex = tile%worldsize;
    uint16_t scry=(wy+tiley*4)+(tilex+1)*YOFF;
    uint16_t scrx=wx+(tilex+2)*XOFF-tiley*8;
    int16_t oy,ox;
    oy = -offsetx*YOFF/SUBDIVS - offsety*YOFF/SUBDIVS+4;
    ox =  offsetx*XOFF/SUBDIVS - offsety*8/SUBDIVS-3;
    int8_t t=0;
    if (!wading) t=-random(10);
    gb.display.drawBitmap(scrx+ox+t,scry-(getTileHeight(tile)<<1)-oy+t,bitmap,0,flip);
}

bool canMoveTo(uint16_t i) {
    if (getTileHeightToTile(i,prevtile)>MAXSTEP) return false;
    if (getTileType(i)==TER_HUT && humans[0].facing != NE ) return false;
    if (getTileType(prevtile)==TER_HUT && humans[0].facing != SW ) return false;
    if (getTileType(i)==TER_TREE) return false;
    //if (getTileType(i)==TER_WATER) return false;
    if (getTileType(i)==TER_STONE) return false;
    return true;
}

bool canWWMoveFromTo(uint16_t i,uint16_t j) {
    if (getTileHeightToTile(j,i)>MAXWOLFSTEP) return false;
    if (getTileType(j)==TER_HUT) return false;
    //if (getTileType(i)==TER_WATER) return false;
    //if (getTileType(i)==TER_STONE) return false;
    return true;
}

int8_t getTileDX(uint16_t i) {
    int8_t h_this = getTileHeight(i);
    uint16_t y = i / worldsize;
    uint16_t x = i % worldsize;
    int8_t h_prev = h_this, h_next=h_this ;// assume flat in case we are at the edge
    if (x>0) h_prev = getTileHeight(i-1);
    if (x<worldsize-1) h_next = getTileHeight(i+1);
    if (h_prev>h_next) return -1; //sloping down towards east
    else if (h_prev<h_next) return 1; // sloping down towards west
    return 0; // this axis is flat
}

int8_t getTileDY(uint16_t i) {
    int8_t h_this = getTileHeight(i);
    uint16_t y = i / worldsize;
    uint16_t x = i % worldsize;
    int8_t h_prev = h_this, h_next=h_this ;// assume flat in case we are at the edge
    if (y>0) h_next = getTileHeight(i-worldsize);
    if (y<worldsize-1) h_prev = getTileHeight(i+worldsize);
    if (h_prev<h_next) return 1; //rising up towards ne
    else if (h_prev>h_next) return -1; // sloping down towards ne (yes, this axis is mirrored)
    return 0; // this axis is flat
}

int8_t getTileHeightToTile(uint16_t a, uint16_t b) {
    int8_t c = getTileHeight(a);
    int8_t d = getTileHeight(b);
    return c-d;
}

uint8_t getTileSlope(uint16_t index) {
    int8_t dx=0,dy=0;
    dx=getTileDX(index);
    dy=getTileDY(index);
    switch (dy) {
    case 0:
        if (dx==0) return TER_PLAIN;
        else if (dx>0) return TER_BUMP_NW;
        return TER_BUMP_SE;
        break;
    case 1:
        if (dx==0) return TER_BUMP_SW;
        else if (dx>0) return TER_BUMP_W;
        return TER_BUMP_S;
        break;
    case -1:
        if (dx==0) return TER_BUMP_NE;
        else if (dx>0) return TER_BUMP_N;
        return TER_BUMP_E;
        break;
    }
}

void setSlopeAutomatic(uint16_t t) {
    uint8_t e = getTileType(t); // currently existing tile
    if (e<5 || e>12 && e != TER_PLAIN) return; // only change terrain formation tiles, not content
    uint8_t slope = getTileSlope(t);
    setTileType(t,slope);
}

bool recursiveGroundRaise(uint16_t tile) {
    uint16_t target=tile;
    if (tile>worldsize) {
        // Top row
        if (tile%worldsize) {
                target = target - worldsize - 1; //N tile
                if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target); // recursive function, calls back to itself
        }
        target++; // NE tile
        if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
        if (tile%worldsize<worldsize-1) {
            target++; // E tile
            if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
        }
    }
    // Middle row
    if (tile%worldsize>0) {
            target = tile - 1; // NW tile
            if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
    }
    if (tile%worldsize<worldsize-1) {
            target+=2; //SE tile
            if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
    }
    // Bottom row
    if (tile<worldsize*worldsize-worldsize) {
        if (tile%worldsize>0) {
            target = tile + worldsize -1; // W tile
            if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
        }
        target++; // SW tile
        if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
        if (tile<worldsize*worldsize-1) {
            target++; //S tile
            if (getTileHeightToTile(target,tile)<-MAXELEVATIONDIFF) raiseTile(target);
        }
    }


    return false;
}

bool raiseTile(uint16_t t) {
    int8_t z = getTileHeight(t);
    if (z==0) setTileType(t,TER_PLAIN);
    z++;
    if (z>7) z=7;
    setTileHeight(t,z);
    recursiveGroundRaise(t);
    if (z==7) digCount = DIGMAX;
    return true;
}

uint16_t getFacingTile(uint16_t tile, walkDir facing) {
    switch (facing) {
    case NE:
        if (tile<worldsize) return -1; //out of bounds
        return tile-worldsize;
        break;
    case SE:
        if (tile%worldsize == worldsize-1) return -1; //out of bounds
        return tile+1;
    case SW:
        if (tile>worldsize*worldsize-worldsize) return -1;
        return tile+worldsize;
    case NW:
        if (tile%worldsize == 0) return -1;
        return tile-1;
    }
    return -1;
}

void spawnWolf() {
    for (uint8_t i = 0; i< (gamelevel-1) * WOLFMULT ; i++) {
        if (werewolves[i].energy <= 0) {
            uint16_t k;
            k=random(worldsize*worldsize-1);
                    uint8_t a = (uint8_t)random(3)-1;
                    switch (a) {
                case 0:
                     werewolves[i].ai = HOMING; break;
                case 1:
                    werewolves[i].ai = CHASE; break;
                case 2:
                    werewolves[i].ai = INTERCEPT; break;
                    }

                    werewolves[i].energy = WOLFENERGY;
                    werewolves[i].tile = k;
                    k *= SUBDIVS;
                    werewolves[i].position.x = k%worldsize;
                    werewolves[i].position.y = k/worldsize;
                    numwolves++;
            i=(gamelevel-1)*WOLFMULT; break;
        }
    }
}

void despawnWolf(uint8_t n) {
    werewolves[n].energy=0;
    werewolves[n].visible=false;
    numwolves--;
}

void despawnAllWolves() {
    for (uint8_t i = 0; i< (gamelevel-1)*WOLFMULT ; i++) {
            despawnWolf(i);
    }
}

void moveWolves() {
    for (uint8_t i = 0; i< (gamelevel-1)*WOLFMULT ; i++) {
        if (werewolves[i].energy>0) {
            if (wolvestick) {
            uint16_t fromtile = werewolves[i].tile;
            coords fromposition = werewolves[i].position;
            int16_t aix,aiy,bix,biy;
            switch (werewolves[i].ai) {
        case INTERCEPT:
            if (humans[i].facing==NE) {werewolves[i].position.y--; werewolves[i].animframe = 1 - werewolves[i].animframe; werewolves[i].facing = SE;}
            if (humans[i].facing==SE) {werewolves[i].position.x++; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NW;}
            if (humans[i].facing==SW) {werewolves[i].position.y++; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = SW;}
            if (humans[i].facing==NW) {werewolves[i].position.x--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NE;}
            if (xorshift8()>0xf0) {
                     uint8_t a = (uint8_t)random(3);
                    switch (a) {
                case 0:
                     werewolves[i].ai = HOMING; break;
                case 1:
                    werewolves[i].ai = CHASE; break;
                case 2:
                    werewolves[i].ai = INTERCEPT; break;
                case 3:
                    werewolves[i].ai = TURNCW; break;


                    }
            }
            break;
        case CHASE:
            if (werewolves[i].position.x<oldplayer.x) {werewolves[i].position.x++; werewolves[i].animframe = 1 - werewolves[i].animframe; werewolves[i].facing = SE;}
            if (werewolves[i].position.x>oldplayer.x) {werewolves[i].position.x--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NW;}
            if (werewolves[i].position.y<oldplayer.y) {werewolves[i].position.y++; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = SW;}
            if (werewolves[i].position.y>oldplayer.y) {werewolves[i].position.y--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NE;}
            aiy = werewolves[i].position.y - player.y;
            aix = werewolves[i].position.x - player.x;
            if (aix<0) aix = -aix;
            if (aiy<0) aiy = -aiy;
            if (aix<30 || aiy <30) werewolves[i].ai = HOMING;
            break;
        case TURNCW:
            werewolves[i].goingto.x = werewolves[i].position.x+random(48)-24;
            werewolves[i].goingto.y = werewolves[i].position.y+random(48)-24;
            while (worldToTile(werewolves[i].goingto.x,werewolves[i].goingto.y)>worldsize*worldsize-1) {
                werewolves[i].goingto.x = werewolves[i].position.x+random(48)-24;
                werewolves[i].goingto.y = werewolves[i].position.y+random(48)-24;
            }
            werewolves[i].ai = GOTO;
            break;
        case GOTO:
            if (werewolves[i].position.x<werewolves[i].goingto.x) {werewolves[i].position.x++; werewolves[i].animframe = 1 - werewolves[i].animframe; werewolves[i].facing = SE;}
            if (werewolves[i].position.x>werewolves[i].goingto.x) {werewolves[i].position.x--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NW;}
            if (werewolves[i].position.y<werewolves[i].goingto.y) {werewolves[i].position.y++; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = SW;}
            if (werewolves[i].position.y>werewolves[i].goingto.y) {werewolves[i].position.y--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NE;}
            if (werewolves[i].position.x==werewolves[i].goingto.x && werewolves[i].position.y==werewolves[i].goingto.y) werewolves[i].ai = HOMING;
            break;
        case HOMING:
            if (werewolves[i].position.x<player.x) {werewolves[i].position.x++; werewolves[i].animframe = 1 - werewolves[i].animframe; werewolves[i].facing = SE;}
            if (werewolves[i].position.x>player.x) {werewolves[i].position.x--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NW;}
            if (werewolves[i].position.y<player.y) {werewolves[i].position.y++; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = SW;}
            if (werewolves[i].position.y>player.y) {werewolves[i].position.y--; werewolves[i].animframe = 1 - werewolves[i].animframe;werewolves[i].facing = NE;}
            break;
            }
            werewolves[i].tile = worldToTile(werewolves[i].position.x,werewolves[i].position.y);
            if (canWWMoveFromTo(fromtile,werewolves[i].tile)==false) {
                werewolves[i].position=fromposition;
                werewolves[i].tile= fromtile;
                     uint8_t a = (uint8_t)random(3);
                    switch (a) {
                case 0:
                     werewolves[i].ai = HOMING; break;
                case 1:
                    werewolves[i].ai = CHASE; break;
                case 2:
                    werewolves[i].ai = INTERCEPT; break;
                case 3:
                    werewolves[i].ai = TURNCW; break;
                    }
            }
            }
            if (werewolves[i].position.x>(worldsize-1)*SUBDIVS) werewolves[i].position.x=(worldsize-1)*SUBDIVS;
            if (werewolves[i].position.y>(worldsize-1)*SUBDIVS) werewolves[i].position.y=(worldsize-1)*SUBDIVS;
            if (werewolves[i].position.x<0) werewolves[i].position.x=1;
            if (werewolves[i].position.y<0) werewolves[i].position.y=1;
            if (onScreen(werewolves[i].position)) werewolves[i].visible = true;
            else werewolves[i].visible = false;
            werewolves[i].energy-= monsterdrain;
            if (werewolves[i].energy<=0) despawnWolf(i);
        } else werewolves[i].visible=false;
    }
}

void initworld() {
    timeofday=0; //value>>8 0 is full daytime, 127 is full night
    wave=0; oldtime=0; wolvestick=0;watertick=0;
    playergolds=0;
wading=1;
focusing=false;
inHut = false;
    worldsize = 10+gamelevel*MAPMULT;
    caught=false; humans[0].energy=0xFFF;golds=0;playergolds=0;
    gb.display.load565Palette(pokulous_pal);
    timeofday=random(0x3FFF); tweenfactor=0; golds=0;
    player.x=5*SUBDIVS;
    player.y=0;
    uint16_t i = 0;
    /** Empty the world **/
    for (i=0;i<worldsize*worldsize;i++) {
        setTileHeight(i,0);
        setTileType(i,TER_UNDEFINED);
    }

    #define NUMPEAKS worldsize/10
    uint16_t bigpeaks = NUMPEAKS*random(2)+NUMPEAKS*3;
    uint16_t smallpeaks = NUMPEAKS*random(2)+NUMPEAKS*30;
    uint16_t cliffs = NUMPEAKS*random(2)+NUMPEAKS*3;
    uint16_t shards = NUMPEAKS*random(4)+NUMPEAKS*30;

    switch (random(3)) {
case 0:
    //normal ground
    break;
case 1:
    // lots of land
    bigpeaks += NUMPEAKS*10;
    shards /= 10;
    smallpeaks /= 10;
    break;
case 2:
    // tiny islands
    bigpeaks =0;
    cliffs /=10;
    shards =0;
    smallpeaks /= 10;
    break;
case 3:
    // very rugged
    shards += NUMPEAKS*50;
    bigpeaks = 0;
    smallpeaks /=10;
    }

    /** Generate big peaks **/
    for (uint8_t r=0; r<bigpeaks;r++) {
        uint16_t rx = random(worldsize-16)+8;
        uint16_t ry = random(worldsize-16)+8;
        uint8_t sx = random(NUMPEAKS)+1;
        uint8_t sy = random(NUMPEAKS)+1;
        uint8_t he = random(5)+2;
        for (uint8_t ty = 0; ty < sy; ty++) {
            for (uint8_t tx = 0; tx < sx; tx++) {
                    uint32_t index = rx+tx+(ry+ty) * worldsize;
                    if (index<worldsize*worldsize) {
                        setTileHeight(index,he);
                        setTileType(index,TER_PLAIN);
                    }
        }
        }
    }
    /** Generate small peaks **/
    for (uint16_t r=0; r<smallpeaks;r++) {
        uint16_t rx = random(worldsize-2)+1;
        uint16_t ry = random(worldsize-2)+1;
        uint8_t sx = random(3)+1;
        uint8_t sy = random(3)+1;
        uint8_t he = random(3)+1;
        for (uint8_t ty = 0; ty < sy; ty++) {
            for (uint8_t tx = 0; tx < sx; tx++) {
                    uint32_t index = rx+tx+(ry+ty) * worldsize;
                    if (index<worldsize*worldsize) {
                        setTileHeight(index,he);
                        setTileType(index,TER_PLAIN);
                    }
        }
        }
    }
    /** generate slopes **/
    int8_t level=7;
    while (level>-1) {
        i =0;
        for (int16_t y = 0; y<worldsize; y++) {
        for (int16_t x = 0; x<worldsize; x++, i++) {
            uint8_t a = getTileHeight(i);
            if (a==level) {
                if (a==0) setTileType(i,TER_WATER); // don't leave undefined tiles
                else {
                    // set tile that is northwest(previous on x-axis) of this tile
                    if ((x>0) && getTileType(i-1)==TER_UNDEFINED) {setTileType((i-1),TER_PLAIN); setTileHeight((i-1),level-1);}
                    // set tile that is northeast (previous on logical y-axis) of this tile
                    if ((y>0) && getTileType(i-worldsize)==TER_UNDEFINED) {setTileType((i-worldsize),TER_PLAIN); setTileHeight((i-worldsize),level-1);}
                     // set tile that is southeast(next on x-axis) of this tile
                    if ((x<worldsize-1) && getTileType(i+1)==TER_UNDEFINED) {setTileType((i+1),TER_PLAIN); setTileHeight((i+1),level-1);}
                    // set tile that is southwest (next on logical y-axis) of this tile
                    if ((y<worldsize-1) && getTileType(i+worldsize)==TER_UNDEFINED) {setTileType((i+worldsize),TER_PLAIN); setTileHeight((i+worldsize),level-1);}
                }
            }
        }
    }
    level--;
    }

    /** Generate cliffs **/
    for (uint8_t r=0; r<cliffs;r++) {
        uint16_t rx = random(worldsize-16)+8;
        uint16_t ry = random(worldsize-16)+8;
        uint8_t sx = random(NUMPEAKS)+1;
        uint8_t sy = random(NUMPEAKS)+1;
        uint8_t he = random(5)+2;
        for (uint8_t ty = 0; ty < sy; ty++) {
            for (uint8_t tx = 0; tx < sx; tx++) {
                    uint32_t index = rx+tx+(ry+ty) * worldsize;
                    if (index<worldsize*worldsize) {
                        setTileHeight(index,he);
                        setTileType(index,TER_PLAIN);
                    }
        }
        }
    }

    /** soften cliffs **/
    level=7; int8_t edge = random(6)+1;
    while (level>edge) {
        i =0;
        for (int16_t y = 0; y<worldsize; y++) {
        for (int16_t x = 0; x<worldsize; x++, i++) {
            uint8_t a = getTileHeight(i);
            if (a==level) {
                if (a==0) setTileType(i,TER_WATER); // don't leave undefined tiles
                else {
                    // set tile that is northwest(previous on x-axis) of this tile
                    if ((x>0) && getTileType(i-1)==TER_UNDEFINED) {setTileType((i-1),TER_PLAIN); setTileHeight((i-1),level-1);}
                    // set tile that is northeast (previous on logical y-axis) of this tile
                    if ((y>0) && getTileType(i-worldsize)==TER_UNDEFINED) {setTileType((i-worldsize),TER_PLAIN); setTileHeight((i-worldsize),level-1);}
                     // set tile that is southeast(next on x-axis) of this tile
                    if ((x<worldsize-1) && getTileType(i+1)==TER_UNDEFINED) {setTileType((i+1),TER_PLAIN); setTileHeight((i+1),level-1);}
                    // set tile that is southwest (next on logical y-axis) of this tile
                    if ((y<worldsize-1) && getTileType(i+worldsize)==TER_UNDEFINED) {setTileType((i+worldsize),TER_PLAIN); setTileHeight((i+worldsize),level-1);}
                }
            }
        }
    }
    level--;
    }

    /** Generate small cliffs **/
    for (uint16_t r=0; r<shards;r++) {
        uint16_t rx = random(worldsize-2)+1;
        uint16_t ry = random(worldsize-2)+1;
        uint8_t sx = random(1)+1;
        uint8_t sy = random(1)+1;
        uint8_t he = random(5)+2;
        for (uint8_t ty = 0; ty < sy; ty++) {
            for (uint8_t tx = 0; tx < sx; tx++) {
                    uint32_t index = rx+tx+(ry+ty) * worldsize;
                    if (index<worldsize*worldsize) {
                        setTileHeight(index,he);
                        setTileType(index,TER_PLAIN);
                    }
        }
        }
    }

    /** Check the world **/
    for (i=0;i<worldsize*worldsize;i++) {
        if (getTileType(i)==TER_UNDEFINED) {
            setTileHeight(i,0);
            setTileType(i,TER_WATER);
        }
    }
    /** SET TILE SLOPE TYPES **/

    for (uint16_t j=0; j<worldsize*worldsize;j++) {
        if (getTileHeight(j)) setTileType(j,getTileSlope(j));
        #if TREESNROCKS
        if (getTileType(j)!=TER_WATER) {
                uint8_t r=xorshift8();
                if (r>0xff-HUTPROB) {setTileType(j,TER_HUT);setTileHeight(j,7);}
                else if (r>0xff-GOLDPROB && golds<MAXGOLD) {
                        setTileType(j,TER_GOLD);
                        //golds++;
                }
                else if (r>0xff-STONEPROB) setTileType(j,TER_STONE);
                else if (r>0xff-TREEPROB) setTileType(j,TER_TREE);
        }
        #endif
    }

    i=0; uint16_t huts[100],numhuts=0;

    /** BEACHES **/

    for (uint16_t j = 0; j<worldsize*worldsize; j++) {
                if (world[j]==TER_WATER) {
                uint16_t y = j/worldsize+1;
                uint16_t x = j % worldsize;
                if ((j%worldsize)&&(world[j-1]!=TER_WATER)&&(world[j+1]==TER_WATER)&&x>0) world[j-1]=TER_BEACHSE;
                if (j>worldsize&&(world[j-worldsize]!= TER_WATER)&&(world[j+worldsize]==TER_WATER)&&y<worldsize-1) world[j-worldsize]=TER_BEACHSW;
                }
            }
    //setTileType(0,TER_HUT);
    //setTileType(5,TER_HUT);
    /** CLEAR FRONT PORCHES OF HUTS, count golds **/

    for (uint16_t j = 0; j<worldsize*worldsize; j++) {
        //if (getTileType(j)==TER_GOLD) golds++;
        if (getTileType(j)==TER_HUT) {
            if (j < (worldsize-1)*worldsize) {
                    setTileType(j+worldsize, TER_PLAIN);//clear porch
                    huts[numhuts++]=j;
            } else {setTileType(j, TER_PLAIN);} // invalid dwelling
        }
    }
    /** MAKE SURE AT LEAST 1 HUT **/
    if (numhuts==0) {
            uint16_t ut = worldsize*worldsize/2+worldsize/2;
            setTileType(ut,TER_HUT);
            setTileHeight(ut,4);
            setTileType(ut+worldsize,TER_PLAIN);
    }

    /** PUT GUY ON MAP **/
    uint16_t t = random(numhuts);
    //if (numhuts<1)
        playertile = (uint16_t)random(worldsize*worldsize);
    //else playertile = huts[t]+2*worldsize;//110;//worldsize/2+worldsize/2*worldsize;//huts[t];
    //playertile=202;
    //setTileHeight(102,7);
    //setTileType(102,TER_PLAIN);
    //if (playertile > worldsize*worldsize) playertile = random(worldsize);
    player.x = (playertile%worldsize)*SUBDIVS;
    player.y = playertile/worldsize*SUBDIVS;
//focusOnTile(playertile);

    /** Count gold on map **/
    golds=0;
    for (uint16_t i=0;i<worldsize*worldsize;i++) {
    if (getTileType(i)==TER_GOLD) golds++;
    }
    /** Make sure enough gold on the map **/
    while (golds<gamelevel*GOLDMULT) {
        setTileType(random(worldsize*worldsize-1),TER_GOLD);
        golds++;
    }
}

void drawtile(int32_t x, int32_t y, const uint8_t* bmp) {
    gb.display.drawBitmap(x+cx, y+cy, bmp);
}

void drawhuman(uint8_t n) {
    switch (humans[n].facing) {
    case NE:
        if (!humans[n].animframe) drawToTile(playertile,pixelman_nw_1,player.x%SUBDIVS,player.y%SUBDIVS,1);
        else drawToTile(playertile,pixelman_nw_2,player.x%SUBDIVS,player.y%SUBDIVS,1);
        break;
    case SE:
        if (!humans[n].animframe) drawToTile(playertile,pixelman_se_1,player.x%SUBDIVS,player.y%SUBDIVS,0);
        else drawToTile(playertile,pixelman_se_2,player.x%SUBDIVS,player.y%SUBDIVS,0);
        break;
    case SW:
        if (!humans[n].animframe) drawToTile(playertile,pixelman_se_1,player.x%SUBDIVS,player.y%SUBDIVS,1);
        else drawToTile(playertile,pixelman_se_2,player.x%SUBDIVS,player.y%SUBDIVS,1);
        break;
    case NW:
        if (!humans[n].animframe) drawToTile(playertile,pixelman_nw_1,player.x%SUBDIVS,player.y%SUBDIVS,0);
        else drawToTile(playertile,pixelman_nw_2,player.x%SUBDIVS,player.y%SUBDIVS,0);
        break;
    default:
        while(1) {
            if(gb.update()) gb.display.print("ERROR!!!");
        }
    }
}

void spawnGold(uint16_t t) {

}

void drawwolf(uint8_t n) {
    switch (werewolves[n].facing) {
    case NE:
        if (!werewolves[n].animframe) drawToTile(werewolves[n].tile,werewolf_nw_1,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,1);
        else drawToTile(werewolves[n].tile,werewolf_nw_2,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,1);
        break;
    case SE:
        if (!werewolves[n].animframe) drawToTile(werewolves[n].tile,werewolf_se_1,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,0);
        else drawToTile(werewolves[n].tile,werewolf_se_2,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,0);
        break;
    case SW:
        if (!werewolves[n].animframe) drawToTile(werewolves[n].tile,werewolf_se_1,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,1);
        else drawToTile(werewolves[n].tile,werewolf_se_2,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,1);
        break;
    case NW:
        if (!werewolves[n].animframe) drawToTile(werewolves[n].tile,werewolf_nw_1,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,0);
        else drawToTile(werewolves[n].tile,werewolf_nw_2,werewolves[n].position.x%SUBDIVS,werewolves[n].position.y%SUBDIVS,0);
        break;
    }
}

void drawworld(int32_t x, int32_t y) {

    uint16_t i=0;
    uint8_t tiletype;
    int8_t tileheight;
    int32_t scrx=x, scry=y;
    wave+=6;

    centertile =-1;
    int16_t rowx = 0;
    inHut = false;
    bool hdrawn=false;
    for (uint8_t iy=0;iy<worldsize;iy++,rowx-=8) {
        scry=y+iy*4;
        scrx=x;
        for (uint8_t ix=0;ix<worldsize;ix++, i++, scrx+=XOFF, scry+=YOFF) {
            if (scrx+rowx>-5*XOFF  && scry>-5*YOFF && scrx+rowx < 110+XOFF && scry < 88+ YOFF ) {
            tiletype = getTileType(i);
            tileheight = getTileHeight(i);
            bool pillar=false;
            if (tileheight>PILLARHEIGHT) pillar=true;
            tileheight <<= GROUNDLIFT;

            switch (tiletype) {
            case TER_WATER:
                //wave=xorshift8();
                if ((wave>0x7F)) {
                    if (ix&1) drawtile(scrx+rowx,scry-tileheight,ter_water1);
                    else drawtile(scrx+rowx,scry-tileheight,ter_water2);
                } else {
                    if (ix&1) drawtile(scrx+rowx,scry-tileheight,ter_water2);
                    else drawtile(scrx+rowx,scry-tileheight,ter_water1);
                }
                break;
            case TER_PLAIN:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_plain);
                break;
            case TER_GOLD:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_gold);
                break;
            case TER_HUT:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-10-tileheight,ter_hut);
                if (i==playertile) {
                        drawToTile(i,anim_blue_flag1,2,0,0);
                        inHut=true;
                }
                break;
            case TER_BEACHSE:
                drawtile(scrx+rowx,scry-tileheight-1,ter_beach_se);
                break;
            case TER_BEACHSW:
                drawtile(scrx+rowx,scry-tileheight-1,ter_beach_sw);
                break;
            case TER_TREE:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-10-tileheight,ter_tree);
                break;
            case TER_STONE:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_stone);
                break;
            case TER_BUMP_NW:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_nw);
                break;
            case TER_BUMP_NE:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_ne);
                break;
            case TER_BUMP_SE:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_se);
                break;
            case TER_BUMP_SW:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_sw);
                break;
            case TER_BUMP_W:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_w);
                break;
            case TER_BUMP_E:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_e);
                break;
            case TER_BUMP_S:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight-2,ter_bump_s);
                break;
            case TER_BUMP_N:
                if (pillar) drawtile(scrx+rowx,scry-tileheight,dirt_pillar);
                drawtile(scrx+rowx,scry-tileheight,ter_bump_n);
                break;
            default:
                break;
            }
            if (scrx+rowx<45 && scrx+rowx+2*XOFF>45) {
                if (scry-tileheight<40 && scry+tileheight+8>40) centertile=i;
            }
            if (i==playertile+wading && getTileType(i-wading) != TER_HUT) {drawhuman(0); hdrawn=true;}
            else if (i==playertile && getTileType(i) != TER_HUT && hdrawn == false) drawhuman(0);
            for (uint8_t r=0;r<(gamelevel-1)*WOLFMULT;r++) {
                if (werewolves[r].visible) {
                    if (i==werewolves[r].tile) drawwolf(r);
                    if (werewolves[r].tile == playertile) {
                            caught=true;
                    }
                }
            }
            if (caught) {
                    if (xorshift8()>0xFD) drawToTile(playertile,splatter,random(3)-1,random(3)+1,random(1));
                    humans[0].energy--;
                    if (humans[0].energy<0) gamestate = GAMEOVER;
            }
            }
        }
    }
}

DigitalOut ptest(EXT0); //P1_19

#define TEST_PORT          1
#define TEST_PIN           19
#define CLR_TEST LPC_GPIO_PORT->CLR[TEST_PORT] = 1 << TEST_PIN;
#define SET_TEST LPC_GPIO_PORT->SET[TEST_PORT] = 1 << TEST_PIN;

int s=0;

int main () {
    gb.sound.playMusicStream("play22u.snd");

    gb.begin();
    gb.display.load565Palette(pokulous_pal);
    //gb.display.load565Palette(night_palette);
    //gb.display.bgcolor=10;
    gb.display.clear();
    gb.display.setColorDepth(4);
    gb.display.bgcolor = 13;
    gb.display.invisiblecolor = 15;
    gb.sound.ampEnable(true);
    gb.sound.playMusicStream();
    uint8_t timeout=0xFF;

    /** veriables needed in gameloop (can't initialize inside a switch case) **/
    coords prev;
    bool walking;
    uint8_t newtime;
    int tgold=0;


    gb.display.bgcolor = 13;
    while (gb.isRunning()) {
        if (gb.update()) {
        if (s) {SET_TEST}
        else {CLR_TEST}
        s=1-s;
        switch (gamestate) {
    case TITLE:
        while(!gb.buttons.pressed(BTN_A)&&gb.isRunning()) {
            if (gb.update()) {
            gb.display.drawBitmap(0,0,pixonia);
            gb.display.setFont(fontRunes);
            gb.display.color=9;
            gb.display.bgcolor = gb.display.invisiblecolor;
            gb.display.setCursor(9,74);
            if (timeout>127) gb.display.print("Copyright 2017 Pokitto ltd");
            else gb.display.print("     Press 'A' to Start");
            timeout-=2;
            srand(gb.getTime());
            }
        }
        gamestate = NEXTLEVEL;
        initworld();
        break;
    case NEXTLEVEL:
        gb.display.setColor(9,3);
        timeout = 0xf;
        while (timeout--) gb.update();
        while(!gb.buttons.pressed(BTN_A)&&gb.isRunning()) {
        if (gb.update()) {
            gb.display.setFont(fontRunes);
            gb.display.setCursor(0,30);
            gb.display.print("       Level: ");
            gb.display.palette[6]=65312;
            gb.display.setColor(6,3);
            gb.display.println(gamelevel);
            gb.display.setColor(9,3);
            gb.display.println("      ");
            gb.display.print("       Find ");
            gb.display.setColor(6,3);
            gb.display.print(gamelevel*GOLDMULT);
            gb.display.setColor(9,3);
            gb.display.print(" Gold");

        }
        }
        gamestate=GAME;
        break;
    case INTRO:
        gb.display.setColor(9,3);
        timeout = 0xf;
        while (timeout--) gb.update();
        while(!gb.buttons.pressed(BTN_A)&&gb.isRunning()) {
        if (gb.update()) {
            gb.display.setFont(fontRunes);
            gb.display.setCursor(0,30);
            gb.display.print("       \"There's ");
            gb.display.palette[6]=65312;
            gb.display.setColor(6,3);
            gb.display.println("gold");
            gb.display.setColor(9,3);
            gb.display.println("          on the Islands");
            gb.display.println("              of Pixonia!\"");
        }
        }
        gamestate=GAME;
        break;
    case LEVELPASSED:
        gb.display.setColor(9,3);
        timeout = 0xf;
        while (timeout--) gb.update();
        while(!gb.buttons.pressed(BTN_A)&&gb.isRunning()) {
        if (gb.update()) {
            gb.display.setFont(fontRunes);
            gb.display.setCursor(0,30);
            gb.display.print("       You made it! ");
            gb.display.palette[6]=65312;
            gb.display.setColor(6,3);
            //gb.display.println(gamelevel);
            gb.display.setColor(9,3);
            gb.display.println("      ");
            gb.display.print("       Found ");
            gb.display.setColor(6,3);
            gb.display.print(gamelevel*GOLDMULT);
            gb.display.setColor(9,3);
            gb.display.print(" Gold");

        }
        }
        gamelevel++;
        gamestate=NEXTLEVEL;
        initworld();
        break;
    case GAME:
        wolvestick = 1 - wolvestick;
        watertick = 1 - watertick;
        newtime = timeofday>>8;
        if (oldtime > newtime) oldtime=0; // prevent "flash" when timer spins around
        if (oldtime < newtime) {
            oldtime = newtime;
            if (oldtime<127) tweenfactor +=2;
            else {
                    tweenfactor -= 2;
            }
            if (oldtime>150) {
                monsterdrain =4;
            } else monsterdrain =1;
            if (tweenfactor>253) tweenfactor =253;
            if (tweenfactor<2) tweenfactor = 2;
            gb.display.tweenPalette(gb.display.palette,pokulous_pal,night_palette,tweenfactor);
            if (oldtime>MONSTERMORNING && oldtime<MONSTERNIGHT && xorshift16()>0xFFFF-WOLFPROB) {
                    spawnWolf();
                    gb.display.palette[6]=gb.display.palette[6]<<1; // make gold brighter during night
                    if (xorshift8()>0xff-GLIMMERPROB) gb.display.palette[6]=65312; //gold is shining
            }

            #ifndef POK_SIM
            //Pokitto::setBacklight(POK_BACKLIGHT_INITIALVALUE*tweenfactor/255);
            #endif

        }
        if (focusing) focusOnTile(focustile);
        walking=false;
        prev = player;
        prevtile = playertile;
        uint8_t inc;
        if (wading!=1) inc=watertick;
        else inc=1;

        if (caught) inc=0; // we got caught, can't move anymore

        if (gb.buttons.downBtn() && !gb.buttons.aBtn()) {if (player.x < (worldsize-1)*SUBDIVS) player.x+=inc;humans[0].animframe = 1 - humans[0].animframe;humans[0].facing = SE;}
        if (gb.buttons.rightBtn() && !gb.buttons.aBtn()) {if (player.y>0) player.y-=inc;humans[0].animframe = 1 - humans[0].animframe;humans[0].facing = NE;}
        if (gb.buttons.leftBtn() && !gb.buttons.aBtn()) {if (player.y < (worldsize-1)*SUBDIVS) player.y+=inc;humans[0].animframe = 1 - humans[0].animframe;humans[0].facing = SW;}
        if (gb.buttons.upBtn() && !gb.buttons.aBtn()) {if (player.x>0) player.x-=inc; humans[0].animframe = 1 - humans[0].animframe; humans[0].facing = NW;}

        /** scroll world **/
        coords t;
        t = worldToScreen(player.x,player.y);
        if (!focusing) {
        int16_t owx=wx, owy=wy;
        uint8_t slower=0;
        if (t.x>0 && t.x<110 && t.y>0 && t.y<88) {
           slower = 4;
           //if (t.x<20 || t.x>90 || t.y<15 || t.y>65) slower =3;
        }
        if (t.x<40) {wy-=0;wx+=XSPEED>>slower;}
        if (t.y<40) {wy+=YSPEED>>slower;wx+=0;}
        if (t.x>70) {wy+=0;wx-=XSPEED>>slower;}
        if (t.y>50) {wy-=YSPEED>>slower;wx-=0;}
        if (centertile > worldsize*worldsize) {centertile=oldcenter; wx=owx; wy=owy;}
        }
        playertile = worldToTile(player.x,player.y);
        if (getTileType(playertile)==TER_WATER) wading=-1;
        else wading = 1;
        if (playertile != prevtile && canMoveTo(playertile)==false) {
                player=prev;
                playertile= prevtile;
        } else {
                oldplayercount++;
                if (oldplayercount==6) {
                    /*for creature chase ai */
                    oldplayer=player;
                    oldplayercount=0;
                }
        }

        //if (gb.buttons.pressed(BTN_C)) {initworld();}
        if (gb.buttons.pressed(BTN_A)) {
                uint16_t tile = getFacingTile(playertile, humans[0].facing);
                int8_t z = getTileHeight(tile);
                if (z==0) setTileType(tile,TER_PLAIN);
                z++;
                if (z>7) z=7;
                setTileHeight(tile,z);
                setSlopeAutomatic(tile);
        }
        if (gb.buttons.pressed(BTN_B)) {
            uint16_t t = getFacingTile(playertile, humans[0].facing);
            int8_t z = getTileHeight(t);
            uint8_t k = getTileType(t);

            if (getTileType(t)!=TER_HUT) z--;
            if (z<0) z=0;

            if (z<getTileHeight(playertile)-1 || z==0) {
                         if (k==TER_GOLD) {
                    playergolds++;
                    if (playergolds==gamelevel*GOLDMULT) {
                        gamestate=LEVELPASSED;
                    }
                    spawnGold(t);
            }
                if (k < 5 || k > 12) setTileType(t,TER_PLAIN);

            }
            if (z==0) setTileType(t,TER_WATER);
            setTileHeight(t,z);
            setSlopeAutomatic(t);
        }

        moveWolves();
        drawworld(wx,wy);
        oldcenter = centertile;
        gb.display.drawBitmap(1,1,gold);
        gb.display.setCursor(10,0);
        gb.display.setColor(9,gb.display.invisiblecolor);
        timeofday += TIMEINCREMENT;
        if (inHut) timeofday += 7*TIMEINCREMENT;
        gb.display.print(playergolds);
        gb.display.print("/");
        gb.display.println(gamelevel*GOLDMULT);
        gb.display.bgcolor = 13;

        #ifdef DEBUG
        gb.display.bgcolor = 9;
        gb.display.println(numwolves);
        gb.display.print(werewolves[0].position.x);gb.display.print(",");gb.display.println(werewolves[0].position.y);
        gb.display.print(werewolves[0].tile);
        switch (werewolves[0].ai) {
        case HOMING:
            gb.display.println("HOMING");break;
        case CHASE:
            gb.display.println("CHASE");break;
        case INTERCEPT:
            gb.display.println("ITERC");break;
        case GOTO:
            gb.display.println("GOTO");break;
        case TURNCW:
            gb.display.println("TURNCW");break;
        }

        gb.display.print(player.x);gb.display.print(",");gb.display.println(player.y);
        gb.display.println(timeofday>>8);
        #endif // DEBUG
        break;
    case GAMEOVER:

        timeout = 0xf;
        while (timeout--) gb.update();
        while(!gb.buttons.pressed(BTN_A)&&gb.isRunning()) {
        if (gb.update()) {
            gb.display.setFont(fontRunes);
            gb.display.setCursor(0,20);
            gb.display.setColor(14,0);
            gb.display.println("          GAME OVER ");gb.display.println("");
            gb.display.setColor(9);
            gb.display.println("        Player got eaten ");
            gb.display.println("          by the Beast!   ");
            gb.display.println("");
            gb.display.setColor(6);
            gb.display.palette[6]=65312;
              gb.display.print("       Gold collected: ");
            gb.display.print((uint16_t)float(playergolds*100/(gamelevel*GOLDMULT)));
            gb.display.println("%");

        }
        }
        despawnAllWolves();
        wx=-8,wy=0,cx=10,cy=10;
        prevtile=0, playertile=0, focustile=0, centertile=0,oldcenter, tween_palette[16], golds=0, playergolds=0;
        timeofday=0; //value>>8 0 is full daytime, 127 is full nightwave=0, oldtime=0,wolvestick=0,watertick=0;
        wading=1;
        focusing=false;
        inHut = false;
        tweenfactor=0,numwolves=0;
        gamelevel=1;
        initworld();
        gamestate=TITLE;
        break;
        }
        }
    }
}



