#include "Pokitto.h"
#include "textures.h"

Pokitto::Core game;

void scorepoint(bool voidreset);
void endgame(bool voidreset2);

char status = 0;        //0=main-menu, 1=game, 2=settings, 3=about
char cursor = 0;
bool showdifficulty = 0;
bool showballmovement = 0;
char ballcolor = 0;         //0=default(changing), 1=orange, 2= yellow, 3= blue, 4=gray
char difficultysetting = 60;

float padplayer = 34;
float padcom = 34;
float ballx = 52;
float bally = 41;
float movex = 0;
float movey = 0;
short scoreplayer = 0;
short scorecom = 0;
char screenW = 110;
char screenH = 88;
char movecom = 0;
char moveplayer = 0;
short difficulty = 60;
bool gamestart = 0;
bool win = 0;

int main ()
{
    game.display.width = screenW;
    game.display.height = screenH;
    game.display.load565Palette(default36);
    game.setFrameRate(60);

    game.begin();
    game.display.setFont(font5x7);

    while (game.isRunning())
    {
        if (game.update())
        {
            /*
            =====================MAINMENU======================
            */
            if (status == 0)
            {
                //______________LOGIC/CONTROL_______________
                if (cursor == 0)
                {
                    cursor = 30;
                }
                if (game.buttons.pressed(BTN_DOWN) && (cursor < 50))
                {
                    cursor += 10;
                }
                if (game.buttons.pressed(BTN_UP) && (cursor > 30))
                {
                    cursor -= 10;
                }
                if (game.buttons.released(BTN_A))
                {
                    status = ((cursor / 10) - 2);
                    cursor = 0;
                    endgame(1);
                }
                //_________________DISPLAY__________________
                game.display.drawBitmap(0,0,title);
                game.display.setCursor(20,30);
                game.display.print("Play");
                game.display.setCursor(20,40);
                game.display.print("Settings");
                game.display.setCursor(20,50);
                game.display.print("About");
                game.display.setCursor(90,80);
                game.display.print("1.0");

                game.display.setCursor(10,cursor);
                game.display.print(">");
            }
            /*
            =====================SETTINGS==========================
            */
            if (status == 2)
            {
                //__________________LOGIC/CONTROL____________________
                if (cursor == 0)
                {
                    cursor = 20;
                }
                if (game.buttons.pressed(BTN_DOWN) && (cursor < 60))
                {
                    if (cursor == 20) cursor += 20;
                    else cursor += 10;
                }
                if (game.buttons.pressed(BTN_UP) && (cursor > 20))
                {
                    if (cursor == 40) cursor -= 20;
                    else cursor -= 10;
                }
                if (game.buttons.pressed(BTN_B))
                {
                    status = 0;
                    cursor = 0;
                }
                if (game.buttons.pressed(BTN_LEFT))
                {
                    if (cursor == 20 && ballcolor > 0) ballcolor -= 1;
                    if (cursor == 40) showballmovement = 0;
                    if (cursor == 50) showdifficulty = 0;
                    if (cursor == 60 && difficultysetting < 80) difficultysetting += 10;
                }
                if (game.buttons.pressed(BTN_RIGHT))
                {
                    if (cursor == 20 && ballcolor < 4) ballcolor += 1;
                    if (cursor == 40) showballmovement = 1;
                    if (cursor == 50) showdifficulty = 1;
                    if (cursor == 60 && difficultysetting > 40) difficultysetting -= 10;
                }
                //_________________DISPLAY___________________
                game.display.setCursor(1,1);
                game.display.print("SETTINGS:");

                game.display.setCursor(15,20);
                game.display.print("Ball: ");
                if (ballcolor == 0)
                {
                    game.display.drawBitmap(50,21,ballgreen);
                    game.display.drawBitmap(58,21,ballred);
                }
                if (ballcolor == 1) game.display.drawBitmap(50,21,ballorange);
                if (ballcolor == 2) game.display.drawBitmap(50,21,ballyellow);
                if (ballcolor == 3) game.display.drawBitmap(50,21,ballblue);
                if (ballcolor == 4) game.display.drawBitmap(50,21,ballgray);
                game.display.setCursor(15,40);
                game.display.print("shwBllMvmnt: ");
                if (showballmovement == 0) game.display.print("off");
                if (showballmovement == 1) game.display.print("on");
                game.display.setCursor(15,50);
                game.display.print("shwDffclty: ");
                if (showdifficulty == 0) game.display.print("off");
                if (showdifficulty == 1) game.display.print("on");
                game.display.setCursor(15,60);
                game.display.print("strtDffclty: ");
                if (difficultysetting == 80) game.display.print("--");
                if (difficultysetting == 70) game.display.print("-");
                if (difficultysetting == 60) game.display.print("0");
                if (difficultysetting == 50) game.display.print("+");
                if (difficultysetting == 40) game.display.print("++");
                game.display.drawBitmap(2,76,backarrow);

                game.display.setCursor(5,cursor);
                game.display.print(">");
            }
            /*
            =========================ABOUT=========================
            */
            if (status == 3)
            {
                //________________LOGIC/CONTROL___________________
                if (game.buttons.pressed(BTN_B))
                {
                    status = 0;
                    cursor = 0;
                    endgame(1);
                }
                //____________________DISPLAY_____________________
                game.display.drawBitmap(2,2,paddlegreen);
                game.display.drawBitmap(13,2,paddlered);
                game.display.drawBitmap(24,2,ballgreen);
                game.display.drawBitmap(35,2,ballred);
                game.display.drawBitmap(46,2,ballorange);
                game.display.drawBitmap(57,2,ballyellow);
                game.display.drawBitmap(68,2,ballblue);
                game.display.drawBitmap(79,2,ballgray);

                game.display.setCursor(5,55);
                game.display.print("By: Zuzu36");
                game.display.setCursor(5,65);
                game.display.print("Made for Pokitto");
                game.display.drawBitmap(2,76,backarrow);
            }
            /*
            =========================GAME==========================
            */
            if (status == 1)
            {
                //_________________CONTROL___________________
                if (game.buttons.repeat(BTN_UP,0) && (padplayer > 0) && (gamestart == 1))
                {
                    padplayer -= 2;
                    moveplayer = -1;
                }
                else moveplayer = 0;
                if (game.buttons.repeat(BTN_DOWN,0) && (padplayer < 67) && (gamestart == 1))
                {
                    padplayer += 2;
                    moveplayer = 1;
                }
                else moveplayer = 0;
                if (game.buttons.pressed(BTN_A) && (gamestart == 0))
                {
                    if (win == 0)
                    {
                        movex = 3;
                    }
                    if (win == 1)
                    {
                        movex = -3;
                    }
                    gamestart = 1;
                }
                if (game.buttons.held(BTN_C,15))
                {
                    status = 0;
                    endgame(1);
                }
                //_________________LOGIC______________________
                if ((movecom > -2) && (movecom < 2))
                {
                    movecom = random(-2,2);
                }
                if ((difficulty == 10)||(difficulty == 100))
                {
                    endgame(1);
                }
                else difficulty = (difficultysetting - ((scoreplayer - scorecom)*2));
                //*******************GAMESTART****************
                if (gamestart == 1)
                {
                    ballx += movex;
                    bally += movey;
                    padcom += movecom;

                    if (movey == 0)
                    {
                        movey = random(-1,1);
                    }
                    //-----------------BALL------------------
                    if ((ballx - 1 <= 6) && ((bally + 6 >= padplayer) && (bally <= padplayer + 20)))
                    {
                        movex = 3;
                        movey += ((moveplayer / 2)+((bally - padplayer) / 20));
                    }
                    if ((ballx + 7 >= screenW - 6) && ((bally + 6>= padcom) && (bally <= padcom + 20)))
                    {
                        movex = -3;
                        movey += ((movecom / 4)+((bally -padcom) / 20));
                    }
                    if ((bally <= 0) || (bally + 6 >= 88))
                    {
                        movey *= -1;
                    }
                    //----------------SCORE-------------------
                    if (ballx <= 0)
                    {
                        scorecom += 1;
                        win = 0;
                        scorepoint(1);
                    }
                    if (ballx + 6 >= screenW)
                    {
                        scoreplayer += 1;
                        win = 1;
                        scorepoint(1);
                    }
                    if ((scoreplayer == 100) || (scorecom == 100))
                    {
                        endgame(1);
                    }
                    //-------------------COM------------------
                    if ((ballx > difficulty) && (movex > 0))
                    {
                        if (bally < padcom && padcom > 1)
                        {
                            movecom = -2;
                        }
                        if (bally + 6 > padcom + 20 && padcom + 20 < 86)
                        {
                            movecom = 2;
                        }
                    }
                    if (padcom - 1 <= 0)
                    {
                        movecom = 2;
                    }
                    if (padcom + 21 >= 86)
                    {
                        movecom = -2;
                    }
                }
                else
                {
                    game.display.setCursor(30,26);
                    game.display.print("Ready?");
                    game.display.drawBitmap(70,24,btna);
                }
                //_____________DRAW/PRINT/COLOR_______________
                if (showdifficulty == 1)
                {
                    game.display.setCursor(1,81);
                    game.display.print(difficulty);
                }
                if (showballmovement == 1)
                {
                    game.display.setCursor(1,1);
                    game.display.print(movey);
                }
                if (scoreplayer < 10) game.display.setCursor(43,5);
                else game.display.setCursor(37,5);
                game.display.print(scoreplayer);
                game.display.print(" - ");
                game.display.print(scorecom);
                game.display.drawBitmap(0,padplayer,paddlegreen);
                game.display.drawBitmap(screenW - 6,padcom,paddlered);

                if ((ballcolor == 0) && ((movex < 0) || ((win == 1) && (gamestart == 0)))) game.display.drawBitmap(ballx,bally,ballred);
                if ((ballcolor == 0) && ((movex > 0) || ((win == 0) && (gamestart == 0)))) game.display.drawBitmap(ballx,bally,ballgreen);
                if (ballcolor == 1) game.display.drawBitmap(ballx,bally,ballorange);
                if (ballcolor == 2) game.display.drawBitmap(ballx,bally,ballyellow);
                if (ballcolor == 3) game.display.drawBitmap(ballx,bally,ballblue);
                if (ballcolor == 4) game.display.drawBitmap(ballx,bally,ballgray);
            }
            game.display.bgcolor = 14;
            game.display.color = 15;
        }
        //game.update();
    }
    return 1;
}

void scorepoint(bool voidreset)
{
    padplayer = 34;
    padcom = 34;
    ballx = 52;
    bally = 44;
    movex = 0;
    movey = 0;
    gamestart = 0;
    movecom = 0;
    moveplayer = 0;
}

void endgame(bool voidreset2)
{
    padplayer = 34;
    padcom = 34;
    ballx = 52;
    bally = 44;
    movex = 0;
    movey = 0;
    scoreplayer = 0;
    scorecom = 0;
    gamestart = 0;
    win = 0;
    movecom = 0;
    moveplayer = 0;
    difficulty = 60;
}

