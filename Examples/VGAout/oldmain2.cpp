#include "Pokitto.h"
#include "sct_11u6x.h"


/* Working principle:
SCT is set to flip HSync at 31.77 uS. The low state (hsync) lasts for 3.77 uS
SCT is set to pull Hsync low at 28 us. Reset occurs at 31.77 us, counters get zeroed.
A VSync_count interrupt gets called at the end of every scanline, VSync_count is incremented
When VSync_count reaches the drawing state (end of vsync back porch), main loop begins pushing out pixels via RGB lines
Main loop pushes X amount of pixels out, then stalls until VSync_count is incremented, signifying a HSync front porch
When 480 lines have been pushed out or VSync_state is changed away from the drawing state, no more pixels are pushed
and the main loop just rolls over */

/* In Pokitto PEX header, the EXT0 PWM pin is P1_19, and pinmode 2 is SCT0_OUT0 — SCTimer0/PWM output 0 */

#define CLR_VSYNC LPC_GPIO_PORT->CLR[0] = 1 << 12
#define SET_VSYNC LPC_GPIO_PORT->SET[0] = 1 << 12

#define CLR_R LPC_GPIO_PORT->CLR[0] = 1 << 14
#define SET_R LPC_GPIO_PORT->SET[0] = 1 << 14

mbed::DigitalOut VSync(EXT2);
mbed::DigitalOut R(EXT4);
mbed::PwmOut HSync(EXT0);

int vsync_counter=0;

enum VSYNC_STATES
{
  VSYNC_LOW_ENABLED = 1, // 0
  VSYNC_BACKPORCH_STARTS = 3, // 0.064ms
  VSYNC_BACKPORCH_ENDS_DRAWING_BEGINS = 34, // 1.084ms
  VSYNC_FRONTPORCH_STARTS = 514, // 16.334ms
  VSYNC_FRONTPORCH_ENDS = 528 // //
};


/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	/* This is only used to wakeup up the device from sleep */
}

/**
 * @brief	Handle interrupt from State Configurable Timer
 * @return	Nothing
 */
extern "C" {
void SCT0_1_IRQHandler(void)
{


	/* Clear the Interrupt */
	Chip_SCT_ClearEventFlag((LPC_SCT_T*)LPC_SCT0, SCT_EVT_0);

}}

/**
 * @brief	Handle interrupt from HSync reset (end of scanline)
 * @return	Nothing
 */

void vsync_counting_interrupt() {

    CLR_R;//blanking period

    /* Vsync state changer */

    switch (vsync_counter) {
    case VSYNC_LOW_ENABLED:
        CLR_VSYNC;
        break;
    case VSYNC_BACKPORCH_STARTS:
        SET_VSYNC;
        break;
    case VSYNC_FRONTPORCH_ENDS:
        vsync_counter=0; //loop around
        break;
    }
    vsync_counter++;
}

/**
 * @brief	Initialize HSync output
 * @return	Nothing
 */

