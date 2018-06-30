/*
   *** Video output module ***
   This file is part of the zBoy project!

   Copyright (C) Mateusz Viste 2010, 2011, 2012, 2013, 2014, 2015
*/


void RefreshScreen_NoScale(int x1, int x2, int y1, int y2) {
  /*
  static int x, y;
  for (x = x1; x <= x2; x++) {
    for (y = y1; y <= y2; y++) {
      drv_putpixel(x, y, ScreenBuffer[x][y]);
    }
  }
  /* SDL_UpdateRect(screen, x1, y1, x2+1-x1, y2+1-y1); */ /* Draw whatever is in the buffer to the screen */
  // drv_refreshscreen();
}


void RefreshScreen_BasicScale(int x1, int x2, int y1, int y2, int GraphicScaleFactor) {
  /* This is the most basic scaling method. Mostly known as "Nearest neighbour" scaling. * /
  static int x, y, xx, yy, tmp1, tmp2, tmp3;
  for (x = x1; x <= x2; x++) {
    tmp1 = (x * GraphicScaleFactor);
    for (y = y1; y <= y2; y++) {
      tmp2 = (y * GraphicScaleFactor);
      for (xx = 0; xx < GraphicScaleFactor; xx++) {
        tmp3 = tmp1 + xx;
        for (yy = 0; yy < GraphicScaleFactor; yy++) {
          drv_putpixel(tmp3, tmp2 + yy, ScreenBuffer[x][y]);
        }
      }
    }
  }
  /* Draw whatever is in the buffer to the screen */
  // drv_refreshscreen();
  /* SDL_UpdateRect(screen, x1 * GraphicScaleFactor, y1 * GraphicScaleFactor, (x2+1-x1) * GraphicScaleFactor, (y2+1-y1) * GraphicScaleFactor); */
}


void RefreshScreen_Scale2x(int LcdModifiedFirstX, int LcdModifiedLastX, int LcdModifiedFirstY, int LcdModifiedLastY, int GraphicScaleFactor) {
/* Scale2x is real-time graphics effect able to increase the size of small bitmaps guessing the missing pixels without blurring the images.
   It was originally developed for the AdvanceMAME project in the year 2001 to improve the quality of old games with a low video resolution. * /

  static int x, y, z, t, ScaleStepX, ScaleStepY;
  static int B, D, E, F, H, E0, E1, E2, E3;
  static int Scale2xScaleFactor = 0;
  static int tmpx1, tmpx2, tmpy1, tmpy2;
  if (Scale2xScaleFactor == 0) Scale2xScaleFactor = (GraphicScaleFactor >> 1);  /* Init it the 1st time -> SHR 1 means "divided by 2" (faster than /2) * /

  if (LcdModifiedFirstX > 0) LcdModifiedFirstX -= 1;  /* When using Scale2x, you can't work on per-pixel basis. * /
  if (LcdModifiedFirstY > 0) LcdModifiedFirstY -= 1;  /* If you change a pixel, make sure to also redraw        * /
  if (LcdModifiedLastX < 159) LcdModifiedLastX += 1;  /* all its neighbours, as these could be influenced       * /
  if (LcdModifiedLastY < 143) LcdModifiedLastY += 1;  /* as well.                                               * /

  for (y = LcdModifiedFirstY; y <= LcdModifiedLastY; y++) {
    for (x = LcdModifiedFirstX; x <= LcdModifiedLastX; x++) {
      /* First prepare the source pixels... * /
      E = ScreenBuffer[x][y];
      /* if ((x > 0) && (y > 0)) A = ScreenBuffer[x - 1][y - 1]; else A = E; * /
      if (y > 0) B = ScreenBuffer[x][y - 1]; else B = E;
      /* if ((x < 159) && (y > 0)) C = ScreenBuffer[x + 1][y - 1]; else C = E; * /
      if (x > 0) D = ScreenBuffer[x - 1][y]; else D = E;
      if (x < 159) F = ScreenBuffer[x + 1][y]; else F = E;
      /* if ((x > 0) && (y < 143)) G = ScreenBuffer[x - 1][y + 1]; else G = E; * /
      if (y < 143) H = ScreenBuffer[x][y + 1]; else H = E;
      /* if ((x < 159) && (y < 143)) I = ScreenBuffer[x + 1][y + 1]; else I = E; * /
      /* apply the Scale2x algorithm * /
      if (B != H && D != F) {
          E0 = D == B ? D : E;
          E1 = B == F ? F : E;
          E2 = D == H ? D : E;
          E3 = H == F ? F : E;
        } else {
          E0 = E;
          E1 = E;
          E2 = E;
          E3 = E;
      }

      z = (y * GraphicScaleFactor);
      t = (x * GraphicScaleFactor);
      for (ScaleStepX = 0; ScaleStepX < Scale2xScaleFactor; ScaleStepX++) {
        for (ScaleStepY = 0; ScaleStepY < Scale2xScaleFactor; ScaleStepY++) {
          tmpx1 = t + ScaleStepX;
          tmpx2 = tmpx1 + Scale2xScaleFactor;
          tmpy1 = z + ScaleStepY;
          tmpy2 = tmpy1 + Scale2xScaleFactor;

          drv_putpixel(tmpx1, tmpy1, E0);
          drv_putpixel(tmpx2, tmpy1, E1);
          drv_putpixel(tmpx1, tmpy2, E2);
          drv_putpixel(tmpx2, tmpy2, E3);
        }
      }
    }
  }
  /* Draw whatever is in the buffer to the screen * /
  drv_refreshscreen();
  /* SDL_UpdateRect(screen, LcdModifiedFirstX * GraphicScaleFactor, LcdModifiedFirstY * GraphicScaleFactor, (LcdModifiedLastX + 1 - LcdModifiedFirstX) * GraphicScaleFactor, (LcdModifiedLastY + 1 - LcdModifiedFirstY) * GraphicScaleFactor); */
}


