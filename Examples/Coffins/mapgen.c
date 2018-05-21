#include <stdio.h>
#include <stdlib.h>

#define MAPSIZE 64

#define ID_FLOOR 0
#define ID_WALL_TOP 1
#define ID_WALL 2
#define ID_WALL_SKULL 3
#define ID_WALL_FLAME 4
#define ID_WALL_HOLE 5
#define ID_WALL_SHIELD 6
#define ID_COLUMN_TOP_SQUARE_1 7
#define ID_COLUMN_TOP_SQUARE_2 9
#define ID_COLUMN_TOP_SQUARE_3 10
#define ID_COLUMN_TOP_ROUND_1 11
#define ID_COLUMN_TOP_ROUND_2 13
#define ID_COLUMN_BOTTOM_SQUARE_1 8
#define ID_COLUMN_BOTTOM_ROUND_1 12
#define ID_COLUMN_BOTTOM_ROUND_2 14
#define ID_COFFIN_CLOSED_TOP 15
#define ID_COFFIN_CLOSED_BOTTOM 16
#define ID_COFFIN_OPEN_TOP 17
#define ID_COFFIN_OPEN_BOTTOM 18
#define ID_COBWEB 33

void mapinit(char map[][MAPSIZE], int width, int height);
void mapgen(char map[][MAPSIZE], int mapwidth, int mapheight, int startx, int starty, int endx, int endy);
void mapprint(char map[][MAPSIZE], int width, int height);

void mapinit(char map[][MAPSIZE], int width, int height){
    int i,j;

    //Fill with empty space
    for( i = 0; i < width; i++ ){
        for( j = 0; j < height; j++ ){
            map[j][i] = 0;
        }
    }
    //Generate walls around the edges
    for( i = 0; i < width; i++ ){
        map[0][i] = 1;
        map[height-1][i] = 1;
    }
    for( j = 0; j < height; j++ ){
        map[j][0] = 1;
        map[j][width-1] = 1;
    }
}

