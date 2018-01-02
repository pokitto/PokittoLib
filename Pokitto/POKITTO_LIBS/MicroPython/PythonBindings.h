/**************************************************************************/
/*!
    @file     PythonBindings.h
    @author   Hannu Viitala

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2017, Hannu Viitala
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

#ifndef PYTHON_BINDINGS_H
#define PYTHON_BINDINGS_H

#if MICROPY_ENABLE_GC==1  // This means micropython is used

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define BTN_UP      1
#define BTN_RIGHT   2
#define BTN_DOWN    3
#define BTN_LEFT    0

// Main function in uPy library
EXTERNC int PythonMain(int argc, char **argv);

// Pokitto simulator API for uPython.

// Ring buffer struct for events
#define EVENT_RING_BUFFER_SIZE 10
typedef struct _EventRingBufferItem {
    uint8_t type;
    uint8_t key;
} EventRingBufferItem;

// Ring buffer
EXTERNC bool Pok_addToRingBuffer(uint8_t type, uint8_t key);
EXTERNC bool Pok_readAndRemoveFromRingBuffer(EventRingBufferItem* itemOut);

// Display
EXTERNC uint8_t Pok_Display_getNumberOfColors();
EXTERNC uint16_t Pok_Display_getWidth();
EXTERNC uint16_t Pok_Display_getHeight();
EXTERNC void Pok_Display_blitFrameBuffer(int16_t x, int16_t y, int16_t w, int16_t h, int16_t invisiblecol_, const uint8_t *buffer);
EXTERNC void Pok_Display_setSprite(uint8_t index, int16_t x, int16_t y, int16_t w, int16_t h, int16_t invisiblecol_, uint8_t *buffer, uint16_t* palette16x16bit, bool doResetDirtyRect);
EXTERNC void Pok_Display_setSpritePos(uint8_t index, int16_t x, int16_t y);
EXTERNC void Pok_Display_fillRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
EXTERNC void Pok_Display_write(const uint8_t *buffer, uint8_t size);
EXTERNC void Pok_Display_print(uint8_t x, uint8_t y, const char str[], uint8_t color);
EXTERNC uint16_t POK_game_display_RGBto565(uint8_t r, uint8_t g, uint8_t b);
EXTERNC void POK_game_display_setPalette(uint16_t* paletteArray, int16_t len);
EXTERNC void Pok_Display_setClipRect(int16_t x, int16_t y, int16_t w, int16_t h);

// Core
EXTERNC bool Pok_Core_update(bool useDirectMode, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
EXTERNC bool Pok_Core_isRunning();
EXTERNC bool Pok_Core_buttons_repeat(uint8_t button, uint8_t period);
EXTERNC bool Pok_Core_buttons_held(uint8_t button, uint8_t period);
EXTERNC bool Pok_Core_buttons_released(uint8_t button);

EXTERNC struct tm * localtime_cpp(const time_t * timer);
EXTERNC time_t time_cpp(time_t* timer);

// Debug
EXTERNC int pc_printf(const char* format, ...);
EXTERNC void pc_puts(const char* strWithNull);
EXTERNC void pc_putsn(const char* str, int len);

#endif // MICROPY_ENABLE_GC

#endif // PYTHON_BINDINGS_H