void RefreshScreen_Scale3x(int LcdModifiedFirstX, int LcdModifiedLastX, int LcdModifiedFirstY, int LcdModifiedLastY, int GraphicScaleFactor) {
/* The AdvMAME3x/Scale3x algorithm is not the same as EPX, but is similar in a mathematical sense.
   A B C --\  1 2 3
   D E F    > 4 5 6
   G H I --/  7 8 9
    1=E; 2=E; 3=E; 4=E; 5=E; 6=E; 7=E; 8=E; 9=E;
    IF D==B AND D!=H AND B!=F => 1=D
    IF (D==B AND D!=H AND B!=F AND E!=C) OR (B==F AND B!=D AND F!=H AND E!=A) => 2=B
    IF B==F AND B!=D AND F!=H => 3=F
    IF (H==D AND H!=F AND D!=B AND E!=A) OR (D==B AND D!=H AND B!=F AND E!=G) => 4=D
    5=E
    IF (B==F AND B!=D AND F!=H AND E!=I) OR (F==H AND F!=B AND H!=D AND E!=C) => 6=F
    IF H==D AND H!=F AND D!=B => 7=D
    IF (F==H AND F!=B AND H!=D AND E!=G) OR (H==D AND H!=F AND D!=B AND E!=I) => 8=H
    IF F==H AND F!=B AND H!=D => 9=F
* /

  static int x, y, ScaleStepX, ScaleStepY, tmpx1, tmpx2, tmpx3, tmpy1, tmpy2, tmpy3;
  /*static unsigned int LcdModifiedFirstY, LcdModifiedLastY, LcdModifiedFirstX, LcdModifiedLastX; * /
  static int A, B, C, D, E, F, G, H, I, pix1, pix2, pix3, pix4, pix5, pix6, pix7, pix8, pix9;
  static int EPXscaleFactor = 0;
  if (EPXscaleFactor == 0) EPXscaleFactor = (GraphicScaleFactor / 3);  /* Init it the 1st time -> SHR 1 means "divided by 2" (faster than /2) * /

  if (LcdModifiedFirstX > 0) LcdModifiedFirstX -= 1;  /* When using EPX, you can't work on per-pixel basis. * /
  if (LcdModifiedFirstY > 0) LcdModifiedFirstY -= 1;  /* If you change a pixel, make sure to also redraw    * /
  if (LcdModifiedLastX < 159) LcdModifiedLastX += 1;  /* all its neighbours, as these could be influenced   * /
  if (LcdModifiedLastY < 143) LcdModifiedLastY += 1;  /* as well.                                           * /

  for (y = LcdModifiedFirstY; y <= LcdModifiedLastY; y++) {
    for (x = LcdModifiedFirstX; x <= LcdModifiedLastX; x++) {
        /* First prepare the source pixels... * /
        E = ScreenBuffer[x][y];
        if ((x > 0) && (y > 0)) A = ScreenBuffer[x - 1][y - 1]; else A = E;
        if (y > 0) B = ScreenBuffer[x][y - 1]; else B = E;
        if ((x < 159) && (y > 0)) C = ScreenBuffer[x + 1][y - 1]; else C = E;
        if (x > 0) D = ScreenBuffer[x - 1][y]; else D = E;
        if (x < 159) F = ScreenBuffer[x + 1][y]; else F = E;
        if ((x > 0) && (y < 143)) G = ScreenBuffer[x - 1][y + 1]; else G = E;
        if (y < 143) H = ScreenBuffer[x][y + 1]; else H = E;
        if ((x < 159) && (y < 143)) I = ScreenBuffer[x + 1][y + 1]; else I = E;
        /* Apply the Scale3x algorithm... * /
        if ((B != H) && (D != F)) {
            pix1 = D == B ? D : E;
            pix2 = (D == B && E != C) || (B == F && E != A) ? B : E;
            pix3 = B == F ? F : E;
            pix4 = (D == B && E != G) || (D == H && E != A) ? D : E;
            pix5 = E;
            pix6 = (B == F && E != I) || (H == F && E != C) ? F : E;
            pix7 = D == H ? D : E;
            pix8 = (D == H && E != I) || (H == F && E != G) ? H : E;
            pix9 = H == F ? F : E;
          } else {
            pix1 = E;
            pix2 = E;
            pix3 = E;
            pix4 = E;
            pix5 = E;
            pix6 = E;
            pix7 = E;
            pix8 = E;
            pix9 = E;
        }
        /* Now output the result onscreen * /
        for (ScaleStepX = 0; ScaleStepX < EPXscaleFactor; ScaleStepX++) {
          for (ScaleStepY = 0; ScaleStepY < EPXscaleFactor; ScaleStepY++) {
            tmpx1 = (x * GraphicScaleFactor) + ScaleStepX;
            tmpx2 = tmpx1 + EPXscaleFactor;
            tmpx3 = tmpx2 + EPXscaleFactor;
            tmpy1 = (y * GraphicScaleFactor) + ScaleStepY;
            tmpy2 = tmpy1 + EPXscaleFactor;
            tmpy3 = tmpy2 + EPXscaleFactor;

            drv_putpixel(tmpx1, tmpy1, pix1);
            drv_putpixel(tmpx2, tmpy1, pix2);
            drv_putpixel(tmpx3, tmpy1, pix3);
            drv_putpixel(tmpx1, tmpy2, pix4);
            drv_putpixel(tmpx2, tmpy2, pix5);
            drv_putpixel(tmpx3, tmpy2, pix6);
            drv_putpixel(tmpx1, tmpy3, pix7);
            drv_putpixel(tmpx2, tmpy3, pix8);
            drv_putpixel(tmpx3, tmpy3, pix9);
          }
        }
      }
    }
    /* Draw whatever is in the buffer to the screen * /
    drv_refreshscreen();
    /* SDL_UpdateRect(screen, LcdModifiedFirstX * GraphicScaleFactor, LcdModifiedFirstY * GraphicScaleFactor, (LcdModifiedLastX + 1 - LcdModifiedFirstX) * GraphicScaleFactor, (LcdModifiedLastY + 1 - LcdModifiedFirstY) * GraphicScaleFactor); */
}


