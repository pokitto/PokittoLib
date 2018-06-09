

/*
 * BMP image as 4bpp (16 colour index) data
 */

#include <stdint.h>
#include "Animation.h"

const uint16_t m_lids_smug_pal[] = {
0,65535,38918,62943,950,1375,26207,2440,48893,59358,38905,34615,30260,40922,51196,21485
};

const uint8_t m_lids_smug[] = {
104,24,
170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,
170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,
170,170,170,170,170,170,170,170,221,173,221,221,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,218,221,221,218,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,221,170,170,170,170,170,173,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,221,218,170,170,170,170,170,221,218,170,170,170,170,170,170,170,170,170,
170,170,170,170,170,221,218,170,170,170,170,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,221,170,170,170,170,170,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,218,186,170,170,170,170,170,170,170,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,173,171,170,170,170,170,170,170,170,
170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,171,186,170,170,170,170,170,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,186,170,170,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,170,170,170,170,170,170,170,170,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,170,170,170,
170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,170,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,170,170,170,170,170,170,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,170,170,
170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,170,170,170,170,170,173,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,187,170,170,170,170,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,186,170,170,170,
170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,186,170,170,170,170,170,170,173,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,187,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,170,170,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,187,170,170,170,
173,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,203,170,170,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,188,186,170,170,221,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,203,170,170,170,170,170,173,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,186,170,170,
218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,188,186,170,170,170,170,173,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,203,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,172,186,170,170,170,170,173,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,203,170,170,
170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,172,203,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,204,186,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,219,203,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,188,186,170,
170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,173,221,221,171,203,170,170,170,170,218,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,188,186,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,203,186,170,170,170,218,221,221,221,221,221,221,221,221,221,221,170,170,170,170,170,170,170,170,170,170,170,172,187,170,

};
