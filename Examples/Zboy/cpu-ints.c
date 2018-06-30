/*
 * This file is part of the zBoy project.
 * Copyright (C) Mateusz Viste 2013-2015
 *
 * Provides defines to handle interrupts.
 *
 * Note: When an interrupt condition is met (for ex. VBLANK, or TIMA overflow, etc), then the hardware
 *       checks whether the interrupt for this condition is ENABLED or not, by looking into the IE
 *       register. If the interrupt is ENABLED, then it gets REQUESTED, by setting the same flag in
 *       the IF register.
 *       However, the interrupt will be HANDLED only when/if the IME is enabled. Once the interrupt
 *       is HANDLED (ie. PC is set to proper vector), the interrupt is ACKNOWLEDGED (reset the flag of
 *       the IF register).
 */

enum ints {
  INT_JOYPAD = bx00010000,
  INT_SERIAL = bx00001000,
  INT_TIMA   = bx00000100,
  INT_LCDC   = bx00000010,
  INT_VBLANK = bx00000001
};


/* Int caller: Request an interrupt via IF, but ONLY if it is enabled on IE. */
#define INT(x) IoRegisters[0xFF0F] |= (MemoryInternalHiRAM[0xFFFF] & x);
