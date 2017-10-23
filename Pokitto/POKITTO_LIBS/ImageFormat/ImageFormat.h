/**************************************************************************/
/*!
    @file     ImageFormat.h
    @author   Hannu Viitala

    @section LICENSE

    Pokitto development stage library
    Software License Agreement

    Copyright (c) 2015, Jonne Valola ("Author")
    All rights reserved.

    This library is intended solely for the purpose of Pokitto development.

    Redistribution and use in source and binary forms, with or without
    modification requires written permission from Author.
*/
/**************************************************************************/

#ifndef IMAGE_FORMAT_H

#define POK_TRACE(str) printf("%s (%d): %s", __FILE__, __LINE__,str)

extern int openImageFileFromSD(char*, uint16_t **, uint8_t **);
extern int directDrawImageFileFromSD(int16_t /*sx*/, int16_t /*sy*/, char* /*filepath*/);
extern int directDrawImageFileFromSD(uint16_t /*ix*/, uint16_t /*iy*/, uint16_t /*iw*/, uint16_t /*ih*/, int16_t /*sx*/, int16_t /*sy*/, char* /*filepath*/);

#define IMAGE_FORMAT_H


#ifndef boolean
//typedef bool boolean;
#endif

#endif // IMAGE_FORMAT_H
