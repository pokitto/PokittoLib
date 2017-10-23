#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "helpers.h"

#define _MAX_DRIVE	3
#define _MAX_DIR	256
#define _MAX_FNAME	256
#define _MAX_EXT	256

#include <ctype.h>

char* _strupr( char* s )
  {
  char* p = s;
  while (*p = toupper( *p )) p++;
  return s;
  }

typedef struct tagRGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPFILEHEADER {
  int16_t  bfType;
  int32_t  bfSize;
  int16_t  bfReserved1;
  int16_t  bfReserved2;
  int32_t  bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;

#define BI_RLE4 2
typedef struct tagBITMAPINFOHEADER {
  int32_t biSize;
  int32_t  biWidth;
  int32_t  biHeight;
  int16_t  biPlanes;
  int16_t  biBitCount;
  int32_t biCompression;
  int32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t  biYPelsPerMeter;
  int32_t biClrUsed;
  int32_t biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} __attribute__((packed)) BITMAPINFO, *PBITMAPINFO;

RGBQUAD myColors[257];


