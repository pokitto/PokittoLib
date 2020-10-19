#include "PokittoDisplay.h"

using namespace Pokitto;

#if PROJ_SCREENMODE != TASMODE

void Display::drawPixelNOP(int16_t x,int16_t y, uint8_t col) {
}

void Display::drawPixelRaw(int16_t x,int16_t y, uint8_t col) {
    if( PROJ_COLORDEPTH == 8 ){

        m_scrbuf[x+width*y] = col;

    } else if( PROJ_COLORDEPTH == 2 ) {

        if (col) {
            col &= 3;
        }
        uint16_t i = y*(width>>2) + (x>>2);
        uint8_t pixel = m_scrbuf[i];
        uint8_t column = x&0x03;
        if (column==3) pixel = (pixel&0xFC)|(col); // bits 0-1
        else if (column==2) pixel = (pixel&0xF3)|(col<<2); // bits 2-3
        else if (column==1) pixel = (pixel&0xCF)|(col<<4); // bits 4-5
        else pixel = (pixel&0x3F)|(col<<6); // bits 6-7
        m_scrbuf[i] = pixel;

    } else if( PROJ_COLORDEPTH == 4 ) {

        uint16_t i = y*(width>>1) + (x>>1);
        uint8_t pixel = m_scrbuf[i];
        if (x&1) pixel = (pixel&0xF0)|(col);
        else pixel = (pixel&0x0F) | (col<<4);
        m_scrbuf[i] = pixel;

    }
}

void Display::drawPixel(int16_t x,int16_t y, uint8_t col) {
    if (col==invisiblecolor) return; // do not draw transparent pixels
    if ((uint16_t)x >= width || (uint16_t)y >= height) return;
    col &= (PALETTE_SIZE-1);
    drawPixelRaw( x, y, col );
}

void Display::drawPixel(int16_t x,int16_t y) {
    if ((uint16_t)x >= width || (uint16_t)y >= height) return;
    drawPixelRaw( x, y, color );
}

uint8_t Display::getPixel(int16_t x,int16_t y) {
    if ((uint16_t)x >= width || (uint16_t)y >= height)
        return 0;

#if PROJ_COLORDEPTH == 8
    return m_scrbuf[y*width+x];
#endif // PROJ_COLORDEPTH
        
#if PROJ_COLORDEPTH == 2
    uint16_t i = y*(width>>2) + (x>>2);
    uint8_t pixel = m_scrbuf[i];
    uint8_t column = x&0x03;
    if (column==0) return pixel & 0x03; // bits 0-1
    else if (column==1) return (pixel & 0x0C)>>2; // bits 2-3
    else if (column==2) return (pixel & 0x30)>>4; // bits 4-5
    else return pixel>>6;; // bits 6-7
#endif

#if PROJ_COLORDEPTH == 4
    uint16_t i = y*(width>>1) + (x>>1);
    uint8_t pixel = m_scrbuf[i];
    if (x&1) return pixel & 0x0F;
    else return pixel>>4;
#endif // PROJ_COLORDEPTH

    return 0;
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    if ((uint16_t)x0 >= width || (uint16_t)y0 >= height || (uint16_t)x1 >= width || (uint16_t)y1 >= height ) {
        if (clipLine (&x0,&y0,&x1,&y1)==0) return; // line out of bounds
    }
    if (x0 == x1)
        drawColumn(x0,y0,y1);
    else if (y0 == y1)
        drawRow(x0,x1,y0);
    else {
        int e;
        signed int dx,dy,j, temp;
        signed char s1,s2, xchange;
        signed int x,y;

        x = x0;
        y = y0;

        //take absolute value
        if (x1 < x0) {
            dx = x0 - x1;
            s1 = -1;
        }
        else if (x1 == x0) {
            dx = 0;
            s1 = 0;
        }
        else {
            dx = x1 - x0;
            s1 = 1;
        }

        if (y1 < y0) {
            dy = y0 - y1;
            s2 = -1;
        }
        else if (y1 == y0) {
            dy = 0;
            s2 = 0;
        }
        else {
            dy = y1 - y0;
            s2 = 1;
        }

        xchange = 0;

        if (dy>dx) {
            temp = dx;
            dx = dy;
            dy = temp;
            xchange = 1;
        }

        e = ((int)dy<<1) - dx;

        for (j=0; j<=dx; j++) {
            drawPixel(x,y);

            if (e>=0) {
                if (xchange==1) x = x + s1;
                else y = y + s2;
                e = e - ((int)dx<<1);
            }
            if (xchange==1)
                y = y + s2;
            else
                x = x + s1;
            e = e + ((int)dy<<1);
        }
    }
}

