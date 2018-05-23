#include "Pokitto.h"
Pokitto::Core game;
int width=110;
int height=88;

// squiddy based on Aphrodite's Pico 8 game
// Pokitto port copyright 2017 Dave Akers

extern int16_t lil_map[];   // table of all the level maps
extern int16_t tile_table[];    // table of colors for the map screen
extern uint8_t *bitmap_table[];  // table of pointers to the bitmaps for the game screen
extern uint8_t squiddy[];        // bitmap for squddy, the main character
extern uint8_t grid[];              //bimap for the grid )(used on the title screen)
extern uint8_t ball[];          // bitmap for the ball
extern uint8_t heart[];         // bitmap for the heart, used in a level description
extern uint8_t face[];          // bitmap for the face, used in a level descriiption
extern uint8_t cw[];            // bitmap for the copyright symbol
extern const char *msg_table[64];   // table of text messages for each level
#define TWOPI (3.14159265359 * 2)      // 2 times pi
#define TILESIZE (10)                   // height and width of a tile
#define MINSX (30)                      // ddefine the screnn coordinate limits for squiddy
#define MAXSX (80)                      // the screen scrolls to keep squiddy in these limits
#define MAXSY (60)
#define MINSY (20)
// game states
#define S_TITLE ( 0)
#define S_MAP   ( 1)
#define S_TRANSITION (2)
#define S_PLAY (3)
#define S_WON (4)
#define S_LOST (5)

#define WORDSPERLEVEL (72)

const int NUM_LEVELS = 32;
struct ldata{
    double startx;          // squiddys starting x position, in number of tiles
    double starty;          // squiddys starting x position, in number of tiles
    double startangle;      // xgzfg znbld, in rotations
    double dist;            // distance in tiles between squiddy and the ball
    double speed;           // rotation speed, in rotatio9ns per second
    float attract;          // force of the magnetic attraction, in pixels per second
};
static ldata leveldata[NUM_LEVELS] = {
{ 7.5, 7.5, 0.5,2. , 0.5 , 30 },    // 0
{ 3  , 7. , 0.5,2. , 0.75, 30 },    // 1
{ 3  ,12  , 0.5,1.5,-0.50, 10 },    // 2
{ 3  ,12  , 0.5,2  , 0.75,  1 },    // 3
{ 2  , 2  , 0.0,1.5, 0.60,  1 },    // 4

{ 2  , 6  , 0.25,2. , 0.6 , 10 },    // 5
{ 2  ,13. , 0.5 ,2. ,-0.5 , 20 },    // 6
{ 3  , 3  , 0.75,2  , 0.5 , 15 },    // 7
{ 4  ,11  , 0.25,1.5, 0.5 , 40 },    // 8
{ 2  ,13  , 0.75,2  , 0.50,  1 },    // 9

{ 1.5,13  , 0.75,2.  , 0.5 ,  1 },    // 10
{ 4  , 2. , 0   ,1.5 , 0.5 ,  1 },    // 11
{ 2  , 6  , 0.25,2   , 0.60, 10 },    // 12
{ 8  , 6.5, 0.0 ,3   ,-0.30,  1 },    // 13  sb 9.5, 7.5
{ 2  , 2  , 0.0 ,2   , 0.30, 10 },    // 14

{ 2  , 2. , 0   ,2.  , 0.3 ,  5 },    // 15
{ 4  , 5. , 0   ,2.  , 0.3 ,  1 },    // 16
{11.5, 3.5, 0   ,1.5 , 0.60, 20 },    // 17
{ 5.5, 2.5, 0.0 ,1.5 , 0.60, 20 },    // 18
{ 1.5, 1.5, 0.0 ,1.5 ,-0.40, 20 },    // 19

{ 4  , 2  , 0.  ,1.5 ,-0.5 ,  5 },    // 20
{ 2  , 6. , 0   ,1.5 ,-0.5 ,-10 },    // 21
{ 3  , 4  , 0.0 ,1.5 ,-0.50, 10 },    // 22
{ 7.5, 7.5, 0.0 ,2.5 ,-0.50, 20 },    // 23
{ 1  , 6.5, 0.5 ,1.5 , 0.50, 10 },    // 24  y was 7.5

{ 7.5, 2.5, 0   ,1.4 ,-1.5 , 20 },    // 25
{ 3  , 5. , 0   ,1.5 ,-0.5 , 12 },    // 26
{ 3  , 5. , 0   ,3.  ,-0.50, 10 },    // 27
{ 7.5,11. , 0.0 ,1.5 ,-0.50, 10 },    // 28
{ 1.5,13.5, 0.0 ,1.5 ,-0.50, 10 },    // 29

{ 1  , 1  , 0   ,1.5 ,-0.33, 0  },        // 30
{ 7.5, 4.5, 0.25,2.  , 0.6 , 10 },        // 31


};
/////////////////////////////////////////////////////////////////////////////
//
// Read man - return index for a given map position
//
//////////////////////////////////////////////////////////////////////////////

