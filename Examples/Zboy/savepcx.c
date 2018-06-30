/*
 * Saves the content of ScreenBuffer to OutFile as a 160x144 PCX graphic file.
 * The PCX file is a 16-color, 1-bit, 4-planes image.
 *
 * This file is part of the zBoy emulator project.
 * Copyright (C) Mateusz Viste 2010,2011,2012,2013,2014,2015
 *
 * Note: I could optimise PCX snapshots even further, by limiting the number
 *       of planes when possible (ie. 2 planes for 4 colors and 3 planes for
 *       8 colors), but I found out that modern image viewers usually don't
 *       know how to read such variants. This is why I decided to stick to the
 *       commonly-supported 1bpp x 4 planes model all the time.
 */

#include <stdint.h>
#include <stdio.h>

void SavePcxFile(char *outfile) {
}
