/* generated automatically by colorize/compile.sh -- do not modify! */

void colorize(uint32_t crc, uint32_t *screenpalette) {
  uint32_t newpal[16];
  switch (crc) {
    case 0xC1F88833:
      { const uint32_t pal[16] = {0xc7caff,0x686fff,0x000cff,0x000020,0x00FFFF,0x00FFFF,0x00FFFF,0x00FFFF,0xc15200,0x7a3b0d,0x9d4c11,0x000000,0xFF00FF,0xAA00AA,0x550055,0x100010};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x6C742478:
      { const uint32_t pal[16] = {0xDE6800,0xA0A0B0,0xB82100,0x950000,0x000000,0x0000FF,0x555555,0xCECECE,0xE3E2CC,0xA0A0B0,0xA74925,0x000000,0xF0E000,0x00FF00,0xE83020,0x101010};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x0509069C:
    case 0xB681E243:
      { const uint32_t pal[16] = {0xf0f0f0,0xE00000,0x5757ff,0x000080,0xF1F335,0xab6434,0xff8303,0x202020,0xFF00FF,0xF1F335,0xF05703,0x101010,0xFF00FF,0xD0D0D0,0x00FF00,0x2A2AF3};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x79804305:
      { const uint32_t pal[16] = {0xF0F0F0,0x00AA00,0x005500,0x000000,0x00FFFF,0x00FFFF,0x00FFFF,0x00FFFF,0x00FF00,0x00AA00,0x005500,0x002000,0xFF4040,0xAA3030,0x552020,0x100000};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0xF59CEDEA:
      { const uint32_t pal[16] = {0xFFFFFF,0x40B040,0x118011,0x000000,0xffffff,0xaaaaaa,0x555555,0x000000,0xff00ff,0x5050ff,0x111180,0x000010,0xeeeeff,0xff00ff,0x40B040,0x40B06D};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x35A5234A:
      { const uint32_t pal[16] = {0xe0e0ff,0x7070ba,0x202060,0x000000,0xff00ff,0xaa00aa,0x550055,0xff00ff,0xffe0e0,0xaa8080,0x552020,0x000000,0xff00ff,0xff00ff,0xff00ff,0xff00ff};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x90776841:
    case 0x2C27EC70:
      { const uint32_t pal[16] = {0xE3E2CC,0xC1AE6B,0x6D3624,0x000000,0xFFFFC0,0xFF5050,0x555555,0x101010,0x00F0F0,0xDB7636,0xA74925,0x101010,0x0000FF,0x0000AA,0x000055,0x0000F0};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0xB76C769B:
      { const uint32_t pal[16] = {0xf0f0f0,0x34c430,0xdb5228,0x101010,0xF0F0F0,0x0da010,0x69d46b,0x202020,0x00FFFF,0x5d7cc9,0x303030,0x000000,0x9a5628,0x303030,0xFF00FF,0x000000};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    case 0x63F9407D:
    case 0x46DF91AD:
      { const uint32_t pal[16] = {0xE3E2CC,0x000000,0xA24420,0x001000,0xFFFFC0,0xFF5050,0x555555,0x101010,0xE3E2CC,0x2549A7,0xA74925,0x001000,0x00FF00,0x00FF00,0x00FF00,0x00FF00};
      memcpy(newpal, pal, sizeof(newpal));
      break; }
    default: return;
  }
  screenpalette[0] = newpal[0];    /* background */
  screenpalette[1] = newpal[1];    /* background */
  screenpalette[2] = newpal[2];    /* background */
  screenpalette[3] = newpal[3];    /* background */
  screenpalette[32] = newpal[4];   /* window */
  screenpalette[33] = newpal[5];   /* window */
  screenpalette[34] = newpal[6];   /* window */
  screenpalette[35] = newpal[7];   /* window */
  screenpalette[64] = newpal[8];   /* OBJ0 */
  screenpalette[65] = newpal[9];   /* OBJ0 - mario skin */
  screenpalette[66] = newpal[10];  /* OBJ0 - mario pants */
  screenpalette[67] = newpal[11];  /* OBJ0 */
  screenpalette[128] = newpal[12]; /* OBJ1 */
  screenpalette[129] = newpal[13]; /* OBJ1 */
  screenpalette[130] = newpal[14]; /* OBJ1 */
  screenpalette[131] = newpal[15]; /* OBJ1 */
}