int8_t shiftval[] = {
    12, 8, 4, 0,
    12, 8, 4, 0,
    12, 8, 4, 0,
    12, 8, 4, 0,
    12, 8, 4, 0 };


int16_t read_map(int x, int y, int level)
{
    int index;
    int col;
    uint16_t data;
    if ((x < 0) || (y < 0)) return (0);
    index = ((level -1) * WORDSPERLEVEL) + ((y/TILESIZE) * 4);
    col = x/TILESIZE;
    index += col/4;         // 4 words oer row
    data = lil_map[index];
    //  one word has 4 nibbles of data col0col1col2ol3
    data = data >> shiftval[col];
    return (data & 0x0f);
}
////////////////////////////////////////////////////////////////////
////
//// draw_world draw the workd using bitmaps
////
////////////////////////////////////////////////////////////////////

void draw_world(int xstart, int ystart, int step, int index,int aframe){
int i,x,y,j;
uint16_t temp;
int color;
int rowstart;
int xfirst;

    xfirst = xstart;
    rowstart = 0;
    while( xfirst < (1 - step))
    {
        xfirst += step;
        rowstart++;
    }

    y= ystart;
    j = 0;
    while ( y < (1-step))
    {
        y = y + step;
        j++;
    }
    index += j << 2;
    for (; (y < height) && (j < 18); y+= step, j++, index += 4)
    {
        x = xfirst;
        for (i = rowstart; (i < 16) && (x < width) ; i++, x += step)
        {
            temp = lil_map[index + (i >> 2)];
            color = (temp >> shiftval[i]) & 0x0f;
            if (color > 0)
            {
                if (color > 3)
                    game.display.drawBitmap(x,y,bitmap_table[color] + aframe);
                else
                    game.display.drawBitmap(x,y,bitmap_table[color]);
            }
        }
    }

}

