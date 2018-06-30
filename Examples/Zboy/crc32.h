/*
    Returns the CRC32 checksum of message.
    Don't forget to set nBytes to the actual size
    of the message buffer.
*/

#include <stdint.h>

#ifndef ZBOY_CRC32_SENTINEL
  #define ZBOY_CRC32_SENTINEL
  uint32_t getCRC32(uint8_t *message, int nBytes);
#endif
