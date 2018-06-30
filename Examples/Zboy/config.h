/*
 * This file provides functions to load/save zBoy parameters from/to the
 * configuration file.
 *
 * This file is part of the zBoy project.
 *
 * Copyright (C) Mateusz Viste
 */

#ifndef ZBOY_CONFIGH_SENTINEL
  #define ZBOY_CONFIGH_SENTINEL
  void zboy_fixconfig(struct zboyparamstype *zboyconf);
  void zboy_loaddefaultconfig(struct zboyparamstype *zboyconf);
  int zboy_loadconfig(struct zboyparamstype *zboyconf);
  int zboy_saveconfig(struct zboyparamstype *zboyconf);
#endif
