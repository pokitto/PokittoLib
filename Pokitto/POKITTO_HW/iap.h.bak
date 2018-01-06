#ifndef IAP_H_INCLUDED
#define IAP_H_INCLUDED

extern int HelloFromIAP();
extern int CopyPageToFlash(uint32_t,uint8_t*);
extern char iaptest();
extern void IAPstacksave();
#define EEPROM_PROFILE 1
extern void writeEEPROM( uint8_t* eeAddress, uint8_t* buffAddress, uint32_t byteCount );
extern void readEEPROM( uint8_t* eeAddress, uint8_t* buffAddress, uint32_t byteCount );
extern uint8_t eeprom_read_byte(uint8_t*);
extern void eeprom_write_byte(uint8_t*,uint8_t);

/*****************************************************************************
 * $Id$
 *
 * Project:		NXP LPC1100 Secondary Bootloader Example
 *
 * Description: Provides access to In-Application Programming (IAP) routines
 * 			    		contained within the bootROM sector of LPC1100 devices.
 *
 * Copyright(C) 2010, NXP Semiconductor
 * All rights reserved.
 *
 *****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/

#include <stdint.h>

/* IAP Command Status Codes */
#define IAP_STA_CMD_SUCCESS 																0
#define IAP_STA_INVALID_COMMAND 														1
#define IAP_STA_SRC_ADDR_ERROR 															2
#define IAP_STA_DST_ADDR_ERROR 															3
#define IAP_STA_SRC_ADDR_NOT_MAPPED 												4
#define IAP_STA_DST_ADDR_NOT_MAPPED 												5
#define IAP_STA_COUNT_ERROR 																6
#define IAP_STA_INVALID_SECTOR 															7
#define IAP_STA_SECTOR_NOT_BLANK														8
#define IAP_STA_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 		9
#define IAP_STA_COMPARE_ERROR 															10
#define IAP_STA_BUSY 																				11
#define IAP_STA_INVALD_PARAM 																12

/* Define the flash page size, this is the minimum amount of data can be written in one operation */
#define IAP_FLASH_PAGE_SIZE_BYTES							256
#define IAP_FLASH_PAGE_SIZE_WORDS							(IAP_FLASH_PAGE_SIZE_BYTES >> 2)

void vIAP_ReinvokeISP(void);
uint32_t u32IAP_ReadPartID(uint32_t *pu32PartID);
uint32_t u32IAP_ReadBootVersion(uint32_t *pu32Major, uint32_t *pu32Minor);
//uint32_t u32IAP_ReadBootVersion(uint32_t *pu32Major);
uint32_t u32IAP_EraseSectors(uint32_t u32StartSector, uint32_t u32EndSector);
uint32_t u32IAP_PrepareSectors(uint32_t u32StartSector, uint32_t u32EndSector);
uint32_t u32IAP_CopyRAMToFlash(uint32_t u32DstAddr, uint32_t u32SrcAddr, uint32_t u32Len);
uint32_t u32IAP_BlankCheckSectors(uint32_t u32StartSector, uint32_t u32EndSector, uint32_t *pu32Result);
uint32_t u32IAP_Compare(uint32_t u32DstAddr, uint32_t u32SrcAddr, uint32_t u32Len, uint32_t *pu32Offset);
uint32_t u32IAP_ReadUID(uint32_t * pu32UID);
uint32_t u32IAP_ErasePage(uint32_t u32StartPage, uint32_t u32EndPage);

/*****************************************************************************
**                            End Of File
******************************************************************************/



#endif /* IAP_H_INCLUDED */