uint8_t Display::clipLine(int16_t *x0, int16_t *y0, int16_t *x1, int16_t *y1){
    // Check X bounds
    if (*x1 < *x0) {
        //std::swap (*x1,*x0); // swap so that we dont have to check x1 also
        swapWT(int16_t*, x1, x0);
        //std::swap (*y1,*y0); // y needs to be swaaped also
        swapWT(int16_t*, y1, y0);
    }

    if (*x0 >= width) return 0; // whole line is out of bounds

    // Clip against X0 = 0
    if (*x0 < 0) {
        if (*x1 < 0) return 0; // nothing visible
        int32_t dx = (*x1 - *x0);
        int32_t dy = ((*y1 - *y0) << 16); // 16.16 fixed point calculation trick
        int32_t m = dy/dx;
        *y0 = *y0 + ((m*-*x0) >> 16); // get y0 at boundary
        *x0 = 0;
    }

    // Clip against x1 >= width
    if (*x1 >= width) {
        int32_t dx = (*x1 - *x0);
        int32_t dy = ((*y1 - *y0) << 16); // 16.16 fixed point calculation trick
        int32_t m = dy / dx;
        *y1 = *y1 + ((m * ((width - 1) - *x1)) >> 16); // get y0 at boundary
        *x1 = width-1;
    }

    // Check Y bounds
    if (*y1 < *y0) {
        //std::swap (*x1,*x0); // swap so that we dont have to check x1 also
        swapWT(int16_t*, x1, x0);
        //std::swap (*y1,*y0); // y needs to be swaaped also
        swapWT(int16_t*, y1, y0);
    }

    if (*y0 >= height) return 0; // whole line is out of bounds

    if (*y0 < 0) {
        if (*y1 < 0) return 0; // nothing visible
        int32_t dx = (*x1 - *x0) << 16;
        int32_t dy = (*y1 - *y0); // 16.16 fixed point calculation trick
        int32_t m = dx / dy;
        *x0 = *x0 + ((m * -(*y0)) >> 16); // get x0 at boundary
        *y0 = 0;
    }

    // Clip against y1 >= height
    if (*y1 >= height) {
        int32_t dx = (*x1 - *x0) << 16;
        int32_t dy = (*y1 - *y0); // 16.16 fixed point calculation trick
        int32_t m = dx / dy;
        *x1 = *x1 + ((m * ((height - 1) - *y1)) >> 16); // get y0 at boundary
        *y1 = height-1;
    }
    return 1; // clipped succesfully
}

void Display::drawColumn(int x, int sy, int ey){
    if(x < 0 || x >= width) return;

    if (sy > ey) {
        int y = sy;
        sy = ey;
        ey = y; // swap around so that sy is less than ey
    }    

    if (ey >= height)
        ey = height -1;

    if (sy < 0)
        sy = 0;

    if (sy == ey) return; //nothing to do here
    
    for (int y=sy; y <= ey; y++) {
        drawPixelRaw(x,y,color);
    }
}

void Display::drawRow(int x0, int x1, int y){
    if(y < 0 || y >= height) return; //out of bounds

    if (x0 > x1) {
        int x = x0;
        x0 = x1;
        x1 = x; // swap around so that x0 is less than x1
    }    

    if (x1 >= width) //clamp to the border
        x1 = width -1;

    if (x0 < 0) //clamp to the border
        x0 = 0;

    if (x0 == x1) return; //nothing to draw here
    
    for (int x=x0; x <= x1; x++) {
        drawPixelRaw(x,y,color); //use Raw access without bound checks
    }
}

void Display::drawRectangle(int x0, int y0, int w, int h) {
    drawColumn(x0,y0,y0+h);
    drawColumn(x0+w,y0,y0+h);
    drawRow(x0,x0+w,y0);
    drawRow(x0,x0+w,y0+h);
}

void Display::fillRectangle(int x0,int y0, int w, int h){
    int16_t x,y,x1,y1;
    x1=x0+w;y1=y0+h;
    if ((x0<0 && x1<0) || (x0>=width && x1 >=width)) return; //completely out of bounds
    if ((y0<0 && y1<0) || (y0>=height && y1 >=height)) return; //completely out of bounds
    if (x0>x1) {x=x1;x1=x0;}
    else x=x0;
    if (y0>y1) {y=y1;y1=y0;}
    else y=y0;
    if (x<0) x=0;
    if (y<0) y=0;
    for (;x<x1;x++) drawColumn(x,y,y1);
}

void Display::drawCircle(int16_t x0, int16_t y0, int16_t r) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r);
    drawPixel(x0, y0 - r);
    drawPixel(x0 + r, y0);
    drawPixel(x0 - r, y0);

    while (x < y) {
        if (f >= 0) {

            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y);
        drawPixel(x0 - x, y0 + y);
        drawPixel(x0 + x, y0 - y);
        drawPixel(x0 - x, y0 - y);
        drawPixel(x0 + y, y0 + x);
        drawPixel(x0 - y, y0 + x);
        drawPixel(x0 + y, y0 - x);
        drawPixel(x0 - y, y0 - x);

    }
}

void Display::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y);
            drawPixel(x0 + y, y0 + x);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y);
            drawPixel(x0 + y, y0 - x);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x);
            drawPixel(x0 - x, y0 + y);
        }
        if (cornername & 0x1) {

            drawPixel(x0 - y, y0 - x);
            drawPixel(x0 - x, y0 - y);
        }
    }
}

void Display::fillCircle(int16_t x0, int16_t y0, int16_t r) {
    drawFastVLine(x0, y0 - r, 2 * r );
    fillCircleHelper(x0, y0, r, 3, 0);
}

