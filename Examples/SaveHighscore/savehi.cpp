#include "Pokitto.h"
#include "PokittoCookie.h"

Pokitto::Core mygame;


/* Random stuff to store in the EEPROM */

const char playerrank[3][10] = {
    "Rookie",
    "Medium",
    "Expert"
};

const char* blabla = "This is extra long text that is needed to waste fill cookie memory";


/* Three classes that extend the base Cookie class */

class mycookie : public Pokitto::Cookie {
public:
    char initials[4] = {'A','D','A',0};
    int score=10;
    char rank[10];
    long playtime = 1234567890;
    char message[100];
};

class myothercookie : public Pokitto::Cookie {
public:
    char fun[9] = {'Y','o','g','i','B','e','a','r'};
};

class mythirdcookie : public Pokitto::Cookie {
public:
    int a=1;
    int b=2;
    int c=3;
    int d=4;
    int e=5;
};

/* create instances */
mycookie highscore;
myothercookie yogibear;
mythirdcookie numbercookie;

int main () {
    /* game state variables, only for the show */
    int state=0; //game display state
    int currentblock = 0;
    int currentblockowner = -1;
    char temp; //

    /* wipe the keytable so we start from fresh. You can comment this out */
    highscore.formatKeytable();

    mygame.begin(); //start game

    /* put 'numbers' cookie first into memory */
    numbercookie.begin("NUMBERS", numbercookie);
    numbercookie.saveCookie();

    /* populate and save the 'demo' cookie */
    if (!highscore.rank[0]) strcpy(highscore.rank,playerrank[0]);
    if (highscore.playtime==0) highscore.playtime=1234567890;
    highscore.begin("HISCTEST", highscore); //initialize cookie
    highscore.loadCookie();
    strcpy(highscore.message,blabla);

    /* and to the last slot in order, put the YogiBear cookie */
    yogibear.begin("YOGISAVE", yogibear); //init other cookie
    yogibear.saveCookie(); //store it in eeprom

    while (mygame.isRunning()) {

        if (mygame.update()) {
            switch (state) {
        case 0:
            mygame.display.setFont(font5x7);
            mygame.display.color=1;
            mygame.display.println("Highscore Cookie test\n");
            mygame.display.color=2;
            mygame.display.println("Random \"data\":\n");
            mygame.display.color=3;
            mygame.display.setFont(fontZXSpec);
            mygame.display.fontSize=2;
            mygame.display.print((const char*)highscore.initials);
            mygame.display.print(" ");
            mygame.display.println((int)highscore.score);
            mygame.display.println("\n");
            mygame.display.fontSize=1;
            mygame.display.print("Rank: ");
            mygame.display.println((const char*)highscore.rank);
            mygame.display.print("Playtime: ");
            mygame.display.println(highscore.playtime);
            mygame.display.print("Blabla: ");
            mygame.display.println(highscore.message);
            mygame.display.setFont(font5x7);
            mygame.display.color=1;
            mygame.display.println("\n\nPress A to generate random highscore\nand store it ");
            mygame.display.println("\nPress C for EEPROM view");
            if (mygame.buttons.pressed(BTN_A)) {
                //generate a random, but higher new highscore
                highscore.initials[0] = random(65,90);
                highscore.initials[1] = random(65,90);
                highscore.initials[2] = random(65,90);
                highscore.score += random(10);
                strcpy(highscore.rank,playerrank[random(0,3)]);
                highscore.playtime = (long)random(0,0xFFFF)*(long)random(0,0xFFFF);
                // save new highscore
                highscore.saveCookie();
            }
            break;
        case 1:
            if (eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+currentblock))&0x80) {
            currentblockowner = eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+currentblock))&0x7F;
            } else currentblockowner = -1;
            mygame.display.color=1;
            mygame.display.setFont(font5x7);
            mygame.display.println("EEPROM Cookie Keytable:\n");
            for (int i=0, k=0, r=0;i<12;i++) {
                    mygame.display.color=2;
                    if (i<10) mygame.display.print(" ");
                    mygame.display.print(i);
                    mygame.display.print(":");
                    if (currentblockowner==i) mygame.display.color=1;
                    else mygame.display.color=3;
                    for (int j=0; j<8; j++, k++) {
                            temp = eeprom_read_byte((uint16_t*)k);
                            if (temp>31) mygame.display.print((const char)temp);
                            else mygame.display.print("-");
                    }
                    r++;
                    if (r==3) {mygame.display.print("\n");r=0;}
                    else mygame.display.print(" ");
            }
            mygame.display.color=1;
            mygame.display.println("\nEEPROM Block Reservation Table:\n");
            for (int i=0, k=0, r=0;i<21;i++,k++) {
                    mygame.display.color=2;
                    if (i<10) mygame.display.print(" ");
                    if (k==currentblock) mygame.display.color=1;
                    mygame.display.print(k);
                    mygame.display.color=2;
                    mygame.display.print(":");
                    temp = eeprom_read_byte((uint16_t*)(k+SBMAXKEYS*SBKEYSIZE));
                    if (temp&0x80) mygame.display.color=3;
                    else mygame.display.color=1;
                    if (temp&0x80) mygame.display.print((int)(temp&0x7F),16);
                    else mygame.display.print("-");
                    if ((temp&0x7F)<16) mygame.display.print(" ");
                    r++;
                    if (r==7) {mygame.display.print("\n");r=0;}
                    //else mygame.display.print(" ");
            }
            mygame.display.color=1;
            mygame.display.print("\nEEPROM Block ");
            mygame.display.color=2;
            mygame.display.print(currentblock);
            mygame.display.color=3;
            mygame.display.print(' ');
            //print owner of block
            if (eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+currentblock))&0x80) {
            currentblockowner = eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+currentblock))&0x7F;
            for (int i = 0;i<SBKEYSIZE;i++) {
                    mygame.display.print((const char)eeprom_read_byte((uint16_t*)(SBKEYSIZE*currentblockowner+i)));
                    }
            } else mygame.display.print("free");
            mygame.display.println('\n');
            mygame.display.color=1;
            for (int i=0, k=0, r=0;i<SBBLOCKSIZE;i++,k++) {
                    temp = eeprom_read_byte((uint16_t*)(k+SBMAXKEYS*SBKEYSIZE+SBMAXBLOCKS+SBBLOCKSIZE*currentblock));
                    mygame.display.color=3;
                    if (temp>31 && temp <127) {
                        // printable character
                        mygame.display.color=1;
                        mygame.display.print((char)(temp));
                    } else if (temp) {
                        // numeric value
                        mygame.display.color=2;
                        mygame.display.print((int)(temp));
                    } else {
                        //zero
                        mygame.display.print((int)(temp));
                    }
                    r++;
                    if (r==16) {mygame.display.print("\n");r=0;}
                    else mygame.display.print(" ");
            }
            if (mygame.buttons.pressed(BTN_LEFT)) currentblock--;
            if (mygame.buttons.pressed(BTN_RIGHT)) currentblock++;
            if (mygame.buttons.pressed(BTN_UP)) currentblock-=7;
            if (mygame.buttons.pressed(BTN_DOWN)) currentblock+=7;
            if (currentblock<0) currentblock += 21;
            if (currentblock>20) currentblock -= 21;
            if (mygame.buttons.pressed(BTN_B)) {
                    highscore.deleteCookie();
            }
            mygame.display.color=1;
            mygame.display.println("\nPress C to go back to highscore");
            if (currentblockowner != -1) mygame.display.print("Press B to erase the demo cookie");
            break;
            } //game state
            if (mygame.buttons.pressed(BTN_C)) state = 1-state;

        } //update

        } //isrunning

} // main
