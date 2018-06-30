/*
    Serial link emulation
    This file is part of the zBoy project
    Copyright (C) Mateusz Viste 2010


    FF02 is the serial control address
      Bit 7 - if set, the game wants to interact with serial link
      Bit 0 - Clock source selection (0 = external   1 = internal)

    FF01 is the serial data buffer (for in & out operations)

 Timing: When trigerring a serial transfer with an internal clock, the serial INT will occur 976 us later.
         A GameBoy is running at 4194304 CPU cycles/s
         1 CPU cycle = 0.0002384185791015625 ms = 0.2384185791015625 us
         So a serial INT should occur after 4093.64 CPU cycles.
*/

#include <arpa/inet.h>
#include <unistd.h>

//#define printf(...)

struct sockaddr_in si_me, si_other;
int s;
socklen_t slen = sizeof(si_other);


#define WaitForData() \
for (;;) { \
            if (recvfrom(s, databuff, 2, MSG_DONTWAIT, (struct sockaddr *) &si_other, &slen) > 0) { \
                /* if (databuff[0] == nextseqrecv) { */ /* is it the right seq? If not, we ignore it */ \
                    nextseqrecv += 1; \
                    datasent = 0; \
                    IoRegisters[0xFF01] = databuff[1]; \
                    waitingsince = 0; \
                    /* if ((MemoryInternalHiRAM[0xFFFF] & bx00001000) != 0) IoRegisters[0xFF0F] |= bx00001000; */  /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */ \
                    IoRegisters[0xFF0F] |= bx00001000;   /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */ \
                    IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */ \
                    printf("got seq #%u\n", databuff[0]); \
                    break; \
                /*  } else { */ \
                /*    printf("got invalid seq (expected %u, but got %u)!\n", nextseqrecv, databuff[0]); */ \
                /* } */ \
            } \
            /* if (sendto(s, databuff, 2, 0, (struct sockaddr *) &si_other, slen) != 2) printf("send() problem"); */ \
}



/*#MACRO CheckSerialLink() */
inline void CheckSerialLink(int cycles) {

  if (NetPlay == 0) {  /* No net player - just fakes a Serial GB interface which is connected nowhere */
      if ((IoRegisters[0xFF02] & bx10000000) != 0) {    /* The game tries to send some stuff out */
        if ((IoRegisters[0xFF02] & 1) != 0) {  /* If external clock is selected, ignore, otherwise let's see */
          /*printf("%08d cycles: %c\n", TotalCycles, IoRegisters[0xFF01]); */ /* for DEBUG purpose, you can print out to console */
          IoRegisters[0xFF01] = 0xFF;    /* FF means no data has been received */
          IoRegisters[0xFF0F] |= bx00001000;   /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
          IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */
        }
      }
    } else {   /* Net player... */

      static uint8_t lastbytesent = 0;
      static uint8_t lastseqsent = 0;
      static uint8_t nextseqrecv = 1;
      static uint32_t waitingsince = 0;
      static uint8_t databuff[2];
      static uint8_t datasent = 0;
      static uint8_t lastmode = 8;

      if ((IoRegisters[0xFF02] & bx10000000) == 0) {    /* The game do not try to send some stuff out */
        while (recvfrom(s, databuff, 2, MSG_DONTWAIT, (struct sockaddr *) &si_other, &slen) >= 0); /* purge buffer */
      } else {
        if (lastmode != (IoRegisters[0xFF02] & 1)) { /* The game switched serial link clock mode (external/internal) */
          waitingsince = 0;
          datasent = 0;
          //printf("Switched serial link mode\n");
          lastmode = (IoRegisters[0xFF02] & 1);
        }

        if ((IoRegisters[0xFF02] & 1) != 0) { /* Internal serial clock - we send data first */
            if (datasent == 0) {
                lastseqsent += 1;
                lastbytesent = IoRegisters[0xFF01];
                databuff[0] = lastseqsent;
                databuff[1] = lastbytesent;
                if (sendto(s, databuff, 2, 0, (struct sockaddr *) &si_other, slen) != 2) printf("send() problem");
                printf("sent seq #%u (internal clock)\n", lastseqsent);
                datasent = 1;
                waitingsince = 0;
              } else if (waitingsince >= 4094) {
                printf("Waiting for seq #%u on serial link (internal clock)\n", nextseqrecv);
                WaitForData();
              } else {
                waitingsince += cycles;
            }
          } else { /* External clock - we are polling for data */
            if (waitingsince == 0) {
                /* datasent = 1; */
                printf("Awaiting for seq #%u on serial link (external clock)\n", nextseqrecv);
                waitingsince = 1; /* Reset the waitingsince timer */
                                      /* Now let's send our stuff */
                      lastseqsent += 1;
                      lastbytesent = IoRegisters[0xFF01];
                      databuff[0] = lastseqsent;
                      databuff[1] = lastbytesent;
                      if (sendto(s, databuff, 2, 0, (struct sockaddr *) &si_other, slen) != 2) printf("send() problem");
                      printf("sent seq #%u (external clock)\n", lastseqsent);
                      datasent = 1;

              } else if (waitingsince >= 4094) {
                if (recvfrom(s, databuff, 2, MSG_DONTWAIT, (struct sockaddr *) &si_other, &slen) > 0) {
                  //if (databuff[0] == nextseqrecv) { /* is it the right seq? If not, we ignore it */
                      printf("Got seq #%u (external clock)\n", databuff[0]);
                      nextseqrecv += 1;
                      datasent = 0;
                      IoRegisters[0xFF01] = databuff[1];
                      waitingsince = 0;
                      /* if ((MemoryInternalHiRAM[0xFFFF] & bx00001000) != 0) IoRegisters[0xFF0F] |= bx00001000; */  /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
                      IoRegisters[0xFF0F] |= bx00001000;   /* When serial handling done, request serial interrupt (INT 58), via bit 3 of FF0F */
                      IoRegisters[0xFF02] &= bx01111111;   /* Reset the bit 7 of FF02 (see page 31 of GameBoy CPU manual) */
                      /* Now let's send our stuff */
                      //lastseqsent += 1;
                      //lastbytesent = IoRegisters[0xFF01];
                      //databuff[0] = lastseqsent;
                      //databuff[1] = lastbytesent;
                      //if (sendto(s, databuff, 2, 0, (struct sockaddr *) &si_other, slen) != 2) printf("send() problem");
                      //printf("sent seq #%u (external clock)\n", lastseqsent);
                  //  } else {
                  //    printf("got invalid seq (expected %u, but got %u)!\n", nextseqrecv, databuff[0]);
                  //}
                }

              } else {
                waitingsince += cycles;
            }

        }
      }
  }
}
/*#ENDMACRO */
