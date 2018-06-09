/*#include "pokitto.h"
#include "math.h"

Pokitto::Core game;

//110x88
typedef struct
{
    float x;
    float y;
} Point2d;


typedef struct
{
    float x;
    float y;
    float z;
} Point3d;

const int LEN_POINTS=64;
Point3d points[LEN_POINTS];

float angle=0.1;

Point2d p2d(Point3d p3d)
{
    Point2d out2d;

    float fov = 128;
    float x0 = p3d.x;
    float y0 = p3d.y;
    float z0 = p3d.z;
    float x2d = fov * x0 / z0;
    float y2d = fov * y0 / z0;

    x2d = x2d + 55; //center w
    y2d = y2d + 44;  //center h

    out2d.x=x2d;
    out2d.y=y2d;

    return out2d;
}

Point3d rot3d(Point3d p3d,Point3d pivot,float ax,float ay,float az)
{
    float a;
    float b;
    float c;
    float a1;
    float b1;
    float c1;
    float a2;
    float b2;
    float c2;
    float a3;
    float b3;
    float c3;
    Point3d np3d;

    a = p3d.x-pivot.x;
    b = p3d.y-pivot.y;
    c = p3d.z-pivot.z;

    a1 = a*cos(az)-b*sin(az);
    b1 = a*sin(az)+b*cos(az);
    c1 = c;

    c2 = c1*cos(ay)-a1*sin(ay);
    a2 = c1*sin(ay)+a1*cos(ay);
    b2 = b1;

    b3 = b2*cos(ax)-c2*sin(ax);
    c3 = b2*sin(ax)+c2*cos(ax);
    a3 = a2;

    a3 = a3+pivot.x;
    b3 = b3+pivot.y;
    c3 = c3+pivot.z;

    np3d.x=a3;
    np3d.y=b3;
    np3d.z=c3;
    return np3d;
}

int main ()
{
    game.begin();
    game.display.load565Palette(def565palette);

    int i=0;
    for (int x=0; x<4; x++)
    {
        for (int y=0; y<4; y++)
        {
            for (int z=0; z<4; z++)
            {
                points[i].x=x*12;
                points[i].y=y*12;
                points[i].z=100+z*12;
                i++;
            }
        }
    }

    Point2d pp;
    Point3d pr;
    Point3d center;
    center.x=0;
    center.y=0;
    center.z=100;

    while (game.isRunning())
    {
        if (game.update())
        {
            angle+=0.1;
            game.display.setColor(C_YELLOW);
            for (int i=0; i<LEN_POINTS; i++)
            {
                pr = rot3d(points[i],center, angle,angle/2,0);
                pp = p2d(pr);
                game.display.fillCircle(pp.x,pp.y,2);
            }
        }
    }
    return 1;
}
*/

/*
#include "pokitto.h"
#include "math.h"
Pokitto::Core game;

//110x88
const int S=16;
const int S2=S/2;
int cx,cy=0;

void pir(int x,int y,int w,int h,int cx,int cy)
{
  game.display.color=1;
  game.display.fillTriangle(x,y,w/2+cx,h/2+cy,x+w,y);
  game.display.color=3;
  game.display.fillTriangle(x+w,y,w/2+cx,h/2+cy,x+w,y+h);
  game.display.color=6;
  game.display.fillTriangle(x,y,w/2+cx,h/2+cy,x,y+h);
  game.display.color=8;
  game.display.fillTriangle(x,y+h,w/2+cx,h/2+cy,x+w,y+h);
}

int main () {
game.begin();
while (game.isRunning()) {
    if (game.update()) {
        for (int x=0;x<game.display.width;x+=S)
        {
            for (int y=0;y<game.display.height;y+=S)
            {
                cx=S2*sin(game.frameCount/300.0*(x+2.0*y));
                cy=S2*cos(game.frameCount/300.0*(2.0*x+y));
                pir(x,y,S,S,x+cx,y+cy);
            }
        }
    }
}

return 1;
}*/

#include "pokitto.h"
#include "math.h"
Pokitto::Core game;

//110x88
unsigned char xp=55;
signed char xo=0;
unsigned char yp=80;

unsigned char partIndx=0;
struct Particle {
   float x;
   float y;
   float dx;
   float dy;
   float life=9999;
} ;
const unsigned MAX_PART=200;
static Particle particles[MAX_PART];

static unsigned char PAL_FILE[5]={C_YELLOW,C_ORANGE,C_RED,C_BROWN,C_DARKGRAY};

void addParticle(unsigned char x,unsigned char y)
{
    particles[partIndx].x=x;
    particles[partIndx].y=y;
    particles[partIndx].dx=random(-100,100)/300.0;
    particles[partIndx].dy=random(-100,-80)/80.0;
    particles[partIndx].life=0;
    partIndx++;
    if (partIndx>MAX_PART) {partIndx=0;}
}

void renderParticles()
{
    for (int i=0;i<MAX_PART;i++)
        {
            if(particles[i].life<50)
            {

                game.display.color=PAL_FILE[((int)particles[i].life/10)];
                particles[i].life++;
                particles[i].x+=particles[i].dx;
                particles[i].y+=particles[i].dy;

                float radius= particles[i].life/10;
                game.display.fillCircle(particles[i].x,particles[i].y,radius);
                if(particles[i].life>10){
                    game.display.color=C_BLACK;
                    game.display.drawCircle(particles[i].x,particles[i].y,radius);
                }
            }
        }
}

int main () {
game.begin();
game.display.load565Palette(def565palette);
while (game.isRunning()) {
    if (game.update()) {

        if (game.buttons.upBtn()) {yp-=2;}
        if (game.buttons.downBtn()) {yp+=2;}
        if (game.buttons.rightBtn()) {xp+=2;}
        if (game.buttons.leftBtn()) {xp-=2;}

        //Particles
        renderParticles();

        //Cursor
        game.display.color=C_RED;
        xo=-15*sin(game.frameCount/10.0);
        addParticle(xp+xo,yp);
        addParticle(xp+xo,yp);
        addParticle(xp+xo,yp);
        game.display.drawCircle((xp+xo),yp,2);

        //Cross
        game.display.drawLine(xp-1,yp,xp+1,yp);
        game.display.drawLine(xp,yp-1,xp,yp+1);
    }
}
return 1;
}

