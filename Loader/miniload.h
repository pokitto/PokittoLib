#ifndef MINILOAD_H
#define MINILOAD_H

#include <mbed.h>
#include "Pokitto_settings.h"
#include "PokittoDisk.h"
#include "miniGFX.h"
#include "minibuttons.h"
#include "iap.h"

#define APPL_ADDRESS ((uint32_t)0x00020000) /* Application start address */
#define IAP_CODE_START ((uint32_t)0x00030001) /* Iap routine start */
#define LOADER_ADDRESS ((uint32_t)0x00039000) /* Loader address */
//#define FAKEJUMP // fake the jump to absolute address in order for debugger to be able to step

extern void execute_code(unsigned);
extern void start_application(unsigned long);

#define LCD_CD_PORT           0
#define LCD_CD_PIN            2
#define LCD_WR_PORT           1
#define LCD_WR_PIN            12
#define LCD_RD_PORT           1
#define LCD_RD_PIN            24
#define LCD_RES_PORT          1
#define LCD_RES_PIN           0

/**************************************************************************/
/**                          LCD CONTROL MACROS                          **/
/**************************************************************************/

#define CLR_RESET LPC_GPIO_PORT->CLR[LCD_RES_PORT] = 1 << LCD_RES_PIN; //RST = (0); // Clear pin
#define SET_RESET LPC_GPIO_PORT->SET[LCD_RES_PORT] = 1 << LCD_RES_PIN; // RST = (1); // Set pin

#define CLR_CD  LPC_GPIO_PORT->CLR[LCD_CD_PORT] = 1 << LCD_CD_PIN; // RS = (0); // Clear pin
#define SET_CD LPC_GPIO_PORT->SET[LCD_CD_PORT] = 1 << LCD_CD_PIN; // RS = (1); // Set pin

#define CLR_WR { LPC_GPIO_PORT->CLR[LCD_WR_PORT] = 1 << LCD_WR_PIN; __asm("nop");}//WR = (0); // Clear pin
#define SET_WR LPC_GPIO_PORT->SET[LCD_WR_PORT] = 1 << LCD_WR_PIN; //WR = (1); // Set pin

#define CLR_RD LPC_GPIO_PORT->CLR[LCD_RD_PORT] = 1 << LCD_RD_PIN; //RD = (0); // Clear pin
#define SET_RD LPC_GPIO_PORT->SET[LCD_RD_PORT] = 1 << LCD_RD_PIN; //RD = (1); // Set pin

#define SET_CS  //CS tied to ground
#define CLR_CS

#define CLR_CS_CD_SET_RD_WR {CLR_CD; SET_RD; SET_WR;}
#define CLR_CS_SET_CD_RD_WR {SET_CD; SET_RD; SET_WR;}
#define SET_CD_RD_WR {SET_CD; SET_RD; SET_WR;}
#define SET_WR_CS SET_WR;

#define SET_MASK_P2 LPC_GPIO_PORT->MASK[2] = ~(0x7FFF8); //mask P2_3 ...P2_18
#define CLR_MASK_P2 LPC_GPIO_PORT->MASK[2] = 0; // all on

extern void m_initLCD();
extern void m_lcdClear();
extern const unsigned char fntC64[];
extern const unsigned char gfxC64[];

#define CORNER_LUP      65 // A
#define BAR_VERT        66 // B
#define CORNER_LDOWN    67 // C
#define BAR_HORIZ       68 // D
#define CORNER_RDOWN    69 // E
#define CORNER_RUP      70 // F
#define DASH_VERT       71 // G
#define HATCH_DIAG      72 // H

#endif

