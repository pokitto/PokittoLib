#include "Pokitto.h"
#include "Synth.h"
#include "dma_11u6x.h"

Pokitto::Core game;
Pokitto::Display disp;
Pokitto::Sound snd;
Pokitto::Buttons btn;

uint8_t mysine[] = {
0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,
0x98,0x9b,0x9e,0xa2,0xa5,0xa7,0xaa,0xad,
0xb0,0xb3,0xb6,0xb9,0xbc,0xbe,0xc1,0xc4,
0xc6,0xc9,0xcb,0xce,0xd0,0xd3,0xd5,0xd7,
0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,
0xea,0xeb,0xed,0xee,0xf0,0xf1,0xf3,0xf4,
0xf5,0xf6,0xf8,0xf9,0xfa,0xfa,0xfb,0xfc,
0xfd,0xfd,0xfe,0xfe,0xfe,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xfe,0xfe,0xfe,0xfd,
0xfd,0xfc,0xfb,0xfa,0xfa,0xf9,0xf8,0xf6,
0xf5,0xf4,0xf3,0xf1,0xf0,0xee,0xed,0xeb,
0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
0xda,0xd7,0xd5,0xd3,0xd0,0xce,0xcb,0xc9,
0xc6,0xc4,0xc1,0xbe,0xbc,0xb9,0xb6,0xb3,
0xb0,0xad,0xaa,0xa7,0xa5,0xa2,0x9e,0x9b,
0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,
0x67,0x64,0x61,0x5d,0x5a,0x58,0x55,0x52,
0x4f,0x4c,0x49,0x46,0x43,0x41,0x3e,0x3b,
0x39,0x36,0x34,0x31,0x2f,0x2c,0x2a,0x28,
0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,
0x15,0x14,0x12,0x11,0xf,0xe,0xc,0xb,
0xa,0x9,0x7,0x6,0x5,0x5,0x4,0x3,
0x2,0x2,0x1,0x1,0x1,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x2,
0x2,0x3,0x4,0x5,0x5,0x6,0x7,0x9,
0xa,0xb,0xc,0xe,0xf,0x11,0x12,0x14,
0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
0x25,0x28,0x2a,0x2c,0x2f,0x31,0x34,0x36,
0x39,0x3b,0x3e,0x41,0x43,0x46,0x49,0x4c,
0x4f,0x52,0x55,0x58,0x5a,0x5d,0x61,0x64,
0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};

int tonefreq=46;
uint8_t amplitude = 255;//127;
uint8_t wavetype = 1, arpmode=1;
uint32_t changed = 1;
char notestr[6];
uint8_t sbindx=0,sbx=0,prevy=0;

/* Size of the source and destination buffers in 32-bit words.
   Allowable values  = 126, 256, 512, or 1024 */
#define SIZE_BUFFERS            (256)

/* Source and destination buffers */
//uint32_t src[SIZE_BUFFERS], dst[SIZE_BUFFERS];
uint8_t src[SIZE_BUFFERS], dst[SIZE_BUFFERS];

/* DMA completion flag */
static volatile bool dmaDone;

// IRQ handlers have to be extern "C" in mbed!!!!!!!
extern "C" void DMA_IRQHandler(void)
{
	/* Rrror interrupt on channel 0? */
	if ((Chip_DMA_GetIntStatus(LPC_DMA) & DMA_INTSTAT_ACTIVEERRINT) != 0) {
		/* This shouldn't happen for this simple DMA example, so set the LED
		   to indicate an error occurred. This is the correct method to clear
		   an abort. */
		Chip_DMA_DisableChannel(LPC_DMA, DMA_CH0);
		while ((Chip_DMA_GetBusyChannels(LPC_DMA) & (1 << DMA_CH0)) != 0) {}
		Chip_DMA_AbortChannel(LPC_DMA, DMA_CH0);
		Chip_DMA_ClearErrorIntChannel(LPC_DMA, DMA_CH0);
		Chip_DMA_EnableChannel(LPC_DMA, DMA_CH0);
		//Board_LED_Set(0, true);
	}

	/* Clear DMA interrupt for the channel */
	Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMA_CH0);

	dmaDone = true;
}

void modifysrc(int j) {
    for (uint32_t i=0; i<SIZE_BUFFERS;i++) {
            src[i] = random(0,255);
    }
}

