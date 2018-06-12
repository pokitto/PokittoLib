/**************************************************************************/
/*!
    @file     PokittoCookie.h
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

#ifndef POKITTO_COOKIE_H
#define POKITTO_COOKIE_H

#include "Pokitto_settings.h"
#include <stdint.h>

#define SBNOMOREKEYS 1
#define SBNOTENOUGHBLOCKSFREE 2
#define SBMAXKEYS 48 //number of available keys
#define SBMAXBLOCKS 112 //0xF8C (settings reserved area) - 0x180 (start of block table) / 32 bytes (block size)
#define SBBLOCKSIZE 32 //block size in bytes
#define SBKEYSIZE 8
#define SBINVALIDSLOT 255
#define SBINVALIDBLOCK -1

namespace Pokitto {

/** Pokitto Cookies are queues of 32 byte blocks that are saved in the EEPROM in a safe way
 *
 * Example:
 * @code
 * // Create saveable data and initialize your app to use Cookies
 *
 *   class gdata : public Cookie {
 *   int highscore;
 *   }
 *
 *   gdata gamedata;
 *
 *   gamedata.begin("ASTEROCK"); // register your Cookie with key "ASTEROCK"
 *
 *   // you use the data objects inside the Cookie as normal:
 *
 *   gamedata.highscore = player.currentscore; // It's a new highscore!
 *
 *   // Finally you call the save method to write your data to EEPROM
 *
 *   gamedata.save;
 * @endcode
 */

class Cookie {
public:
    /** Cookie class constructor
    */
    Cookie();

    /** begin - Register your Cookie with a 8-byte key to begin using it
     *
     *  @param 8-byte key string
     *  @param size of cookie data in bytes
     *  @param pointer to beginning of cookie data in memory
     *
     *  @returns
     *       0 on success (free blocks available),
     *   non-0 on failure (no more free keys/blocks)
     */
     int begin(const char*, int, char*);


    /** begin - Register your Cookie with a 8-byte key to begin using it
     *
     *  @param 8-byte key string
     *  @param reference to cookie data object in memory
     *
     *  @returns
     *       0 on success (free blocks available),
     *   non-0 on failure (no more free keys/blocks)
     */
     template< typename T >
     int begin(const char * key, T & object) {
       return begin(key, sizeof(T), reinterpret_cast<char *>(&object));
     }

    /** initialize - create cookie structure. Can be called many times
     *  @returns
     *       0 on success (free blocks available),
     *   non-0 on failure (no more free keys/blocks)
     */
     int initialize();

    /** saveCookie - Save your Cookie
     *
     *  @returns
     *      true on success (saved successfully and verified),
     *      false on failure (something is wrong)
     */
     bool saveCookie();

     /** loadCookie - Load your Cookie
     *
     *  @returns
     *      true on success
     *      false on failure
     */
     bool loadCookie();

     /** deleteCookie -  your Cookie
     */
     void deleteCookie();

    /** isOK - Get status of Cookie
     *
     *  @returns
     *       true on success (Cookie is initialized and ready to use),
     *      false on failure (Cookie did not initialize and can't be used)
     */
     bool isOK();

    /** readKeytableEntry - return the key at slot n
     *
     *  @param slot number to check
     *  @param pointer to a 9 char buffer for return answer
     *
     */
     void readKeytableEntry(int, char*);


    /** formatKeyTable(int) - erase all keys
     *
     *  @param slot number to erase
     *
     */
     void formatKeytable();


    /** eraseKeyAt(int) - erase key at slot n
     *
     *  @param slot number to erase
     *
     */
     void eraseKeytableEntry(int);

     /** cleanKeytable() - erase keys that have no blocks reserved from the keyTable
     *
     */
     void cleanKeytable();

//private:
public:

    /** exists - find out if the key exists
     *
     *  @param 8-byte key string
     *
     *  @returns
     *       slotnumber on key exists ,
     *       SBINVALIDSLOT on does not exist
     */
     int exists(const char*);

    /** getFreeKeytableSlot - Are there any keys left to use
     *
     *  @returns
     *      slot number 0...47 on success
     *      -1 on failure
     */
     int getFreeKeytableSlot();

    /** getFreeBlocks - Are there any storage blocks left to use
     *
     *  @returns
     *      number of blocks available
     */
     int getFreeBlocks();

    /** getAssignedBlocks - return number of blocks already reserved for cookie
     *
     *  @returns
     *      number of blocks assigned to cookie
     */
     int getAssignedBlocks();


    /** isFreeBlock - check if block n is free to use
     *
     *  @param block number
     *  @returns
     *      true when free
     *      false when reserved
     */
     bool isFreeBlock(int);

    /** isMyBlock - check if block n is already reserved for this cookie
     *
     *  @param block number
     *  @returns
     *      true when is already reserved
     *      false when is not
     */
     bool isMyBlock(int);

    /** blockIsOwnedBy - check if block n is owned by Keytable entry k
     *
     *  @param block number
     *  @param keytable entry number
     *  @returns
     *      true when keytable entry k is owner of block n
     *      false when is not
     */
     bool blockIsOwnedBy(int,int);

    /** writeKeyToKeyTable - write the key into the key table
     *
     *  @param 8-byte key string
     *  @param slot number into which the key is written 0...47
     *
     */
     void writeKeyToKeytable(const char*, int);


    /** getBlockTableEntry - read an entry from the blocktable
     *
     *  @param block table index number
     *
     *  @returns
     *         block table entry (1 byte)
     */
     char getBlockTableEntry(int);

     /** readBlock - read the data of a block
     *
     *  @param block index number
     *  @param pointer to a buffer of SBBLOCKSIZE size to hold the return data
     *
     */
     void readBlock(int, char*);

     /** freeBlock - free block from blocktable and delete data of block in EEPROM
     *
     *  @param block index number
     *
     */
     void freeBlock(int);

     /** reserveBlock - search and reserve a block from blocktable for use by this cookie
     *
     *  @returns
     *      true when it was possible to reserve a block
     *      false when it was not
     *
     */
     bool reserveBlock();

    /** writeQueue - write the cookie data as a stream of bytes into the EEPROM blocks
     *
     *  @param byte to be written
     */
     void writeQueue(char);

    /** readQueue - write the cookie data as a stream of bytes into the EEPROM blocks
     *
     *  @returns
     *      byte value from EEPROM memory
     */
     char readQueue();


    /** findMyNextBlock - find the next block assigned to this cookie
     *
     *  @returns
     *      number of next block
     */
     int findMyNextBlock();

    /** keystring
    * identification string for the Cookie
    */
     char _key[SBKEYSIZE];

    /** Keyorder
    * order number of key in key table
    */
     char _keyorder;

    /** Status
    * false = uninitialized
    * true = ready
    */
     bool _status;

    /** Datasize
    * size (in bytes) of cookie data to be saved and reloaded
    */
    int _datasize;

    /** Pointer
    * pointer to cookie data
    */
    char* _pointer;

    /** Head
    * data "head" for byte write/read operations
    */
    int _head;

    /** Current block
    * block number that we are reading/writing
    */
    char _block;
};


} // namespace


#endif // POKITTO_Cookie_H
