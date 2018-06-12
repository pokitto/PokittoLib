/**************************************************************************/
/*!
    @file     PokittoCookie.cpp
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
#include "PokittoCookie.h"

#ifndef POK_SIM
#else
#include "PokittoSimulator.h"
#endif

using namespace Pokitto;

//char Cookie::_key[SBKEYSIZE];
//char Cookie::_keyorder;
//bool Cookie::_status;

#define HARDCODEDOFFSET 25 //bypasses Cookie parent instance general data (that does not need to be saved in EEPROM)

Cookie::Cookie() {
    _status = false;
    _keyorder = SBINVALIDSLOT;
}

int Cookie::initialize() {
    //initialize is called from begin() and can be called several times during program run
    int datasize = _datasize;
    // check if key already exists
    _keyorder = exists(_key);
    if (_keyorder < SBMAXKEYS) {
            // key already exists
            // check amount of existing storage reserved for cookie
            datasize -= getAssignedBlocks()*SBBLOCKSIZE;
            if (datasize<=0) {
                // the size of data matches the size requested
                // therefore retrieve data from storage
                _status = true; //were good to go
                loadCookie();
            } else {
            // if that does not cover the whole size (maybe a newer version of program, who knows)
            // then do not load but reserve more blocks and store a new version
                while (datasize>0) {
                    if(reserveBlock()) datasize -= SBBLOCKSIZE;
                    else return SBNOTENOUGHBLOCKSFREE; //no space to allocate
                }
                _status = true; //were good to go
                eraseKeytableEntry(_keyorder);
                writeKeyToKeytable(_key,_keyorder); // write the key in the key table in EEPROM
                saveCookie();
            }
    } else {
            // new key needed
            // check if we have free keyslots
            _keyorder = getFreeKeytableSlot();
            if (_keyorder>=SBMAXKEYS) return SBNOMOREKEYS; //no space for key
            // check if we have free storage blocks
            if (getFreeBlocks()*SBBLOCKSIZE<datasize) return SBNOTENOUGHBLOCKSFREE; //no space to allocate
            while (datasize>0) {
                //reserve enough blocks for the data until all data can fit
                if(reserveBlock()) datasize -= SBBLOCKSIZE;
                else return SBNOTENOUGHBLOCKSFREE; //no space to allocate
            }
    }
    _status = true; //were good to go
    eraseKeytableEntry(_keyorder);
    writeKeyToKeytable(_key,_keyorder); // write the key in the key table in EEPROM
    return 0;
}

int Cookie::begin(const char* idkey, int datasize, char* ptr) {
    _status=false;
    _datasize=datasize-HARDCODEDOFFSET;// warning! hardcoded! sizeof(this); //do not include the data of the parent Cookie instance
    _pointer = ptr + HARDCODEDOFFSET;// warning! hardcoded! sizeof(this); //point to the beginning of the inherited instance
    char _idkey[8];
    // make _idkey exactly 8 readable characters long
    for (int t = 0 ; t < 8 ; t++) _idkey[t]=' ';
    for (int t = 0 ; t < 8 ; t++) {if (idkey[t]==0) break; _idkey[t]=idkey[t];}
    // clean Keytable of keys with no storage
    cleanKeytable();
    memcpy(_key, _idkey, SBKEYSIZE); //store name of key
    initialize();
    return 0; //success
}

bool Cookie::saveCookie() {
    if (!_status || !_pointer) initialize(); //reinitialize if needed
    if (!_status || !_pointer) return false; //return if initialize still failed
    char* p = _pointer;
    _head=0;
    _block=0;
    _block=findMyNextBlock();
    for (int i=0; i<_datasize; i++) writeQueue(*p++);
}

bool Cookie::loadCookie() {
    if (!_status || !_pointer) return false;
    char* p = _pointer;
    _head=0;
    _block=0;
    _block=findMyNextBlock();
    for (int i=0; i<_datasize; i++) *p++ = readQueue();
}

void Cookie::deleteCookie() {
    if (!_status) return;
    // free all blocks held by Cookie
    for (int i=0; i<SBMAXBLOCKS; i++) {
            if (isMyBlock(i)) freeBlock(i);
    }
    // erase Cookie entry from keytable
    eraseKeytableEntry(_keyorder);
    // set status to deleted
    _status = false;
}

int Cookie::exists(const char* idkey) {
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

int Cookie::getFreeKeytableSlot() {
    int freeslot=SBINVALIDSLOT;
    for (int i=0; i<SBMAXKEYS; i++) {
    #ifndef POK_SIM
    if (eeprom_read_byte((uint16_t*)(i*SBKEYSIZE))==0) {freeslot=i; break;}
    #endif
    }
    return freeslot;
}

int Cookie::getAssignedBlocks() {
    int assignedblocks=0;
    for (int i=0;i<SBMAXBLOCKS;i++) {
        if (isMyBlock(i)) assignedblocks++;
    }
    return assignedblocks;
}

int Cookie::getFreeBlocks() {
    int freeblocks=0;
    for (int i=0;i<SBMAXBLOCKS;i++) {
        if (isFreeBlock(i)) freeblocks++;
    }
    return freeblocks;
}

bool Cookie::isFreeBlock(int n) {
    if (n>=SBMAXBLOCKS) return false;
    #ifndef POK_SIM
    if (!(eeprom_read_byte((uint16_t*)(SBMAXKEYS*SBKEYSIZE+n))&0x80)) return true; //highest bit 0, its free
    #endif
    return false; //its not free
}

bool Cookie::isMyBlock(int n) {
    if (n>=SBMAXBLOCKS) return false;
    if (isFreeBlock(n)) return false; //"free" blocks can not be "reserved" at the same time!
    #ifndef POK_SIM
    char temp; int address;
    address = (SBMAXKEYS*SBKEYSIZE+n);
    temp = eeprom_read_byte((uint16_t*)address);
    if ((temp&0x7F) ==_keyorder) return true;
    #endif
    return false; //its not your block
}

bool Cookie::blockIsOwnedBy(int n, int k) {
    if (n>=SBMAXBLOCKS) return false;
    if (k>=SBMAXKEYS) return false;
    if (isFreeBlock(n)) return false; //"free" blocks can not be "owned" by anyone
    #ifndef POK_SIM
    char temp; int address;
    address = (SBMAXKEYS*SBKEYSIZE+n);
    temp = eeprom_read_byte((uint16_t*)address);
    if ((temp&0x7F) == k) return true;
    #endif
    return false; //its not your block
}

void Cookie::writeKeyToKeytable(const char* key, int slot) {
    for (int i=0; i<SBKEYSIZE; i++) {
    #ifndef POK_SIM
    if (key[i]) eeprom_write_byte((uint16_t*)(slot*SBKEYSIZE+i),key[i]);
    else eeprom_write_byte((uint16_t*)(slot*SBKEYSIZE+i),0);
    #endif
    }
}

void Cookie::readKeytableEntry(int n, char* answer) {
    answer[8]=0;
    if (n >= SBMAXKEYS) n=SBMAXKEYS-1;
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        answer[i] = eeprom_read_byte((uint16_t*)(n*SBKEYSIZE+i));
        #endif
    }
}

char Cookie::getBlockTableEntry(int n) {
    if (n>=SBMAXBLOCKS) return 0x80; // out of bounds will return a reserved block marker
    #ifndef POK_SIM
        return eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+n));
    #endif
    return 0x80;
}

void Cookie::readBlock(int n, char* data) {
    for (int i=0; i<SBBLOCKSIZE; i++) {
    data[i]=0;
    #ifndef POK_SIM
        if (n < SBMAXBLOCKS) data[i] = eeprom_read_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+SBMAXBLOCKS+n*SBBLOCKSIZE+i));
    #endif
    }
}

void Cookie::formatKeytable() {
    for (int j=0; j<SBMAXKEYS; j++) {
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        eeprom_write_byte((uint16_t*)(j*SBKEYSIZE+i),0);
        #endif
    }
    }
}

void Cookie::freeBlock(int n) {
    if (n >= SBMAXBLOCKS) return; //out of bounds
    #ifndef POK_SIM
        // delete entry from blocktable
        eeprom_write_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+n),0);
    #endif
    for (int i=0; i<SBBLOCKSIZE;i++) {
    #ifndef POK_SIM
        // wipe data in the block
        eeprom_write_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+SBMAXBLOCKS+n*SBBLOCKSIZE+i),0);
    #endif
    }
}

bool Cookie::reserveBlock() {
    for (int i=0; i<SBMAXBLOCKS;i++) {
    #ifndef POK_SIM
        // reserve block from blocktable
        if (isFreeBlock(i)) {
                //free block found, mark it for us in the blocktable
                eeprom_write_byte((uint16_t*)(SBKEYSIZE*SBMAXKEYS+i),_keyorder | 0x80);
                return true;
        }
    #endif
    }
    return false; // no free block found
}

void Cookie::eraseKeytableEntry(int n) {
    if (n >= SBMAXKEYS) n=SBMAXKEYS-1;
    for (int i=0; i<SBKEYSIZE; i++) {
        #ifndef POK_SIM
        eeprom_write_byte((uint16_t*)(n*SBKEYSIZE+i),0);
        #endif
    }
}

void Cookie::cleanKeytable() {
    //Remove any keys without blocks
    for (int entry=0; entry<SBMAXKEYS; entry++) {
            if (eeprom_read_byte((uint16_t*)(entry*SBKEYSIZE))) {
                bool isEmpty=true;
                for (int block=0; block<SBMAXBLOCKS; block++) if (blockIsOwnedBy(block,entry)) {isEmpty=false;break;}
                //this entry has no blocks reserved, so lets clean it from the keytable
                if (isEmpty) eraseKeytableEntry(entry);
            }
    }
    for (int block=0;block<SBMAXBLOCKS;block++) {
            int blockentry = eeprom_read_byte((uint16_t*)(SBMAXKEYS*SBKEYSIZE+block));
            if (blockentry&0x80) {
                    blockentry &= 0x7F;
                    bool isEmpty=true;
                    for (int key=0;key<SBMAXKEYS;key++) {
                            if (eeprom_read_byte((uint16_t*)(key*SBKEYSIZE))) {isEmpty=false;break;}
                    }
                    if (isEmpty) eeprom_write_byte((uint16_t*)(SBMAXKEYS*SBKEYSIZE+block),0);
            }
    }
}

char Cookie::readQueue() {
    char data=0;
    #ifndef POK_SIM
    int address;
    address = SBMAXKEYS*SBKEYSIZE+SBMAXBLOCKS+SBBLOCKSIZE*_block+_head%SBBLOCKSIZE;
    data=eeprom_read_byte((uint16_t*)address);
    #endif
    _head++;
    if (_head%SBBLOCKSIZE==0 && _head) {
            _block++;
            _block=findMyNextBlock();
    }
    return data;
}

void Cookie::writeQueue(char data) {
    #ifndef POK_SIM
    eeprom_write_byte((uint16_t*)(SBMAXKEYS*SBKEYSIZE+SBMAXBLOCKS+SBBLOCKSIZE*_block+_head%SBBLOCKSIZE),data);
    #endif
    _head++;
    if (_head%SBBLOCKSIZE==0 && _head) {
            _block++;
            _block=findMyNextBlock();
    }
}

int Cookie::findMyNextBlock() {
    if (!_status) return SBINVALIDBLOCK;
    for (int i=_block; i<SBMAXBLOCKS;i++) if (isMyBlock(i)) return i;
}