int main()
{
    modifysrc(0);
    game.begin();
    /* DMA part */
    DMA_CHDESC_T dmaDesc;
	int i;

	uint32_t startTime, ticks[3];
    /* DMA initialization - enable DMA clocking and reset DMA if needed */
	Chip_DMA_Init(LPC_DMA);
    /* Enable DMA controller and use driver provided DMA table for current descriptors */
	Chip_DMA_Enable(LPC_DMA);
	Chip_DMA_SetSRAMBase(LPC_DMA, DMA_ADDR(Chip_DMA_Table));
	/* Setup channel 0 for the following configuration:
	   - High channel priority
	   - Interrupt A fires on descriptor completion */
	Chip_DMA_EnableChannel(LPC_DMA, DMA_CH0);
	Chip_DMA_EnableIntChannel(LPC_DMA, DMA_CH0);
	Chip_DMA_SetupChannelConfig(LPC_DMA, DMA_CH0,
								(DMA_CFG_HWTRIGEN | DMA_CFG_TRIGTYPE_EDGE | DMA_CFG_TRIGPOL_HIGH |
								 DMA_CFG_BURSTPOWER_128 | DMA_CFG_CHPRIORITY(0)));

    /* DMA descriptor for memory to memory operation - note that addresses must
	   be the END address for src and destination, not the starting address.
	     DMA operations moves from end to start. */
	//dmaDesc.source = DMA_ADDR(&src[SIZE_BUFFERS - 1]) + 3;
	//dmaDesc.dest = DMA_ADDR(&dst[SIZE_BUFFERS - 1]) + 3;
	dmaDesc.source = DMA_ADDR(&src[SIZE_BUFFERS - 1]);
	//dmaDesc.dest = DMA_ADDR(&dst[SIZE_BUFFERS - 1]);
	dmaDesc.dest = DMA_ADDR(&game.display.screenbuffer[SIZE_BUFFERS - 1]);
	dmaDesc.next = DMA_ADDR(0);

	/* Enable DMA interrupt */
	uint32_t rtr = NVIC->ISER[0];
	NVIC_EnableIRQ(DMA_IRQn);

    /* Reset timer and perform a bunch memory to memory moves with DMA */

	for (i = 0; i < 100; i++) {
		dmaDone = false;

		//modifyData(src, dst, SIZE_BUFFERS);
        modifysrc(i);

		/* Setup transfer descriptor and validate it */
		Chip_DMA_SetupTranChannel(LPC_DMA, DMA_CH0, &dmaDesc);
		Chip_DMA_SetValidChannel(LPC_DMA, DMA_CH0);

		/* Setup data transfer and software trigger in same call */
		/*Chip_DMA_SetupChannelTransfer(LPC_DMA, DMA_CH0,
									  (DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA | DMA_XFERCFG_SWTRIG |
									   DMA_XFERCFG_WIDTH_32 | DMA_XFERCFG_SRCINC_1 | DMA_XFERCFG_DSTINC_1 |
									   DMA_XFERCFG_XFERCOUNT(SIZE_BUFFERS)));*/
        Chip_DMA_SetupChannelTransfer(LPC_DMA, DMA_CH0,
									  (DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA | DMA_XFERCFG_SWTRIG |
									   DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_SRCINC_1 | DMA_XFERCFG_DSTINC_0 |
									   DMA_XFERCFG_XFERCOUNT(SIZE_BUFFERS)));
		/* Wait for DMA completion */
		while (dmaDone == false) {
			if (game.update()) {
                game.display.color=3;
                game.display.print("Hello! ");
			}

		}

	}


    snd.setVolume(snd.getMaxVol()); // volume to max level depending on headset/speaker safety level
    disp.persistence = true;
    disp.color=wavetype+1;
    disp.bgcolor=0;
    uint8_t sbindx=0;
    int oldvol=0;
    int linecenter = (disp.height/4); // center oscilloscope line vertically
    linecenter *=2;
    linecenter += 0;
    snd.ampEnable(1);
    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
    //snd.playTone(1,100,255,0);
    // want to have 2 oscillators 1 octave apart ?
    snd.playTone(2,tonefreq+12,amplitude,wavetype,arpmode);

    disp.clear();
    disp.print("Frq:");
    getNoteString(notestr,tonefreq);
    disp.println(notestr);
    disp.print("Vol:");
    disp.println((int)amplitude);
    disp.print("Wav:");
    disp.print((int)wavetype);
    switch (wavetype) {
    case 0:
        disp.println(" Off");break;
    case 1:
        disp.println(" Square");break;
    case 2:
        disp.println(" Saw");break;
    case 3:
        disp.println(" Triang");break;
    case 4:
        disp.println(" Noise");break;
    case 5:
        disp.println(" SqNois");break;
    }
    disp.print("HWvol:");
    disp.println((int)snd.getVolume());
    disp.lcdRefresh(disp.screenbuffer);



    while (game.isRunning()) {

        #ifdef POK_SIM
            #define DEBOU 2048*32
        #else
            #define DEBOU 256
        #endif

        /*
        uint32_t* j = (uint32_t*)0x5000e204;
        *j = 0xab;//(0x1F*(uint32_t)val++)>>8;

        Chip_DMA_SetupTranChannel(LPC_DMA, DMA_CH0, &dmaDesc);
		Chip_DMA_SetValidChannel(LPC_DMA, DMA_CH0);
		dmaDesc.source = DMA_ADDR(&mysine[255]);
            dmaDesc.dest = DMA_ADDR(j);
            dmaDesc.next = DMA_ADDR(0);*/

		/*if (!val) {
            dmaDesc.source = DMA_ADDR(&mysine[255]);
            dmaDesc.dest = DMA_ADDR(&soundbuf[255]);
            dmaDesc.next = DMA_ADDR(0);
		} else {
            dmaDesc.source = DMA_ADDR(&src[255]);
            dmaDesc.dest = DMA_ADDR(&soundbuf[255]);
            dmaDesc.next = DMA_ADDR(0);
		}*/
		/*
		Chip_DMA_SetupChannelTransfer(LPC_DMA, DMA_CH0,
									  (DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA | DMA_XFERCFG_SWTRIG |
									   DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_SRCINC_1 | DMA_XFERCFG_DSTINC_0 |
									   DMA_XFERCFG_XFERCOUNT(255)));*/

		//0x1f*
		//val=1-val;
		//fill buffer



        if (changed == 0) {
            if(btn.upBtn()) { amplitude++; changed = DEBOU; }
            if(btn.downBtn()) { amplitude--; changed = DEBOU; }
            if(btn.leftBtn() && (tonefreq > 0)) { tonefreq --; changed = DEBOU; }
            if(btn.rightBtn() &&  (tonefreq < 88)) { tonefreq ++; changed = DEBOU; }
            if(btn.released(BTN_A) && wavetype > 0 ) { wavetype--; changed = DEBOU*10;}
            if(btn.released(BTN_B) && wavetype < 5 ) { wavetype++; changed = DEBOU*10;}
            if (snd.getVolume() != oldvol) changed=DEBOU;

            if (changed) {
                    disp.clear();
                    disp.color=0;//owavetype+1;
                    disp.fillRectangle(0,0,109,34);
                    disp.color=wavetype+1;
                    disp.bgcolor=0;
                    disp.print("Frq:");
                    getNoteString(notestr,tonefreq);
                    disp.println(notestr);
                    disp.print("Vol:");
                    disp.println((int)amplitude);
                    disp.print("Wav:");
                    disp.print((int)wavetype);
                    switch (wavetype) {
                    case 0:
                        disp.println(" Off");break;
                    case 1:
                        disp.println(" Square");break;
                    case 2:
                        disp.println(" Saw");break;
                    case 3:
                        disp.println(" Triang");break;
                    case 4:
                        disp.println(" Noise");break;
                    case 5:
                        disp.println(" SqNois");break;
                    }
                    disp.print("HWvol:");
                    disp.println((int)snd.getVolume());
                    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
                    oldvol = (int) snd.getVolume();
                    // want to have 2 oscillators 1 octave apart ?
                    snd.playTone(2,tonefreq+12,amplitude,wavetype,arpmode);
            }
        } else changed--;
        disp.color=0;
        disp.drawColumn(sbx,38,disp.height);
        disp.color=wavetype+1;
        uint8_t tindex = sbindx*2;
        int16_t y = linecenter + (((int)128-(soundbuf[tindex]))>>2);
        disp.drawLine(sbx-1,prevy,sbx,y);
        game.update();
        disp.color=0;
        prevy=y;
        sbx++;sbindx++;
        if (sbx>disp.width-1) {sbx=0;}
    }
    return 0; // good manners!
}
