/*
   This file is part of the zBoy project
   Copyright (C) Mateusz Viste 2010

   Divider register (DIV) emulation

   This register is very similar to the Timer register (TIMA), only much easier. The divider register (DIV) is
   located at memory address 0xFF04 and one of its uses is to supply a seed for a random number generator in
   games. You can test this by not emulating the divider register and then playing tetris. You will notice that
   the only object you are given in tetris is the square block which makes the game a bit easy. The register
   increments itself through values of 0-255 and starts back at 0 again when going one higher than 255. You
   need to intercept the write to the DIV address. When the game attempts to write to DIV then DIV is set to 0.

   The DIV register is incrementing 16384 times a second (that is, every 256 clock cycles)
*/

int DividerCycleCounter = 255;

void incDivider(int cpuCyclesDone) {
  if (DividerCycleCounter < cpuCyclesDone) {
      if (IoRegisters[0xFF04] == 255) {
          IoRegisters[0xFF04] = 0;
        } else {
          IoRegisters[0xFF04] += 1;
      }
      DividerCycleCounter += (256 - cpuCyclesDone);
    } else {
      DividerCycleCounter -= cpuCyclesDone;
  }
}
