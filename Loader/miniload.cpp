/**************************************************************************/
/*!
    @file     minibuttons.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Jonne Valola
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

#include "miniload.h"

#define APP_FILES       1
#define APP_SETTINGS    2
#define APP_ABOUT       3

#define TAB_REDRAW      0
#define TAB_WAIT        1
#define TAB_UPDATE      2
#define TAB_LEFT        3
#define TAB_RIGHT       4

#define NUMITEMS        256
#define LEFTCHARS       16
#define RIGHTCHARS      7
#define MAXVISIBLEITEMS 13

#define USEDIRS 0

#define SETTINGSTAB 1 // 1 if settings tab is enabled and visible

#define DEBUG_RESET_SETTINGS 0 // make 1 if you want to start with default settings

DigitalIn ABtn(POK_BTN_A_PIN);
DigitalIn BBtn(POK_BTN_B_PIN);
DigitalIn CBtn(POK_BTN_C_PIN);
DigitalIn UBtn(POK_BTN_UP_PIN);
DigitalIn DBtn(POK_BTN_DOWN_PIN);
DigitalIn LBtn(POK_BTN_LEFT_PIN);
DigitalIn RBtn(POK_BTN_RIGHT_PIN);



time_t oldseconds = 0;
struct tm oldtm;
bool olddateformat=0;

struct settings {
    char filename[20];
    uint8_t vol;
    uint8_t defaultvol;
    uint8_t loaderwait;
    uint8_t volwait;
    uint8_t timeformat;
    uint8_t lasthoursset;
    uint8_t lastminutesset;
    uint8_t dateformat;
    uint8_t lastdayset;
    uint8_t lastmonthset;
    uint8_t lastyearset;
    uint8_t rtcalarmmode;
    uint32_t reserved;
    uint32_t wakeuptime;
};

int appstate=APP_FILES, tabstate=TAB_REDRAW;
int selecteditem=0, listlength=0, beginitem=0, enditem=13;
char leftlist[NUMITEMS][LEFTCHARS];
char rightlist[NUMITEMS][RIGHTCHARS];
bool hirunner=false, nofiles=true;
int loadProg();
uint8_t data[256];
bool writeEEset=false;

settings EEset;

void swapItems(int a, int b)
{
    char templist[LEFTCHARS];
    for (int i=0; i < LEFTCHARS; i++) {
            templist[i] = leftlist[a][i];
            leftlist[a][i] = leftlist[b][i];
            leftlist[b][i] = templist[i];
    }
    for (int i=0; i < RIGHTCHARS; i++) {
            templist[i] = rightlist[a][i];
            rightlist[a][i] = rightlist[b][i];
            rightlist[b][i] = templist[i];
    }
}

void bubbleSortFiles(int n)
{
   int i, j, k;
   for (i = 0; i < n-1; i++) {
       for (j = 0; j < n-i-1; j++) {
           k = 0;
           while (k<8) {
                if (leftlist[j][k] > leftlist[j+1][k]) {
                    swapItems(j,j+1);
                } else if (leftlist[j][k] == leftlist[j+1][k]) k++; //go to next letter
                else k=8; //don't swap
            }
        }
    }
}


void readset() {
    uint8_t* p = (uint8_t*)&EEset;
    for (int i=0;i<40;i++) {
            *p++=eeprom_read_byte((uint16_t*)(i+EESETTINGS_FILENAME));
    }
}

void writeset() {
    uint8_t* p = (uint8_t*)&EEset;
    for (int i=0;i<40;i++) {
            eeprom_write_byte((uint16_t*)(i+EESETTINGS_FILENAME),*p++);
    }
  struct tm timeinfo;
  int year, month ,day;
  timeinfo.tm_year = EEset.lastyearset;
  timeinfo.tm_mon = EEset.lastmonthset;
  timeinfo.tm_mday = EEset.lastdayset;
  timeinfo.tm_hour = EEset.lasthoursset;
  timeinfo.tm_min = EEset.lastminutesset;
  timeinfo.tm_sec = 0;
  timeinfo.tm_isdst = 0;
  set_time(mktime (&timeinfo));
  writeEEset=false;
}

void defaultset() {
    //if EEPROM is empty, install default values
    EEset.loaderwait = 3;
    eeprom_write_byte((uint16_t*)EESETTINGS_LOADERWAIT,EEset.loaderwait);
    EEset.volwait = 10;
    eeprom_write_byte((uint16_t*)EESETTINGS_VOLWAIT,EEset.volwait);
    //reset default volume
    if (eeprom_read_byte((uint16_t*)EESETTINGS_VOL)==0) eeprom_write_byte((uint16_t*)EESETTINGS_VOL,127);
    //reset system clock
    set_time(1519862400);  // Set RTC time to 1/3/2018 00:00
    EEset.lasthoursset=EEset.lastminutesset=0;
    EEset.lastdayset=1;
    EEset.lastmonthset=3;
    EEset.lastyearset=118;
    eeprom_write_byte((uint16_t*)EESETTINGS_LASTDAYSET,EEset.lastdayset);
    eeprom_write_byte((uint16_t*)EESETTINGS_LASTMONTHSET,EEset.lastmonthset);
    eeprom_write_byte((uint16_t*)EESETTINGS_LASTYEARSET,EEset.lastyearset);
    eeprom_write_byte((uint16_t*)EESETTINGS_LASTHOURSSET,EEset.lasthoursset);
    eeprom_write_byte((uint16_t*)EESETTINGS_LASTMINUTESSET,EEset.lastminutesset);
}

void emptyLists() {
    listlength=0;
    for(int i=0;i<NUMITEMS;i++) {
            for(int j=0;j<LEFTCHARS;j++)  leftlist[i][j] = 0;
            for(int j=0;j<RIGHTCHARS;j++) rightlist[i][j] = 0;
    }
}

void splitItem(char* source, char* left, char* right) {
    int split=0;
    #if USEDIRS
    if ((source[0]=='.' && source[1]==NULL )|| (source[0]=='.' && source[1]=='.')) {
            *left++ = source[0];*left = source[1];
            return;
    }
    #endif
    for(int i=0;i<LEFTCHARS+RIGHTCHARS;i++) {
            if (source[i]=='.') split=1;
            if (source[i]== 0 ) return;
            else {
                    if (!split) *left++ = source[i];
                    else *right++ = source[i];
            }
    }
}

int readFilesToList(int start) {
    char* fn;
    emptyLists();
    #if USEDIRS
    fn = getFirstDirEntry();
    #else
    fn = getFirstFile("BIN");
    #endif
    if (!fn) {
            strcpy(&leftlist[0][0],"NO FILES");
            nofiles=true;
            return 0;
    }
    nofiles=false;
    if (start==0) {splitItem(fn,&leftlist[0][0],&rightlist[0][0]); start++;}
    for (int i=start; i<NUMITEMS;i++) {
            #if USEDIRS
            fn=getNextDirEntry();
            #else
            fn=getNextFile("BIN");
            #endif
            if (fn[0]==0) return i;
            splitItem(fn,&leftlist[i][0],&rightlist[i][0]);
    }
    return 0; //should not get here
}


void appendnumber(char* targetstring, uint16_t value, const char* units) {
    char stri[RIGHTCHARS];
    uint16_t temp=0, i=0;
    // thousands
    if (value>999) {
            temp = value/1000;
            stri[i++] = '0' + temp;
            value -= 1000*temp;
    }
    // hudreds
    if (value>99) {
            temp = value/100;
            stri[i++] = '0' + temp;
            value -= 100*temp;
    } else if (i) stri[i++] = '0'; // if i was set, thousands were, and add missing zero
    // tens
    if (value>9) {
            temp = value/10;
            stri[i++] = '0' + temp;
            value -= 10*temp;
    } else if (i) stri[i++] = '0'; // if i was set, thousands or hundreds were, and add missing zero
    // ones
    stri[i++] = '0' + value;
    strcpy(targetstring,&stri[0]);
    strcpy(targetstring+i,units);
}

int readSettingsToList(int start) {
    if (writeEEset) writeset();
    readset();
    time_t seconds = time(NULL);
    struct tm *tmp = gmtime(&seconds);
    EEset.lasthoursset=tmp->tm_hour;
    EEset.lastminutesset=tmp->tm_min;
    EEset.lastyearset=tmp->tm_year;
    EEset.lastmonthset=tmp->tm_mon;
    EEset.lastdayset=tmp->tm_mday;
    EEset.dateformat=eeprom_read_byte((uint16_t*)EESETTINGS_DATEFORMAT);
    if (EEset.loaderwait==0) defaultset();
    char n; char stri[RIGHTCHARS];
    emptyLists();
    //loader wait time
    strcpy(&leftlist[0][0],"LOADER WAIT:");
    appendnumber(&rightlist[0][0],EEset.loaderwait," SEC");
    //vol selection wait time
    strcpy(&leftlist[1][0],"VOLUME WAIT:");
    appendnumber(&rightlist[1][0],EEset.volwait," SEC");
    //time format
    strcpy(&leftlist[2][0],"TIME FORMAT:");
    if (EEset.timeformat==0) strcpy(&rightlist[2][0],"24 HRS");
    else strcpy(&rightlist[2][0],"12 HRS");
    //Current time
    strcpy(&leftlist[3][0],"SET HOURS:");
    if (EEset.timeformat==0) appendnumber(&rightlist[3][0],EEset.lasthoursset," H");
    else if (EEset.lasthoursset>12) appendnumber(&rightlist[3][0],EEset.lasthoursset%12," PM");
    else appendnumber(&rightlist[3][0],EEset.lasthoursset%12," AM");
    strcpy(&leftlist[4][0],"SET MINUTES:");
    appendnumber(&rightlist[4][0],EEset.lastminutesset," MIN");
    // dateformat
    strcpy(&leftlist[5][0],"DATE FORMAT:");
    if (EEset.dateformat==0) strcpy(&rightlist[5][0],"D/M/Y");
    else strcpy(&rightlist[5][0],"M/D/Y");
    // date
    strcpy(&leftlist[6][0],"SET DAY:");
    appendnumber(&rightlist[6][0],EEset.lastdayset," ");
    strcpy(&leftlist[7][0],"SET MONTH:");
    appendnumber(&rightlist[7][0],EEset.lastmonthset," ");
    strcpy(&leftlist[8][0],"SET YEAR:");
    appendnumber(&rightlist[8][0],EEset.lastyearset+1900," ");
    return 9;
}


void printList(int x, int y, int gap, unsigned int col, unsigned int selcol) {
    char* item;
    char* item2;
    int index=beginitem;
    item = &leftlist[beginitem][0];
    item2 = &rightlist[beginitem][0];
    while (item[0]) {
        set_cursor(x,y);
        if (index==selecteditem+beginitem) pok_directcolor=selcol;
        else pok_directcolor=col;
        print(item);
        set_cursor(x+gap*font[0],y);
        print(item2);//println(item2);
        item = &leftlist[++index][0];
        item2 = &rightlist[index][0];
        y += font[1];
        if (index==beginitem+MAXVISIBLEITEMS || index == listlength) break;
    }
}

void wipeList(int x, int y) {
    for (int index=0; index<MAXVISIBLEITEMS;index++) {
        set_cursor(x, y+index*font[1]);
        pok_directcolor=DC_BLACK;
        print("hhhhhhhhhhhhhhhhh");//println("hhhhhhhhhhhhhhhhh");
    }
}

void drawdate(uint8_t x, uint8_t y) {
    time_t seconds = time(NULL);
    struct tm* tmp = gmtime(&seconds);
    bool dateformat = eeprom_read_byte((uint16_t*)EESETTINGS_DATEFORMAT);
    set_cursor(x,y);
    if (oldtm.tm_mday==tmp->tm_mday && oldtm.tm_mon == tmp->tm_mon && oldtm.tm_year == tmp->tm_year && olddateformat==dateformat) return;
    uint16_t c = pok_directcolor;
    pok_directcolor=0;
    print("hhhhhhhhhhhh"); // wipe the old date
    pok_directcolor=c;
    set_cursor(x,y);
    oldtm.tm_mday = tmp->tm_mday;
    oldtm.tm_mon = tmp->tm_mon;
    oldtm.tm_year = tmp->tm_year;
    olddateformat = dateformat;
    if (dateformat==0) {
        print(tmp->tm_mday);print("/");
        print(tmp->tm_mon);print("/");
        print(tmp->tm_year+1900);
    } else {
        print(tmp->tm_mon);print("/");
        print(tmp->tm_mday);print("/");
        print(tmp->tm_year+1900);
    }
}

void drawclock(uint8_t x, uint8_t y) {
    time_t seconds = time(NULL);
    if (oldseconds==seconds) return;
    oldseconds=seconds;
    struct tm *tmp = gmtime(&seconds);
    uint16_t c = pok_directcolor;
    if (eeprom_read_byte((uint16_t*)EESETTINGS_TIMEFORMAT)) set_cursor(x-3*8,y);
    else set_cursor(x,y);
    pok_directcolor=0;
    if (tmp->tm_sec==0) print("hhhhhhhh");
    else                print("    hhhh");
    if (eeprom_read_byte((uint16_t*)EESETTINGS_TIMEFORMAT)) set_cursor(x-3*8,y);
    else set_cursor(x,y);

    pok_directcolor=c;
    if (eeprom_read_byte((uint16_t*)EESETTINGS_TIMEFORMAT)) {
        print(tmp->tm_hour%12);
    } else {
        print(tmp->tm_hour);
    }
    print(":");print(tmp->tm_min);print(":");
    print(tmp->tm_sec);
    if (eeprom_read_byte((uint16_t*)EESETTINGS_TIMEFORMAT)) {
        if (tmp->tm_hour>12) print(" PM");
        else print(" AM");
    }
}

void drawTabs(unsigned int a, unsigned int b, unsigned int c) {
    olddateformat = 1 - eeprom_read_byte((uint16_t*)EESETTINGS_DATEFORMAT); //force refresh of date
    pok_directcolor=a;
    set_cursor(0,0);
    print("abbbbc"); // file tab
    pok_directcolor=b;
    #if SETTINGSTAB
    print("abbbbbbbbc    "); //settings tab
    #else
    print("              "); //settings tab
    #endif
    pok_directcolor=c;
    print("abbbbbc\n"); // about tab //println("abbbbbc"); // about tab
    pok_directcolor=a;
    print("|FILE|");
    pok_directcolor=b;
    #if SETTINGSTAB
    print("|SETTINGS|    ");
    #else
    print("              ");
    #endif
    pok_directcolor=c;
    print("|ABOUT|\n");
}

void drawTabWindow(unsigned int a) {
    pok_directcolor=a;
    set_cursor(0,3*8);
    for (int i=0;i<5;i++) print("|                         |\n");
    print("f                         f\n");
    print("\n\n\n\n");
    print("f                         f\n");
    print("f                         f\n");
    for (int i=0;i<5;i++) print("|                         |\n");
    print("dbbbbbbbbbbbbbbbbbbbbbbbbbe\n");

}

void drawPopupFrame(unsigned int col) {
    pok_directcolor=DC_BLACK;
    for (int y=0;y<176;y+=8) print("ggggggggggggggggggggggggggg\n");
    for (int y=4*8;y<19*8;y+=8) {
    set_cursor(5*8,y);
    print("hhhhhhhhhhhhhhhhhh\n");
    }
    pok_directcolor=col;
    set_cursor(5*8,4*8);
    print("abbbbbbbbbbbbbbbbc\n");
    for (int y=5*8;y<18*8;y+=8) {
        set_cursor(5*8,y);
        print("|                |\n");
    }
    set_cursor(5*8,18*8);
    print("dbbbbbbbbbbbbbbbbe\n");
}

void errorPopup(char* text) {
    drawPopupFrame(DC_RED);
    pok_directcolor=DC_RED;
    set_cursor(12*8,6*8);
    print("ijkl");
    set_cursor(12*8,7*8);
    print("mnop");
    set_cursor(12*8,8*8);
    print("qrst");
    set_cursor(12*8,9*8);
    print("uvwx");
    set_cursor(7*8,12*8);
    print(text);
    emptyLists();
    strcpy(&leftlist[0][0],"TRY AGAIN");
    int prevselected=selecteditem;
    selecteditem=0;beginitem=0;
    while (1) {
        pokPollButtons();
        printList(10*8,15*8,10, DC_GRAY,DC_WHITE);
        if (aBtn()) {
                while (aBtn()) pokPollButtons();
                tabstate=TAB_REDRAW;selecteditem=prevselected;return;
        }
    }
}


void loaderPopup() {
    //m_lcdClear();
    char filename[20];
    for (int n=0; n<20;n++) filename[n]=0;
    int i=0;
    while (leftlist[selecteditem+beginitem][i]) {
            filename[i]=leftlist[selecteditem+beginitem][i];
            i++;
    }
    filename[i]='.';filename[i+1]='B';filename[i+2]='I';filename[i+3]='N';
    drawPopupFrame(DC_GREEN);
    pok_directcolor=DC_GREEN;
    set_cursor(12*8,6*8);
    print("ijkl");
    set_cursor(12*8,7*8);
    print("mnop");
    set_cursor(12*8,8*8);
    print("qrst");
    set_cursor(12*8,9*8);
    print("uvwx");
    set_cursor(7*8,12*8);
    print("LOAD PROGRAM?");
    emptyLists();
    strcpy(&leftlist[0][0],"YES");
    strcpy(&leftlist[1][0],"NO");
    int prevselected=selecteditem;
    selecteditem=0;beginitem=0;
    while (1) {
        pokPollButtons();
        printList(12*8,14*8,10,DC_GRAY,DC_WHITE);
        if (upBtn()) {while (upBtn()) {pokPollButtons();} selecteditem=0;}
        if (downBtn()) {while (downBtn()) {pokPollButtons();} selecteditem=1;}
        if (aBtn()) {
                while (aBtn()) {pokPollButtons();}
                if (selecteditem==1) {tabstate=TAB_REDRAW;selecteditem=prevselected;return;}
                else break;
        }
    }
    writeEEPROM((uint16_t*)EESETTINGS_FILENAME,(unsigned char*)filename,20); //save the filename
    loadProg();

}

void tabFiles() {
    listlength = readFilesToList(0);
    #ifdef BUBBLESORT
    bubbleSortFiles(listlength);
    #endif
    //#define STARTACCEL 20
    #define REPEATCUT 30000/50
    #define SINGLECUT 2000/200
    uint16_t up = 0, down = 0;
    while (1) {
    pokPollButtons();
    switch (tabstate) {
    case TAB_REDRAW:
        m_lcdClear();
        drawTabs(DC_GREEN, DC_DCYAN, DC_DMAGENTA);
        pok_directcolor=DC_GREEN;
        print("|    dbbbbbbbbbbbbbbbbbbbbc\n"); //top of window
        drawTabWindow(DC_GREEN);
        tabstate=TAB_UPDATE;
        break;
    case TAB_UPDATE:
        printList(3*8,6*8, 10, DC_GRAY, DC_WHITE);
        tabstate=TAB_WAIT;
        break;
    case TAB_WAIT:
        if (aBtn()&&nofiles==false) {
                while (aBtn()) {pokPollButtons();}
                loaderPopup();
                listlength = readFilesToList(0);
                tabstate = TAB_REDRAW; //if loaderPopup returns,redraw UI
        }
        if (upBtn()) up++;
        if (up>SINGLECUT) { // threshold for single press passed
            if (!upBtn()||up>REPEATCUT) { //if button has been released or threshold for longpress passed
            if (!upBtn()) up=0; //reset counter for longpress
            selecteditem--;
            if (selecteditem<0) {
                    if (beginitem==0) selecteditem=0;
                    else {
                        //screoll up
                        wipeList(3*8,6*8);
                        beginitem -= MAXVISIBLEITEMS;
                        selecteditem = MAXVISIBLEITEMS-1;
                    }
            }
            tabstate = TAB_UPDATE;
            }
        }
        if (downBtn())
        {
            down++;
        }
        if (down>SINGLECUT){
            if (!downBtn()||down>REPEATCUT) {
            if (!downBtn()) down=0;
            if (selecteditem+beginitem<listlength-1) selecteditem++;
            if (selecteditem>=MAXVISIBLEITEMS) {
                //scroll down
                wipeList(3*8,6*8);
                selecteditem = 0;
                beginitem += MAXVISIBLEITEMS;
            }
            tabstate = TAB_UPDATE;
            }
        }
        if (leftBtn()) { while(leftBtn()) pokPollButtons(); selecteditem=beginitem=0; tabstate = TAB_LEFT;}
        if (rightBtn()) { while(rightBtn()) pokPollButtons();  selecteditem=beginitem=0; tabstate = TAB_RIGHT;}
        break;
    case TAB_LEFT:
        appstate = APP_ABOUT; selecteditem=0; return;
    case TAB_RIGHT:
        #if SETTINGSTAB
        appstate = APP_SETTINGS; selecteditem=0; return;
        #else
        appstate = APP_ABOUT; selecteditem=0; return;
        #endif
    }
    pok_directcolor = DC_GREEN;
    drawclock(18*8,19*8);
    drawdate(1*8,19*8);
    }
}

void tabSettings() {
    #define REPEATCUT 30000/50
    #define SINGLECUT 1000/200
    uint16_t up = 0, down = 0;
    while (1) {
    pokPollButtons();
    switch (tabstate) {
    case TAB_REDRAW:
        m_lcdClear();
        drawTabs(DC_DGREEN, DC_CYAN, DC_DMAGENTA);
        pok_directcolor=DC_CYAN;
        //println("|    dbbbbbbbbbbbbbbbbbbbbc"); //top of window
        print("abbbbbe        dbbbbbbbbbbc\n");
        drawTabWindow(DC_CYAN);
        tabstate=TAB_UPDATE;
        break;
    case TAB_UPDATE:
        listlength = readSettingsToList(0);
        printList(3*8,6*8, 13, DC_GRAY, DC_WHITE);
        tabstate=TAB_WAIT;
        break;
    case TAB_WAIT:
        if (aBtn()) {
                while (aBtn()) {pokPollButtons();}
                switch (selecteditem) {
            case 0:
                EEset.loaderwait++; if (EEset.loaderwait>5) EEset.loaderwait=5;
                eeprom_write_byte((uint16_t*)EESETTINGS_LOADERWAIT,EEset.loaderwait);break;
            case 1:
                EEset.volwait++; if (EEset.volwait>10) EEset.volwait=10;
                eeprom_write_byte((uint16_t*)EESETTINGS_VOLWAIT,EEset.volwait);break;
            case 2:
                EEset.timeformat = (1-EEset.timeformat)&1;
                eeprom_write_byte((uint16_t*)EESETTINGS_TIMEFORMAT,EEset.timeformat);break;
            case 3:
                EEset.lasthoursset++;
                if (EEset.timeformat==0) EEset.lasthoursset%=24; else EEset.lasthoursset%=12;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTHOURSSET,EEset.lasthoursset);break;
            case 4:
                EEset.lastminutesset = (EEset.lastminutesset+1)%60;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTMINUTESSET,EEset.lastminutesset);break;
            case 5:
                EEset.dateformat = (1-EEset.dateformat)&1;
                eeprom_write_byte((uint16_t*)EESETTINGS_DATEFORMAT,EEset.dateformat);break;
            case 6:
                EEset.lastdayset = (EEset.lastdayset+1)%32;
                if (EEset.lastdayset==0) EEset.lastdayset=1;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTDAYSET,EEset.lastdayset);break;
            case 7:
                EEset.lastmonthset = (EEset.lastmonthset+1)%13;
                if (EEset.lastmonthset==0) EEset.lastmonthset=1;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTMONTHSET,EEset.lastmonthset);break;
            case 8:
                EEset.lastyearset = (EEset.lastyearset+1);
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTYEARSET,EEset.lastyearset);break;
                }
        writeEEset=true;
        tabstate = TAB_REDRAW;
        }
        if (bBtn()) {
                while (bBtn()) {pokPollButtons();}
                switch (selecteditem) {
            case 0:
                EEset.loaderwait--; if (EEset.loaderwait<1) EEset.loaderwait=1;
                eeprom_write_byte((uint16_t*)EESETTINGS_LOADERWAIT,EEset.loaderwait);break;
            case 1:
                if (EEset.volwait>=0) EEset.volwait--;
                eeprom_write_byte((uint16_t*)EESETTINGS_VOLWAIT,EEset.volwait);break;
            case 2:
                EEset.timeformat = (1-EEset.timeformat)&1;
                eeprom_write_byte((uint16_t*)EESETTINGS_TIMEFORMAT,EEset.timeformat);break;
            case 3:
                EEset.lasthoursset--;
                if (EEset.timeformat) EEset.lasthoursset%=24; else EEset.lasthoursset%=12;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTHOURSSET,EEset.lasthoursset);break;
            case 4:
                EEset.lastminutesset = EEset.lastminutesset-1;
                if (EEset.lastminutesset >= 60) EEset.lastminutesset=59;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTMINUTESSET,EEset.lastminutesset);break;
            case 5:
                EEset.dateformat = (1-EEset.dateformat)&1;
                eeprom_write_byte((uint16_t*)EESETTINGS_DATEFORMAT,EEset.dateformat);break;
            case 6:
                EEset.lastdayset = EEset.lastdayset-1;
                if (EEset.lastdayset>=32) EEset.lastdayset=31;
                if (EEset.lastdayset==0) EEset.lastdayset=31;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTDAYSET,EEset.lastdayset);break;
            case 7:
                EEset.lastmonthset = EEset.lastmonthset-1;
                if (EEset.lastmonthset==0) EEset.lastmonthset=12;
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTMONTHSET,EEset.lastmonthset);break;
            case 8:
                EEset.lastyearset = (EEset.lastyearset-1);
                eeprom_write_byte((uint16_t*)EESETTINGS_LASTYEARSET,EEset.lastyearset);break;
                }
        writeEEset=true;
        tabstate = TAB_REDRAW;
        }
        if (upBtn()) up++;
        if (up>SINGLECUT) { // threshold for single press passed
            if (!upBtn()||up>REPEATCUT) { //if button has been released or threshold for longpress passed
            if (!upBtn()) up=0; //reset counter for longpress
            selecteditem--;
            if (selecteditem<0) {
                    if (beginitem==0) selecteditem=0;
                    else {
                        //screoll up
                        wipeList(3*8,6*8);
                        beginitem -= MAXVISIBLEITEMS;
                        selecteditem = MAXVISIBLEITEMS-1;
                    }
            }
            tabstate = TAB_UPDATE;
            }
        }
        if (downBtn()) {
                down++;
        }
        if (down>SINGLECUT){
            if (!downBtn()||down>REPEATCUT) {
            if (!downBtn()) down=0;
            if (selecteditem+beginitem<listlength-1) selecteditem++;
            if (selecteditem>=MAXVISIBLEITEMS) {
                //scroll down
                wipeList(3*8,6*8);
                selecteditem = 0;
                beginitem += MAXVISIBLEITEMS;
            }
            tabstate = TAB_UPDATE;
            }
        }
        if (leftBtn()) { while(leftBtn()) pokPollButtons();  tabstate = TAB_LEFT;}
        if (rightBtn()) { while(rightBtn()) pokPollButtons();  tabstate = TAB_RIGHT;}
        break;
    case TAB_LEFT:
        appstate = APP_FILES; selecteditem=0; return;
    case TAB_RIGHT:
        appstate = APP_ABOUT; selecteditem=0; return;
    }
    pok_directcolor = DC_CYAN;
    drawclock(18*8,19*8);
    drawdate(1*8,19*8);
    }
}

void tabAbout() {
    uint32_t* bootinfo;
    uint32_t bootver;
    bootinfo = (uint32_t*)0x3FFF4;
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3FF04; //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3FE04; //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E) bootinfo = (uint32_t*)0x3F004; //for futureproofing
    bootver = *(bootinfo+1);
    while (1) {
    pokPollButtons();
    switch (tabstate) {
    case TAB_REDRAW:
        m_lcdClear();
        drawTabs(DC_DGREEN, DC_DCYAN, DC_MAGENTA);
        pok_directcolor=DC_MAGENTA;
        //println("|    dbbbbbbbbbbbbbbbbbbbbc"); //top of window
        //println("abbbbbebbbbbbbbdbbbbbbbbbbc");
        print("abbbbbbbbbbbbbbbbbbbe     |\n");
        drawTabWindow(DC_MAGENTA);
        tabstate=TAB_UPDATE;
        break;
    case TAB_UPDATE:
        set_cursor(3*8,6*8);
        pok_directcolor=DC_MAGENTA;
        print("LOADER V.");print(bootver);
        set_cursor(3*8,8*8);
        pok_directcolor=DC_GRAY;
        print("COPYRIGHT 2018");
        set_cursor(3*8,9*8);
        print("POKITTO OY FINLAND");
        tabstate=TAB_WAIT;
        break;
    case TAB_WAIT:
        if (leftBtn()) { while(leftBtn()) pokPollButtons(); tabstate = TAB_LEFT;}
        if (rightBtn()) { while(rightBtn()) pokPollButtons(); tabstate = TAB_RIGHT;}
        break;
    case TAB_LEFT:
        #if SETTINGSTAB
        appstate = APP_SETTINGS; return;
        #else
        appstate = APP_FILES; return;
        #endif
    case TAB_RIGHT:
        appstate = APP_FILES; return;
    }
    pok_directcolor = DC_MAGENTA;
    drawclock(18*8,19*8);
    drawdate(1*8,19*8);
    }
}


int loadProg () {
    uint32_t counter=0;
    char filename[20];
    char c=1;
    int namelen=0;
    for (int i=0; i<20; i++) {
            filename[i]=0;
    }
    for (int i=0; i<20; i++) {
            readEEPROM((uint16_t*)(i+EESETTINGS_FILENAME),(uint8_t*)&c,1);
            if (c) {filename[i]=c; namelen=i;}
            else break;
    }
    pok_directcolor=DC_GREEN;
    m_lcdClear();
    set_cursor(12*8,6*8);
    print("ijkl");
    set_cursor(12*8,7*8);
    print("mnop");
    set_cursor(12*8,8*8);
    print("qrst");
    set_cursor(12*8,9*8);
    print("uvwx");
    set_cursor(110-(8+namelen)*4,12*8);
    print("LOADING ");
    print(filename);
    set_cursor(110-(10)*4,14*8);
    pok_directcolor=DC_WHITE;
    print("PLEASE WAIT");
    /** check SD card initialization **/
    if (pokInitSD()) {
        errorPopup("SD CARD INIT FAIL");
    }
    /** open the file **/
    if (fileOpen(filename, FILE_MODE_BINARY)) {
        errorPopup("FILE OPEN FAIL");
    }
    /** prepare the flash writing **/
    int progress=0;
    int opg=-1;
    uint32_t fsize=0;
    fileEnd();
    fsize = fileGetPosition();
    //if (fsize>LOADER_ADDRESS) fsize = LOADER_ADDRESS;
    fileRewind();

    while (1) {
        set_cursor(110-10*8,17*8);
        pok_directcolor=DC_GREEN;
        if (opg != (int)counter) {
            for (int i=0;i<20;i++) {
                if (i>=progress) {
                    pok_directcolor=DC_GREEN;
                    print("g");
                } else {
                    pok_directcolor=DC_WHITE;
                    print("h");
                }
                }
        }
        if (counter >= fsize) break;
        opg=progress;
        if (fileReadBytes(&data[0],0x100)<0x100) {
                if (fsize-counter>0x100) errorPopup("ERROR READING FILE");
        };
        if (counter<LOADER_ADDRESS || counter >= 0x3F000){ //write also bytes ABOVE the bootloader, again because of mbed
            if (CopyPageToFlash(counter,data)) {
                    errorPopup("FLASH WRITE ERROR");
            } else counter += 0x100;
        } else counter += 0x100;
        progress = (counter*20)/fsize;
        }
    set_cursor(110-4*8,19*8);
    print("SUCCESS !!");
    SCB->AIRCR = 0x05FA0004; //issue system reset
    return 1;
}