void Display::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t cornername, int16_t delta) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta-1); //added -1 here, jonne
            drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta-1); //added -1 here, jonne
        }
        if (cornername & 0x2) {

            drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta-1); //added -1 here, jonne
            drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta-1); //added -1 here, jonne
        }
    }
}

void Display::drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r) {
    if (r<2) {drawRectangle(x,y,w,h);return;}
    // smarter version
    drawFastHLine(x + r, y, w - 2 * r); // Top
    drawFastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
    drawFastVLine(x, y + r, h - 2 * r); // Left
    drawFastVLine(x + w - 1, y + r, h - 2 * r); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1);
    drawCircleHelper(x + w - r - 1, y + r, r, 2);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
    drawCircleHelper(x + r, y + h - r - 1, r, 8);
}

void Display::fillRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r) {
    if (r<2) {fillRectangle(x,y,w,h);return;}
    fillRectangle(x + r, y, w - 2 * r, h-1);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1);
}

void Display::drawTriangle(int16_t x0, int16_t y0,
                           int16_t x1, int16_t y1,
                           int16_t x2, int16_t y2) {
    drawLine(x0, y0, x1, y1);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x0, y0);
}

void Display::map1BitColumn(int16_t x, int16_t sy, int16_t ey, const uint8_t* bitmap, uint16_t column){
    if ((uint16_t)sy>=height && (uint16_t)ey>=height) return; //completely out of bounds
    if ((uint16_t)x>=width) return; //completely out of bounds
    if (sy>ey) {
        int y=sy;
        sy=ey;
        ey=y; // swap around so that x0 is less than x1
    }
    uint16_t bmw,bmh;
    float texelstep, texelindex;
    bmw = *(bitmap);
    bmh = *(bitmap+1);
    if (column>bmw-1) column=bmw-1;
    bitmap += 2;
    bitmap += column;
    texelstep = (float)bmh/((float)ey-(float)sy);
    texelindex = 0;
    for (int y=sy; y <= ey; y++, texelindex += texelstep) {
        uint8_t texel;
        uint8_t currbyte, bit;
        currbyte = texelindex / 8;
        bit = 7-((uint16_t) texelindex & 0x7);
        texel=*(bitmap+currbyte*bmw);
        if (texel & (1<<bit)) drawPixel(x,y);
        else if (bgcolor != invisiblecolor) drawPixel(x,y,bgcolor);
    }
}

int Display::bufferChar(int16_t x, int16_t y, uint16_t index){
    const uint8_t* bitmap = font;
    uint8_t w = *bitmap;
    uint8_t h = *(bitmap + 1);
    uint8_t hbytes=0, xtra=1;
    if (h==8 || h==16) xtra=0; //don't add if exactly on byte limit
    hbytes=(h>>3)+xtra; //GLCD fonts are arranged w+1 times h/8 bytes
    //bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
    bitmap = bitmap + 4 + index * (w * hbytes + 1); //add an offset to the pointer (fonts !)
    //int8_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    int8_t i, j, numBytes;
    numBytes = *bitmap++; //first byte of char is the width in bytes
    // GLCD fonts are arranged LSB = topmost pixel of char, so its easy to just shift through the column
    uint16_t bitcolumn; //16 bits for 2x8 bit high characters

    if( fontSize != 2 ) fontSize = 1;

    void (*drawPixelFG)(int16_t,int16_t, uint8_t) = &Display::drawPixelNOP;
    void (*drawPixelBG)(int16_t,int16_t, uint8_t) = &Display::drawPixelNOP;
    if( x>=0 && y >= 0 && x+w*fontSize<width && y+(h+1)*fontSize<height ){
	if( color != invisiblecolor )
	    drawPixelFG = &Display::drawPixelRaw;
	if( bgcolor != invisiblecolor )
	    drawPixelBG = &Display::drawPixelRaw;
    }else{
	if( color != invisiblecolor )
	    drawPixelFG = &Display::drawPixel;
	if( bgcolor != invisiblecolor )
	    drawPixelBG = &Display::drawPixel;
    }

    void (*drawPixel[])(int16_t,int16_t, uint8_t) = {drawPixelBG, drawPixelFG};
    uint8_t colors[] = {static_cast<uint8_t>(bgcolor), static_cast<uint8_t>(color)};

#if PROJ_ARDUBOY > 0
#else
    if( fontSize != 2 ){
#endif

        for (i = 0; i < numBytes; i++) {
            bitcolumn = *bitmap++;
            if (hbytes == 2) bitcolumn |= (*bitmap++)<<8; // add second byte for 16 bit high fonts
            for (j = 0; j <= h; j++) { // was j<=h
                uint8_t c = colors[ bitcolumn & 1 ];

#if PROJ_ARDUBOY > 0
                drawPixel[ bitcolumn&1 ](x, y + 7 - j,c);
#elif PROJ_SUPPORT_FONTROTATION > 0
                // if font flipping & rotation is allowed - do not slow down old programs!
                if (flipFontVertical) {
                    drawPixel[ bitcolumn&1 ](x, y + h - j,c);
                } else {
                    drawPixel[ bitcolumn&1 ](x, y + j,c);
                }
#else
                // "Normal" case
                drawPixel[ bitcolumn&1 ](x, y + j,c);
#endif // PROJ_ARDUBOY
                bitcolumn>>=1;

            }
#if PROJ_SUPPORT_FONTROTATION > 0
            if (flipFontVertical) x--;
            else x++;
#else
            x++;
#endif
        }

#if PROJ_SUPPORT_FONTROTATION > 0
        if (flipFontVertical) return -numBytes-adjustCharStep;
        else return numBytes+adjustCharStep; // for character stepping
#else
        return numBytes+adjustCharStep;
#endif


#if PROJ_ARDUBOY > 0
#else
    }else{

	for (i = 0; i < numBytes; i++) {
	    bitcolumn = *bitmap++;
	    if (hbytes == 2) bitcolumn |= (*bitmap++)<<8; // add second byte for 16 bit high fonts
	    for (j = 0; j <= h; j++) { // was j<=h
		uint8_t c = colors[ bitcolumn & 1 ];

		drawPixel[ bitcolumn&1 ](x + (i<<1)  , y + (j<<1), c);
		drawPixel[ bitcolumn&1 ](x + (i<<1)+1, y + (j<<1), c);
		drawPixel[ bitcolumn&1 ](x + (i<<1)  , y + (j<<1)+1, c);
		drawPixel[ bitcolumn&1 ](x + (i<<1)+1, y + (j<<1)+1, c);
		bitcolumn>>=1;

	    }
	}

	return (numBytes+adjustCharStep)<<1;

    }
#endif // PROJ_ARDUBOY

}