void RefreshScreen_Eagle(int LcdModifiedFirstX, int LcdModifiedLastX, int LcdModifiedFirstY, int LcdModifiedLastY, int GraphicScaleFactor) {
/* Eagle works as follows: for every pixel we will generate 4 out pixels, first, set all 4 to
   the colour of the in pixel we are currently scaling (like nearest-neighbor), next lets look
   at the pixels up and to the left, if they are the same colour as each other set the top left
   pixel to that colour, continue doing the same for all four pixels, and then move to the next
   one.

   Assume an input matrix of 3x3 pixels where the center most pixel is the pixel to be scaled,
   and an output matrix of 2x2 pixels (i.e., the scaled pixel)

   first:        |Then
   . . . --\ CC  |S T U  --\ 1 2
   . C . --/ CC  |V C W  --/ 3 4
   . . .         |X Y Z
                 | IF V==S==T => 1=S
                 | IF T==U==W => 2=U
                 | IF V==X==Y => 3=X
                 | IF W==Z==Y => 4=Z
* /
  static int x, y, ScaleStepX, ScaleStepY;
  static int pixC, pix1, pix2, pix3, pix4, pixS, pixT, pixU, pixV, pixW, pixX, pixY, pixZ;
  static int EagleScaleFactor = 0;
  if (EagleScaleFactor == 0) EagleScaleFactor = (GraphicScaleFactor >> 1);  /* Init it the 1st time -> SHR 1 means "divided by 2" (faster than /2) * /

  if (LcdModifiedFirstX > 0) LcdModifiedFirstX -= 1;  /* When using Eagle, you can't work on per-pixel basis. * /
  if (LcdModifiedFirstY > 0) LcdModifiedFirstY -= 1;  /* If you change a pixel, make sure to also redraw      * /
  if (LcdModifiedLastX < 159) LcdModifiedLastX += 1;  /* all its neighbours, as these could be influenced     * /
  if (LcdModifiedLastY < 143) LcdModifiedLastY += 1;  /* as well.                                             * /

  for (y = LcdModifiedFirstY; y <= LcdModifiedLastY; y++) {
    for (x = LcdModifiedFirstX; x <= LcdModifiedLastX; x++) {
        /* Now apply the Eagle algorithm * /
        pixC = ScreenBuffer[x][y];
        pix1 = pixC;
        pix2 = pixC;
        pix3 = pixC;
        pix4 = pixC;
        if ((y == 0) || (x == 0)) pixS = pixC; else pixS = ScreenBuffer[x - 1][y - 1];
        if (y == 0) pixT = pixC; else pixT = ScreenBuffer[x][y - 1];
        if ((y == 0) || (x == 159)) pixU = pixC; else pixU = ScreenBuffer[x + 1][y - 1];
        if (x == 0) pixV = pixC; else pixV = ScreenBuffer[x - 1][y];
        if (x == 159) pixW = pixC; else pixW = ScreenBuffer[x + 1][y];
        if ((y == 143) || (x == 0)) pixX = pixC; else pixX = ScreenBuffer[x - 1][y + 1];
        if (y == 143) pixY = pixC; else pixY = ScreenBuffer[x][y + 1];
        if ((y == 143) || (x == 159)) pixZ = pixC; else pixZ = ScreenBuffer[x + 1][y + 1];

        if ((pixV == pixS) && (pixS == pixT)) pix1 = pixS;
        if ((pixT == pixU) && (pixU == pixW)) pix2 = pixU;
        if ((pixV == pixX) && (pixX == pixY)) pix3 = pixX;
        if ((pixW == pixZ) && (pixZ == pixY)) pix4 = pixZ;

        for (ScaleStepX = 0; ScaleStepX < EagleScaleFactor; ScaleStepX++) {
          for (ScaleStepY = 0; ScaleStepY < EagleScaleFactor; ScaleStepY++) {
            drv_putpixel((x * GraphicScaleFactor) + ScaleStepX, (y * GraphicScaleFactor) + ScaleStepY, pix1);
            drv_putpixel((x * GraphicScaleFactor) + ScaleStepX + EagleScaleFactor, (y * GraphicScaleFactor) + ScaleStepY, pix2);
            drv_putpixel((x * GraphicScaleFactor) + ScaleStepX, (y * GraphicScaleFactor) + ScaleStepY + EagleScaleFactor, pix3);
            drv_putpixel((x * GraphicScaleFactor) + ScaleStepX + EagleScaleFactor, (y * GraphicScaleFactor) + ScaleStepY + EagleScaleFactor, pix4);
          }
        }
      }
    }
    /* Draw whatever is in the buffer to the screen * /
    drv_refreshscreen();
    /* SDL_UpdateRect(screen, LcdModifiedFirstX * GraphicScaleFactor, LcdModifiedFirstY * GraphicScaleFactor, (LcdModifiedLastX + 1 - LcdModifiedFirstX) * GraphicScaleFactor, (LcdModifiedLastY + 1 - LcdModifiedFirstY) * GraphicScaleFactor); */
}


