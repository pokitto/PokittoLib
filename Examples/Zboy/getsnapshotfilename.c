/*
 * This file is part of the zBoy project.
 * computes filenames for snapshot files.
 */

#include <sys/time.h>

inline void getsnapshotfilename(char *stringtoset) {
  time_t t;
  time(&t);
  #ifdef LFNAVAIL
  sprintf(stringtoset, "%s_%X.pcx", RomInfos.ShortFilenameNoExt, (unsigned int) t);
  #else
  sprintf(stringtoset, "%X.pcx", (unsigned int) t);
  #endif
}