//////////////////////////////////////////////////////////////////
////
//// draw_map - draw the world using solid colors
////
//////////////////////////////////////////////////////
void draw_map(int xstart, int ystart, int step, int index){
int i,x,y,j;
uint16_t temp;
int color_index;
int rowstart;
int xfirst;

    xfirst = xstart;
    rowstart = 0;
    while( xfirst < (1 - step))
    {
        xfirst += step;
        rowstart++;
    }

    y= ystart;
    j = 0;
    while ( y < (1-step))
    {
        y = y + step;
        j++;
    }
    index += j << 2;
    for (; (y < height) && (j < 18); y+= step, j++, index += 4)
    {
        x = xfirst;
        for (i = rowstart; (i < 16) && (x < width) ; i++, x += step)
        {
            temp = lil_map[index + (i >> 2)];
            color_index = (temp >> shiftval[i]) & 0x0f;
            if (color_index > 0)
            {
                game.display.color = tile_table[color_index];
                game.display.fillRectangle(x,y,step,step);

            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////
////
////  draw_textbox draw a box to write text into
////
////////////////////////////////////////////////////////////////////////////////

void draw_textbox(int x, int y,int boxlen, int boxheight, int bcolor, int frameColor)
{

            game.display.color = bcolor;
            game.display.fillRectangle(x-2,y-3,boxlen,boxheight);
            game.display.color = frameColor;
            game.display.drawRectangle(x-3,y -4,boxlen+1,boxheight+2);

}


int main(){

int xworld, yworld;     // screen coordinates for drawing the world
float ballwx, ballwy;   // world coordinates of the ball
int ballsx, ballsy;     // screen coordinates of the ball
float squidwx,squidwy;  // world coordinates for squiddy
int squidsx,squidsy;    // screen coordinates for squiddy
int release_count;      // number of frames that no buttons were pushed
int level;              // game level
int state, state_count; // game state
ldata *lptr;            // pointer to current level map data
int dist;               // distance between squiddy and the ball
int sx,sy;
int test;
int mag_flags;
int animframe;          // anim frame for arrows
float attract;          // current magnetic attraction
int zoomsize;           // transition size
boolean ball_spins = true;
double angle, angleinc;
game.sound.playMusicStream("music/underclk.snd"); //do before game.begin
game.begin();
game.sound.playMusicStream(); // no parameters to start stream playback
game.display.width = width; // full size
game.display.height = height;
game.setFrameRate(20);
game.display.load565Palette(def565palette);
game.display.setFont(font5x7);
xworld = 0;
yworld = 0;
angle = 0.0;
angleinc = 0.1 ;
squidwx = 50;
squidwy = 40;
animframe = 0;

level = 1;
state = S_TITLE;
state_count = 0;


while (game.isRunning()) {
    if (game.update()) {

#if 0
            // test code to scroll the screen
            if (game.buttons.repeat(BTN_UP,0)){
    			yworld++;
            }
            if (game.buttons.repeat(BTN_DOWN,0)){
    			yworld--;
            }
            if (game.buttons.repeat(BTN_LEFT,0)){
    			xworld++;
            }
             if (game.buttons.repeat(BTN_RIGHT,0)){
    			xworld--;
            }
#endif
        switch(state)
        {


        //game.display.drawPixel(x,y,pixel/16);

    case S_TITLE:
        {
            int i,j;
            // fill the background with the grid
            for (i = 0 ; i < 120 ; i = i + 10)
            {
                for(j = 0; j < 90 ; j= j + 10)
                {
                    game.display.drawBitmap(i,j,grid);
                }
            }
            draw_textbox(10,10,12 * 6, 12, 0, C_DARKGREEN);
            game.display.color = C_GREEN;
            game.display.setCursor(10,10);
            game.display.print("Aphrodite's");

            draw_textbox(30,30,8 * 6, 12, 0, C_DARKGREEN);
            game.display.color = C_GREEN;
            game.display.setCursor(30,30);

            game.display.print("SQUIDDY");


             draw_textbox(20,77,11 * 6, 12, 0, C_WHITE);
            game.display.setCursor(20,77);
             game.display.color = C_WHITE;
             game.display.print("   2017 DSA");
            game.display.drawBitmap(20,77,cw);

            state_count++;
            // blink the "press a" message
            if ((state_count & 0x0f) > 3)
            {
                draw_textbox(30,55,7 * 6, 12, 0, 3);
                game.display.setCursor(30,55);
                game.display.color = C_GREEN;
                game.display.print("PRESS A");

            }
            if (game.buttons.repeat(BTN_A,0))
                state = S_MAP;
                release_count = 0;

        break;
        }
    case S_MAP:
        {
            game.display.color = C_WHITE;
            game.display.setCursor(3,3);
            // show the text message for this level
#if 1
            if (msg_table[level *2] != 0)
                game.display.print(msg_table[level * 2]);
            game.display.setCursor(3,12);
            if (msg_table[(level *2) + 1] != 0)
                game.display.print(msg_table[(level * 2) + 1]);
#endif
            if (level == 9)
            {
                game.display.drawBitmap(16, 3,heart);
                game.display.drawBitmap(16,12,heart);
            }
               if (level == 18)
            {
                game.display.drawBitmap(22, 3,face);

            }
            draw_map(30,20,3,(level-1) * 72);
            lptr = &leveldata[level];
            // draw an x for the players starting position
             sx = (int) (lptr->startx * 3 ) + 30;
             sy = (int) (lptr->starty * 3) + (3 * 3) + 20;
            game.display.color = C_RED;
            game.display.drawLine(sx - 3,sy -3,sx + 3,sy + 3);
            game.display.drawLine(sx - 3,sy +3,sx + 3,sy - 3);

               if (game.buttons.repeat(BTN_A,0) ||
                   game.buttons.repeat(BTN_LEFT,0) ||
                   game.buttons.repeat(BTN_RIGHT,0))
            {
                if (release_count > 1)
                {
                    if (game.buttons.repeat(BTN_A,0))
                    {
                        if (level > 30) level = 1;
                        lptr = &leveldata[level];
                       //  squidwx =  (lptr->startx * TILESIZE);
                       //  squidwy =  (lptr->starty * TILESIZE) + (3 * TILESIZE);
                       //  xworld = (width/2)  - squidwx;
                        // yworld = (height/2) - squidwy - 15;

                           zoomsize = 3;
                           squidwx =  (lptr->startx * zoomsize);
                          squidwy =  (lptr->starty * zoomsize) + (3 * zoomsize);
                          xworld = (width/2)  - squidwx;
                          yworld = (height/2) - squidwy - 15;
                          state_count = 0;
                        angle = lptr->startangle * TWOPI;
                        angleinc = lptr->speed * (TWOPI/20);
                        dist = (int) (lptr->dist * TILESIZE);
                        ball_spins = true;
                        state = S_TRANSITION ; // should be transition
                         attract = lptr->attract / 20.0;
                    }
                    else if (game.buttons.repeat(BTN_LEFT,0))
                    {
                        if (level > 1) level = level -1;
                    }
                    else if (game.buttons.repeat(BTN_RIGHT,0))
                    {
                        if (level < 30) level = level +1;
                    }
                    release_count = 0;

                }
            }
            else
            {
                if (release_count < 5) release_count++ ;
            }
        }
        break;
    case S_TRANSITION:
    {
          zoomsize = 3 + (state_count);
          squidwx =  (lptr->startx * zoomsize);
          squidwy =  (lptr->starty * zoomsize) + (3 * zoomsize);
          xworld = (width/2)  - squidwx;
          yworld = (height/2) - squidwy - 15;
          draw_map(xworld,yworld,zoomsize,(level-1) * 72);

        state_count++;
        if (state_count > (7))
        {
            squidwx =  (lptr->startx * TILESIZE);
            squidwy =  (lptr->starty * TILESIZE) + (3 * TILESIZE);
            xworld = (width/2)  - squidwx;
            yworld = (height/2) - squidwy - 15;
            state = S_PLAY;
        }


    }
    break;
    case S_WON:
    case S_LOST:
    {
        int boxlen,cury;
        ballsx = ballwx + xworld;
            ballsy = ballwy + yworld;

            animframe++;
            if (animframe > 9) animframe = 0;

            game.display.invisiblecolor = 17 ;
            draw_world(xworld,yworld,10,(level -1) * 72,animframe * 52);

            game.display.invisiblecolor = 15 ;

            game.display.color = 0;
            game.display.drawLine(squidsx + 5,squidsy + 5,ballsx+5,ballsy+5);

            game.display.drawBitmap(squidsx,squidsy,squiddy);

            game.display.drawBitmap(ballsx,ballsy,ball);
                  game.display.color = 0;
            if (state == S_WON) boxlen = 6 * 8; else boxlen = 6 * 9;
            if (squidsy > 50) cury = 20; else cury = 70;

            game.display.setCursor(30,cury);
            draw_textbox(30,cury,boxlen,12,C_BLACK,C_DARKGREEN);
            //game.display.color = 0;
            //game.display.fillRectangle(28,cury-3,boxlen,12);
            //game.display.color = C_DARKGREEN;
            //game.display.drawRectangle(27,cury -4,boxlen+1,14);

            game.display.color = C_GREEN;
            if (state == S_WON)
            {
                game.display.print("YOU WON!");
            }
            else
            {
                game.display.print("YOU LOST!");
            }
             if (game.buttons.repeat(BTN_A,0)) //if button a is pressed
            {
                if (release_count > 4)
                {
                    if (state == S_WON) level = level +1;
                    release_count = 0;
                    state = S_MAP;
                }
            }
            else
            {
                if (release_count < 5) release_count++ ;
            }
    }
    break;
    case S_PLAY :
        {
            if (ball_spins == true)
            {
                angle += angleinc;
                ballwx =  (sin(angle) * dist) + squidwx ;
                ballwy =  (cos(angle) * dist) + squidwy ;
            }
            else
            {
                angle -= angleinc;
                squidwx = (sin(angle) * dist) + ballwx ;
                squidwy = (cos(angle) * dist) + ballwy ;
            }

            if (angle >  TWOPI) angle -= TWOPI;
            if (angle < -TWOPI) angle += TWOPI;

            squidsx = (int) squidwx + xworld;
            squidsy = (int) squidwy + yworld;

            // keep player near the center of the screen
            if (squidsx < MINSX)
            {
                xworld += MINSX - squidsx;
                squidsx = MINSX;
            }

            if (squidsx > MAXSX)
            {
                xworld += MAXSX - squidsx;
                squidsx = MAXSX;
            }
            if (squidsy < MINSY)
            {
                yworld += MINSY - squidsy;
                squidsy = MINSY;
            }
            if (squidsy > MAXSY)
            {
                yworld += MAXSY - squidsy;
                squidsy = MAXSY;
            }

            ballsx = ballwx + xworld;
            ballsy = ballwy + yworld;

            // maybe the bitmap draws are faster if the invisible color is > 15?
            game.display.invisiblecolor = 17 ;
            animframe++;
            if (animframe > 9) animframe = 0;

            draw_world(xworld,yworld,10,(level -1) * 72,(animframe) * 52);

            game.display.invisiblecolor = 15 ;

            game.display.color = 0;
            game.display.drawLine(squidsx + 5,squidsy + 5,ballsx+5,ballsy+5);

            game.display.drawBitmap(squidsx,squidsy,squiddy);

            game.display.drawBitmap(ballsx,ballsy,ball);

            // hangle magnets
            mag_flags = 0;
            test = read_map(ballwx, ballwy,level);
            mag_flags |= 1 << test;
            // if the ball isn't centered on a grid square, check more points
            if ((((int)ballwx) % 10) >= 1)
            {
                test = read_map(ballwx+9, ballwy,level);
                mag_flags |= 1 << test;
            }
           if ((((int)ballwy) % 10) >= 1)
            {
                test = read_map(ballwx, ballwy+9,level);
                mag_flags |= 1 << test;
                if ((((int)ballwx) % 10) >= 1)
                {
                    test = read_map(ballwx+9, ballwy+9,level);
                    mag_flags |= 1 << test;
                }

            }
            if (mag_flags & (1 << 4)) {ballwx += attract; squidwx  += attract;}
            if (mag_flags & (1 << 5)) {ballwy += attract; squidwy  += attract;}
            if (mag_flags & (1 << 6)) {ballwx -= attract; squidwx  -= attract;}
            if (mag_flags & (1 << 7)) {ballwy -= attract; squidwy  -= attract;}




            // see if squiddy has won or lost

            test = read_map(squidwx+ 5, squidwy+8,level);


#if 1
            if (test == 0)
            {
                state = S_LOST;
                release_count = 0;
            }
            if (test == 2)
            {
                state = S_WON;
                release_count = 0;
            }
#endif
            if (game.buttons.repeat(BTN_A,0)) //if button a is pressed
            {
                if (release_count > 1)
                {
                    ball_spins = !ball_spins;
                    angle = angle + 3.14159265359;
                    release_count = 0;
                }
            }
            else
            {
                if (release_count < 5) release_count++ ;
            }
        }
    }
}
}
return 1;
}