#define HORIZONTAL 0
#define VERTICAL 1
#define MIN_WIDTH 6
#define MIN_HEIGHT 8
#define HALL_CHANCE 40
#define MIN_HALL_WIDTH 8
#define MIN_HALL_HEIGHT 10
#define MAX_HALL_WIDTH 18
#define MAX_HALL_HEIGHT 20
#define EXTRA_DOOR 10
#define REM_WALL_CHANCE 25
#define MAX_REM_WALL 10
void mapgen(char map[][MAPSIZE], int mapwidth, int mapheight, int startx, int starty, int endx, int endy){
    int i,j,orientation,position,door,door2,doorcount,hall,colspacex,colspacey,remwall;
    int width = endx-startx;
    int height = endy-starty;

    if( width < MIN_WIDTH && height < MIN_HEIGHT ){
        return;
    }

    //Determine whether to generate a big hall with pillars or not.
    //The size of the room determines the column spacing for now
    if( width >= MIN_HALL_WIDTH && height >= MIN_HALL_HEIGHT && width < MAX_HALL_WIDTH && height < MAX_HALL_HEIGHT ){
        hall = rand()%100;//Percent chance out of 100
        if( HALL_CHANCE > hall ){
            hall = rand()%3;//Choose type of hall; square columns, round columns, or coffins
            //Place room columns as densely as the room supports
            //IF you can divide the room into columns evenly
            //but make sure there is greater than one space between columns
            colspacex = width;
            if( colspacex%2 != 0 ){
                colspacex--;
            }
            while( colspacex%2 == 0 && colspacex > 4 ) colspacex /= 2;
            colspacey = height;
            if( colspacey%4 != 0 )
            {
                while( colspacey%4 != 0 ) colspacey++;
            }
            while( colspacey%4 == 0 && colspacey > 4 ) colspacey /= 4;

            for( i = startx+colspacex; i < startx + width - 1; i+=colspacex ){
                for( j = starty+colspacey; j < starty + height - 2; j+=colspacey ){
                    if( hall == 0 ){//Square columns
                        switch( rand()%3 ){
                            case 0:
                                map[j][i] = ID_COLUMN_TOP_SQUARE_1;
                                break;
                            case 1:
                                map[j][i] = ID_COLUMN_TOP_SQUARE_2;
                                break;
                            case 2:
                                map[j][i] = ID_COLUMN_TOP_SQUARE_3;
                                break;
                        }
                        map[j+1][i] = ID_COLUMN_BOTTOM_SQUARE_1;
                    }else if( hall == 1 ){//Round columns
                        switch( rand()%2 ){
                            case 0:
                                map[j][i] = ID_COLUMN_TOP_ROUND_1;
                                break;
                            case 1:
                                map[j][i] = ID_COLUMN_TOP_ROUND_2;
                                break;
                        }
                        switch( rand()%2 ){
                            case 0:
                                map[j+1][i] = ID_COLUMN_BOTTOM_ROUND_1;
                                break;
                            case 1:
                                map[j+1][i] = ID_COLUMN_BOTTOM_ROUND_2;
                                break;
                        }
                    }else{//Coffins
                        switch( rand()%2 ){
                            case 0:
                                map[j][i] = ID_COFFIN_CLOSED_TOP;
                                map[j+1][i] = ID_COFFIN_CLOSED_BOTTOM;
                                break;
                            case 1:
                                map[j][i] = ID_COFFIN_OPEN_TOP;
                                map[j+1][i] = ID_COFFIN_OPEN_BOTTOM;
                                break;
                        }
                    }

                }
            }
            return;//Do not subdivide; return immediately
        }
    }

    //Determine whether we will split the space
    //horizontally or vertically by choosing whichever
    //orientation is larger (this avoids extremely long rooms)
    if( width >= height ){
        //If there is a door (or more than one door!) into a small room,
        //we may not be able to generate a wall in any location!
        //so abort if room is min + number of horiz door tiles
        doorcount = 0;
        for( i = startx; i < endx; i++ ){
            if( map[starty][i] == 0 ) doorcount++;
            if( map[endy][i] == 0 ) doorcount++;
        }
        if( width < MIN_WIDTH + doorcount ){
            return;
        }
        orientation = VERTICAL;
        //puts("Trying vertical");
    }else{
        //If there is a door (or more than one door!) into a small room,
        //we may not be able to generate a wall in any location!
        //so abort if room is min + number of vert door tiles
        doorcount = 0;
        for( i = starty; i < endy; i++ ){
            if( map[i][startx] == 0 ) doorcount++;
            if( map[i][endx] == 0 ) doorcount++;
        }
        if( height < MIN_HEIGHT + doorcount ){
            return;
        }
        orientation = HORIZONTAL;
        //puts("Trying horizontal");
    }
    //printf("startx %d, starty %d\n",startx,starty);
    //mapprint(map,MAPSIZE,MAPSIZE);
    position = -1;
    if( orientation == HORIZONTAL ){
        //Make sure the position is valid:
        //1. It must have generated at least one number
        //2. It must not be too close to existing walls
        //3. It must not be over a door
        while( position == -1 || position < starty + (MIN_HEIGHT/2) || position > endy - (MIN_HEIGHT/2) || map[position][startx] == 0 || map[position][endx] == 0 ){
            position = starty + (rand()%height);
        }
        //Generate a door at a random position
        door = startx + 1 + (rand()%(width-1));
        //Generate an extra door if wall is long enough.
        //Doors may overlap or be next to each other because such
        //doors shouldn't be a problem and it might result in interesting maps.
        //If it is not long enough set to -1 so it won't interfere
        door2 = width >= EXTRA_DOOR ? startx + 1 + (rand()%(width-1)) : -1;
        //printf("HORIZ %d\n",position);
        for( i = startx+1; i < startx + width; i++ ){
            if( i != door && i != door2 )
                map[position][i] = 1;
        }
        //Determine whether or not we will remove a wall.
        //We can't remove bottom and right walls because the regions beyond them
        //haven't been generated yet, so we only try to remove top and left walls.
        //If we are not on the map edges
        if( startx > 0 && endx < mapwidth-1 ){
            //If the new space is taller than it is wide
            //and the wall is short enough
            if( /*position - starty > width &&*/ position - starty < MAX_REM_WALL ){
                remwall = 1;
                //Check if there is no wall touching the wall we want to remove.
                //If not, we can remove the wall
                for( i = starty+1; i < position; i++ ){
                    if( map[i][startx-1] != 0 ){
                        remwall = 0;
                        break;
                    }
                }
                if( remwall && REM_WALL_CHANCE > (rand()%100) ){
                    for( i = starty+1; i < position; i++ ){
                        map[i][startx] = 0;//2;//Clear left side of upper half
                    }
                }
            }
            //If the new space is taller than it is wide
            //and the wall is short enough
            if( /*endy - position > width &&*/ endy - position < MAX_REM_WALL ){
                remwall = 1;
                //Check if there is no wall touching the wall we want to remove.
                //If not, we can remove the wall
                for( i = position+1; i < endy; i++ ){
                    if( map[i][startx+1] != 0 ){
                        remwall = 0;
                        break;
                    }
                }
                if( remwall && REM_WALL_CHANCE > (rand()%100) ){
                    for( i = position+1; i < endy; i++ ){
                        map[i][startx] = 0;//4;//Clear left side of lower half
                    }
                }
            }
        }
        //Recursively call to fill the two new spaces we generated
        mapgen(map, mapwidth, mapheight, startx, starty, endx,position);
        mapgen(map, mapwidth, mapheight, startx, position, endx, endy);
    }else if( orientation == VERTICAL ){
        //Make sure the position is valid:
        //1. It must have generated at least one number
        //2. It must not be too close to existing walls
        //3. It must not be over a door
        while( position == -1 || position < startx + (MIN_WIDTH/2) || position > endx - (MIN_WIDTH/2) || map[starty][position] == 0 || map[endy][position] == 0 ){
            position = startx + (rand()%width);
        }
        //Generate a door at a random position
        //(allocating space for it to be 2 high)
        door = starty + 1 + (rand()%(height-2));
        //Generate an extra door if wall is long enough.
        //Doors may overlap or be next to each other because such
        //doors shouldn't be a problem and it might result in interesting maps.
        //If it is not long enough set to -1 so it won't interfere
        door2 = height >= EXTRA_DOOR ? starty + 1 + (rand()%(height-2)) : -1;
        //printf("VERT %d\n",position);
        for( i = starty+1; i < starty + height; i++ ){
            if( i != door && i != door+1 && i != door2 && i != door2+1 )
                map[i][position] = 1;
        }
        //Determine whether or not we will remove a wall.
        //We can't remove bottom and right walls because the regions beyond them
        //haven't been generated yet, so we only try to remove top and left walls.
        //If we are not on the map edges
        if( starty > 0 && endy < mapheight-1 ){
            //If the new space is wider than it is tall
            //and the wall is short enough
            if( /*position - startx > height &&*/ position - startx < MAX_REM_WALL ){
                remwall = 1;
                //Check if there is no wall touching the wall we want to remove.
                //If not, we can remove the wall
                for( i = startx+1; i < position; i++ ){
                    if( map[starty-1][i] != 0 ){
                        remwall = 0;
                        break;
                    }
                }
                if( remwall && REM_WALL_CHANCE > (rand()%100) ){
                    for( i = startx+1; i < position; i++ ){
                        map[starty][i] = 0;//3;//Clear top side of left half
                    }
                }
            }
            //If the new space is wider than it is tall
            //and the wall is short enough
            if( /*endx - position > height &&*/ endx - position < MAX_REM_WALL ){
                remwall = 1;
                //Check if there is no wall touching the wall we want to remove.
                //If not, we can remove the wall
                for( i = position+1; i < endx; i++ ){
                    if( map[starty-1][i] != 0 ){
                        remwall = 0;
                        break;
                    }
                }
                if( remwall && REM_WALL_CHANCE > (rand()%100) ){
                    for( i = position+1; i < endx; i++ ){
                        map[starty][i] = 0;//5;//Clear top side of right half
                    }
                }
            }
        }
        //Recursively call to fill the two new spaces we generated
        mapgen(map, mapwidth, mapheight, startx, starty, position,endy);
        mapgen(map, mapwidth, mapheight, position, starty, endx, endy);
    }
}

