/*
    Serial link emulation
    This file is part of the zBoy project
    Copyright (C) Mateusz Viste 2010-2015


    FF02 is the serial control address
      Bit 7 - if set, the game wants to interact with serial link
      Bit 0 - Clock source selection (0 = external   1 = internal)

    FF01 is the serial data buffer (for in & out operations)

 Timing: When trigerring a serial transfer with an internal clock, the serial INT will occur 976 us later.
         A GameBoy is running at 4194304 CPU cycles/s
         1 CPU cycle = 0.0002384185791015625 ms = 0.2384185791015625 us
         So a serial INT should occur after 4093.64 CPU cycles.
*/

#include <unistd.h>

#include "net.h"


inline void CheckSerialLink(int cycles) {

  if (NetPlay == 0) {  /* No net player - just fakes a Serial GB interface which is connected nowhere */
      if ((IoRegisters[0xFF02] & bx10000000) != 0) {    /* The game tries to send some stuff out */
        if ((IoRegisters[0xFF02] & 1) != 0) {  /* If external clock is selected, ignore, otherwise let's see */
          /*printf("%08d cycles: %c\n", TotalCycles, IoRegisters[0xFF01]); */ /* for DEBUG purpose, you can print out to console */
          IoRegisters[0xFF01] = 0xFF;    /* FF means no data has been received */
          INT(INT_SERIAL);               /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
          IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */
        }
      }
    } else {   /* Net player... */

#ifndef NETPLAY
      cycles = cycles;  /* this one just to avoid a gcc warning when NETPLAY is not defined */
#else
      static uint8_t lastbytesent = 0;
      static uint8_t lastseqsent = 0;
      static uint8_t nextseqrecv = 1;
      static uint32_t waitingsince = 0;
      static uint8_t databuff[2];
      static uint8_t datasent = 0;
      if (datasent != 0) {
        /* If I don't get any answer for some time, let's resend the last data */
        if (waitingsince >= 4094) { /* Now I should have got the expected answer */
          waitingsince = 0; /* reset the waitingsince timer */
          databuff[0] = lastseqsent;
          databuff[1] = lastbytesent;
          net_send(databuff, 2);
          /* printf("Retransmission of seq %u\n", lastseqsent); */
        }
      }
      if ((IoRegisters[0xFF02] & bx10000000) != 0) {    /* The game tries to send some stuff out */
        if (((IoRegisters[0xFF02] & 1) != 0) || (datasent == 1)) { /* Internal serial clock - we send data first */
            if (datasent == 0) {
              lastseqsent += 1;
              lastbytesent = IoRegisters[0xFF01];
              databuff[0] = lastseqsent;
              databuff[1] = lastbytesent;
              // if (
	      net_send(databuff, 2);
		    //  != 2) printf("send() problem");
              datasent = 1;
            }
            if (net_recvpeek(databuff, 2) > 0) {
                net_recv(databuff, 2); /* consume the data from socket */
                if (databuff[0] == nextseqrecv) { /* is it the right seq? If not, we ignore it */
                    nextseqrecv += 1;
                    datasent = 0;
                    IoRegisters[0xFF01] = databuff[1];
                    waitingsince = 0;
                    INT(INT_SERIAL);                  /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
                    IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */
                  } else {
                    /* printf("seq out of order! (int clock)\n"); */
                    waitingsince += cycles;
                }
              } else {
                  waitingsince += cycles;
            }
          } else { /* External clock - we are polling for data */
            if (net_recvpeek(databuff, 2) == 2) {
                if (databuff[0] == nextseqrecv) { /* is it the right seq? If not, we ignore it */
                    waitingsince = 0; /* Reset the waitingsince timer */
                    lastseqsent += 1;
                    nextseqrecv += 1;
                    lastbytesent = IoRegisters[0xFF01];
                    databuff[0] = lastseqsent;
                    databuff[1] = lastbytesent;
                    datasent = 2;
                    // if (
		    net_send(databuff, 2);
		    // != 2) printf("send() problem");
                    // if (
		    net_recv(databuff, 2);
		    // != 2) printf("recvfrom() problem\n");
                    IoRegisters[0xFF01] = databuff[1];
                    INT(INT_SERIAL);                     /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
                    IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */
                  } else {
		  // if (
		  net_recv(databuff, 2);
		  // != 2) printf("recvfrom() problem\n"); /* trash received data */
                    /* printf("seq out of order! (ext clock)\n"); */
                    waitingsince += cycles;
                }
              } else {
                waitingsince += cycles;
            }
        }
      }
#endif
  }
}
