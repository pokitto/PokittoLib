#ifndef DRAWING_H_INCLUDED
#define DRAWING_H_INCLUDED
uint16_t colorDepth = 8;

// load only part of a palette if needed
void load8bitPalette(const uint16_t* p, uint8_t from, uint8_t numCols) {
//    if(numCols-from > 255)return;
    for (int i=from; i<=numCols; i++) game.display.palette[i] = p[i];
    game.display.paletteptr = game.display.palette;
}

void load4bitPalette(const uint16_t* p, uint8_t from, uint8_t numCols) {
//    if(numCols-from > 15)return;
    for (int i=from; i<numCols; i++) game.display.palette[i+256] = p[i];
    game.display.paletteptr = game.display.palette;
}

void load2bitPalette(const uint16_t* p, uint8_t from, uint8_t numCols) {
//    if(numCols-from > 3)return;
    for (int i=from; i<numCols; i++) game.display.palette[i+272] = p[i];
    game.display.paletteptr = game.display.palette;
}

// draw pixel to 8 bit buffer (110x88x256)
void drawPixel8(uint16_t x, uint16_t y, uint16_t col){
    if(x<0 || x>109 || y<0 || y>87) return;
    if(col == game.display.invisiblecolor) return;
    game.display.screenbuffer[x+110*y]=col;
}

// draw pixel to 4 bit buffer (110x176x16)
void drawPixel4(uint16_t x, uint16_t y, uint16_t col){
    if(x<0 || x>109 || y<0 || y>175) return;
    if(col == game.display.invisiblecolor) return;

    uint16_t i = y*55 + (x>>1);
    uint16_t pixel = game.display.screenbuffer[i];
    if (x&1) pixel = (pixel & B11110000)|(col);
        else pixel = (pixel & B00001111) | (col<<4);
    game.display.screenbuffer[i] = pixel;
}

// draw pixel to 2 bit buffer (220x176x4)
void drawPixel2(uint16_t x, uint16_t y, uint16_t col){
    if(x<0 || x>219 || y<0 || y>175) return;
    if(col == game.display.invisiblecolor) return;

    uint16_t i = y*55 + (x>>2);
    uint16_t pixel = game.display.screenbuffer[i];
    col &= 3; // just in case
    uint8_t column = x&3;
         if (column==3) pixel = (pixel & B11111100) | (col); // bits 0-1
    else if (column==2) pixel = (pixel & B11110011) | (col<<2); // bits 2-3
    else if (column==1) pixel = (pixel & B11001111) | (col<<4); // bits 4-5
    else                pixel = (pixel & B00111111) | (col<<6); // bits 6-7
    game.display.screenbuffer[i] = pixel;
}





void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t bpp, bool flip=0){

    int16_t w = *bitmap; // first byte is width
	int16_t h = *(bitmap + 1); // second byte is height

    switch(bpp){
        case 2:{
           uint16_t i = 0;
           for(int16_t y1=0; y1<h; y1++){
                for(int16_t x1=0; x1<w;){
                    if(flip==0){
                        drawPixel2(x+x1,y+y1, (bitmap[i+2] >> 6) &3); x1++;
                        drawPixel2(x+x1,y+y1, (bitmap[i+2] >> 4) &3); x1++;
                        drawPixel2(x+x1,y+y1, (bitmap[i+2] >> 2) &3); x1++;
                        drawPixel2(x+x1,y+y1,  bitmap[i+2]       &3); x1++;
                    }else{
                        drawPixel2(x+(w-x1-1),y+y1, (bitmap[i+2] >> 6) &3); x1++;
                        drawPixel2(x+(w-x1-1),y+y1, (bitmap[i+2] >> 4) &3); x1++;
                        drawPixel2(x+(w-x1-1),y+y1, (bitmap[i+2] >> 2) &3); x1++;
                        drawPixel2(x+(w-x1-1),y+y1,  bitmap[i+2]       &3); x1++;
                    }
                    i++;
                }
            }
        break;
        }
        case 4:{

           uint16_t i = 0;
           for(int16_t y1=0; y1<h; y1++){
                for(int16_t x1=0; x1<w;){
                    if(flip==0){
                        drawPixel4(x+x1,y+y1, (bitmap[i+2] >> 4) &15); x1++;
                        drawPixel4(x+x1,y+y1,  bitmap[i+2]       &15); x1++;
                    }else{
                        drawPixel4(x+(w-x1-1),y+y1, (bitmap[i+2] >> 4) &15); x1++;
                        drawPixel4(x+(w-x1-1),y+y1,  bitmap[i+2]       &15); x1++;
                    }
                    i++;
                }
            }
            break;
        }
        case 8:{
            for(int16_t y1=0; y1<h; y1++){
                for(int16_t x1=0; x1<w; x1++){
                    uint16_t i = y1*w + x1;
                    uint16_t pixel = bitmap[i+2];
                    if(flip==0){
                        drawPixel8(x+x1,y+y1, pixel);
                    }else{
                    drawPixel8(x+(w-x1-1),y+y1, pixel);
                    }
                }
            }
            break;
        }

    }

}



// draw x amount of lines from an image with a start line, for drawing the middle of an image.
void drawBitmapLines(int16_t x, int16_t y, int16_t start, int16_t numLines, const uint8_t *bitmap, int16_t bpp){

    int16_t w = *bitmap; // first byte is width
	int16_t h = *(bitmap + 1); // second byte is height

    if(start + numLines < h) h = start + numLines;

    switch(bpp){
        case 2:{
           uint16_t i = 2 + (start*(w/4));
           for(int16_t y1=start; y1<h; y1++){
                for(int16_t x1=0; x1<w;){
                    drawPixel2(x+x1,y+y1, (bitmap[i] >> 6) &3); x1++;
                    drawPixel2(x+x1,y+y1, (bitmap[i] >> 4) &3); x1++;
                    drawPixel2(x+x1,y+y1, (bitmap[i] >> 2) &3); x1++;
                    drawPixel2(x+x1,y+y1,  bitmap[i]       &3); x1++;
                    i++;
                }
            }
        break;
        }
        case 4:{

           uint16_t i = 2 + (start*(w/2));
           for(int16_t y1=start; y1<h; y1++){
                for(int16_t x1=0; x1<w;){
                    drawPixel4(x+x1,y+y1, (bitmap[i] >> 4) &15); x1++;
                    drawPixel4(x+x1,y+y1,  bitmap[i]       &15); x1++;
                    i++;
                }
            }
            break;
        }
        case 8:{
           uint16_t i = 2 + (start*w);
            for(int16_t y1=0; y1<h; y1++){
                for(int16_t x1=0; x1<w; x1++){
                    uint16_t pixel = bitmap[i++];
                    drawPixel8(x+x1,y+y1, pixel);
                }
            }
            break;
        }

    }

}

#endif /* DRAWING_H_INCLUDED */