void mapprint(char map[][MAPSIZE], int width, int height){
    int i,j;

    for( i = 0; i < height; i++ ){
        for( j = 0; j < width; j++ ){
            if( map[i][j] == 0 ){
                printf("0");
            }else if( map[i][j] == 1 ){
                printf("1");
            }
        }
        puts("");
    }
}

void mappretty(char map[][MAPSIZE],int width, int height){
    int i,j,v;
    for (i=0; i < height-1; i++) {
        for (j=0;j<width-1;j++){
            if (map[i][j]==1 && map[i+1][j]==0){
                v = rand()%25 + 3;
                //If v is any of the random wall variations
                if( v <= ID_WALL_SHIELD ){
                    map[i+1][j] = v;
                }else {
                    map[i+1][j]=ID_WALL;
                }
            }

            if (map[i][j]==0 && map[i+1][j]==1 && map[i][j-1]==1 && rand()%10 <=1) {
                map[i][j]=ID_COBWEB;
            }
        }
    }
}


//char map[MAPSIZE][MAPSIZE];

/*
int main(int argc, char** argv){
    srand(42);//Can choose the seed to get the same map every time

    mapinit(map,MAPSIZE,MAPSIZE);
    mapgen(map,MAPSIZE,MAPSIZE,0,0,MAPSIZE-1,MAPSIZE-1);

    mapprint(map,MAPSIZE,MAPSIZE);
}
*/