int main () {
    /** reprogram reset vectors **/
    uint32_t* flash;
    uint32_t* ram;
    pokInitButtons();
    ram   = (uint32_t*)0x10000004; // reset vector in ram
    __enable_irq();
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; //enable systick
    m_initLCD();
    readset();
    time_t seconds = time(NULL);
    struct tm *tmp = gmtime(&seconds);
    EEset.lasthoursset=tmp->tm_hour;
    EEset.lastminutesset=tmp->tm_min;
    EEset.lastyearset=tmp->tm_year;
    EEset.lastmonthset=tmp->tm_mon;
    EEset.lastdayset=tmp->tm_mday;
    EEset.dateformat=eeprom_read_byte((uint16_t*)EESETTINGS_DATEFORMAT);
    if (EEset.loaderwait==0) defaultset();

    font = fntC64;
    set_cursor(0,0);

    print("READING SD CARD... ");
    if (pokInitSD()) {
        pok_directcolor=DC_RED;
        print("FAIL\n\n");
        pok_directcolor=DC_WHITE;
        print("PRESS A TO CONTINUE\n");
        while (!aBtn()) pokPollButtons();
    } else {
        pok_directcolor=DC_GREEN;
        print("WAIT!\n");
    }
    pok_directcolor=DC_WHITE;
    #if DEBUG_RESET_SETTINGS
    defaultset();
    #endif
    while (1) {
        tabstate=TAB_REDRAW;
        switch (appstate) {
        case APP_FILES:
                tabFiles();
                break;
        case APP_SETTINGS:
                tabSettings();
                break;
        case APP_ABOUT:
                tabAbout();
                break;
        default:
                break;
        }

    }
    return 1;
}

