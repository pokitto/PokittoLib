/*
    TOYOSHIKI TinyBASIC V1.0
    Reference source
    (C)2012 Tetsuya Suzuki, All rights reserved.
*/

#include "USBSerial.h"

extern USBSerial pc;

extern void c_puts(const char *s);
extern void newline(void);
extern void error();
extern void c_putch(char c);
extern void c_gets();
extern void inew();
extern bool cursormutex;

void basic(void);