void initHSync() {
    /**
 * @brief LPC11U6X System Control block structure
 */
typedef struct {					/*!< SYSCTL Structure */
	__IO uint32_t  SYSMEMREMAP;		/*!< System Memory remap register */
	__IO uint32_t  PRESETCTRL;		/*!< Peripheral reset Control register */
	__IO uint32_t  SYSPLLCTRL;		/*!< System PLL control register */
	__I  uint32_t  SYSPLLSTAT;		/*!< System PLL status register */
	__IO uint32_t  USBPLLCTRL;		/*!< USB PLL control register */
	__I  uint32_t  USBPLLSTAT;		/*!< USB PLL status register */
	__I  uint32_t  RESERVED1[1];
	__IO uint32_t  RTCOSCCTRL;		/*!< RTC Oscillator control register */
	__IO uint32_t  SYSOSCCTRL;		/*!< System Oscillator control register */
	__IO uint32_t  WDTOSCCTRL;		/*!< Watchdog Oscillator control register */
	__I  uint32_t  RESERVED2[2];
	__IO uint32_t  SYSRSTSTAT;		/*!< System Reset Status register */
	__I  uint32_t  RESERVED3[3];
	__IO uint32_t  SYSPLLCLKSEL;	/*!< System PLL clock source select register */
	__IO uint32_t  SYSPLLCLKUEN;	/*!< System PLL clock source update enable register*/
	__IO uint32_t  USBPLLCLKSEL;	/*!< USB PLL clock source select register */
	__IO uint32_t  USBPLLCLKUEN;	/*!< USB PLL clock source update enable register */
	__I  uint32_t  RESERVED4[8];
	__IO uint32_t  MAINCLKSEL;		/*!< Main clock source select register */
	__IO uint32_t  MAINCLKUEN;		/*!< Main clock source update enable register */
	__IO uint32_t  SYSAHBCLKDIV;	/*!< System Clock divider register */
	__I  uint32_t  RESERVED5;
	__IO uint32_t  SYSAHBCLKCTRL;	/*!< System clock control register */
	__I  uint32_t  RESERVED6[4];
	__IO uint32_t  SSP0CLKDIV;		/*!< SSP0 clock divider register */
	__IO uint32_t  USART0CLKDIV;	/*!< UART clock divider register */
	__IO uint32_t  SSP1CLKDIV;		/*!< SSP1 clock divider register */
	__IO uint32_t  FRGCLKDIV;		/*!< FRG clock divider (USARTS 1 - 4) register */
	__I  uint32_t  RESERVED7[7];
	__IO uint32_t  USBCLKSEL;		/*!< USB clock source select register */
	__IO uint32_t  USBCLKUEN;		/*!< USB clock source update enable register */
	__IO uint32_t  USBCLKDIV;		/*!< USB clock source divider register */
	__I  uint32_t  RESERVED8[5];
	__IO uint32_t  CLKOUTSEL;		/*!< Clock out source select register */
	__IO uint32_t  CLKOUTUEN;		/*!< Clock out source update enable register */
	__IO uint32_t  CLKOUTDIV;		/*!< Clock out divider register */
	__I  uint32_t  RESERVED9;
	__IO uint32_t  UARTFRGDIV;		/*!< USART fractional generator divider (USARTS 1 - 4) register */
	__IO uint32_t  UARTFRGMULT;		/*!< USART fractional generator multiplier (USARTS 1 - 4) register */
	__I  uint32_t  RESERVED10;
	__IO uint32_t  EXTTRACECMD;		/*!< External trace buffer command register */
	__I  uint32_t  PIOPORCAP[3];	/*!< POR captured PIO status registers */
	__I  uint32_t  RESERVED11[10];
	__IO uint32_t  IOCONCLKDIV[7];	/*!< IOCON block for programmable glitch filter divider registers */
	__IO uint32_t  BODCTRL;			/*!< Brown Out Detect register */
	__IO uint32_t  SYSTCKCAL;		/*!< System tick counter calibration register */
	__I  uint32_t  RESERVED12[6];
	__IO uint32_t  IRQLATENCY;		/*!< IRQ delay register */
	__IO uint32_t  NMISRC;			/*!< NMI source control register */
	__IO uint32_t  PINTSEL[8];		/*!< GPIO pin interrupt select register 0-7 */
	__IO uint32_t  USBCLKCTRL;		/*!< USB clock control register */
	__I  uint32_t  USBCLKST;		/*!< USB clock status register */
	__I  uint32_t  RESERVED13[25];
	__IO uint32_t  STARTERP0;		/*!< Start logic 0 interrupt wake-up enable register */
	__I  uint32_t  RESERVED14[3];
	__IO uint32_t  STARTERP1;		/*!< Start logic 1 interrupt wake-up enable register */
	__I  uint32_t  RESERVED15[6];
	__IO uint32_t  PDSLEEPCFG;		/*!< Power down states in deep sleep mode register */
	__IO uint32_t  PDWAKECFG;		/*!< Power down states in wake up from deep sleep register */
	__IO uint32_t  PDRUNCFG;		/*!< Power configuration register*/
	__I  uint32_t  RESERVED16[110];
	__I  uint32_t  DEVICEID;		/*!< Device ID register */
} LPC_SYSCTL_T;

/**
 * @brief LPC11u6x IO Configuration Unit register block structure
 */
typedef struct {			/*!< LPC11U6X IOCON Structure */
	__IO uint32_t  PIO0[24];
	__IO uint32_t  PIO1[32];
	__I  uint32_t  reserved[4];
	__IO uint32_t  PIO2A[2];/* PIO2_0/1 only */
	__I  uint32_t  reserved1[1];
	__IO uint32_t  PIO2B[22];	/* PIO2_2 to PIO_2_23 */
} LPC_IOCON_T;

    #define LPC_SYSCTL_BASE           0x40048000
    #define LPC_SYSCTL                ((LPC_SYSCTL_T           *) LPC_SYSCTL_BASE)

    /* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / 100); //100 ticks per sec

	/* Enable AHB clock for SCT0 and SCT1 */
	LPC_SYSCTL->SYSAHBCLKCTRL |= (1 << 31); // 31 = SYSCTL_CLOCK_SCT0_1

	/* Reset SCT0 */
	LPC_SYSCTL->PRESETCTRL &= ~(1 << (uint32_t) 9); //assert SCT0 reset
	LPC_SYSCTL->PRESETCTRL |= (1 << (uint32_t) 9); //deassert SCT0 reset

    /*  SCT0_OUT0 on PIO1_19 (Pokitto EXT0) mapped to FUNC3
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 16, (IOCON_FUNC1 | IOCON_MODE_INACT |
										   IOCON_ADMODE_EN));*/
    #define LPC_IOCON_BASE            0x40044000
    #define LPC_IOCON                 ((LPC_IOCON_T            *) LPC_IOCON_BASE)

    /* SCT0_OUT0 on PIO1_19 mapped to FUNC3 ... mode 2 = SCT0_OUT0 — SCTimer0/PWM output 0 */
    LPC_IOCON->PIO1[19] = 2;

    /* Configure the SCT as a 32bit counter using the bus clock */
	LPC_SCT0->CONFIG = 0x1 | 0; //UNIFY (bit 0) = 1, 32 bit mode | CLKMODE (bits 2:1) = 00 bus clock clocks the SCT and prescalers

    /* from mbed pwmout */
    // Unified 32-bit counter, autolimit
    LPC_SCT0->CONFIG |= ((0x3 << 17) | 0x01);
    // halt and clear the counter
    LPC_SCT0->CTRL |= (1 << 2) | (1 << 3);
    LPC_SCT0->OUT0_SET = (1 << 0); // event 0
    LPC_SCT0->OUT0_CLR = (1 << 1); // event 1
    // Event 0 : MATCH and MATCHSEL=0
    LPC_SCT0->EV0_CTRL  = (1 << 12);
    LPC_SCT0->EV0_STATE = 0x1;
    // Event 1 : MATCH and MATCHSEL=1
    LPC_SCT0->EV1_CTRL  = (1 << 12) | (1 << 0);
    LPC_SCT0->EV1_STATE = 0x3;

    /* Initial CTOUT0 state is high */
    /* The SCT supports 4 outputs, each of which has a corresponding bit in this register.
Software can write to any of the output registers when both counters are halted to control
the outputs directly. */
/*Writing a 1 to bit n makes the corresponding output HIGH. 0 makes
the corresponding output LOW (output 0 = bit 0, output 1 = bit 1,...,
output 3 = bit 3).*/
	//LPC_SCT0->OUTPUT = (7 << 0); /*!< output register */

    /* The PWM will use a cycle time of (PWMCYCLERATE)Hz based off the bus clock */
    /* bus clock is main clock (12MHz / AHBDIV (=1) in this case */
	int cycleTicks = (12000000) / 1000; // 12000 ticks = 1000Hz

	/* Setup for match mode */
	LPC_SCT0->REGMODE = 0; /*!< SCT match/capture registers mode register  */
	/* SCT match/capture registers mode register */
    /* If UNIFY = 1 in the CONFIG register, only the _L bits of this register are used. The L bits
control whether each set of match/capture registers operates as unified 32-bit
capture/match registers. Each bit controls one pair of match/capture registers (register 0 =
bit 0, register 1 = bit 1,..., register 4 = bit 4). 0 = registers operate as match registers. 1 = registers operate as capture registers*/

    /* Start at 50% duty cycle */
	uint8_t dutyCycle = 50;

    /* Setup match counter 0 for the number of ticks in a PWM sweep, event 0
	    will be used with the match 0 count to reset the counter.  */
    /* Match registers can be read at any time. Writing to the MATCH_L, MATCH_H, or unified
register is only allowed when the corresponding counter is halted */

	LPC_SCT0->MATCH0 = cycleTicks; // trigger value
	LPC_SCT0->MATCHREL0 = cycleTicks; // reload value
	LPC_SCT0->MATCH1 = cycleTicks/2; // should be 50% duty cycle
    LPC_SCT0->MATCHREL1 = cycleTicks/2; // reload value
    /* Setup event 1 to trigger on match 1 and set CTOUT0 high */
    /* remember, event 0 is now match 0, the total PWM cycle */
	//LPC_SCT0->EV1_CTRL = (1 << 0) | (1 << 12); // 10000 0000 0001 = MATCH1 is trigger, rule MATCH
    //LPC_SCT0->EV1_STATE = 1;  // enable this event
    //LPC_SCT0->OUT0_SET = (1 << 1); // bit #1, meaning event #1 sets CTOUT0 to high

    /* Don't use states */
	//LPC_SCT0->STATE = 0;

    NVIC_SetVector((IRQn_Type)13, (uint32_t)&SCT0_1_IRQHandler);
    NVIC_EnableIRQ((IRQn_Type)13);


	/* Enable the interrupt */
	LPC_SCT0->EVEN = (1<<0); //bit 0 is 1. When flag register bit 0 is also 1, interrupt is requested

	/* Unhalt the counter to start */
	LPC_SCT0->CTRL &= ~(1 << 2); // bit 2, HALT When this bit is 1 counter does not run and no events can occur.




}

