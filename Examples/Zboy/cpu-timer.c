/*
    Z80 Timer emulation
    This file is part of the zBoy project.
    Copyright (C) Mateusz Viste 2010,2011,2012,2013


   FF05 - TIMA - Timer counter (R/W)
          This timer is incremented by a clock frequency
          specified by the TAC register ($FF07). When the
          value overflows (gets bigger than FFh) then it
          will be reset to the value specified in TMA
          (FF06), and an interrupt will be requested.

   FF07 - TAC - Timer Control (R/W)
          Bit 2 - Timer Stop
            0: Stop Timer
            1: Start Timer
          Bits 1+0 - Input Clock Select
            00: 4.096 KHz
            01: 262.144 Khz
            10: 65.536 KHz
            11: 16.384 KHz
*/


unsigned int TimerC = 0;     /* Timer counter */


static inline void CheckTIMA(void) {
  if (IoRegisters[0xFF05] == 255) {
      IoRegisters[0xFF05] = IoRegisters[0xFF06];
      INT(INT_TIMA);  /* Request an interrupt via bit 2 of the IF (if bit 2 enabled in IE) */
    } else {
      IoRegisters[0xFF05] += 1;
  }
}

const int TIMMAX[] = {1024, 16, 64, 256};

inline void uTimer(unsigned int cycles) {  /* Timer emulation */
  if ((IoRegisters[0xFF07] & bx00000100) != 0) {   /* test TAC bit 2 (start/stop timer) */
    TimerC += cycles;

    int max = TIMMAX[ IoRegisters[0xFF07] & bx00000011 ];
    if( TimerC >= max ){
      TimerC -= max;
      CheckTIMA();
    }
  }
}
