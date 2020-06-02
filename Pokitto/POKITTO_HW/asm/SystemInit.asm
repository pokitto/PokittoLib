# SystemInit.S by FManga
# This code is in the PUBLIC DOMAIN

.code 16
.syntax unified

# LPC
.equ LPC_SCT_CONFIG, 0x5000C000
.equ LPC_SCT_CTRL, 0x5000C004

.equ LPC_SYSPLLCTRL,    0x40048008  
.equ LPC_SYSOSCCTRL,    0x40048020
.equ LPC_SYSPLLCLKSTAT, 0x4004800C
.equ LPC_SYSPLLCLKSEL,  0x40048040
.equ LPC_SYSPLLCLKUEN,  0x40048044
.equ LPC_SYSAHBCLKCTRL, 0x40048080

.equ LPC_MAINCLKSEL,   0x40048070
.equ LPC_MAINCLKUEN,   0x40048074
.equ LPC_SYSAHBCLKDIV, 0x40048078
.equ LPC_USBPLLCTRL,   0x40048010
.equ LPC_USBPLLSTAT,   0x40048014
.equ LPC_USBPLLCLKSEL, 0x40048048
.equ LPC_USBPLLCLKUEN, 0x4004804C
.equ LPC_USBCLKSEL,    0x400480C0
.equ LPC_USBCLKDIV,    0x400480C8
.equ LPC_PDRUNCFG,   0x40048238
.equ LPC_SYSPLLSTAT, 0x4004800C
.equ LPC_SSP0CLKDIV, 0x40048094

.equ LPC_RTCCTRL, 0x40024000
     
.equ LPC_PRESETCTRL, 0x40048004

.equ LPC_SPI0_CR0, 0x40040000
.equ LPC_SPI0_CR1, 0x40040004
.equ LPC_SPI0_CPSR, 0x40040010

.equ LPC_GPIO_PORT_DIR0, 0xA0002000
.equ LPC_GPIO_PORT_DIR1, 0xA0002004
.equ LPC_GPIO_PORT_DIR2, 0xA0002008

.equ LPC_GPIO_PORT_MASK0, 0xA0002080
.equ LPC_GPIO_PORT_MASK1, 0xA0002084
.equ LPC_GPIO_PORT_MASK2, 0xA0002088

.equ LPC_GPIO_PORT_PIN0, 0xA0002100
.equ LPC_GPIO_PORT_PIN1, 0xA0002104
.equ LPC_GPIO_PORT_PIN2, 0xA0002108

.equ LPC_GPIO_PORT_MPIN0, 0xA0002180
.equ LPC_GPIO_PORT_MPIN1, 0xA0002184
.equ LPC_GPIO_PORT_MPIN2, 0xA0002188
	
.equ LPC_GPIO_PORT_CLR0, 0xA0002280
.equ LPC_GPIO_PORT_CLR1, 0xA0002284
.equ LPC_GPIO_PORT_CLR2, 0xA0002288

.equ LPC_GPIO_PORT_SET0, 0xA0002200
.equ LPC_GPIO_PORT_SET1, 0xA0002204
.equ LPC_GPIO_PORT_SET2, 0xA0002208

.equ LPC_GPIO_PORT_NOT0, 0xA0002300
.equ LPC_GPIO_PORT_NOT1, 0xA0002304
.equ LPC_GPIO_PORT_NOT2, 0xA0002308

.equ LCD_CD_PORT, 0
.equ LCD_CD_PIN, 2
.equ LCD_WR_PORT, 1
.equ LCD_WR_PIN, 12
.equ LCD_RD_PORT, 1
.equ LCD_RD_PIN, 24
.equ LCD_RES_PORT, 1
.equ LCD_RES_PIN, 0

.equ LCD_CD_SET, LPC_GPIO_PORT_SET0
.equ LCD_CD_CLR, LPC_GPIO_PORT_CLR0
.equ LCD_WR_SET, LPC_GPIO_PORT_SET1
.equ LCD_WR_CLR, LPC_GPIO_PORT_CLR1
.equ LCD_RD_SET, LPC_GPIO_PORT_SET1
.equ LCD_RD_CLR, LPC_GPIO_PORT_CLR1
.equ LCD_RES_SET, LPC_GPIO_PORT_SET1
.equ LCD_RES_CLR, LPC_GPIO_PORT_CLR1
.equ LCD_MPIN, LPC_GPIO_PORT_MPIN2