inline void RefreshScreen(int LcdModifiedFirstX, int LcdModifiedLastX, int LcdModifiedFirstY, int LcdModifiedLastY, struct zboyparamstype *zboyparams) {
  
  /* switch (zboyparams->scalingalgo) {
     case REFRESHSCREEN_BASICSCALE:
      RefreshScreen_BasicScale(LcdModifiedFirstX, LcdModifiedLastX, LcdModifiedFirstY, LcdModifiedLastY, zboyparams->GraphicScaleFactor);
      break;
    case REFRESHSCREEN_2X:
      RefreshScreen_Scale2x(LcdModifiedFirstX, LcdModifiedLastX, LcdModifiedFirstY, LcdModifiedLastY, zboyparams->GraphicScaleFactor);
      break;
    case REFRESHSCREEN_3X:
      RefreshScreen_Scale3x(LcdModifiedFirstX, LcdModifiedLastX, LcdModifiedFirstY, LcdModifiedLastY, zboyparams->GraphicScaleFactor);
      break;
    case REFRESHSCREEN_EAGLE:
      RefreshScreen_Eagle(LcdModifiedFirstX, LcdModifiedLastX, LcdModifiedFirstY, LcdModifiedLastY, zboyparams->GraphicScaleFactor);
      break;
    case REFRESHSCREEN_NOSCALE:
    default:
  */
  // RefreshScreen_NoScale(LcdModifiedFirstX, LcdModifiedLastX, LcdModifiedFirstY, LcdModifiedLastY);
  //      break;
  // }
}
