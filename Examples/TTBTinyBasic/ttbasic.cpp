/*
    TOYOSHIKI TinyBASIC V1.0
    Reference source
    (C)2012 Tetsuya Suzuki, All rights reserved.
*/

// Compiler requires description
//#include <stdlib.h>
//#include "sci.h"
#include "mbed.h"
#include "ttbasic.h"
#include "Pokitto.h"

Pokitto::Display d;

#if defined(TARGET_LPC1114)
//Serial pc(dp16, dp15);
#else
//Serial pc(USBTX, USBRX);
#endif

// Depending on device functions
// TO-DO Rewrite these functions to fit your machine
// And see 'getrnd()'
//void c_putch(char c){putch2(c);}
//char c_getch(){return getch2();}
//char c_kbhit(){return(kbhit2());}

// Standard C libraly (about same) functions
char c_toupper(char c) {return(c <= 'z' && c >= 'a' ? c - 32 : c);}
char c_isprint(char c) {return(c >= 32  && c <= 126);}
char c_isspace(char c) {return(c <= ' ' &&(c == ' ' || (c <= 13 && c >= 9)));}
char c_isdigit(char c) {return(c <= '9' && c >= '0');}
char c_isalpha(char c) {return ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'));}
char* c_strchr(char *s, char c){
    while(*s) {
        if(*s == c) return (s);
    ++s;
    }
    return NULL;
}

void c_putch(char c){
    if (c != 13) {
    cursormutex=true;
    int x=d.cursorX;
    int y=d.cursorY;
    d.bgcolor = 0;
    d.print(" ");  // print whitespace
    d.setCursor(x,y);
    d.print(c);
    cursormutex=false;
    }
    pc.putc(c);
    }
char c_getch(){
    char c;
    c = pc.getc();
    //if (c_isprint(c)) d.print(c);
    return c;
    }
char c_kbhit(){return(pc.readable());}
void newline(void){
    c_putch(13); //CR
    c_putch(10); //LF
}

// TOYOSHIKI TinyBASIC symbols
// TO-DO Rewrite defined values to fit your machine as needed
#define SIZE_LINE 27 //Command line buffer length + NULL
#define SIZE_IBUF 64 //i-code conversion buffer size
#define SIZE_LIST 1024 //List buffer size
#define SIZE_ARRY 32 //Array area size
#define SIZE_GSTK 6 //GOSUB stack size(2/nest)
#define SIZE_LSTK 15 //FOR stack size(5/nest)

// RAM mapping
char lbuf[SIZE_LINE]; //Command line buffer
unsigned char ibuf[SIZE_IBUF]; //i-code conversion buffer
short var[26]; //Variable area
short arr[SIZE_ARRY]; //Array area
unsigned char listbuf[SIZE_LIST]; //List area
unsigned char* clp; //Pointer current line
unsigned char* cip; //Pointer current Intermediate code
unsigned char* gstk[SIZE_GSTK]; //GOSUB stack
unsigned char gstki; //GOSUB stack index
unsigned char* lstk[SIZE_LSTK]; //FOR stack
unsigned char lstki; //FOR stack index

// Prototypes (necessity minimum)
short iexp(void);

// Keyword table
const char* kwtbl[] = {
    "GOTO", "GOSUB", "RETURN",
    "FOR", "TO", "STEP", "NEXT",
    "IF", "REM", "STOP",
    "INPUT", "PRINT", "LET",
    ",", ";",
    "-", "+", "*", "/", "(", ")",
    ">=", "#", ">", "=", "<=", "<",
     "@", "RND", "ABS", "SIZE",
    "LIST", "RUN", "NEW"
};

// Keyword count
#define SIZE_KWTBL (sizeof(kwtbl) / sizeof(const char*))

// i-code(Intermediate code) assignment
enum{
    I_GOTO, I_GOSUB, I_RETURN,
    I_FOR, I_TO, I_STEP, I_NEXT,
    I_IF, I_REM, I_STOP,
    I_INPUT, I_PRINT, I_LET,
    I_COMMA, I_SEMI,
    I_MINUS, I_PLUS, I_MUL, I_DIV, I_OPEN, I_CLOSE,
    I_GTE, I_SHARP, I_GT, I_EQ, I_LTE, I_LT,
    I_ARRAY, I_RND, I_ABS, I_SIZE,
    I_LIST, I_RUN, I_NEW,
    I_NUM, I_VAR, I_STR,
    I_EOL
};

// List formatting condition
#define IS_OP(p) (p >= I_MINUS && p <= I_LT) // Operator style
#define IS_SEP(p) (p == I_COMMA || p == I_SEMI) // Separator style
#define IS_TOKSP(p) (/*p >= I_GOTO && */p <= I_LET && p != I_RETURN) // Token style

// Error messages
unsigned char err;// Error message index
const char* errmsg[] ={
    "OK",
    "Devision by zero",
    "Overflow",
    "Subscript out of range",
    "Icode buffer full",
    "List full",
    "GOSUB too many nested",
    "RETURN stack underflow",
    "FOR too many nested",
    "NEXT without FOR",
    "NEXT without counter",
    "NEXT mismatch FOR",
    "FOR without variable",
    "FOR without TO",
    "LET without variable",
    "IF without condition",
    "Undefined line number",
    "\'(\' or \')\' expected",
    "\'=\' expected",
    "Illegal command",
    "Syntax error",
    "Internal error",
    "Abort by [ESC]"
};

// Error code assignment
enum{
    ERR_OK,
    ERR_DIVBY0,
    ERR_VOF,
    ERR_SOL,
    ERR_IBUFOF,
    ERR_LBUFOF,
    ERR_GSTKOF,
    ERR_GSTKUF,
    ERR_LSTKOF,
    ERR_LSTKUF,
    ERR_NEXTWOV,
    ERR_NEXTUM,
    ERR_FORWOV,
    ERR_FORWOTO,
    ERR_LETWOV,
    ERR_IFWOC,
    ERR_ULN,
    ERR_PAREN,
    ERR_VWOEQ,
    ERR_COM,
    ERR_SYNTAX,
    ERR_SYS,
    ERR_ESC
};


void c_puts(const char *s) {while(*s) c_putch(*s++);}
void c_gets(){
    char c;
    unsigned char len;

    len = 0;
    while((c = c_getch()) != 13){
        if( c == 9) c = ' '; // TAB exchange Space
        if((c == 8) && (len > 0)){ // Backspace manipulation
            len--;
            c_putch(8); c_putch(' '); c_putch(8);
        } else
        if(c_isprint(c) && (len < (SIZE_LINE - 1))){
            lbuf[len++] = c;
            c_putch(c);
        }
    }
    newline();
    lbuf[len] = 0; // Put NULL

    if(len > 0){
        while(c_isspace(lbuf[--len])); // Skip space
        lbuf[++len] = 0; // Put NULL
    }
}

// GOSUB-RETURN stack
void gpush(unsigned char* pd){
    if(gstki < SIZE_GSTK){
        gstk[gstki++] = pd;
        return;
    }
    err = ERR_GSTKOF;
}

unsigned char* gpop(){
    if(gstki > 0){
        return gstk[--gstki];
    }
    err = ERR_GSTKUF;
    return NULL;
}

// FOR-NEXT stack
void lpush(unsigned char* pd){
    if(lstki < SIZE_LSTK){
        lstk[lstki++] = pd;
        return;
    }
    err = ERR_LSTKOF;
}

unsigned char* lpop(){
    if(lstki > 0){
        return lstk[--lstki];
    }
    err = ERR_LSTKUF;
    return NULL;
}

// Print OK or error message
void error()
{
    newline();
    c_puts(errmsg[err]);
    newline();
    err = 0;
}

// Print numeric specified columns
void putnum(short value, short d){
    unsigned char i;
    unsigned char sign;

    if(value < 0){
        sign = 1;
        value = -value;
    } else {
        sign = 0;
    }

    lbuf[6] = 0;
    i = 6;
    do {
        lbuf[--i] = (value % 10) + '0';
        value /= 10;
    } while(value > 0);

    if(sign)
        lbuf[--i] = '-';

    //String length = 6 - i
    while(6 - i < d){ // If short
        c_putch(' '); // Fill space
        d--;
    }
    c_puts(&lbuf[i]);
}

// Input numeric and return value
// Called by only INPUT statement
short getnum(){
    short value, tmp;
    char c;
    unsigned char len;
    unsigned char sign;

    len = 0;
    while((c = c_getch()) != 13){
        if((c == 8) && (len > 0)){ // Backspace manipulation
            len--;
            c_putch(8); c_putch(' '); c_putch(8);
        } else
        if( (len == 0 && (c == '+' || c == '-')) ||
            (len < 6 && c_isdigit(c))){ // Numeric or sign only
            lbuf[len++] = c;
            c_putch(c);
        }
    }
    newline();
    lbuf[len] = 0;

    switch(lbuf[0]){
    case '-':
        sign = 1;
        len = 1;
        break;
    case '+':
        sign = 0;
        len = 1;
        break;
    default:
        sign = 0;
        len = 0;
        break;
    }

    value = 0; // Initialize value
    tmp = 0; // Temp value
    while(lbuf[len]){
        tmp = 10 * value + lbuf[len++] - '0';
        if(value > tmp){ // It means overflow
            err = ERR_VOF;
        }
        value = tmp;
    }
    if(sign)
        return -value;
    return value;
}

// Byte X,L,H -> Short HL
// Used to get line number or I_NUM value
short getvalue(unsigned char* ip){
    if(*ip == 0)
        return 32767; // Case X = 0
    return((short)*(ip + 1) + ((short)*(ip + 2) << 8));
}

// Get argument in parenthesis
short getparam(){
    short value;

    if(*cip != I_OPEN){
        err = ERR_PAREN;
        return 0;
    }
    cip++;
    value = iexp();
    if(err) return 0;

    if(*cip != I_CLOSE){
        err = ERR_PAREN;
        return 0;
    }
    cip++;

    return value;
}

// Search line by line number
unsigned char* getlp(short lineno){
    unsigned char *lp;

    lp = listbuf;
    while(*lp){
        if(getvalue(lp) >= lineno)
            break;
        lp += *lp;
    }
    return lp;
}

// Convert token to i-code
// Return byte length or 0
unsigned char toktoi(){
    unsigned char i; // Loop counter(i-code sometime)
    unsigned char len; //byte counter
    short value;
    short tmp;
    char* pkw; // Temporary keyword pointer
    char* ptok; // Temporary token pointer
    char c; // Surround the string character, " or '
    char* s; // Pointer to charactor at line buffer

    s = lbuf;
    len = 0; // Clear byte length
    while(*s){
        while(c_isspace(*s)) s++; // Skip space

        //Try keyword conversion
        for(i = 0; i < SIZE_KWTBL; i++){
            pkw = (char *)kwtbl[i]; // Point keyword
            ptok = s; // Point top of command line
            // Compare
            while((*pkw != 0) && (*pkw == c_toupper(*ptok))){
                pkw++;
                ptok++;
            }
            if(*pkw == 0){// Case success
                // i have i-code
                if(len >= SIZE_IBUF - 1){// List area full
                    err = ERR_IBUFOF;
                    return 0;
                }
                ibuf[len++] = i;
                s = ptok;
                break;
            }
        }

        // Case statement needs an argument except numeric, valiable, or strings
        switch(i){
        case I_REM:
            while(c_isspace(*s)) s++; // Skip space
            ptok = s;
            for(i = 0; *ptok++; i++); // Get length
            if(len >= SIZE_IBUF - 2 - i){
                err = ERR_IBUFOF;
                return 0;
            }
            ibuf[len++] = i; // Put length
            while(i--){ // Copy strings
                ibuf[len++] = *s++;
            }
            return len;
        default:
            break;
        }

        if(*pkw != 0){ // Case not keyword
            ptok = s; // Point top of command line

            // Try numeric conversion
            if(c_isdigit(*ptok)){
                value = 0;
                tmp = 0;
                do{
                    tmp = 10 * value + *ptok++ - '0';
                    if(value > tmp){
                        err = ERR_VOF;
                        return 0;
                    }
                    value = tmp;
                } while(c_isdigit(*ptok));

                if(len >= SIZE_IBUF - 3){
                    err = ERR_IBUFOF;
                    return 0;
                }
                s = ptok;
                ibuf[len++] = I_NUM;
                ibuf[len++] = value & 255;
                ibuf[len++] = value >> 8;
            } else

            // Try valiable conversion
            if(c_isalpha(*ptok)){
                if(len >= SIZE_IBUF - 2){
                    err = ERR_IBUFOF;
                    return 0;
                }
                if(len >= 2 && ibuf[len -2] == I_VAR){ // Case series of variables
                     err = ERR_SYNTAX; // Syntax error
                     return 0;
                }
                ibuf[len++] = I_VAR; // Put i-code
                ibuf[len++] = c_toupper(*ptok) - 'A'; // Put index of valiable area
                s++;
            } else

            // Try string conversion
            if(*s == '\"' || *s == '\''){// If start of string
                c = *s;
                s++; // Skip " or '
                ptok = s;
                for(i = 0; (*ptok != c) && c_isprint(*ptok); i++) // Get length
                    ptok++;
                if(len >= SIZE_IBUF - 1 - i){ // List area full
                    err = ERR_IBUFOF;
                    return 0;
                }
                ibuf[len++] = I_STR; // Put i-code
                ibuf[len++] = i; // Put length
                while(i--){ // Put string
                    ibuf[len++] = *s++;
                }
                if(*s == c) s++; // Skip " or '

            // Nothing mutch
            } else {
                err = ERR_SYNTAX;
                return 0;
            }
        }
    }
    ibuf[len++] = I_EOL; // Put end of line
    return len; // Return byte length
}

//Listing 1 line of i-code
void putlist(unsigned char* ip){
    unsigned char i;
    short value;

    while(*ip != I_EOL){
        // Case keyword
        if(*ip < SIZE_KWTBL){
            c_puts(kwtbl[*ip]);
            if(IS_TOKSP(*ip) || *ip == I_SEMI)c_putch(' ');
            if(*ip == I_REM){
                ip++;
                i = *ip++;
                while(i--){
                    c_putch(*ip++);
                }
                return;
            }
            ip++;
        } else

        // Case numeric
        if(*ip == I_NUM){
            putnum(getvalue(ip), 0);
            ip += 3;
            if(!IS_OP(*ip) && !IS_SEP(*ip)) c_putch(' ');
        } else

        // Case valiable
        if(*ip == I_VAR){
            ip++;
            c_putch(*ip++ + 'A');
            if(!IS_OP(*ip) && !IS_SEP(*ip)) c_putch(' ');
        } else

        // Case string
        if(*ip == I_STR){
            ip++;
            value = 0;
            i = *ip;
            while(i--){
                if(*(ip + i + 1) == '\"')
                    value = 1;
            }
            if(value) c_putch('\''); else c_putch('\"');
            i = *ip++;
            while(i--){
                c_putch(*ip++);
            }
            if(value) c_putch('\''); else c_putch('\"');

        // Nothing match, I think, such case is impossible
        } else {
            err = ERR_SYS;
            return;
        }
    }
}

// Insert i-code to the list
void inslist(){
    unsigned char len;
    unsigned char *lp1, *lp2;

    cip = ibuf;
    clp = getlp(getvalue(cip));

    lp1 = clp;
    if(getvalue(lp1) == getvalue(cip)){
        // Temporary measures of the case that
        // same line numbere exists and list area full,
        // existing line is deleted and new line is not inserted in.

        // if((getsize() - *lp1) < *cip){
        //  err = ERR_LBUFOF;
        //  return;
        // }

        lp2 = lp1 + *lp1;
        while((len = *lp2) != 0){
            while(len--){
                *lp1++ = *lp2++;
            }
        }
        *lp1 = 0;
    }

    // Case line number only
    if(*cip == 4)
            return;

    // Check insertable
    while(*lp1){
        lp1 += *lp1;
    }
    if(*cip > (listbuf + SIZE_LIST - lp1 - 1)){
        err = ERR_LBUFOF;
        return;
    }

    // Make space
    len = lp1 - clp + 1;
    lp2 = lp1 + *cip;
    while(len--){
        *lp2-- = *lp1--;
    }

    // Insert
    len = *cip;
    while(len--){
        *clp++ = *cip++;
    }
}

// Return free memory
short getsize(){
    short value;
    unsigned char* lp;

    lp = listbuf;
    while(*lp){
        lp += *lp;
    }

    value = listbuf + SIZE_LIST - lp - 1;
    return value;
}

// Return Absolute value
short getabs(){
    short value;

    value = getparam();
    if(err) return 0;

    if(value < 0) value *= -1;
    return value;
}

// Return random number
// TO-DO Rewrite this function to fit your machine
short getrnd(void){
    short value;

    value = getparam();
    if(err) return 0;

    return(rand() % value);
}

short getarray()
{
    short index;

    index = getparam();
    if(err) return 0;

    if(index < SIZE_ARRY){
        return arr[index];
    } else {
        err = ERR_SOL;
        return 0;
    }
}

short ivalue(){
    short value;

    switch(*cip){
     case I_PLUS:
         cip++;
         value = ivalue();
         break;
     case I_MINUS:
         cip++;
         value = 0 - ivalue();
         break;
     case I_VAR:
         cip++;
         value = var[*cip++];
         break;
     case I_NUM:
         value = getvalue(cip);
         cip += 3;
         break;
     case I_ARRAY:
         cip++;
        value = getarray();
        break;
     case I_OPEN:
         value = getparam();
        break;
     case I_RND:
        cip++;
        value = getrnd();
        break;
     case I_ABS:
        cip++;
        value = getabs();
        break;
     case I_SIZE:
        cip++;
        if(*cip == I_OPEN){
            cip++;
            if(*cip == I_CLOSE)
                cip++;
            else{
                err = ERR_PAREN;
            }
        }
        value = getsize();
        break;

     default:
        value = 0;
        err = ERR_SYNTAX;
        break;
    }
    return value;
}

short icalc()
{
    short value1, value2;

    value1 = ivalue();

    while(1){
        if(*cip == I_DIV){
            cip++;
            value2 = ivalue();
            if(value2 == 0){
                err = ERR_DIVBY0;
                break;
            }
            value1 /= value2;
        } else
        if(*cip == I_MUL){
            cip++;
            value2 = ivalue();
            value1 *= value2;
        } else {
            break;
        }
    }
    return value1;
}

short iexp()
{
    short value1, value2;

    value1 = icalc();

    while(*cip == I_PLUS || *cip == I_MINUS){
        value2 = icalc();
        value1 += value2;
    }
    return value1;
}

void iprint(){
    short value;
    short len;
    unsigned char i;

    len = 0;
    while(1){
        switch(*cip){
        case I_SEMI:
        case I_EOL:
            break;
        case I_STR:
            cip++;
            i = *cip++;
            while(i--){
                c_putch(*cip++);
            }
            break;
        case I_SHARP:
            cip++;
            len = iexp();
            if(err) return;
            break;
        default:
            value = iexp();
            if(err) return;
            putnum(value, len);
            break;
        }

        if(*cip == I_COMMA){
            cip++;
        }else{
            break;
        }
    };
    newline();
}

void iinput(){
    unsigned char i;
    short value;
    short index;

    while(1){
        switch(*cip){
        case I_STR:
            cip++;
            i = *cip++;
            while(i--){
                c_putch(*cip++);
            }
            if(*cip == I_VAR){
                cip++;
                value = getnum();
                var[*cip++] = value;
            } else
            if(*cip == I_ARRAY){
                cip++;
                index = getparam();
                if(err) return;
                if(index >= SIZE_ARRY){
                    err = ERR_SOL;
                    return;
                }
                value = getnum();
                arr[index] = value;
            }
            break;
        case I_VAR:
            cip++;
            c_putch(*cip + 'A');
            c_putch(':');
            value = getnum();
            var[*cip++] = value;
            break;
        case I_ARRAY:
            cip++;
            index = getparam();
            if(err)
                return;
            if(index >= SIZE_ARRY){
                err = ERR_SOL;
                return;
            }
            c_putch('@');c_putch('(');
            putnum(index,0);
            c_putch(')');c_putch(':');
            value = getnum();
            arr[index] = value;
            break;
        }

        switch(*cip){
        case I_COMMA:
            cip++;
            break;
        case I_SEMI:
        case I_EOL:
            return;
        default:
            err = ERR_SYNTAX;
            return;
        }
    }
}

//char iif(){
signed char iif(){
    short value1, value2;
    unsigned char i;

    value1 = iexp();
    if(err) return -1;

    i = *cip++;

    value2 = iexp();
    if(err) return -1;

    switch(i){
    case I_EQ:
        return value1 == value2;
    case I_SHARP:
        return value1 != value2;
    case I_LT:
        return value1 <  value2;
    case I_LTE:
        return value1 <= value2;
    case I_GT:
        return value1 >  value2;
    case I_GTE:
        return value1 >= value2;
    default:
         err = ERR_IFWOC;
         return -1;
    }
}

void ivar(){
    short value;
    short index;

    index = *cip++;
    if(*cip == I_EQ){
        cip++;
        value = iexp();
        if(err) return;
    } else {
        err = ERR_VWOEQ;
        return;
    }

    if(index < 26){
        var[index] = value;
    } else {
        err = ERR_SOL;
    }
}

void iarray(){
    short value;
    short index;

    index = getparam();
    if(err) return;
    if(*cip == I_EQ){
        cip++;
        value = iexp();
        if(err) return;
    } else {
        err = ERR_VWOEQ;
        return;
    }

    if(index < SIZE_ARRY){
        arr[index] = value;
    } else {
        err = ERR_SOL;
    }
}

void ilet(){
    switch(*cip){
     case I_VAR:
        cip++;
        ivar();
        break;
    case I_ARRAY:
        cip++;
        iarray();
        break;
     default:
        err = ERR_LETWOV;
        break;
    }
}

void ilist(){
    short lineno;

    if(*cip == I_NUM){
        lineno = getvalue(cip);
        cip += 3;
    } else {
        lineno = 0;
    }

    for(    clp = listbuf;
            *clp && (getvalue(clp) < lineno);
            clp += *clp);

    while(*clp){
        putnum(getvalue(clp), 0);
        c_putch(' ');
        putlist(clp + 3);
        if(err){
            break;
        }
        newline();
        clp += *clp;
    }
}

void inew(void){
    unsigned char i;

    for(i = 0; i < 26; i++)
        var[i] = 0;
    gstki = 0;
    lstki = 0;
    *listbuf = 0;
    clp = listbuf;
}

unsigned char* iexe(){
    short lineno;
    unsigned char cd;
    unsigned char* lp;
    short vto, vstep;
    short index;

    while(1){
        if(c_kbhit()){
            if(c_getch() == 27){
                while(*clp){
                    clp += *clp;
                }
                err = ERR_ESC;
                return clp;
            }
        }

        switch(*cip){
        case I_GOTO:
            cip++;
            lineno = iexp();
            clp = getlp(lineno);
            if(lineno != getvalue(clp)){
                err = ERR_ULN;
                return NULL;
            }
            cip = clp + 3;
            continue;
        case I_GOSUB:
            cip++;
            lineno = iexp();
            if(err) return NULL;
            lp = getlp(lineno);
            if(lineno != getvalue(lp)){
                err = ERR_ULN;
                return NULL;
            }
            gpush(clp);
            gpush(cip);
            if(err) return NULL;
            clp = lp;
            cip = clp + 3;
            continue;
        case I_RETURN:
            cip = gpop();
            lp = gpop();
            if(err) return NULL;
            clp = lp;
            break;
        case I_FOR:
            cip++;
            if(*cip++ != I_VAR){
                err = ERR_FORWOV;
                return NULL;
            }
            index = *cip;
            ivar();
            if(err) return NULL;

            if(*cip == I_TO){
                cip++;
                vto = iexp();
            } else {
                err = ERR_FORWOTO;
                return NULL;
            }
            if(*cip == I_STEP){
                cip++;
                vstep = iexp();
            } else {
                vstep = 1;
            }

            lpush(clp);
            lpush(cip);
            lpush((unsigned char*)vto);
            lpush((unsigned char*)vstep);
            lpush((unsigned char*)index);
            if(err) return NULL;
            break;
        case I_NEXT:
            cip++;
            if(*cip++ !=I_VAR){
                err = ERR_NEXTWOV;
                return NULL;
            }

            if(lstki < 5){
                err = ERR_LSTKUF;
                return NULL;
            }
            index = (short)lstk[lstki - 1];
            if(index != *cip){
                err = ERR_NEXTUM;
                return NULL;
            }
            cip++;
            vstep = (short)lstk[lstki - 2];
            var[index] += vstep;

            vto = (short)lstk[lstki - 3];
            if( ((vstep < 0) && (var[index] < vto)) ||
                ((vstep > 0) && (var[index] > vto))){
                lstki -= 5;
                break;
            }
            cip = lstk[lstki - 4];
            clp = lstk[lstki - 5];
            continue;

        case I_IF:
            cip++;
            cd = iif();
            if(err){
                err = ERR_IFWOC;
                return NULL;
            }
            if(cd)
                continue;
            // If false, same as REM
        case I_REM:
            // Seek pointer to I_EOL
            // No problem even if it points not realy end of line
            while(*cip != I_EOL) cip++;
            break;
        case I_STOP:
            while(*clp){
                clp += *clp;
            }
            return clp;
        case I_INPUT:
            cip++;
            iinput();
            break;
        case I_PRINT:
            cip++;
            iprint();
            break;
        case I_LET:
            cip++;
            ilet();
            break;
        case I_VAR:
            cip++;
            ivar();
            break;
        case I_ARRAY:
            cip++;
            iarray();
            break;
        case I_LIST:
        case I_NEW:
        case I_RUN:
            err = ERR_COM;
            return NULL;
        }

        switch(*cip){
        case I_SEMI:
            cip++;
            break;
        case I_EOL:
            return clp + *clp;
        default:
            err = ERR_SYNTAX;
            return NULL;
        }
    }
}

void irun(){
    unsigned char* lp;

    gstki = 0;
    lstki = 0;
    clp = listbuf;

    while(*clp){
        cip = clp + 3;
        lp = iexe();
        if(err)
            return;
        clp = lp;
    }
}

void icom(){
    cip = ibuf;
    switch(*cip){
    case I_LIST:
        cip++;
        if(*cip == I_EOL || *(cip + 3) == I_EOL)
            ilist();
        else
            err = ERR_SYNTAX;
        break;
    case I_NEW:
        cip++;
        if(*cip == I_EOL)
            inew();
        else
            err = ERR_SYNTAX;
        break;
    case I_RUN:
        cip++;
        irun();
        break;
    default:
        iexe();
        break;
    }

    if(err && err != ERR_ESC){
        if(cip >= listbuf && cip < listbuf + SIZE_LIST && *clp)
        {
            newline(); c_puts("ERR LINE:");
            putnum(getvalue(clp), 0);
            c_putch(' ');
            putlist(clp + 3);
        }
        else
        {
            newline(); c_puts("TYPED: ");
            putlist(ibuf);
        }
    }

}

void basic(){
    unsigned char len;

//  sci2_init();
    //pc.baud(9600);
    //inew();
    //c_puts("TOYOSHIKI TINY BASIC"); newline();
//    c_puts("PIC24F EDITION"); newline();
    //c_puts("mbed EDITION"); newline();
    //error(); // Print OK, and Clear error flag

    // Input 1 line and execute
    //while(1){
        //c_putch('>');// Prompt
        c_gets(); // Input 1 line
        len = toktoi(); // Convert token to i-code
        if(err){ // Error
            newline(); c_puts("YOU TYPE:");
            c_puts(lbuf);
            error();
            //continue; // Do nothing
        } else {

        if(*ibuf == I_NUM){ // Case the top includes line number
            *ibuf = len; // Change I_NUM to byte length
            inslist(); // Insert list
            if(err){
                error(); // List buffer overflow
            }
        } else {
            icom(); // Execute direct
            error(); // Print OK, and Clear error flag
        }
        }
    //}
}


