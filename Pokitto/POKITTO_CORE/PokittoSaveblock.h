/**************************************************************************/
/*!
    @file     PokittoSaveblock.h
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

#ifndef POKITTO_SAVEBLOCK_H
#define POKITTO_SAVEBLOCK_H

#include "Pokitto_settings.h"
#include <stdint.h>

#define SBNOMOREKEYS 1
#define SBNOBLOCKSFREE 2

namespace Pokitto {

/** Pokitto Saveblocks are 32 byte blocks that are saved in the EEPROM in a safe way
 *
 * Example:
 * @code
 * // Create saveable data and initialize your app to use saveblocks
 *
 *   class gdata : public Saveblock {
 *   int highscore;
 *   }
 *
 *   gdata gamedata;
 *
 *   gamedata.begin("ASTEROCK"); // register your saveblock with key "ASTEROCK"
 *
 *   // you use the data objects inside the saveblock as normal:
 *
 *   gamedata.highscore = player.currentscore; // It's a new highscore!
 *
 *   // Finally you call the save method to write your data to EEPROM
 *
 *   gamedata.save;
 * @endcode
 */

class Saveblock {
public:
    /** Saveblock class constructor
    */
    Saveblock();

    /** begin - Register your saveblock with a 8-byte key to begin using it
     *
     *  @param 8-byte key string
     *
     *  @returns
     *       0 on success (free blocks available),
     *   non-0 on failure (no more free keys/blocks)
     */
    static int begin(const char*);

    /** save - Save your saveblock
     *
     *  @returns
     *       0 on success (saved successfully and verified),
     *   non-0 on failure (something is wrong)
     */
    static int save();

    /** isOK - Get status of saveblock
     *
     *  @returns
     *       true on success (Saveblock is initialized and ready to use),
     *      false on failure (Saveblock did not initialize and can't be used)
     */

    static bool isOK();

    /** getKeyAt - return the key at slot n
     *
     *  @param slot number to check
     *
     *  @returns
     *      8 byte key
     */
    static void getKeyAt(int, char*);


    /** formatKeyTable(int) - erase all keys
     *
     *  @param slot number to erase
     *
     */
    static void formatKeyTable();


    /** eraseKeyAt(int) - erase key at slot n
     *
     *  @param slot number to erase
     *
     */
    static void eraseKeyAt(int);

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
    static int exists(const char*);

    /** getFreeKeySlot - Are there any keys left to use
     *
     *  @returns
     *      slot number 0...47 on success
     *      -1 on failure
     */
    static int getFreeKeySlot();

    /** getFreeBlocks - Are there any storage blocks left to use
     *
     *  @returns
     *      number of blocks available
     */
    static int getFreeBlocks();

    /** isFreeBlock - check if block n is free to use
     *
     *  @param block number
     *  @returns
     *      true when free
     *      false when reserved
     */
    static bool isFreeBlock(int);


    /** writeKey - write the key into the key table
     *
     *  @param 8-byte key string
     *  @param slot number into which the key is written 0...47
     *
     */
    static void writeKey(const char*, int);


    /** keystring
    * identification string for the saveblock
    */
    static char _key[];

    /** Keyorder
    * order number of key in key table
    */
    static char _keyorder;

    /** Status
    * false = uninitialized
    * true = ready
    */
    static bool _status;

};


} // namespace


#endif // POKITTO_SAVEBLOCK_H
