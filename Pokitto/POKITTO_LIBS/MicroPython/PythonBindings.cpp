/**************************************************************************/
/*!
    @file     PythonBindings.cpp
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

#include "PokittoCore.h"
#include "PokittoDisplay.h"
#include "PythonBindings.h"
#include "time.h"

using namespace Pokitto;

#if MICROPY_ENABLE_GC==1  // This means micropython is used

// Ring buffer size
#define EVENT_RING_BUFFER_SIZE 10

// Ring buffer Local data
EventRingBufferItem eventRingBuffer[EVENT_RING_BUFFER_SIZE];
int rbNextFreeItemIndex = 0;
int rbOldestItemIndex = -1; // empty

// Add item to the end of ring buffer
bool Pok_addToRingBuffer(uint8_t type, uint8_t key) {

    if(rbNextFreeItemIndex == rbOldestItemIndex) {
        //POK_TRACE("!!HV buffer full. CANNOT ADD! ***\r\n");
        return false; // The ring buffer is full. Do nothing.
    }

    // Store the item and the new index.
    eventRingBuffer[rbNextFreeItemIndex].key = key;
    eventRingBuffer[rbNextFreeItemIndex].type = type;

    // if was empty, update
    if(rbOldestItemIndex == -1) {
        rbOldestItemIndex = rbNextFreeItemIndex;
        //POK_TRACE("!!HV added first item ***\r\n");
    }

     // Advance index
    if(++rbNextFreeItemIndex >= EVENT_RING_BUFFER_SIZE)
        rbNextFreeItemIndex = 0; // continue from the start of array

    //!!HV
    //char test[64];
    int size = rbNextFreeItemIndex-rbOldestItemIndex;
    if(size<0)
        size += EVENT_RING_BUFFER_SIZE;
    //sprintf(test, "!!HV: Added to buffer. size = %d\r\n", size);
    //POK_TRACE(test);

    return true;
}

// Remove item from the start of ring buffer
bool Pok_readAndRemoveFromRingBuffer(EventRingBufferItem* itemOut){

    if(rbOldestItemIndex == -1) {
        //POK_TRACE("!!HV buffer is empty. cannot read. ***\n");
        return false; // The ring buffer is empty. Do nothing.
    }

    // Read oldest item.
    *itemOut = eventRingBuffer[rbOldestItemIndex];

    // Advance oldest index
    if(++rbOldestItemIndex >= EVENT_RING_BUFFER_SIZE)
        rbOldestItemIndex = 0; // continue from the start of array

    if(rbOldestItemIndex == rbNextFreeItemIndex) {
        rbOldestItemIndex = -1; // buffer is empty now
        //POK_TRACE("!!HV buffer is empty now ***\r\n");
    }

    //!!HV
    //char test[64];
    int size = rbNextFreeItemIndex-rbOldestItemIndex;
    if(size<0)
        size += EVENT_RING_BUFFER_SIZE;
    //sprintf(test, "!!HV: Removed from buffer. size = %d\r\n", size);
    //POK_TRACE(test);

    return true;
}

uint8_t Pok_Display_getNumberOfColors() {

    return Display::getNumberOfColors();
}

uint16_t Pok_Display_getWidth() {

    return Display::getWidth();
}

uint16_t Pok_Display_getHeight() {

    return Display::getHeight();
}

void Pok_Display_write(const uint8_t *buffer, uint8_t size) {

    Display::write(buffer, size);
}

void Pok_Display_print(uint8_t x, uint8_t y, const char str[], uint8_t color) {

    Display::color = color;
    Display::print( x, y, str );
}

void Pok_Display_blitFrameBuffer(int16_t x, int16_t y, int16_t w, int16_t h, int16_t invisiblecol_, const uint8_t *buffer) {
    if( invisiblecol_ != -1)
        Display::invisiblecolor = (uint8_t)invisiblecol_;
    Display::drawBitmapData(x, y, w, h, buffer );
}

void Pok_Display_setSprite(uint8_t index, int16_t x, int16_t y, int16_t w, int16_t h, int16_t invisiblecol_, uint8_t *buffer, uint16_t* palette16x16bit, bool doResetDirtyRect) {
    Display::invisiblecolor = (uint8_t)invisiblecol_;
    Display::setSprite(index, buffer, palette16x16bit, x, y, w, h, doResetDirtyRect );
}

void Pok_Display_setSpritePos(uint8_t index, int16_t x, int16_t y) {
    Display::setSpritePos(index, x, y);
}

void Pok_Display_fillRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    Display::color = color;
    Display::fillRectangle(x, y, w, h);
}


uint16_t POK_game_display_RGBto565(uint8_t r, uint8_t g, uint8_t b) {
    return Display::RGBto565(r, g, b);
}

void POK_game_display_setPalette(uint16_t* paletteArray, int16_t len) {
    for( int i = 0; i < len; i++ )
        Display::palette[i] = paletteArray[i];
}

void Pok_Display_setClipRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    if(h == 0)
        Display::setClipRect(0, 0, LCDWIDTH, LCDHEIGHT);  // Remove clip rect
    else
        Display::setClipRect(x, y, w, h);  // Set clip rect
}

bool Pok_Core_update(bool useDirectMode, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {

    return Core::update(useDirectMode, x, y, w, h);
}

bool Pok_Core_isRunning() {

    return Core::isRunning();
}

bool Pok_Core_buttons_repeat(uint8_t button, uint8_t period) {

    return Core::buttons.repeat(button, period);
}

bool Pok_Core_buttons_held(uint8_t button, uint8_t period) {

    return Core::buttons.held(button, period);
}

bool Pok_Core_buttons_released(uint8_t button) {

    return Core::buttons.released(button);
}


struct tm * localtime_cpp(const time_t * timer){
    return(localtime(timer));
}

time_t time_cpp(time_t* timer){
    return(time(timer));
}


#endif // MICROPY_ENABLE_GC
