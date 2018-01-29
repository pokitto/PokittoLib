#include "clock_11u6x.h"


/* Inprecise clock rates for the watchdog oscillator */
STATIC const uint32_t wdtOSCRate[WDTLFO_OSC_4_60 + 1] = {
	0,					/* WDT_OSC_ILLEGAL */
	600000,				/* WDT_OSC_0_60 */
	1050000,			/* WDT_OSC_1_05 */
	1400000,			/* WDT_OSC_1_40 */
	1750000,			/* WDT_OSC_1_75 */
	2100000,			/* WDT_OSC_2_10 */
	2400000,			/* WDT_OSC_2_40 */
	2700000,			/* WDT_OSC_2_70 */
	3000000,			/* WDT_OSC_3_00 */
	3250000,			/* WDT_OSC_3_25 */
	3500000,			/* WDT_OSC_3_50 */
	3750000,			/* WDT_OSC_3_75 */
	4000000,			/* WDT_OSC_4_00 */
	4200000,			/* WDT_OSC_4_20 */
	4400000,			/* WDT_OSC_4_40 */
	4600000				/* WDT_OSC_4_60 */
};

/* Compute a PLL frequency */
STATIC uint32_t Chip_Clock_GetPLLFreq(uint32_t PLLReg, uint32_t inputRate)
{
	uint32_t msel = ((PLLReg & 0x1F) + 1);

	return inputRate * msel;
}

/* Return System PLL output clock rate */
uint32_t Chip_Clock_GetSystemPLLOutClockRate(void)
{
	return Chip_Clock_GetPLLFreq(LPC_SYSCTL->SYSPLLCTRL,
								 Chip_Clock_GetSystemPLLInClockRate());
}


/* Compute a WDT rate */
STATIC uint32_t Chip_Clock_GetWDTRate(uint32_t reg)
{
	uint32_t div;
	CHIP_WDTLFO_OSC_T clk;

	/* Get WDT oscillator settings */
	clk = (CHIP_WDTLFO_OSC_T) ((reg >> 5) & 0xF);
	div = reg & 0x1F;

	/* Compute clock rate and divided by divde value */
	return wdtOSCRate[clk] / ((div + 1) << 1);
}

/* Return estimated watchdog oscillator rate */
uint32_t Chip_Clock_GetWDTOSCRate(void)
{
	return Chip_Clock_GetWDTRate(LPC_SYSCTL->WDTOSCCTRL);
}


/* Return System PLL input clock rate */
uint32_t Chip_Clock_GetSystemPLLInClockRate(void)
{
	uint32_t clkRate;

	switch ((CHIP_SYSCTL_PLLCLKSRC_T) (LPC_SYSCTL->SYSPLLCLKSEL & 0x3)) {
	case SYSCTL_PLLCLKSRC_IRC:
		clkRate = Chip_Clock_GetIntOscRate();
		break;

	case SYSCTL_PLLCLKSRC_MAINOSC:
		clkRate = Chip_Clock_GetMainOscRate();
		break;

	case SYSCTL_PLLCLKSRC_RTC32K:
		clkRate = Chip_Clock_GetRTCOscRate();
		break;

	default:
		clkRate = 0;
	}

	return clkRate;
}


/* Return main clock rate */
uint32_t Chip_Clock_GetMainClockRate(void)
{
	uint32_t clkRate = 0;

	switch ((CHIP_SYSCTL_MAINCLKSRC_T) (LPC_SYSCTL->MAINCLKSEL & 0x3)) {
	case SYSCTL_MAINCLKSRC_IRC:
		clkRate = Chip_Clock_GetIntOscRate();
		break;

	case SYSCTL_MAINCLKSRC_PLLIN:
		clkRate = Chip_Clock_GetSystemPLLInClockRate();
		break;

	case SYSCTL_MAINCLKSRC_WDTOSC:
		clkRate = Chip_Clock_GetWDTOSCRate();
		break;

	case SYSCTL_MAINCLKSRC_PLLOUT:
		clkRate = Chip_Clock_GetSystemPLLOutClockRate();
		break;
	}

	return clkRate;
}

/* Return system clock rate */
uint32_t Chip_Clock_GetSystemClockRate(void)
{
	/* No point in checking for divide by 0 */
	return Chip_Clock_GetMainClockRate() / LPC_SYSCTL->SYSAHBCLKDIV;
}