// BITMAP BLITTING FUNCTIONS

void Display::drawMonoBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint8_t index) {
    uint8_t w = *bitmap;
    uint8_t h = *(bitmap + 1);
    uint8_t xtra=0;
    if (w&0x7) xtra=1;
    bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
    int8_t scrx,scry;
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    int8_t bitptr;
    for (scry = y; scry < y+h; scry+=1) {
        if ((x&1)==0) { /** EVEN pixel starting line**/
            for (scrx = x, bitptr=7; scrx < w+x; scrx+=2) {
                uint8_t targetpixel = *scrptr;
                if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF) | color<<4; // upper nibble
                else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF) | bgcolor<<4; // upper nibble
                bitptr--;
                if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF0) | color; // lower nibble
                else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF0) | bgcolor; // lower nibble
                bitptr--;
                if (bitptr<0) { bitptr = 7; bitmap++; }
                *scrptr = targetpixel;
                scrptr++;
            }
        } else { /** ODD pixel starting line **/
            for (scrx = x, bitptr=7; scrx < w+x; scrx+=2) {
                uint8_t targetpixel = *scrptr;
                // store higher nibble of source pixel in lower nibble of target
                if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF0) | color; // lower nibble
                else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF0) | bgcolor; // lower nibble
                *scrptr = targetpixel; // store
                bitptr--;scrptr++;targetpixel = *scrptr;
                // store lower nibble of source pixel in higher nibble of target
                if (*bitmap & (1<<bitptr)) targetpixel = (targetpixel & 0xF) | color<<4; // higher nibble
                else if (bgcolor != invisiblecolor) targetpixel = (targetpixel & 0xF) | bgcolor<<4; // higher nibble
                *scrptr = targetpixel; // store
                bitptr--; // do not increment scrptr here !
            }
        }
        if (bitptr!=7) bitmap++; // force skip to next line
        // increment the y jump in the scrptr
        scrptr = scrptr + ((width - w)>>1);
    }
}

void Display::drawBitmapData2BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
    for (i = 0; i < w; i++) {
        byteNum = i / 4;
        bitNum = (i % 4)<<1;
        for (j = 0; j < h; j++) {
            uint8_t source = *(bitmap + (j * byteWidth) + byteNum);
            uint8_t output = (source & (0xC0 >> bitNum));
            output >>= (6-bitNum);
            if (output != Display::invisiblecolor) {
                setColor(output);
                drawPixel(x + i, y + j);
            }
        }
    }
}

