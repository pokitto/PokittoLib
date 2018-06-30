/*
    This file is part of the zBoy project
    Copyright (C) Mateusz Viste 2011,2012
*/

void tableQsort(int *myarray, int LastEl) {
    int qstack[200];
    int i, j, first = 0, last, stackptr = 0;
    int temp, swaptmp;

    /*first = Lbound(myarray) */
    last  = LastEl; /*ubound(myarray) */
    for (;;) {
        do {
            temp = myarray[(last + first) >> 1]; /* >> 1 is the same than \ 2, but faster */
            i = first;
            j = last;
            do {
                while (myarray[i] > temp) i++; /* reverse both < and > here to */
                while (myarray[j] < temp) j--; /* change the sorting order.    */
                if (i > j) break;
                if (i < j) {
                    swaptmp = myarray[i];
                    myarray[i] = myarray[j];
                    myarray[j] = swaptmp;
                }
                i++;
                j--;
            } while (i <= j);
            if (i < last) {
                qstack[stackptr] = i;
                qstack[stackptr + 1] = last;
                stackptr += 2;
            }
            last = j;
        } while (first < last);
        if (stackptr == 0) break;
        stackptr -= 2;
        first = qstack[stackptr];
        last = qstack[stackptr + 1];
    }
}
