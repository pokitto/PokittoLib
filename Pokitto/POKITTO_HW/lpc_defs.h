#ifndef LPCDEFS_H
#define LPCDEFS_H

#include <stdint.h>

#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */


#define LPC_PMU_BASE              0x40038000
#define LPC_IOCON_BASE            0x40044000
#define LPC_SYSCTL_BASE           0x40048000
#define LPC_GPIO_PORT_BASE        0xA0000000
#define LPC_GPIO_GROUP_INT0_BASE  0x4005C000
#define LPC_GPIO_GROUP_INT1_BASE  0x40060000
#define LPC_PIN_INT_BASE          0xA0004000
#define LPC_USART0_BASE           0x40008000
#define LPC_USART1_BASE           0x4006C000
#define LPC_USART2_BASE           0x40070000
#define LPC_USART3_BASE           0x40074000
#define LPC_USART4_BASE           0x4004C000
#define LPC_I2C0_BASE             0x40000000
#define LPC_I2C1_BASE             0x40020000
#define LPC_SSP0_BASE             0x40040000
#define LPC_SSP1_BASE             0x40058000
#define LPC_USB0_BASE             0x40080000
#define LPC_ADC_BASE              0x4001C000
#define LPC_SCT0_BASE             0x5000C000
#define LPC_SCT1_BASE             0x5000E000
#define LPC_TIMER16_0_BASE        0x4000C000
#define LPC_TIMER16_1_BASE        0x40010000
#define LPC_TIMER32_0_BASE        0x40014000
#define LPC_TIMER32_1_BASE        0x40018000
#define LPC_RTC_BASE              0x40024000
#define LPC_WWDT_BASE             0x40004000
#define LPC_DMA_BASE              0x50004000
#define LPC_CRC_BASE              0x50000000
#define LPC_FLASH_BASE            0x4003C000
#define LPC_DMATRIGMUX_BASE       0x40028000UL

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


#define LPC_SYSCTL_BASE           0x40048000

#define LPC_SYSCTL                ((LPC_SYSCTL_T           *) LPC_SYSCTL_BASE)



#define LPC_DMA                   ((LPC_DMA_T              *) LPC_DMA_BASE)

/* DMA channel source/address/next descriptor */
typedef struct {
	uint32_t  xfercfg;		/*!< Transfer configuration (only used in linked lists and ping-pong configs) */
	uint32_t  source;		/*!< DMA transfer source end address */
	uint32_t  dest;			/*!< DMA transfer desintation end address */
	uint32_t  next;			/*!< Link to next DMA descriptor, must be 16 byte aligned */
} DMA_CHDESC_T;


/* DMA channel mapping - each channel is mapped to an individual peripheral
   and direction or a DMA imput mux trigger */
typedef enum {
	SSP0_RX_DMA = 0,				/*!< SSP0 receive DMA channel */
	DMA_CH0 = SSP0_RX_DMA,
	DMAREQ_SSP0_TX,					/*!< SSP0 transmit DMA channel */
	DMA_CH1 = DMAREQ_SSP0_TX,
	DMAREQ_SSP1_RX,					/*!< SSP1 receive DMA channel */
	DMA_CH2 = DMAREQ_SSP1_RX,
	DMAREQ_SSP1_TX,					/*!< SSP1 transmit DMA channel */
	DMA_CH3 = DMAREQ_SSP1_TX,
	DMAREQ_USART0_RX,					/*!< USART0 receive DMA channel */
	DMA_CH4 = DMAREQ_USART0_RX,
	DMAREQ_USART0_TX,					/*!< USART0 transmit DMA channel */
	DMA_CH5 = DMAREQ_USART0_TX,
	DMAREQ_USART1_RX,					/*!< USART1 transmit DMA channel */
	DMA_CH6 = DMAREQ_USART1_RX,
	DMAREQ_USART1_TX,					/*!< USART1 transmit DMA channel */
	DMA_CH7 = DMAREQ_USART1_TX,
	DMAREQ_USART2_RX,					/*!< USART2 transmit DMA channel */
	DMA_CH8 = DMAREQ_USART2_RX,
	DMAREQ_USART2_TX,					/*!< USART2 transmit DMA channel */
	DMA_CH9 = DMAREQ_USART2_TX,
	DMAREQ_USART3_RX,					/*!< USART3 transmit DMA channel */
	DMA_CH10 = DMAREQ_USART3_RX,
	DMAREQ_USART3_TX,					/*!< USART3 transmit DMA channel */
	DMA_CH11 = DMAREQ_USART3_TX,
	DMAREQ_USART4_RX,					/*!< USART4 transmit DMA channel */
	DMA_CH12 = DMAREQ_USART4_RX,
	DMAREQ_USART4_TX,					/*!< USART4 transmit DMA channel */
	DMA_CH13 = DMAREQ_USART4_TX,
	DMAREQ_RESERVED_14,
	DMA_CH14 = DMAREQ_RESERVED_14,
	DMAREQ_RESERVED_15,
	DMA_CH15 = DMAREQ_RESERVED_15
} DMA_CHID_T;

/* On LPC412x, Max DMA channel is 22 */
#define MAX_DMA_CHANNEL			(DMA_CH15 + 1)

#undef _BIT
/* Set bit macro */
#define _BIT(n) (1 << (n))

/* _SBF(f,v) sets the bit field starting at position "f" to value "v".
 * _SBF(f,v) is intended to be used in "OR" and "AND" expressions:
 * e.g., "((_SBF(5,7) | _SBF(12,0xF)) & 0xFFFF)"
 */
#undef _SBF
/* Set bit field macro */
#define _SBF(f, v) ((v) << (f))

/* _BITMASK constructs a symbol with 'field_width' least significant
 * bits set.
 * e.g., _BITMASK(5) constructs '0x1F', _BITMASK(16) == 0xFFFF
 * The symbol is intended to be used to limit the bit field width
 * thusly:
 * <a_register> = (any_expression) & _BITMASK(x), where 0 < x <= 32.
 * If "any_expression" results in a value that is larger than can be
 * contained in 'x' bits, the bits above 'x - 1' are masked off.  When
 * used with the _SBF example above, the example would be written:
 * a_reg = ((_SBF(5,7) | _SBF(12,0xF)) & _BITMASK(16))
 * This ensures that the value written to a_reg is no wider than
 * 16 bits, and makes the code easier to read and understand.
 */
#undef _BITMASK
/* Bitmask creation macro */
#define _BITMASK(field_width) ( _BIT(field_width) - 1)

/* NULL pointer */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* Number of elements in an array */
#define NELEMENTS(array)  (sizeof(array) / sizeof(array[0]))

#define LPC_SCT0                  ((LPC_SCT_T              *) LPC_SCT0_BASE)
#define LPC_SCT1                  ((LPC_SCT_T              *) LPC_SCT1_BASE)
#define LPC_TIMER16_0             ((LPC_TIMER_T            *) LPC_TIMER16_0_BASE)
#define LPC_TIMER16_1             ((LPC_TIMER_T            *) LPC_TIMER16_1_BASE)
#define LPC_TIMER32_0             ((LPC_TIMER_T            *) LPC_TIMER32_0_BASE)
#define LPC_TIMER32_1             ((LPC_TIMER_T            *) LPC_TIMER32_1_BASE)

#endif