.equ PIO0_6, 0x40044018
.equ PIO0_8, 0x40044020     
.equ PIO0_9, 0x40044024     
.equ PIO1_31, 0x400440DC
.equ PIO2_0,  0x400440F0
.equ PIO2_1,  0x400440F4
.equ PIO2_2,  0x400440FC
     
.equ ARM_NVIC_ISER, 0xe000e100
     
.macro lcd_clr_cd scratch_bit:req, scratch_ptr:req
	movs \scratch_bit, (1<<LCD_CD_PIN)
	ldr \scratch_ptr, =LPC_GPIO_PORT_CLR0
	str \scratch_bit, [\scratch_ptr]
.endm

.macro lcd_write data:req, scratch:req
	ldr \scratch, =LPC_GPIO_PORT_MPIN0
	lsls \data, \data, #3
	str \data, [\scratch, #8]
.endm	

# PVCOPY

.macro SET REG:req, VAL:req
.word \REG
.word \VAL
.endm

.macro CALL REG:req, VAL:req
.word (\REG)+1
.word \VAL
.endm

.macro OR REG:req, VAL:req
.word \REG+2
.word \VAL
.endm

.macro AND REG:req, VAL:req
.word \REG+3
.word \VAL
.endm

.macro END
.word 0
.endm


.global SystemInit
.global PVCOPY

.func PVCOPY
PVCOPY:
	push {r4, lr}
	ldr r4, =jmptbl
	movs r2, r0
next:	
	movs r3, #3
	ldm r2!, {r0, r1}
	cmp r0, #0
	beq exit
	ands r3, r0
	lsls r3, #2
	ldr r3, [r4, r3]
	bx r3
	
exit:
	pop {r4, pc}
	
PVCOPYSET:
	str r1, [r0]
	b next
	
PVCOPYFUN:
	movs r3, r0
	movs r0, r1
	lsrs r1, r3, #24
	lsls r3, #8
	lsrs r3, #8
	adds r1, #1
1:	
	push {r1, r2, r3, r4}
	blx r3
	pop {r1, r2, r3, r4}
	subs r1, #1
	beq next
	ldm r2!, {r0}
	b 1b

PVCOPYOR:
	lsrs r0, #2
	lsls r0, #2
	ldr r3, [r0]
	orrs r3, r1
	str r3, [r0]
	b next
	
PVCOPYAND:
	lsrs r0, #2
	lsls r0, #2
	ldr r3, [r0]
	ands r3, r1
	str r3, [r0]
	b next

	.pool
jmptbl:	
	.word PVCOPYSET+1
	.word PVCOPYFUN+1
	.word PVCOPYOR+1
	.word PVCOPYAND+1
.endfunc
	

	
// like memset, but for words and word-aligned memory
.func wordset	
wordset:
	cmp r1, 0
	beq 2f
1:	
	stm r0!, {r2}
	subs r1, 4
	bgt 1b
2:	
	bx lr
.endfunc

.func poll
.align 4
poll:
	ldr r1, [r0]
        lsls r1, 31
	beq poll
	bx lr
.endfunc

.func toggle
.align 4
toggle:
	movs r1, 1
	str r1, [r0]
	movs r2, 0
	str r2, [r0]
	str r1, [r0]
	bx lr
.endfunc
	
.func SystemInit
SystemInit:
	push {lr}
	ldr r0, =PIO2_2
	movs r1, 68
	movs r2, 0x80
	bl wordset
	ldr r0, =PV_INIT_GPIO
	bl PVCOPY
	pop {pc}
.endfunc
	
.func wait
wait:
	movs r1, #10
1:	
	subs r1, #1
	bne 1b
	subs r0, #1
	bne wait
	bx lr
.endfunc	

.macro WAIT_MS MS:req
	CALL wait, \MS
.endm

.func lcdCmd
.align 4
lcdCmd: 				// r0 = cmd<<16 + arg
	lsrs r1, r0, #16 		// r1 = cmd
	uxth r0, r0 	 		// remove cmd from r0

	lcd_clr_cd r3, r4		// CLR_CD. r3 = pin, r4 = CLR0

	// MPIN[2] = CMD<<3
	ldr r2, =LPC_GPIO_PORT_MPIN0
	lsls r1, r1, #3
	str r1, [r2, #8]
	
	movs r1, r2			// r1 = MPIN
	
	ldr r3, =(1<<LCD_WR_PIN)	// CLR_WR
	str r3, [r4, #4]		// [r4, 4] is CLR1. LCD_WR_PORT

	lsls r0, r0, #3			// data = data<<3
	nop
					// SET_WR
	ldr r2, =LPC_GPIO_PORT_SET0
	str r3, [r2, #4]		// [r2, 4] is SET1. LCD_WR_PORT
	
	movs r3, (1<<LCD_CD_PIN)
	str r3, [r2, #0]		// SET_CD. [r2, 0] is SET0.
	
	str r0, [r1, #8]		// MPIN[2] = data (r0)
	
	ldr r3, =(1<<LCD_WR_PIN)	// CLR_WR
	str r3, [r4, #4]		// [r4, 4] is CLR1. LCD_WR_PORT

	nop
	nop
					// SET_WR
	ldr r2, =LPC_GPIO_PORT_SET0
	str r3, [r2, #4]		// [r2, 4] is SET1. LCD_WR_PORT

	bx lr
	.pool
.endFunc

.macro LCD_CMD CMD:req, ARG:req, RPT
	.word lcdCmd+1 + ((\RPT+0)<<24)
	.word (\CMD<<16) | (\ARG)
.endm

.macro LCD_CMD_ARG CMD:req, ARG:req
	.word (\CMD<<16) | (\ARG)
.endm
	
.align 4
PV_INIT_GPIO:
	// system_LPC11U6x.c: 487
	// enable GPIO | IOCON | SRAM1+SRAM2 | USART0
	OR LPC_SYSAHBCLKCTRL, (1<<16) | (1<<6) | (1<<30) | (1<<19) | (3<<26) | (1<<12)
    AND LPC_RTCCTRL, ~1
    SET LPC_RTCCTRL, (1<<7)

    // init buttons
    SET 0x40044084,0x88
    SET 0x40044070,0x88
    SET 0x40044088,0x88
    SET 0x40044094,0x88
    SET 0x4004406c,0x88
    SET 0x400440c4,0x88
    SET 0x4004407c,0x88

	// 488 // same is repeated further down.
	// SET LPC_SYSPLLCTRL, 0x23
	
	// 494, 495
	SET PIO2_0, 1
	SET PIO2_1, 1

	// 497 - Redundant? Default is already 0.
	SET LPC_SYSOSCCTRL, 0

	// 498
	AND LPC_PDRUNCFG, ~(1<<5)

	WAIT_MS 250

	// 507
	SET LPC_SYSPLLCLKSEL, 1

	// 508
	CALL toggle, LPC_SYSPLLCLKUEN

	// 511 // this line in system_LPC11U6x.c makes no sense.
	// CALL poll, LPC_SYSPLLCLKSTAT

	// 522
	OR LPC_PDRUNCFG, 1<<7

    //    .ifdef OSCT
	// 523
	SET LPC_SYSPLLCTRL, 0x25
    //    .else
	// 523
	//SET LPC_SYSPLLCTRL, 0x23
    //    .endif

	// 522
	AND LPC_PDRUNCFG, ~(1<<7)

	// 525
	CALL poll, LPC_SYSPLLSTAT

	// 528
	SET LPC_MAINCLKSEL, 3

	// 529
	CALL toggle, LPC_MAINCLKUEN
		
	// 532
	CALL poll, LPC_MAINCLKUEN

	// 534
	SET LPC_SYSAHBCLKDIV, 1

	// 556, 559
	AND LPC_PDRUNCFG, ~(1<<10) & ~(1<<8)

	// 560
	SET LPC_USBPLLCLKSEL, 1

	// 561
	CALL toggle, LPC_USBPLLCLKUEN

	// 564
	CALL poll, LPC_USBPLLCLKUEN

	// 566
	SET LPC_USBPLLCTRL, 0x23

	// 567
	CALL poll, LPC_USBPLLSTAT

	// 569
	SET LPC_USBCLKSEL, 0

	// 573
	SET LPC_USBCLKDIV, 1

/*
	// spi_api.c:77
	SET LPC_SSP0CLKDIV, 1
	// spi_api.c:79
	OR LPC_PRESETCTRL, 1

	// spi_api.c:89
	SET PIO0_9, 0x81
	SET PIO0_8, 0x81
	SET PIO0_6, 0x82

// SPI.cpp
	// disable ssp, set lbm, ms, sod, divider to 0
	AND LPC_SPI0_CR1, ~(1<<1) & ~0xD & ~0xFF00

	SET LPC_SPI0_CR0, 0x7

	// set prescaler to 0. spi_frequency:151
	SET LPC_SPI0_CPSR, 0
	
	OR LPC_SPI0_CR1, (1<<1) // enable ssp
*/
// end SPI.cpp
	
	SET ARM_NVIC_ISER, 0x7F
	SET PIO1_31, 0x80
	
	SET LPC_GPIO_PORT_MASK0, 0
	SET LPC_GPIO_PORT_MASK1, 0
	SET LPC_GPIO_PORT_MASK2, ~(0x7FFF8)

	// #define LCD_CD_PORT           0
	// #define LCD_CD_PIN            2
	// LPC_GPIO_PORT->DIR[LCD_CD_PORT] |= (1  << LCD_CD_PIN );
	// GPIO_DIR[0]:
	SET LPC_GPIO_PORT_DIR0, 0x00000004

	// #define LCD_WR_PORT           1
	// #define LCD_WR_PIN            12
	// LPC_GPIO_PORT->DIR[LCD_WR_PORT] |= (1  << LCD_WR_PIN );
	// #define LCD_RD_PORT           1
	// #define LCD_RD_PIN            24
	// LPC_GPIO_PORT->DIR[LCD_RD_PORT] |= (1  << LCD_RD_PIN );
	// #define LCD_RES_PORT          1
	// #define LCD_RES_PIN           0	
	// LPC_GPIO_PORT->DIR[LCD_RES_PORT] |= (1  << LCD_RES_PIN );
	// GPIO_DIR[1]:
	SET LPC_GPIO_PORT_DIR1, 0x01001001
	
	// LPC_GPIO_PORT->DIR[2] |= (1  << 2 );
	// LPC_GPIO_PORT->DIR[2] |= (0xFFFF  << 3);  // P2_3...P2_18 as output
	// GPIO_DIR[2]:
	SET LPC_GPIO_PORT_DIR2, 0x0007FFFC

	SET LPC_GPIO_PORT_PIN0, 0
	
	// #define LCD_RES_PORT          1
	// #define LCD_RES_PIN           0
	// #define SET_RESET LPC_GPIO_PORT->SET[LCD_RES_PORT] = 1 << LCD_RES_PIN; // RST
	SET LPC_GPIO_PORT_PIN1, (1<<LCD_RES_PIN)
	
	// LPC_GPIO_PORT->SET[2] = 1 << 2; // backlight
	SET LPC_GPIO_PORT_PIN2, 1 << 2
	// LCD initialization

	// Reset LCD
	WAIT_MS 10
 	SET LPC_GPIO_PORT_CLR1, (1<<LCD_RES_PIN)
	WAIT_MS 10
	// (LCD_RD high = write)
 	SET LPC_GPIO_PORT_SET1, (1<<LCD_RES_PIN) | (1<<LCD_RD_PIN)
	WAIT_MS 10
	// driver output control, this also affects direction
	LCD_CMD 0x01, 0x11C, 9

	// originally: 0x11C 100011100 SS,NL4,NL3,NL2
        // NL4...0 is the number of scan lines to drive the screen !!!
        // so 11100 is 1c = 220 lines, correct
        // test 1: 0x1C 11100 SS=0,NL4,NL3,NL2 -> no effect
        // test 2: 0x31C 1100011100 GS=1,SS=1,NL4,NL3,NL2 -> no effect
        // test 3: 0x51C 10100011100 SM=1,GS=0,SS=1,NL4,NL3,NL2 -> no effect
        // test 4: 0x71C SM=1,GS=1,SS=1,NL4,NL3,NL2
        // test 5: 0x
        // seems to have no effect... is this perhaps only for RGB mode ?

	// LCD driving control
	LCD_CMD_ARG 0x02,0x0100
	// INV = 1
	
	// Entry mode... lets try if this affects the direction
	LCD_CMD_ARG 0x03,0x1038
	// originally 0x1030 1000000110000 BGR,ID1,ID0
        // test 1: 0x1038 1000000111000 BGR,ID1,ID0,AM=1 ->drawing DRAM horizontally
        // test 4: am=1, id0=0, id1=0, 1000000001000,0x1008 -> same as above, but flipped on long
        // test 2: am=0, id0=0, 1000000100000, 0x1020 -> flipped on long axis
        // test 3: am=0, id1=0, 1000000010000, 0x1010 -> picture flowed over back to screen

	// Display control 2
	LCD_CMD_ARG 0x08,0x0808 // 100000001000 FP2,BP2
	
	// RGB display interface
	LCD_CMD_ARG 0x0C,0x0000 // all off
	
	// Frame marker position
	LCD_CMD_ARG 0x0F,0x0001 // OSC_EN
	
	// Horizontal DRAM Address
	LCD_CMD_ARG 0x20,0x0000

	// Vertical DRAM Address
	LCD_CMD_ARG 0x21,0x0000
	
// *************Power On sequence ****************
	LCD_CMD_ARG 0x10,0x0000
	LCD_CMD_ARG 0x11,0x1000
	
	WAIT_MS 10
//------------------------ Set GRAM area --------------------------------
	// Gate scan position
	LCD_CMD 0x30,0x0000, 9 // if GS=0, 00h=G1, else 00h=G220
	
	// Vertical scroll control
	LCD_CMD_ARG 0x31,0x00DB // scroll start line 11011011 = 219
	
	// Vertical scroll control
	LCD_CMD_ARG 0x32,0x0000 // scroll end line 0
	
	// Vertical scroll control
	LCD_CMD_ARG 0x33,0x0000 // 0=vertical scroll disabled
	
	// Partial screen driving control
	LCD_CMD_ARG 0x34,0x00DB // db = full screen (end)
	
	// partial screen
	LCD_CMD_ARG 0x35,0x0000 // 0 = start
	
	// Horizontal and vertical RAM position
	LCD_CMD_ARG 0x36,0x00AF //end address 175
	
	LCD_CMD_ARG 0x37,0x0000
	LCD_CMD_ARG 0x38,0x00DB //end address 219
	

	LCD_CMD_ARG 0x39,0x0000 // start address 0
	WAIT_MS 10
	
	// start gamma register control
	LCD_CMD 0xff,0x0003, 13
// ----------- Adjust the Gamma  Curve ----------//
	LCD_CMD_ARG 0x50,0x0203	
	LCD_CMD_ARG 0x51,0x0A09	
	LCD_CMD_ARG 0x52,0x0005	
	LCD_CMD_ARG 0x53,0x1021	
	LCD_CMD_ARG 0x54,0x0602	
	LCD_CMD_ARG 0x55,0x0003	
	LCD_CMD_ARG 0x56,0x0703	
	LCD_CMD_ARG 0x57,0x0507	
	LCD_CMD_ARG 0x58,0x1021	
	LCD_CMD_ARG 0x59,0x0703	
	LCD_CMD_ARG 0xB0,0x2501	
	LCD_CMD_ARG 0xFF,0x0000	
	LCD_CMD_ARG 0x07,0x1017
	
	SET LCD_CD_CLR, (1<<LCD_CD_PIN)
	SET LCD_MPIN,   (0x22)<<3
	SET LCD_WR_CLR, (1<<LCD_WR_PIN)
	SET LCD_WR_SET, (1<<LCD_WR_PIN)
	SET LCD_CD_SET, (1<<LCD_CD_PIN)

        // Set Reset button to GPIO
        // SET 0x40044000,0x89
	
	END