void Display::drawBitmapData4BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    int16_t scrx,scry,xjump,scrxjump;
    int16_t xclip;
    xclip=xjump=scrxjump=0;
    /** y clipping */
    if (y<0) { h+=y; bitmap -= y*(w>>1); y=0;}
    else if (y+h>height) { h -=(y-height);}
    /** x clipping */
    if (x<0) { xclip=(x&1)<<1; w+=x; xjump = ((-x)>>1); bitmap += xjump; x=0;}
    else if (x+w>width) {
        xclip = (x&1)<<1;
        scrxjump = x&1;
        xjump=((x+w-width)>>1)+scrxjump;
        w = width-x;}

    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    /** ONLY 4-bit mode for time being **/
    for (scry = y; scry < y+h; scry+=1) {
        if (scry>=height) return;
        if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
            for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                uint8_t sourcepixel = *bitmap;
                if (xclip) {
                    sourcepixel <<=4;
                    sourcepixel |= ((*(bitmap+1))>>4);
                }
                uint8_t targetpixel = *scrptr;
                if ((sourcepixel>>4) != Display::invisiblecolor ) targetpixel = (targetpixel&0x0F) | (sourcepixel & 0xF0);
                if ((sourcepixel&0x0F) != Display::invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F);
                *scrptr = targetpixel;
                bitmap++;
                scrptr++;
            }
            if (xclip){
                if (w&1) {
                    /**last pixel is odd pixel due to clipping & odd width*/
                    uint8_t sourcepixel = *bitmap;
                    if ((sourcepixel&0x0F) != Display::invisiblecolor) {
                        sourcepixel <<=4;
                        volatile uint8_t targetpixel = *scrptr;// & 0x0F;
                        targetpixel &= 0xF; //clear upper nibble
                        targetpixel |= sourcepixel; //now OR it
                        *scrptr = targetpixel;
                    }
                    //scrptr++;
                }
                bitmap++;
                scrptr++;
            }
            bitmap += xjump; // needed if x<0 clipping occurs
        } else { /** ODD pixel starting line **/
            uint8_t sourcepixel;
            uint8_t targetpixel;
            for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                sourcepixel = *bitmap;
                targetpixel = *scrptr;
                // store higher nibble of source pixel in lower nibble of target
                if((sourcepixel>>4)!=Display::invisiblecolor)
                    targetpixel = (targetpixel & 0xF0) | (sourcepixel >> 4 );
                *scrptr = targetpixel;
                scrptr++;
                targetpixel = *scrptr;
                // store lower nibble of source pixel in higher nibble of target
                if((sourcepixel&0x0F)!=Display::invisiblecolor)
                    targetpixel = (targetpixel & 0x0F) | (sourcepixel << 4);
                *scrptr = targetpixel;
                bitmap++;
            }
            if (xclip) {
                // last line, store higher nibble of last source pixel in lower nibble of last address
                targetpixel = *scrptr;
                sourcepixel = *bitmap >> 4;
                if(sourcepixel!=Display::invisiblecolor)
                    targetpixel = (targetpixel & 0xF0) | sourcepixel;
                *scrptr = targetpixel;
            }
            bitmap+=xjump;
        }
        // increment the y jump in the scrptr
        scrptr = scrptr + ((width - w)>>1)+scrxjump;
    }
}

void Display::drawBitmapData8BPP(int x, int y, int w, int h, const uint8_t* bitmap) {
    int16_t scrx,scry;//,scrxjump;
    int16_t xjump=0;
    /** y clipping */
    if (y<0) {
        h += y;
        bitmap -= y*w;
        y = 0;
    } else if (y+h>height) {
        h -= (y-height);
    }

    /** x clipping */
    if (x<0) {
        w+=x;
        xjump = (-x);
        bitmap += xjump;
        x=0;
    } else if (x+w>width) {
        xjump=(x+w-width);
        w = width-x;
    }

    uint8_t* scrptr = m_scrbuf + (y*width + x);
    for (scry = y; scry < y+h; scry+=1) {
        if (scry>=height) return;
        for (scrx = x; scrx < w+x; scrx++) {
            uint8_t sourcepixel = *bitmap;
            uint8_t targetpixel = *scrptr;
            if (sourcepixel != Display::invisiblecolor )
                targetpixel = sourcepixel;
            *scrptr = targetpixel;
            bitmap++;
            scrptr++;
        }
        bitmap += xjump; // needed if horizontal clipping occurs
        scrptr = scrptr + (width - w);
    }
}

// RLE decoding
#define RLE_ESC_EOL 0
#define RLE_ESC_EOB 1
#define RLE_ESC_OFFSET 2

