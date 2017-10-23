/**************************************************************************/
/*!
    @file     BmpImage.cpp
    @author   Hannu Viitala. Original BMP decoder code by Jonne Valola.

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#if PROJ_WINDOWS
#include "defines_win_SIM.h"
#endif

#if PROJ_LINUX
#include "defines_linux_SIM.h"
#endif

#include "Pokitto.h"
#include "PokittoDisk.h"
#ifdef POK_SIM
#include "FileIO.h"
#endif

#if POK_ENABLE_SD > 0


#include "Pokittodisplay.h"
#include "ImageFormat.h"

Pokitto::Core _game;
Pokitto::Display pokdisp;

int openImageFileFromSD(char* filepath, uint16_t **palette_out, uint8_t **bitmap_out) {

    // Reset the out pointers.
    *palette_out = 0;
    *bitmap_out = 0;

    BITMAPFILEHEADER bf;
    BITMAPINFO bmi;

    uint32_t bytes_read=0, bmpsize=0;

    /** numcol is the number of colors for output */
    unsigned int numcol = 0;

    if (POK_COLORDEPTH == 8) numcol=256;
    else if (POK_COLORDEPTH == 4) numcol=16;
    else if (POK_COLORDEPTH == 2) numcol=4;
    else if (POK_COLORDEPTH == 1) numcol=1;

    if (!isThisFileOpen(filepath)) {
        fileClose(); // close any open files
        fileOpen(filepath, FILE_MODE_READONLY | FILE_MODE_BINARY);
    }
    else
        return -1;  // Already open, not good.

    if (fileOK() && fileReadBytes((uint8_t*)&bf, sizeof(bf)) == sizeof(bf) ) {
        bytes_read += sizeof(bf);
    }
	else
	{
		POK_TRACE("Error reading BMP header\n");
		fileClose();
		return(-1);
	}

    if (fileReadBytes((uint8_t*)&bmi,sizeof(bmi.bmiHeader)) != sizeof(bmi.bmiHeader)) {
		POK_TRACE("Error reading BMP info\n");
		fileClose();
		return(-1);
	}
    bytes_read += sizeof(bmi.bmiHeader);

    /** Check image validity */

    if (bf.bfType != 0x4D42) {
        POK_TRACE("Bitmap file has an unrecognized format (4D42 id missing from beginning).\n");
        POK_TRACE("BMP2POK accepts .BMP files that have an indexed (1,-bit, 4-bit or 8-bit) color palette.\n");
        fileClose();
        return(-1);
    }
    if (bmi.bmiHeader.biBitCount != POK_COLORDEPTH ) {
        POK_TRACE("ERROR!\nThe image color depth should be the same as screen color depth!\n");
    }
    if (bmi.bmiHeader.biWidth%32 && bmi.bmiHeader.biBitCount == 1) {
        POK_TRACE("ERROR!\nPadding of 1-bit (monochrome) images is not yet supported\n");
        POK_TRACE("1-bit images need to have width that is divisible by 32!\n");
        POK_TRACE("Adjust size of source image.\n");
        fileClose();
        return(-1);
    }
	if (bmi.bmiHeader.biWidth%4) {
        POK_TRACE("Width is not divisible by 4\n");
        fileClose();
        return(-1);
	}
	if (bmi.bmiHeader.biWidth%8 && bmi.bmiHeader.biBitCount==4) {
        if (bmi.bmiHeader.biWidth%4) {
            POK_TRACE("ERROR!\n4-bit source images have to have a width that is divisible by 4\n");
            fileClose();
            return(-1);
        }
	}
    if (bmi.bmiHeader.biBitCount != 8 && bmi.bmiHeader.biBitCount != 4 && bmi.bmiHeader.biBitCount != 1)
    {
        POK_TRACE("Only 8bpp, 4bpp & 1bpp BMP files are supported\n");
        fileClose();
        return(-1);
    }
    if (bmi.bmiHeader.biCompression != 0 &&
        !(bmi.bmiHeader.biCompression == BI_RLE4 && bmi.bmiHeader.biBitCount == 4))
    {
        POK_TRACE("Only RLE compression for bitmaps with 4 bpp is supported\n");
        fileClose();
        return(-1);
    }

    /* If the height is negative the bmp image is in the correct way.
       If the heigh is positive the bmp image is vertically mirrored
    */
    int biAbsHeight = bmi.bmiHeader.biHeight;
    if (bmi.bmiHeader.biHeight < 0 )
        biAbsHeight = - bmi.bmiHeader.biHeight;

    /** Read and copy palette **/

    int c = bmi.bmiHeader.biClrUsed;
    if (c==0) c = 1 << bmi.bmiHeader.biBitCount; // from MS BMP specs. 0 means 2^n colors
    bmi.bmiHeader.biClrUsed = c;

    /* Allocate memory for the output parameter */
    if (numcol>bmi.bmiHeader.biClrUsed) numcol = bmi.bmiHeader.biClrUsed;
    *palette_out = (uint16_t*) malloc(numcol*2);
  	if (*palette_out == NULL)
	{
		POK_TRACE("Error allocating temporary palette buffer.\n");
		free(*palette_out);
		return(-1);
	}

    /* seek to the beginning of the color table - because of gimp */
    fileSeekAbsolute(bf.bfOffBits-c*4); //gfx data star minus color table

    for (unsigned int c=0;c<numcol;c++) {

        RGBQUAD rgbValue;
        fileReadBytes((uint8_t*)&rgbValue, sizeof(RGBQUAD));
        bytes_read += sizeof(RGBQUAD);

        unsigned int r,g,b,o;
        r = rgbValue.rgbRed >> 3; // 5 bit
        g = rgbValue.rgbGreen >> 2; // 6 bits
        b = rgbValue.rgbBlue >> 3; // 5 bits
        o = (r<<11)|(g<<5)|b;

        (*palette_out)[c] = o;
    }

    /** Read and copy image data **/

    /* Get image data size. If the biSizeImage is given (>0) for RLE image, use that to reduce memory usage. */
    bmpsize = bmi.bmiHeader.biWidth * biAbsHeight*bmi.bmiHeader.biBitCount/8;
	if (bmi.bmiHeader.biCompression == BI_RLE4)
        bmpsize = (bmi.bmiHeader.biSizeImage > 0) ? bmi.bmiHeader.biSizeImage : bmpsize;

    // SEEK to the beginning of the data
	fileSeekAbsolute(bf.bfOffBits);

   /* Allocate output data buffer */
    *bitmap_out = (uint8_t *) malloc(bmpsize + 2);  // header takes 2 bytes
    if (*bitmap_out == NULL)
    {
        POK_TRACE("Error allocating temporary data buffer, is image too big?\n");
        free(*palette_out);
        return(-1);
    }

    /* Store image size to the pokitto bitmap header */
    uint32_t outindex = 0;
    (*bitmap_out)[outindex++] = bmi.bmiHeader.biWidth;
    (*bitmap_out)[outindex++] = biAbsHeight;

 	if (bmi.bmiHeader.biCompression == BI_RLE4) {
        bool eofReached = false;
        while (outindex < bmpsize && !eofReached ) {
            /* Read byte from the file. */
            unsigned char rleByte;
            if (fileReadBytes(&rleByte, sizeof(rleByte)) != sizeof(rleByte))
            {
                /* End of file reached. Allocate a new bitmap which is of the exact size of the data */
                eofReached = true;

                /* Allocate output data buffer */
                uint8_t* old_bitmap = *bitmap_out;
                *bitmap_out = NULL;
                *bitmap_out = (uint8_t *) malloc(outindex);  // header takes 2 bytes
                if (*bitmap_out == NULL)
                {
                    POK_TRACE("Error allocating temporary data buffer, is image too big?\n");
                    free(old_bitmap);
                    free(*palette_out);
                    return(-1);
                }

                /* Copy data */
                for (int i=0; i<outindex;i++)
                   (*bitmap_out)[i] = old_bitmap[i];

                /* Free original bitmap */
                free(old_bitmap);
            }
            else {
                /* Store byte */
                (*bitmap_out)[outindex++] = rleByte;
            }
        } // end while
	}
	else {
        /* Do vertical mirroring only for uncompressed data.
           Note the compressed RLE data above could not be mirrored.
        */
        int widthInBytes = bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount/8;
        int widthInBytesInc, incY, startY, endY;
        if (bmi.bmiHeader.biHeight > 0 ) {
            /* Mirror vertically */
            widthInBytesInc = - widthInBytes;
            incY = -1;
            startY = biAbsHeight - 1;
            endY = -1;
        }
        else {
            /* Do not mirror */
            widthInBytesInc = widthInBytes;
            incY = 1;
            startY = 0;
            endY = biAbsHeight;
        }

        /* Copy all bytes to the output bitmap */
        for ( int y = startY, beginLine=startY*widthInBytes; y != endY; y += incY, beginLine += widthInBytesInc) {

            /* Go to the beginning of the previous or next line */
            outindex = beginLine;

            for ( int xbyte = 0; xbyte < widthInBytes; xbyte++) {

                /* Read byte from the file. */
                unsigned char byteOfPixels;
                if (fileReadBytes(&byteOfPixels, sizeof(byteOfPixels)) != sizeof(byteOfPixels))
                {
                    POK_TRACE("Error reading BMP data\n");
                    fileClose();
                    free(*bitmap_out);
                    free(*palette_out);
                    return(-1);
                }

                /* Copy a byte from the file to the bitmap */
                (*bitmap_out)[2 + outindex] = byteOfPixels;
                outindex++;
            } // end for
        } // end for
	} // end if

	// Done with the file reading.
	fileClose();

    return 0;
}

