/*
   Adjust emulation timing
   Part of the zBoy project
   Copyright (C) Mateusz Viste 2010,2011,2012,2013
*/

unsigned long long TotalCycles;  /* 64bit. Can go up to 18446744073709551615 (@4MHz = 139461 years) */

inline void AdjustTiming(struct zboyparamstype *zboyparams) {
  return;
  
  static unsigned int t, t1 = 0, framecounter = 0, t3 = 0;
  /*  Note: GameBoy speed is 4194304 CPU cycles/s  */
  if (framecounter == 2) { /* every 3 frames sync the time against t3 (3 frames = 50ms) */
      framecounter = 0;
      for (t = drv_getticks(); (t - t3) < 50; t = drv_getticks())
	if (zboyparams->NoCpuIdle == 0) drv_delay(1);
      t3 = t;
    } else { /* else sync the time against t1 (1 frame = 16.66666...ms) */
      framecounter++;
      for (t = drv_getticks(); (t - t1) < 16; t1 = drv_getticks()) if (zboyparams->NoCpuIdle == 0) drv_delay(1);
  }
  t1 = t;
}