void Display::drawRleBitmap(int16_t x, int16_t y, const uint8_t* rlebitmap)
{
    // ONLY can copy 4-bit bitmap to 4-bit screen mode for time being
    if(POK_COLORDEPTH != 4)
        return;

    int16_t w = *rlebitmap;
    int16_t h = *(rlebitmap + 1);
    rlebitmap = rlebitmap + 2; //add an offset to the pointer to start after the width and height

    // visibility check
    if (y<-h || y>height) return; //invisible
    if (x<-w || x>width) return;  //invisible

    // Clipping is not supported
    if ((x < 0) || (x+w > width) || (y < 0) || (y+h > height)) return;

    // Currently only support RLE bitmaps in 16 color mode.
    if (m_colordepth != 4)  //
        return;

    // Go through each line.
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    bool is_endofbitmap = false;
    for (int16_t scry = y; scry < y+h && !is_endofbitmap;) {

        // Process one line. Go through each pixel run and escape command in RLE data.
        for (int16_t scrx = x;;) {
            uint8_t rle_count = *rlebitmap++;

            if (rle_count == 0) {

                /* Escape or absolute mode */

                uint8_t rle_escape_or_runsize = *rlebitmap++;
                if ( rle_escape_or_runsize == RLE_ESC_EOL) {
                    // End of line.
                    break;
                }
                else if ( rle_escape_or_runsize == RLE_ESC_EOB) {
                    // End of bitmap.
                    is_endofbitmap = true;
                    break;
                }
                else if ( rle_escape_or_runsize == RLE_ESC_OFFSET) {
                    // Move position in target.
                    // TODO: not tested yet.
                    uint8_t xoffset = *rlebitmap++;
                    uint8_t yoffset = *rlebitmap++;
                    scrptr += (xoffset>1);
                    scrx += xoffset;
                    scrptr += yoffset*width;
                    scry += yoffset;
                }
                else {

                    /* Absolute mode. Copy pixels from the source bitmap to the target screen. */

                    int16_t runsize = rle_escape_or_runsize;
                    uint8_t targetpixel = *scrptr;  // initial value
                    uint8_t sourcepixel = *rlebitmap;  // initial value
                    for( int16_t runx = 0; runx < runsize; ) {
                        if (scrx&0x1)  { // screen pixel is in the low nibble
                            if (runx&0x1) { // bitmap pixel is in the low nibble
                                if ((sourcepixel&0x0F) != invisiblecolor)
                                    targetpixel = (targetpixel&0xF0) | (sourcepixel&0x0F); // Copy low to low nibble.
                                rlebitmap++;
                            }
                            else // bitmap pixel is in the high nibble
                                if ((sourcepixel>>4) != invisiblecolor)
                                    targetpixel = (targetpixel&0xF0) | (sourcepixel>>4); // Copy high to low nibble.

                            // Copy the byte to the target.
                            *scrptr = targetpixel;
                            scrptr++;
                        }
                        else  { // screen pixel is in the high nibble
                            targetpixel = *scrptr;
                            sourcepixel = *rlebitmap;
                            if (runx&0x1) { // bitmap pixel is sourcepixel = *rlebitmapin the low nibble
                                if ((sourcepixel&0x0F) != invisiblecolor )
                                    targetpixel = (targetpixel&0x0F) | ((sourcepixel<<4)&0xF0); // Copy low to high nibble.
                                rlebitmap++;  // read the new source byte
                            }
                            else // bitmap pixel is in the high nibble
                                if ((sourcepixel>>4) != invisiblecolor )
                                    targetpixel = (targetpixel&0x0F) | (sourcepixel&0xF0); // Copy high to high nibble.
                        }
                        runx++;
                        scrx++;
                    }  // end for

                    // If this is odd target index, copy the byte to the target.
                    if (scrx&0x1) {
                        *scrptr = targetpixel;
                        scrptr++;
                    }

                    // In absolute mode the source size is always padded to the word boundary.
                    if (runsize%4) {
                        int16_t padpixcount = 4 - (runsize%4);
                        rlebitmap += padpixcount>>1;  // skip n padding bytes
                    }
                }
            }
            else {

                /* Encoded mode. Duplicate one pixel pair to the all required pixels on the target screen */

                int16_t runsize = rle_count;
                uint8_t clonepixelpair = *rlebitmap++;
                uint8_t targetpixel = *scrptr;  // initial value
                for( int16_t runx = 0;  runx < runsize; ) {
                    if (scrx&0x1)  { // screen pixel is in the low nibble
                        if (runx&0x1) { // bitmap pixel is in the low nibble
                            if ((clonepixelpair&0x0F) != invisiblecolor)
                                targetpixel = (targetpixel&0xF0) | (clonepixelpair&0x0F); // Copy low to low nibble.
                        }
                        else // bitmap pixel is in the high nibble
                            if ((clonepixelpair>>4) != invisiblecolor)
                                targetpixel = (targetpixel&0xF0) | (clonepixelpair>>4); // Copy high to low nibble.

                        // Copy the byte to the target.
                        *scrptr = targetpixel;
                        scrptr++;
                    }
                    else  { // screen pixel is in the high nibble
                        targetpixel = *scrptr;
                        if (runx&0x1) {// bitmap pixel is in the low nibble
                            if ((clonepixelpair&0x0F) != invisiblecolor )
                                targetpixel = (targetpixel&0x0F) | ((clonepixelpair<<4)&0xF0); // Copy low to high nibble.
                        }
                        else // bitmap pixel is in the high nibble
                            if ((clonepixelpair>>4) != invisiblecolor )
                                targetpixel = (targetpixel&0x0F) | (clonepixelpair&0xF0); // Copy high to high nibble.
                    }
                    runx++;
                    scrx++;

                }  // end for

                // If this is odd target index, copy the byte to the target.
                if (scrx&0x1) {
                    *scrptr = targetpixel;
                    scrptr++;
                }
            } // end if
        }  // end while

        // Increment the target screen pointer and index.
        scrptr = scrptr + ((width - w)>>1);
        scry++;
    } // end for scry
}

void Display::drawBitmapDataXFlipped2BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    int16_t i, j, byteNum, bitNum, byteWidth = w >> 2;
    for (i = 0; i < w; i++)
    {
        byteNum = i / 4;
        bitNum = (i % 4)<<1;
        for (j = 0; j < h; j++)
        {
            uint8_t source = *(bitmap + j * byteWidth + byteNum);
            uint8_t output = (source & (0xC0 >> bitNum));
            output >>= (6-bitNum);
            if (output != Display::invisiblecolor)
            {
                setColor(output);
                drawPixel(x + i, y + j);
            }
        }
    }
}