void initHSync2() {

    HSync.period(0.000032f); //30.80us
    HSync.pulsewidth(0.000028f); // 3.08us

    // halt and clear the counter
    LPC_SCT0->CTRL |= (1 << 2) | (1 << 3);

    NVIC_SetVector((IRQn_Type)13, (uint32_t)&SCT0_1_IRQHandler);
    NVIC_EnableIRQ((IRQn_Type)13);

    // Event 0 : MATCH and MATCHSEL=0
    LPC_SCT0->EV0_CTRL  = (1 << 12);
    //LPC_SCT0->EV0_STATE = 0x1;
    // Event 1 : MATCH and MATCHSEL=1
    LPC_SCT0->EV1_CTRL  = 0x1001; //(1 << 12) | (1 << 0);
    //LPC_SCT0->EV1_STATE = 0x2;

	/* Enable the interrupt */
	LPC_SCT0->EVEN = (1<<0); //bit 0 is 1. When flag register bit 0 is also 1, interrupt is requested

	/* Unhalt the counter to start */
	LPC_SCT0->CTRL &= ~(1 << 2); // bit 2, HALT When this bit is 1 counter does not run and no events can occur.


}

int main ()
{
int vsync_old=0;
VSync = 1;
//initHSync();
initHSync2();

	while (1)
	{
        if (vsync_counter == VSYNC_BACKPORCH_ENDS_DRAWING_BEGINS) {
                vsync_old = vsync_counter; /* store line we are on */
                for (int i=0; i<480; i++) {
                        /* draw line here, this has to be faster than the HSync frequency */
                        for (int j=0; j < 11; j++) {
                                /* should make a checkerboard-ish pattern */
                                if (j&1) SET_R;
                                else CLR_R;
                        };
                        /* set color off so that it is off during hsync*/
                        R = 0;
                        /* wait for a new line trigger */
                        while (vsync_counter == vsync_old) {};
                }
        }
	}
}
