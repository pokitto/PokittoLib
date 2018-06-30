/*
   This file provides functions to load/save
   zBoy parameters from/to the configuration
   file.

   This file is part of the zBoy project.

   Copyright (C) Mateusz Viste 2010-2015
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h> /* strcat() */
#include "binary.h" /* provides convenience bxXXXXXXXX notations */
#include "drv.h"
#include "zboystructs.h"



/* This routine checks the consistency of a zBoy configuration, and fixes it if needed */
void zboy_fixconfig(struct zboyparamstype *zboyconf) {
  if ((zboyconf->ShowFPS != 0) && (zboyconf->ShowFPS != 1)) zboyconf->ShowFPS = 0;
  if ((zboyconf->ShowRomInfos != 0) && (zboyconf->ShowRomInfos != 1)) zboyconf->ShowRomInfos = 0;
  if ((zboyconf->NoSpeedLimit != 0) && (zboyconf->NoSpeedLimit != 1)) zboyconf->NoSpeedLimit = 0;
  if ((zboyconf->HiScoresMem != 0) && (zboyconf->HiScoresMem != 1)) zboyconf->HiScoresMem = 0;
  if ((zboyconf->NoCpuIdle != 0) && (zboyconf->NoCpuIdle != 1)) zboyconf->NoCpuIdle = 0;
  if ((zboyconf->fpslimit < 1) || (zboyconf->fpslimit > 60)) zboyconf->fpslimit = 60;
  if ((zboyconf->palette != 0) && (zboyconf->palette != 1)) zboyconf->palette = 0;
  if ((zboyconf->colorize != 0) && (zboyconf->colorize != 1)) zboyconf->colorize = 1;
  zboyconf->scalingalgo = REFRESHSCREEN_NOSCALE;
  zboyconf->GraphicScaleFactor = 1;
}


void zboy_loaddefaultconfig(struct zboyparamstype *zboyparams) {
  /* Set zBoy parameters to default values */
  memset(zboyparams, 0, sizeof(struct zboyparamstype));
  zboyparams->HiScoresMem = 0;
  zboyparams->scalingalgo = REFRESHSCREEN_NOSCALE;  /* By default, use Scale2x scaling... */
  zboyparams->GraphicScaleFactor = 1;
  zboyparams->fpslimit = 60;
  zboyparams->colorize = 1;

  /* Here I am preloading a default configuration for input keys mapping */
  zboyparams->key_up = DRV_INPUT_KEY_UP;
  zboyparams->key_down = DRV_INPUT_KEY_DOWN;
  zboyparams->key_left = DRV_INPUT_KEY_LEFT;
  zboyparams->key_right = DRV_INPUT_KEY_RIGHT;
  zboyparams->key_start = DRV_INPUT_KEY_RET;
  zboyparams->key_select = DRV_INPUT_KEY_TAB;
  zboyparams->key_a = DRV_INPUT_KEY_LCTRL;
  zboyparams->key_b = DRV_INPUT_KEY_LALT;
  zboyparams->key_turboa = DRV_INPUT_NONE;
  zboyparams->key_turbob = DRV_INPUT_NONE;
  zboyparams->key_bckg = DRV_INPUT_KEY_F1;
  zboyparams->key_sprt = DRV_INPUT_KEY_F2;
  zboyparams->key_wind = DRV_INPUT_KEY_F3;
  zboyparams->key_save = DRV_INPUT_KEY_F5;
  zboyparams->key_load = DRV_INPUT_KEY_F7;
  zboyparams->key_asht = DRV_INPUT_KEY_F8;
  zboyparams->key_shot = DRV_INPUT_KEY_F9;
  zboyparams->key_rset = DRV_INPUT_KEY_F10;
  zboyparams->key_quit = DRV_INPUT_KEY_ESC;
}


int zboy_loadconfig(struct zboyparamstype *zboyconf){}
int zboy_saveconfig(struct zboyparamstype *zboyconf){}