void Display::drawBitmapDataXFlipped4BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    /** 4bpp fast version */
    int16_t orgw = w;
    int16_t scrx,scry,xclip,xjump,scrxjump;
    xclip=xjump=scrxjump=0;
    /** y clipping */
    if (y<0)
    {
        h+=y;
        bitmap -= y*(w>>1);
        y=0;
    }
    else if (y+h>height)
    {
        h -=(y-height);
    }
    /** x clipping */
    bitmap += ((w>>1)-1); //inverted!
    if (x<0)
    {
        xclip=(x&1)<<1;
        w+=x;
        xjump = ((-x)>>1);
        bitmap -= xjump;
        x=0;
    }
    else if (x+w>width)
    {
        xclip = (x&1)<<1;
        scrxjump = x&1;
        xjump=((x+w-width)>>1)+scrxjump;
        w = width-x;
    }

    //uint8_t* scrptr = m_scrbuf + (y*(width>>1) + ((x+width)>>1));
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
    /** ONLY 4-bit mode for time being **/
    for (scry = y; scry < y+h; scry+=1)
    {
        if (scry>=height) return;
        if ((x&1)==0)   /** EVEN pixel starting line, very simple, just copypaste **/
        {
            for (scrx = x; scrx < w+x-xclip; scrx+=2)
            {
                uint8_t sourcepixel = *(bitmap);
                if (xclip)
                {
                    sourcepixel >>=4;
                    sourcepixel |= ((*(bitmap-1))<<4); // inverted nibbles
                }
                uint8_t targetpixel = *scrptr;
                // NIBBLES ARE INVERTED BECAUSE PICTURE IS FLIPPED !!!
                if ((sourcepixel>>4) != Display::invisiblecolor ) targetpixel = (targetpixel&0xF0) | (sourcepixel>>4);
                if ((sourcepixel&0x0F) != Display::invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel<<4);
                *scrptr = targetpixel;
                bitmap--;
                scrptr++;
            }
            if (xclip)
            {
                if (w&1)
                {
                    /**last pixel is odd pixel due to clipping & odd width*/
                    uint8_t sourcepixel = *bitmap;
                    if ((sourcepixel>>4) != Display::invisiblecolor)
                    {
                        sourcepixel &= 0xf0;
                        uint8_t targetpixel = *scrptr & 0x0F;
                        targetpixel |= sourcepixel;
                        *scrptr = targetpixel;
                    }
                    scrptr++;
                }
            }
            bitmap += (w>>1) + (orgw>>1); // Go the the last (visible) pixel of the next line
        }
        else     /** ODD pixel starting line **/
        {
            for (scrx = x; scrx < w+x-xclip; scrx+=2 )
            {
                uint8_t sourcepixel = *bitmap;
                uint8_t targetpixel = *scrptr;
                // inverted !!! store lower nibble of source pixel in lower nibble of target
                if((sourcepixel&0x0F)!=Display::invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F );
                *scrptr = targetpixel;
                scrptr++;
                targetpixel = *scrptr;
                // inverted ! store higher nibble of source pixel in higher nibble of target
                if((sourcepixel>>4)!=Display::invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel & 0xF0);
                *scrptr = targetpixel;
                bitmap--;
            }

            if (xclip)
            {
                if (w&1)
                {
                    /**last pixel is odd pixel due to clipping & odd width*/
                    uint8_t sourcepixel = *bitmap;
                    if((sourcepixel&0x0F)!=Display::invisiblecolor)
                    {
                        //sourcepixel <<=4;
                        uint8_t targetpixel = *scrptr;// & 0x0F;
                        targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F );
                        *scrptr = targetpixel;
                    }
                    scrptr++;
                }
            }
            bitmap += (w>>1) + (orgw>>1); // Go the the last (visible) pixel of the next line
        }
        // increment the y jump in the scrptr
        scrptr = scrptr + ((width - w)>>1);
    }
}

void Display::drawBitmapDataXFlipped8BPP(int x, int y, int w, int h, const uint8_t* bitmap){
    int16_t scrx,scry;//,scrxjump;
    int16_t xjump=0;
    /** y clipping */
    if (y<0)
    {
        h+=y;
        bitmap -= y*w;
        y=0;
    }
    else if (y+h>height)
    {
        h -=(y-height);
    }
    /** x clipping */
    if (x<0)
    {
        w+=x;
        xjump = (-x);
        bitmap += xjump;
        x=0;
    }
    else if (x+w>width)
    {
        xjump=(x+w-width);
        w = width-x;
    }

    uint8_t* scrptr = m_scrbuf + (y*width + x) + w;
    for (scry = y; scry < y+h; scry+=1)
    {
        if (scry>=height) return;
        for (scrx = x; scrx < w+x; scrx++)
        {
            uint8_t sourcepixel = *bitmap;
            uint8_t targetpixel = *scrptr;
            if (sourcepixel != Display::invisiblecolor )
                targetpixel = sourcepixel;
            *scrptr = targetpixel;
            bitmap++;
            scrptr--;
        }
        bitmap += xjump; // needed if horizontal clipping occurs
        scrptr = scrptr + (width + w);
    }
}

