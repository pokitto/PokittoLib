/*
 * This file is part of the zBoy project.
 * Copyright (C) Mateusz Viste 2010,2011,2012,2013,2014,2015
 * /

void PressAnyKey(void) {
  int event;
  for (;;) {
    event = drv_keypoll();
    if (event == DRV_INPUT_NONE) {
      drv_delay(100);
      drv_refreshscreen();
      continue;
    }
    if (event == DRV_INPUT_QUIT) exit(0);
    break;
  }
}
/**/
