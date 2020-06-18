import upygame as pygame

# pokitto picture
w2 = 16
h2 = 16
pokittoPixels = [
0x00,0x03,0x33,0x33,0x33,0x33,0x33,0x00,
0x00,0x32,0x22,0x22,0x22,0x22,0x32,0x00,
0x00,0x32,0x33,0x33,0x33,0x33,0x22,0x00,
0x00,0x32,0x31,0x11,0x11,0x11,0x22,0x00,
0x00,0x32,0x31,0x13,0x11,0x31,0x22,0x00,
0x02,0x32,0x31,0x11,0x11,0x11,0x22,0x23,
0x03,0x32,0x31,0x13,0x33,0x11,0x22,0x30,
0x00,0x32,0x31,0x11,0x11,0x11,0x22,0x00,
0x00,0x32,0x22,0x22,0x22,0x22,0x22,0x00,
0x00,0x32,0x23,0x22,0x22,0x23,0x32,0x00,
0x00,0x32,0x33,0x32,0x23,0x33,0x32,0x00,
0x00,0x32,0x23,0x22,0x23,0x32,0x22,0x00,
0x00,0x32,0x22,0x23,0x32,0x22,0x22,0x00,
0x00,0x32,0x22,0x22,0x22,0x22,0x32,0x00,
0x00,0x33,0x33,0x33,0x33,0x33,0x33,0x00,
0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,
]
pokittoBuf = bytearray(pokittoPixels)
pokittoSurf = pygame.surface.Surface(w2, h2, pokittoBuf)

# Frogitto picture frames
frogitto_w = 8
frogitto_h = 8
frogittoPixels_f1 = [
0x00,0x00,0x00,0x00,
0x00,0xFC,0xCF,0x00,
0x70,0xCC,0xC7,0x07,
0x77,0xCE,0xE7,0x77,
0x00,0xCE,0xC7,0x00,
0x0C,0xC7,0x77,0x70,
0x0C,0x00,0x00,0x70,
0x00,0x00,0x00,0x00,
]
frogittoBuf_f1 = bytearray(frogittoPixels_f1)
frogittoSurf_f1 = pygame.surface.Surface(frogitto_w, frogitto_h, frogittoBuf_f1)
frogittoPixels_f2 = [
0x70,0x0C,0xC0,0x07,
0x07,0xCC,0xC7,0x70,
0x00,0xCE,0xE7,0x00,
0x00,0xCE,0xC7,0x00,
0x00,0xC7,0x77,0x00,
0x00,0xC0,0x07,0x00,
0x00,0x0C,0x70,0x00,
0x00,0x0C,0x70,0x00,

]
frogittoBuf_f2 = bytearray(frogittoPixels_f2)
frogittoSurf_f2 = pygame.surface.Surface(frogitto_w, frogitto_h, frogittoBuf_f2)
frogittoPixels_f3 = [
0x00,0x00,0x00,0x00,
0x70,0xFC,0xCF,0x07,
0x07,0xCC,0xC7,0x70,
0x00,0xCE,0xE7,0x00,
0x00,0xCE,0xC7,0x00,
0x0C,0xC7,0x77,0x70,
0x0C,0x00,0x00,0x70,
0x00,0x00,0x00,0x00,
]
frogittoBuf_f3 = bytearray(frogittoPixels_f3)
frogittoSurf_f3 = pygame.surface.Surface(frogitto_w, frogitto_h, frogittoBuf_f3)

