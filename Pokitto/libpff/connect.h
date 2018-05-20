#ifndef _CONNECT_H_

namespace PFFS {

//SDFileSystem sd(P0_9,P0_8, P1_29, P1_19, "sd"); // mosi,miso,clk,cs
#define CONNECT_MOSI    P0_9 //p5
#define CONNECT_MISO    P0_8 //p6
#define CONNECT_SCK     P0_6 //p7
#define CONNECT_CS      P0_7 //p13
#define SPI_FREQ        20000000 // was 400000

} // namespace PFFS

#define _CONNECT_H_
#endif

