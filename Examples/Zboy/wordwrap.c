/*
    Word-wrapping function
    Author: Mateusz Viste
    Last modified: 13 Aug 2011

   Note, that the function will modify the char array you will pass to it!
*/

void WordWrap(char *TextToWrap, int WrapValue, char *result) {
  int LastSpace = -1, x, y;

  for (x = 0; x <= WrapValue; x++) {
    if (TextToWrap[x] == 32) {
        LastSpace = x;
      } else if ((TextToWrap[x] == 0) || (TextToWrap[x] == 10)) {
        LastSpace = x;
        break;
    }
    result[x] = TextToWrap[x];
  }
  if (LastSpace < 0) {
      LastSpace = (x - 1);
      y = 0;
    } else {
      y = 1;
  }
  result[LastSpace] = 0;

  if (TextToWrap[LastSpace] == 0) {
      TextToWrap[0] = 0;
    } else {
      y += LastSpace;
      x = 0;
      while (TextToWrap[y] != 0) {
        TextToWrap[x] = TextToWrap[y];
        x++;
        y++;
      }
      TextToWrap[x] = 0;
  }

}