# Blue car sprite frames
w = 16
h = 8
bluecarPixels_f1 = [
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x09,0x55,0x55,0x55,0x00,0x00,
0x00,0x05,0x59,0x55,0x55,0x55,0xD0,0x00,
0xE5,0x55,0x55,0xFF,0x9F,0xFD,0x55,0x00,
0x55,0x99,0x9F,0xFD,0x9F,0xDD,0x99,0x00,
0xE9,0x55,0x99,0x99,0x99,0x55,0x99,0x00,
0x95,0xBD,0x59,0x99,0x95,0xBD,0x50,0x00,
0x00,0xDB,0x00,0x00,0x00,0xDB,0x00,0x00,
]
bluecarBuf_f1 = bytearray(bluecarPixels_f1)
bluecarSurf_f1 = pygame.surface.Surface(w, h, bluecarBuf_f1)
bluecarPixels_f2 = [
0x00,0x00,0x09,0x55,0x55,0x55,0x00,0x00,
0x00,0x05,0x59,0x55,0x55,0x55,0xD0,0x00,
0xE5,0x55,0x55,0xFF,0x9F,0xFD,0x55,0x00,
0x55,0x99,0x9F,0xFD,0x9F,0xDD,0x99,0x00,
0xE9,0x55,0x99,0x99,0x99,0x55,0x99,0x00,
0x95,0x11,0x59,0x99,0x95,0x11,0x50,0x00,
0x00,0xDB,0x00,0x00,0x00,0xDB,0x00,0x00,
0x00,0xBD,0x00,0x00,0x00,0xBD,0x00,0x00,
]
bluecarBuf_f2 = bytearray(bluecarPixels_f2)
bluecarSurf_f2 = pygame.surface.Surface(w, h, bluecarBuf_f2)

# Red car sprite frames
w = 16
h = 8
redcarPixels_f1 = [
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x84,0x44,0x44,0x40,0x00,0x00,
0x00,0x44,0x84,0x44,0x44,0x44,0x00,0x00,
0xE4,0x44,0x4F,0xF8,0xFF,0xD8,0x20,0x00,
0x44,0x88,0x8F,0xD8,0xFD,0xD8,0x20,0x00,
0xE8,0x44,0x88,0x88,0x84,0x48,0x20,0x00,
0x44,0xBD,0x44,0x44,0x4B,0xD4,0x00,0x00,
0x02,0xDB,0x20,0x00,0x2D,0xB2,0x00,0x00,
]
redcarBuf_f1 = bytearray(redcarPixels_f1)
redcarSurf_f1 = pygame.surface.Surface(w, h, redcarBuf_f1)
redcarPixels_f2 = [
0x00,0x00,0x84,0x44,0x44,0x40,0x00,0x00,
0x00,0x44,0x84,0x44,0x44,0x44,0x00,0x00,
0xE4,0x44,0x4F,0xF8,0xFF,0xD8,0x00,0x00,
0x44,0x88,0x8F,0xD8,0xFD,0xD8,0x20,0x00,
0xE8,0x44,0x88,0x88,0x84,0x48,0x20,0x00,
0x84,0x22,0x44,0x44,0x42,0x24,0x20,0x00,
0x02,0xDB,0x20,0x00,0x2D,0xB2,0x00,0x00,
0x00,0xBD,0x00,0x00,0x0B,0xD0,0x00,0x00,
]
redcarBuf_f2 = bytearray(redcarPixels_f2)
redcarSurf_f2 = pygame.surface.Surface(w, h, redcarBuf_f2)

# *** AUDIO DATA

# You can just drag and drop an audio file here, e.g. a wav file.

