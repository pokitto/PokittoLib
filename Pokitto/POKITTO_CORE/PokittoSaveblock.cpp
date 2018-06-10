/**************************************************************************/
/*!
    @file     PokittoSaveblock.cpp
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

#include "Pokitto_settings.h"
#include "Pokitto.h"
#include "PokittoSaveblock.h"

#ifndef POK_SIM
#else
#include "PokittoSimulator.h"
#endif

#define SBMAXKEYS 48 //number of available keys
#define SBMAXBLOCKS 112 //0xF8C (settings reserved area) - 0x180 (start of block table) / 32 bytes (block size)
#define SBBLOCKSIZE 32 //block size in bytes
#define SBKEYSIZE 8
#define SBINVALIDSLOT 255

using namespace Pokitto;

char Saveblock::_key[SBKEYSIZE];
char Saveblock::_keyorder;
bool Saveblock::_status;

Saveblock::Saveblock() {
    _status = false;
    _keyorder = SBINVALIDSLOT;
}

int Saveblock::begin(const char* idkey) {
    _status=false;
    // check if key already exists
    _keyorder = exists(idkey);
    if (_keyorder < SBMAXKEYS) {
            // key already exists
            // ToDo: read back values
    } else {
            // new key needed
            // check if we have free keyslots
            _keyorder = getFreeKeySlot();
            if (_keyorder>=SBMAXKEYS) return SBNOMOREKEYS; //no space for key
            // check if we have free storage blocks
            if (getFreeBlocks()==0) return SBNOBLOCKSFREE; //no space to allocate
    }
    _status = true; //were good to go
    memcpy(_key, idkey, SBKEYSIZE);
    writeKey(_key,_keyorder); // store the key in the table
    return 0; //success
}

int Saveblock::exists(const char* idkey) {
    for (int i=0; i< SBMAXKEYS; i++) {
        #ifndef POK_SIM
            if(eeprom_read_byte((uint16_t*)(i*SBKEYSIZE))==idkey[0]) {
                    int total=0;
                    for (int j=0; j<SBKEYSIZE;j++) {
                        if(eeprom_read_byte((uint16_t*)(i*SBKEYSIZE+j))==idkey[j]) total++;
                    }
                    if (total==SBKEYSIZE) return i; // return the keyslot number where key exists
            }
        #endif
    }
    return SBINVALIDSLOT; //not found
}

int Saveblock::getFreeKeySlot() {
    int freeslot=SBINVALIDSLOT;
    for (int i=0; i<SBMAXKEYS; i++) {
    #ifndef POK_SIM
    if (eeprom_read_byte((uint16_t*)(i*SBKEYSIZE))==0) {freeslot=i; break;}
    #endif
    }
    return freeslot;
}

int Saveblock::getFreeBlocks() {
    int freeblocks=0;
    for (int i=0;i<SBMAXBLOCKS;i++) {
        if (isFreeBlock(i)) freeblocks++;
    }
    return freeblocks;
}

bool Saveblock::isFreeBlock(int n) {
    if (n>=SBMAXBLOCKS) return false;
    #ifndef POK_SIM
    if (!(eeprom_read_byte((uint16_t*)(SBMAXKEYS*SBKEYSIZE+n))&0x80)) return true; //highest bit 0, its free
    #endif
    return false; //its not free
}

void Saveblock::writeKey(const char* key, int slot) {
    for (int i=0; i<=SBKEYSIZE; i++) {
    #ifndef POK_SIM
    eeprom_write_byte((uint16_t*)(slot*SBKEYSIZE+i),key[i]);
    #endif
    }
}

void Saveblock::getKeyAt(int n, char* answer) {
    answer[8]=0;
    if (n >= SBMAXKEYS) n=SBMAXKEYS-1;
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        answer[i] = eeprom_read_byte((uint16_t*)(n*SBKEYSIZE+i));
        #endif
    }
}

void Saveblock::formatKeyTable() {
    for (int j=0; j<SBMAXKEYS; j++) {
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        eeprom_write_byte((uint16_t*)(j*SBKEYSIZE+i),0);
        #endif
    }
    }
}

void Saveblock::eraseKeyAt(int n) {
    if (n >= SBMAXKEYS) n=SBMAXKEYS-1;
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        eeprom_write_byte((uint16_t*)(n*SBKEYSIZE+i),0);
        #endif
    }
}
