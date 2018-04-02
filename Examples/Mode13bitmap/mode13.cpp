/** Mode 13 (110x88x256) by Spinal, a palette rotating plasma demo */

#include "Pokitto.h"

Pokitto::Core game;

unsigned short pal[256]; // assign a 256 entry array to hold the palette

uint8_t col=0;

int PntClr(int x, int y){
	return game.display.getPixel(x,y);
}
void Dot (int x, int y, int c){
	game.display.drawPixel(x,y,c);
}
int RandMinMax(int min, int max){
    return rand() % max + min;
}
int Adjust (int xa, int ya, int x, int y, int xb, int yb){
	if(PntClr(x, y) != 0) return 0;
	int q = abs(xa - xb) + abs(ya - yb);
	int v = (PntClr(xa, ya) + PntClr(xb, yb)) / 2 + (RandMinMax(0,q*10)) / 10;
	if (v < 1) v = 1;
	if (v > 255) v = 255;
	Dot(x, y, v);
	return 1;
}
void SubDivide (int x1, int y1, int x2, int y2){
	if ((x2 - x1 < 2) && (y2 - y1 < 2)) return;
	int x = (x1 + x2) / 2;
	int y = (y1 + y2) / 2;
	Adjust(x1, y1, x, y1, x2, y1);
	Adjust(x1, y2, x, y2, x2, y2);
	Adjust(x2, y1, x2, y, x2, y2);
	Adjust(x1, y1, x1, y, x1, y2);
	if(PntClr(x, y) == 0)	{
		int v = PntClr(x1, y1) + PntClr(x2, y1) + PntClr(x2, y2);
		v = v + PntClr(x1, y2) + PntClr(x1, y) + PntClr(x, y1);
		v = v + PntClr(x2, y) + PntClr(x, y2);
		v = v / 8;
		Dot(x, y, v);
	}
	SubDivide(x1, y1, x, y);
	SubDivide(x, y, x2, y2);
	SubDivide(x, y1, x2, y);
	SubDivide(x1, y, x, y2);
}
void make_plasma(int x1=0,int y1=0,int x2=game.display.width-1,int y2=game.display.height-1){
	game.display.clear();
	if(x1<0)x1=0;
	if(y1<0)y1=0;
	if(x2>game.display.width-1)x2=game.display.width-1;
	if(y2>game.display.height-1)y2=game.display.height-1;
    Dot(x1, y1, RandMinMax(0,255) + 1);
Dot(x2, y1, RandMinMax(0,255) + 1);
Dot(x2, y2, RandMinMax(0,255) + 1);
Dot(x1, y2, RandMinMax(0,255) + 1);
SubDivide(x1, y1, x2, y2);
}
void make_pal(void){
	int a,s,r,g,b;
	for(a=0; a<=63; a++){
		s = 0; 	r = a; 		g = 63-a;	b = 0;		pal[a+s] = game.display.RGBto565(r*4,g*4,b*4);
		s = 64; r = 63-a;	g = 0;		b = a; 		pal[a+s] = game.display.RGBto565(r*4,g*4,b*4);
		s = 128; r = 0;	 	g = 0;		b = 63-a;	pal[a+s] = game.display.RGBto565(r*4,g*4,b*4);
		s = 192; r = 0;		g = a;		b = 0;	 	pal[a+s] = game.display.RGBto565(r*4,g*4,b*4);
	}
    game.display.load565Palette(&pal[0]); // load a palette the same way as any other palette in any other screen mode
}
int main(){
    game.begin();
    game.display.persistence=1;
    srand(game.getTime());
make_pal(); // create 256 colour palette for this demo
make_plasma(0,0,game.display.width-1,game.display.height-1); // create a nice plasma cloud

while (game.isRunning()) {
    if(game.update()){
    // mode 13 has a palette offset, so instead of rotating your palette, you just tell it which colour should be first.
        game.display.palOffset = col++;
    }
}

return 1;
}