# jump2.wav by LloydEvans09 at Freesound.org
# Licensed under CC-BY 3.0, https://creativecommons.org/licenses/by/3.0/
sound1 = b'\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x66\x66\x66\x67\x77\x77\x77\x88\x88\x88\x88\x77\x77\x76\x66\
\x66\x66\x66\x67\x77\x77\x88\x88\x88\x88\x77\x77\x66\x66\x55\x56\x66\x77\x77\x88\
\x89\x98\x88\x77\x76\x66\x55\x55\x56\x67\x77\x88\x99\x99\x88\x77\x66\x65\x55\x55\
\x66\x77\x88\x99\x99\x98\x77\x66\x55\x44\x55\x67\x78\x89\x99\x98\x87\x66\x54\x44\
\x55\x67\x88\x9a\xa9\x88\x76\x54\x44\x45\x67\x88\x9a\xa9\x87\x66\x54\x34\x56\x78\
\x89\xaa\x98\x76\x54\x44\x45\x67\x89\xaa\x98\x76\x54\x44\x56\x78\x9a\xa9\x86\x54\
\x44\x56\x78\x9a\xa9\x86\x54\x44\x56\x78\x9a\x98\x76\x54\x45\x67\x89\xa9\x87\x54\
\x44\x57\x89\xaa\x87\x64\x44\x57\x89\xa9\x87\x54\x45\x67\x9a\xa8\x76\x44\x46\x79\
\xaa\x87\x54\x45\x68\x9a\x98\x65\x45\x67\x89\x88\x76\x56\x67\x88\x87\x76\x66\x77\
\x88\x77\x76\x67\x77\x77\x77\x77\x77\x77\x77\x77\x77\
'

#  Chiptune Kick 1.wav by IanStarGem at Freesound.org
# Licensed under CC0 1.0, https://creativecommons.org/publicdomain/zero/1.0/
sound2 = b'\
\x78\x99\xaa\xbb\xcc\xdd\xef\xfe\xed\xdc\xcb\xba\xa9\x98\x87\x76\x65\x54\x43\x32\
\x21\x10\x00\x00\x01\x11\x22\x33\x44\x55\x56\x67\x78\x89\x99\xaa\xbb\xcc\xcd\xde\
\xee\xff\xee\xdd\xdc\xcb\xbb\xaa\xa9\x98\x88\x77\x66\x65\x55\x44\x33\x32\x22\x11\
\x10\x00\x00\x00\x01\x11\x22\x23\x33\x44\x45\x55\x66\x67\x77\x88\x89\x99\x9a\xaa\
\xbb\xbc\xcc\xdd\xdd\xee\xef\xff\xee\xee\xdd\xdc\xcc\xcb\xbb\xaa\xaa\x99\x99\x88\
\x88\x77\x77\x66\x65\x55\x54\x44\x43\x33\x32\x22\x21\x11\x11\x00\x00\x00\x00\x00\
\x00\x11\x11\x12\x22\x23\x33\x33\x44\x44\x45\x55\x56\x66\x66\x77\x77\x78\x88\x88\
\x99\x99\x9a\xaa\xaa\xab\xbb\xbb\xcc\xcc\xcd\xdd\xdd\xde\xee\xee\xff\xff\xff\xee\
\xee\xee\xdd\xdd\xdd\xcc\xcc\xcc\xbb\xbb\xbb\xaa\xaa\xaa\xa9\x99\x99\x98\x88\x88\
\x88\x77\x77\x77\x76\x66\x66\x66\x55\x55\x55\x54\x44\x44\x44\x43\x33\x33\x33\x32\
\x22\x22\x22\x21\x11\x11\x11\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x01\x11\x11\x11\x11\x22\x22\x22\x22\x22\x33\x33\x33\x33\x33\x44\x44\x44\x44\x44\
\x55\x55\x55\x55\x55\x56\x66\x66\x66\x66\x66\x77\x77\x77\x77\x77\x77\x88\x88\x88\
\x88\x88\x88\x99\x99\x99\x99\x99\x99\x9a\xaa\xaa\xaa\xaa\xaa\xaa\xab\xbb\xbb\xbb\
\xbb\xbb\xbb\xbc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xee\
\xee\xee\xee\xee\xee\xee\xee\xef\xff\xff\xff\xff\xff\xff\xff\xff\xee\xee\xee\xee\
\xee\xee\xee\xee\xee\xee\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xcc\xcc\xcc\xcc\
\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xba\
\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\x99\x99\x99\x99\x99\x99\x99\
\x99\x99\x99\x99\x99\x99\x99\x99\x88\x88\x88\x88\x88\x88\x88\x88\x88\x88\x88\x88\
\x88\x88\x88\x88\x88\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
'