// Note: currently for 4 bpp only
void Display::drawBitmapDataYFlipped(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmap) {

    /** visibility check */
    if (y<-h || y>height) return; //invisible
    if (x<-w || x>width) return;  //invisible

    /** 4bpp fast version */
    if (m_colordepth==4) {

        /** 4bpp fast version */
        int16_t orgw = w;
        int16_t scrx,scry,xjump,scrxjump;
        int16_t xclip;
        bitmap += (h-1)*(w>>1); // Start the bitmap from the last line
        xclip=xjump=scrxjump=0;
        /** y clipping */
        if (y<0) {
            h+=y; bitmap += y*(w>>1); y=0;
        }
        else if (y+h>height) {
            h -=(y-height);
        }

        /** x clipping */
        if (x<0) {
            xclip=(x&1)<<1; w+=x; xjump = ((-x)>>1); bitmap += xjump; x=0;
        }
        else if (x+w>width) {
            xclip = (x&1)<<1;
            scrxjump = x&1;
            xjump=((x+w-width)>>1)+scrxjump;
            w = width-x;
        }

        xjump -= orgw; // Jump one line up more: 2*(orgw>>1)

        uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));
        /** ONLY 4-bit mode for time being **/
        for (scry = y; scry < y+h; scry+=1) {
            if (scry>=height) return;
            if ((x&1)==0) { /** EVEN pixel starting line **/
                for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                    uint8_t sourcepixel = *bitmap;
                    if (xclip) {
                        sourcepixel <<=4;
                        sourcepixel |= ((*(bitmap+1))>>4);
                    }
                    uint8_t targetpixel = *scrptr;
                    if ((sourcepixel>>4) != invisiblecolor ) targetpixel = (targetpixel&0x0F) | (sourcepixel & 0xF0);
                    if ((sourcepixel&0x0F) != invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel & 0x0F);
                    *scrptr = targetpixel;
                    bitmap++;
                    scrptr++;
                }
                if (xclip){
                    if (w&1) {
                        /**last pixel is odd pixel due to clipping & odd width*/
                        uint8_t sourcepixel = *bitmap;
                        if ((sourcepixel&0x0F) != invisiblecolor) {
                            sourcepixel <<=4;
                            volatile uint8_t targetpixel = *scrptr;// & 0x0F;
                            targetpixel &= 0xF; //clear upper nibble
                            targetpixel |= sourcepixel; //now OR it
                            *scrptr = targetpixel;
                        }
                        //scrptr++;
                    }
                    bitmap++;
                    scrptr++;
                }
                bitmap += xjump; // needed if x<0 clipping occurs
            } else { /** ODD pixel starting line **/
                uint8_t sourcepixel;
                uint8_t targetpixel;
                for (scrx = x; scrx < w+x-xclip; scrx+=2) {
                    sourcepixel = *bitmap;
                    targetpixel = *scrptr;
                    // store higher nibble of source pixel in lower nibble of target
                    if((sourcepixel>>4)!=invisiblecolor) targetpixel = (targetpixel & 0xF0) | (sourcepixel >> 4 );
                    *scrptr = targetpixel;
                    scrptr++;
                    targetpixel = *scrptr;
                    // store lower nibble of source pixel in higher nibble of target
                    if((sourcepixel&0x0F)!=invisiblecolor) targetpixel = (targetpixel & 0x0F) | (sourcepixel << 4);
                    *scrptr = targetpixel;
                    bitmap++;
                }
                if (xclip) {
                    // last line, store higher nibble of last source pixel in lower nibble of last address
                    targetpixel = *scrptr;
                    sourcepixel = *bitmap >> 4;
                    if(sourcepixel!=invisiblecolor) targetpixel = (targetpixel & 0xF0) | sourcepixel;
                    *scrptr = targetpixel;
                }
                bitmap+=xjump;
            }
            // increment the y jump in the scrptr
            scrptr = scrptr + ((width - w)>>1)+scrxjump;
        }

        return;
    }
}

void Display::draw4BitColumn(int16_t x, int16_t y, uint8_t h, uint8_t* bitmap)
{
    int8_t scry;
    uint8_t* scrptr = m_scrbuf + (y*(width>>1) + (x>>1));

    /** ONLY 4-bit mode for time being **/

    if ((x&1)==0) { /** EVEN pixel starting line, very simple, just copypaste **/
        for (scry = y; scry < h+y; scry++) {
            uint8_t sourcepixel = *bitmap;
            uint8_t targetpixel = *scrptr;
            targetpixel = (targetpixel&0x0F) | (sourcepixel << 4);
            *scrptr = targetpixel;
            bitmap++;
            scrptr+=55;
        }
    } else { /** ODD pixel starting line **/
        for (scry = y; scry < h+y; scry++) {
            uint8_t sourcepixel = *bitmap;
            uint8_t targetpixel = *scrptr;
            // store source pixel in lower nibble of target
            targetpixel = (targetpixel & 0xF0) | (sourcepixel);
            *scrptr = targetpixel;
            scrptr+=55;
            bitmap++;
        }
    }
}

#endif