int directDrawImageFileFromSD(int16_t sx, int16_t sy, char* filepath) {

    return(directDrawImageFileFromSD(0, 0, 0/* full width */, 0/* full height */, sx, sy, filepath));
}

int directDrawImageFileFromSD(uint16_t ix, uint16_t iy, uint16_t iw, uint16_t ih, int16_t sx, int16_t sy, char* filepath) {

    BITMAPFILEHEADER bf;
    BITMAPINFO bmi;

    uint32_t bytes_read=0;

    if (!isThisFileOpen(filepath)) {
        fileClose(); // close any open files
        fileOpen(filepath, FILE_MODE_READONLY | FILE_MODE_BINARY);
    }
    else {
        POK_TRACE("Error! Already open\n");
        return -1;  // Already open, not good.
    }

    if (fileOK() && fileReadBytes((uint8_t*)&bf, sizeof(bf)) == sizeof(bf) ) {  //!HV why we have to check fileOK()?
        bytes_read += sizeof(bf);
    }
	else
	{
		POK_TRACE("Error reading BMP header\n");
		fileClose();
		return(-1);
	}

    if (fileReadBytes((uint8_t*)&bmi,sizeof(bmi.bmiHeader)) != sizeof(bmi.bmiHeader)) {
		POK_TRACE("Error reading BMP info\n");
		fileClose();
		return(-1);
	}
    bytes_read += sizeof(bmi.bmiHeader);

    /** Check image validity */

    if (bf.bfType != 0x4D42) {
        POK_TRACE("Bitmap file has an unrecognized format (4D42 id missing from beginning).\n");
        POK_TRACE("BMP2POK accepts .BMP files that have an indexed (1,-bit, 4-bit or 8-bit) color palette.\n");
        fileClose();
        return(-1);
    }
    if (bmi.bmiHeader.biBitCount != 24 ) {
        POK_TRACE("ERROR!\nThe image color depth should be the same as screen color depth (%d)!\n");
		fileClose();
        return(-1);
    }
    if (bmi.bmiHeader.biCompression != 0 )
    {
        POK_TRACE("Compression is not supported.\n");
        fileClose();
        return(-1);
    }

    /* If the height is negative the bmp image is in the correct way.
       If the heigh is positive the bmp image is vertically mirrored
    */
    int biAbsHeight = bmi.bmiHeader.biHeight;
    if (bmi.bmiHeader.biHeight < 0 )
        biAbsHeight = - bmi.bmiHeader.biHeight;

    /** Zero size parameter means full image size */
    if(iw==0) iw = bmi.bmiHeader.biWidth; // 0 means full image width
    if(ih==0) ih = biAbsHeight; // 0 means full image width

    /** Check parameters */
    if( ix + iw > bmi.bmiHeader.biWidth ) {
        POK_TRACE("Error! Invalid parameter\n");
		fileClose();
        return(-1);
    }
    if( iy + ih > biAbsHeight ) {
        POK_TRACE("Error! Invalid parameter\n");
 		fileClose();
        return(-1);
    }
    if( sx > pokdisp.getWidth()-1 || sx<-iw) {
        POK_TRACE("Error! Invalid parameter\n");
 		fileClose();
        return(-1);
    }
    if( sy > pokdisp.getHeight()-1  || sy<-ih) {
        POK_TRACE("Error! Invalid parameter\n");
		fileClose();
        return(-1);
    }

    /** Zero size parameter means full image size */
    if(iw==0) iw = bmi.bmiHeader.biWidth; // 0 means full image width
    if(ih==0) ih = biAbsHeight; // 0 means full image width

    /** Clip image to screen dimensions */

    int16_t clipX1OnScreen = max( 0, sx);
    int16_t clipX2OnScreen = min( pokdisp.getWidth()-1, sx+iw-1);
    int16_t clipWidthOnScreen = clipX2OnScreen-clipX1OnScreen+1;
    int16_t clipY1OnScreen = max( 0, sy);
    int16_t clipY2OnScreen = min( pokdisp.getHeight()-1, sy+ih-1);
    int16_t clipHeightOnScreen = clipY2OnScreen-clipY1OnScreen+1;

    uint16_t skipImagePixelsAtLineStart = ix+(clipX1OnScreen-sx);
    uint16_t skipImagePixelsAtLineStartAsBytes = skipImagePixelsAtLineStart*3; // 3 bytes per pixel
    uint16_t skipImagePixelsAtLineEndAsBytes = (bmi.bmiHeader.biWidth-(skipImagePixelsAtLineStart+clipWidthOnScreen))*3; // 3 bytes per pixel

    uint16_t skipImageRowsAtImageStart = iy+(clipY1OnScreen-sy);
    uint16_t skipImageRowsAtImageEnd = biAbsHeight-(skipImageRowsAtImageStart+clipHeightOnScreen);

	/* Vertical loop parameters */
    int incY, startY, pastEndY;
    uint32_t skipImageRowsAsBytes = 0;
    if (bmi.bmiHeader.biHeight > 0 ) {
        /* Mirror vertically */
        incY = -1;
        startY = clipY2OnScreen;
        pastEndY = clipY1OnScreen-1;
        skipImageRowsAsBytes = skipImageRowsAtImageEnd*bmi.bmiHeader.biWidth*3; // 3 bytes per pixel
    }
    else {
        /* Do not mirror */
        incY = 1;
        startY = clipY1OnScreen ;
        pastEndY = clipY2OnScreen+1;
        skipImageRowsAsBytes = skipImageRowsAtImageStart*bmi.bmiHeader.biWidth*3; // 3 bytes per pixel
    }

    /** Read and copy image data directly to the screen **/

    /* Seek to the beginning of the data */
	fileSeekAbsolute(bf.bfOffBits);

    /* Seek until the image rect starts */
    if (skipImageRowsAsBytes>0) fileSeekRelative( skipImageRowsAsBytes );

    /* Copy all bytes to the output bitmap */
    for ( int y = startY; y != pastEndY; y += incY) {

         /* Seek until the image rect starts */
        if (skipImagePixelsAtLineStartAsBytes>0) fileSeekRelative( skipImagePixelsAtLineStartAsBytes );

        for ( int x = clipX1OnScreen; x <= clipX2OnScreen; x++) {

            /* Read RGB pixel from the file. For 24 bpp the pixel is stored to 3 bytes*/
            uint32_t rgb24;
            if (fileReadBytes((uint8_t*)&rgb24, 3) != 3)
            {
                POK_TRACE("Error reading BMP data\n");
                fileClose();
                return(-1);
            }

            /* Copy a pixel from the file directly to the screen */
//            uint8_t r,g,b;
//            r = (xrgb >> (3 + 16))  & 0x1f; // 5 bit
//            g = (xrgb >> (2 + 8))   & 0x3f; // 6 bits
//            b = (xrgb >> 3)         & 0x1f; // 5 bits
//            uint16_t targetpixel = (r<<11) | (g<<5) | b;

            uint16_t targetpixel =
               ((rgb24 >> 8) & 0x0000F800) |  // red (bits 15-11)
               ((rgb24 >> 5) & 0x000007E0) |  // green (bits 10-5)
               ((rgb24 >> 3) & 0x0000001F);  // blue (bits 4-0)
            _game.display.directPixel(x, y, targetpixel);
        } // end for

        /* Skip pixels at line end */
        if (skipImagePixelsAtLineEndAsBytes>0) fileSeekRelative( skipImagePixelsAtLineEndAsBytes );

    } // end for

	// Done with the file reading.
	fileClose();

    return 0;
}

#endif
