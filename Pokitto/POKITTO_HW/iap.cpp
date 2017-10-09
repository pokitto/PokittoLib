#include <stdio.h>
#include <stdint.h>
#include <iap.h>
#include "LPC11U6x.h"
#include "PokittoDisk.h"

#define TICKRATE_HZ (10)	/* 10 ticks per second */
/* SystemTick Counter */
static volatile uint32_t sysTick;

/* LPC1347 IAP entry address */
#define IAP_LOCATION 0x1fff1ff1

#define last_sector_flash  0x00038000  //0x0000F000
#define IAP_LAST_SECTOR 28 /* Page number 896 - 1023, 0x00038000 - 0x0003FFFF */
#define IAP_NUM_BYTES_TO_WRITE 256
#define WRITECOUNT (IAP_NUM_BYTES_TO_WRITE / 4) /* when data array is in uint32_t */

#define IAP_PREWRRITE_CMD 50 /* Prepare sector for write operation command */
#define IAP_WRISECTOR_CMD 51
#define IAP_ERSSECTOR_CMD 52
#define IAP_REPID_CMD 54

/* IAP command variables */
static unsigned int command[5], result[4];

/* IAP entry function */
typedef int (*IAP)(unsigned int[], unsigned int[]);
IAP iap_entry = (IAP) IAP_LOCATION;

int CopyPageToFlash (uint32_t address, uint8_t* data) {
    IAP iap_call = (IAP) IAP_LOCATION;
	__disable_irq();

    unsigned int sector;
    bool firstpage=false;

    /* Calculate sector based on address */
    if (address < 0x18000) sector = address/0x1000; // sectors go in 4 k's
    else if (address >= 0x38000) sector = 28;
    else if (address >= 0x30000) sector = 27;
    else if (address >= 0x28000) sector = 26;
    else if (address >= 0x20000) sector = 25;
    else sector = 24;

    /* Check is it the first page in the sector */
    if (sector<24) {
        if (address == sector * 0x1000) firstpage = true;
    } else {
        if (address == (sector-24)*0x4000 + 0x18000) firstpage = true;
    }

	/* Prepare the sector for writing */
	command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
	command[1] = sector;         							/* Start Sector Number */
	command[2] = sector;		        					/* End Sector Number */
	iap_call(command, result);
    if (result[0]) return 1;

    /* do sector erase only when writing first page of given sector */
    if (firstpage) {
        /* Erase the last sector */
        command[0] = IAP_ERSSECTOR_CMD;	   					/* Erase command code*/
        command[1] = sector;             						/* Start Sector Number */
        command[2] = sector;            						/* End Sector Number */
        command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */
        iap_call(command, result);
        if (result[0]) return 1;
        /* Prepare to write/erase the last sector, needs to be done again because succesful erase re-locks sectors */
        command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
        command[1] = sector;			            				/* Start Sector Number */
        command[2] = sector;        							/* Start Sector Number */
        iap_call(command, result);
        if (result[0]) return 1;
    }

	/* Write data to the sectors */
	command[0] = IAP_WRISECTOR_CMD;						/* Write command code */
	command[1] = (uint32_t) (uint32_t*) address;              		    /* Destination Flash Address */
	command[2] = (uint32_t) data;	    				/* Source RAM Address */
	command[3] = 0x100;             					/* Number of Bytes to be written */
	command[4] = SystemCoreClock / 1000;				/* System clock frequency */
	iap_call(command, result);
    if (result[0]) return 1;

	/* Re-enable interrupt mode */
	__enable_irq();

    return 0; /*succesful write*/

}

__attribute__((section(".IAP_Code"))) int HelloFromIAP() {
    static uint32_t array_data[WRITECOUNT];
    int i;
    /* Initialize the array data to be written to FLASH */
	for (i = 0; i < WRITECOUNT; i++) {
		array_data[i] = 0xB007AB1E;
	}

    IAP iap_call = (IAP) IAP_LOCATION;
    uint8_t teahupoo;
    //readEEPROM(0,&teahupoo,1);
    teahupoo++;
    //writeEEPROM(0,&teahupoo,1);

    /** open file **/
    pokInitSD();
    char fn[20];
    char* now;
    now = (char*)last_sector_flash;
    switch (now[0]) {
case 0xAA:
    fn[0]='B';fn[1]='B';fn[2]='.';fn[3]='B';fn[4]='I';fn[5]='N';fn[6]='\0';break;
case 0xBB:
    fn[0]='C';fn[1]='C';fn[2]='.';fn[3]='B';fn[4]='I';fn[5]='N';fn[6]='\0';break;
default:
    fn[0]='A';fn[1]='A';fn[2]='.';fn[3]='B';fn[4]='I';fn[5]='N';fn[6]='\0';
    }
    if(fileOpen(fn,FILE_MODE_BINARY)) {
            return 1;
    } else {
     for (i = 0; i < WRITECOUNT; i++) {
		fileReadBytes((uint8_t*)&array_data[i],4);
	 }
    }


    /** write sector in flash **/
    /* Read Part Identification Number*/
	command[0] = IAP_REPID_CMD;								/* Read ID command code */
	iap_call(command, result);

	__disable_irq();

	/* Prepare to write/erase the last sector */
	command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* End Sector Number */
	iap_call(command, result);
	/* Erase the last sector */
	command[0] = IAP_ERSSECTOR_CMD;						/* Erase command code*/
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */
	iap_call(command, result);
	/* Prepare to write/erase the last sector */
	command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* Start Sector Number */
	iap_call(command, result);
	/* Write to the last sector */
	command[0] = IAP_WRISECTOR_CMD;								/* Write command code */
	command[1] = (uint32_t) last_sector_flash;		        /* Destination Flash Address */
	command[2] = (uint32_t) &array_data;					/* Source RAM Address */
	command[3] = IAP_NUM_BYTES_TO_WRITE;					/* Number of Bytes to be written */
	command[4] = SystemCoreClock / 1000;					/* System clock frequency */
	iap_call(command, result);

	/* Re-enable interrupt mode */
	__enable_irq();


    SCB->AIRCR = 0x05FA0004; //issue system reset
    //while(1); //should never come here
    return 0;
}




void IAPstacksave()
{
  /*need to save 32 top bytes of RAM to RAM1*/
  #define RAM1_0 (*((volatile unsigned long *) 0x20000000))
  #define RAM1_1 (*((volatile unsigned long *) 0x20000004))
  #define RAM1_2 (*((volatile unsigned long *) 0x20000008))
  #define RAM1_3 (*((volatile unsigned long *) 0x2000000C))
  #define RAM1_4 (*((volatile unsigned long *) 0x20000010))
  #define RAM1_5 (*((volatile unsigned long *) 0x20000014))
  #define RAM1_6 (*((volatile unsigned long *) 0x20000018))
  #define RAM1_7 (*((volatile unsigned long *) 0x2000001C))

  uint32_t *saveloc = (uint32_t*)(0x10002000-0x20); // RAM top - 32 bytes
  RAM1_0 = *saveloc++;
  RAM1_1 = *saveloc++;
  RAM1_2 = *saveloc++;
  RAM1_3 = *saveloc++;
  RAM1_4 = *saveloc++;
  RAM1_5 = *saveloc++;
  RAM1_6 = *saveloc++;
  RAM1_7 = *saveloc;
}


char iaptest() {
    static uint32_t array_data[WRITECOUNT];
    int i;
    /* Initialize the array data to be written to FLASH */
	for (i = 0; i < WRITECOUNT; i++) {
		array_data[i] = 0x11223340 + i;
	}

	/* Read Part Identification Number*/
	command[0] = IAP_REPID_CMD;								/* Read ID command code */
	iap_entry(command, result);

	/* Reinvoke ISP mode so that reprogamming of Flash possible */
	__disable_irq();

	command[0] = IAP_REPID_CMD;
	iap_entry(command, result);

	/* Prepare to write/erase the last sector */
	command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* End Sector Number */
	iap_entry(command, result);

	/* Erase the last sector */
	command[0] = IAP_ERSSECTOR_CMD;						/* Erase command code*/
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* Start Sector Number */
	iap_entry(command, result);

	/* Prepare to write/erase the last sector */
	command[0] = IAP_PREWRRITE_CMD;						/* Prepare to write/erase command code */
	command[1] = IAP_LAST_SECTOR;							/* Start Sector Number */
	command[2] = IAP_LAST_SECTOR;							/* Start Sector Number */
	iap_entry(command, result);

	/* Write to the last sector */
	command[0] = IAP_WRISECTOR_CMD;								/* Write command code */
	command[1] = (uint32_t) last_sector_flash;		/* Destination Flash Address */
	command[2] = (uint32_t) &array_data;					/* Source RAM Address */
	command[3] = IAP_NUM_BYTES_TO_WRITE;					/* Number of Bytes to be written */
	command[4] = SystemCoreClock / 1000;					/* System clock frequency */
	iap_entry(command, result);

	/* Re-enable interrupt mode */
	__enable_irq();

	//while (1) {
	//	__WFI();
	//}

	return 0;

}


//1) EEprom Write
//
//Command code: 61
//Param0: eeprom address (byte, half-word or word aligned)
//Param1: RAM address (byte, half-word or word aligned)
//Param2: Number of bytes to be written ( Byte, Half-words write are ok)
//Param3: System Clock Frequency (CCLK) in kHz
//
//Return Code CMD_SUCCESS | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED
__attribute__((section(".IAP_Code"))) void writeEEPROM( uint8_t* eeAddress, uint8_t* buffAddress, uint32_t byteCount )
{
	unsigned int command[5], result[4];

	command[0] = 61;
	command[1] = (uint32_t) eeAddress;
	command[2] = (uint32_t) buffAddress;
	command[3] = byteCount;
	command[4] = SystemCoreClock/1000;

	/* Invoke IAP call...*/
#if (EEPROM_PROFILE!=0)
	LPC_CT32B0->TCR = 1;
	__disable_irq();
  	iap_entry(command, result);
  	__enable_irq();
	LPC_CT32B0->TCR = 0;
#else
    __disable_irq();
	iap_entry(command, result);
	__enable_irq();
#endif
	if (0 != result[0])
	{
		//Trap error
		while(1);
	}
	return;
}

//2) EEprom Read
//Command code: 62
//Param0: eeprom address (byte, half-word or word aligned)
//Param1: RAM address (byte, half-word or word aligned)
//Param2: Number of bytes to be read ( Byte, Half-words read are ok)
//Param3: System Clock Frequency (CCLK) in kHz
//
//Return Code CMD_SUCCESS | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED
__attribute__((section(".IAP_Code"))) void readEEPROM( uint8_t* eeAddress, uint8_t* buffAddress, uint32_t byteCount )
{
	unsigned int command[5], result[4];

	command[0] = 62;
	command[1] = (uint32_t) eeAddress;
	command[2] = (uint32_t) buffAddress;
	command[3] = byteCount;
	command[4] = SystemCoreClock/1000;

	/* Invoke IAP call...*/
	__disable_irq();
  	iap_entry( command, result);
  	__enable_irq();
	if (0 != result[0])
	{
		//Trap error
		while(1);
	}
	return;
}

__attribute__((section(".IAP_Code"))) void IAPreadPartId( uint8_t* eeAddress, uint8_t* buffAddress, uint32_t byteCount )
{
	unsigned int command[5], result[4];

	command[0] = 62;
	command[1] = (uint32_t) eeAddress;
	command[2] = (uint32_t) buffAddress;
	command[3] = byteCount;
	command[4] = SystemCoreClock/1000;

	/* Invoke IAP call...*/
	__disable_irq();
  	iap_entry( command, result);
  	__enable_irq();
	if (0 != result[0])
	{
		//Trap error
		while(1);
	}
	return;
}

uint8_t eeprom_read_byte(uint8_t* index) {
    uint8_t val;
    readEEPROM(index,&val,1);
    return val;
}

void eeprom_write_byte(uint8_t*index , uint8_t val) {
    writeEEPROM(index,&val,1);
}

/*****************************************************************************
 * $Id$
 *
 * Project: 	NXP LPC11U6x In Application Programming
 *
 * Description: Provides access to In-Application Programming (IAP) routines
 * 			    contained within the bootROM sector of LPC11U6x devices.
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

/* IAP Command Definitions */
#define	IAP_CMD_PREPARE_SECTORS			  50
#define	IAP_CMD_COPY_RAM_TO_FLASH		  51
#define	IAP_CMD_ERASE_SECTORS			    52
#define	IAP_CMD_BLANK_CHECK_SECTORS		53
#define	IAP_CMD_READ_PART_ID			    54
#define	IAP_CMD_READ_BOOT_ROM_VERSION	55
#define	IAP_CMD_COMPARE					      56
#define	IAP_CMD_REINVOKE_ISP			    57
#define IAP_CMD_READ_UID              58

#define IAP_CMD_ERASE_PAGE            59    //new

/* IAP boot ROM location and access function */
#define IAP_ROM_LOCATION				0x1FFF1FF1UL
//#define IAP_EXECUTE_CMD(a, b)			((void (*)())(IAP_ROM_LOCATION))(a, b)

__attribute__((section(".IAP_Code"))) void IAP_EXECUTE_CMD(uint32_t* a, uint32_t* b) {
    void (*user_code_entry)(uint32_t*,uint32_t*);
    uint32_t *p;
    p = (uint32_t *)IAP_ROM_LOCATION;
    user_code_entry = (void (*)(uint32_t*,uint32_t*))(*p);
    user_code_entry(a, b);
}


/*****************************************************************************
** Function name:	u32IAP_PrepareSectors
**
** Description:		Prepares sector(s) for erasing or write operations. This
** 								command must be executed before executing the "Copy RAM to
** 								Flash" or "Erase Sector(s)" commands.
**
** Parameters:		u32StartSector - Number of first sector to prepare.
** 								u32EndSector - Number of last sector to prepare.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_PrepareSectors(uint32_t u32StartSector, uint32_t u32EndSector)
{
	uint32_t u32Status;
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	if (u32EndSector < u32StartSector)
	{
		u32Status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		au32Command[0] = IAP_CMD_PREPARE_SECTORS;
		au32Command[1] = u32StartSector;
		au32Command[2] = u32EndSector;
		__disable_irq();
		IAP_EXECUTE_CMD(au32Command, au32Result);
		__enable_irq();
		u32Status = au32Result[0];
	}
	return u32Status;
}

/*****************************************************************************
** Function name:	u32IAP_CopyRAMToFlash
**
** Description:		Program the flash memory with data stored in RAM.
**
** Parameters:	   	u32DstAddr - Destination Flash address, should be a 256
**                               byte boundary.
**			 		u32SrcAddr - Source RAM address, should be a word boundary
**			 		u32Len     - Number of 8-bit bytes to write, must be a
**			 					 multiple of 256.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_CopyRAMToFlash(uint32_t u32DstAddr, uint32_t u32SrcAddr, uint32_t u32Len)
{
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_COPY_RAM_TO_FLASH;
	au32Command[1] = u32DstAddr;
	au32Command[2] = u32SrcAddr;
	au32Command[3] = u32Len;
	au32Command[4] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

	IAP_EXECUTE_CMD(au32Command, au32Result);

	return au32Result[0];
}

/*****************************************************************************
** Function name:	u32IAP_EraseSectors
**
** Description:		Erase a sector or multiple sectors of on-chip Flash memory.
**
** Parameters:		u32StartSector - Number of first sector to erase.
** 					u32EndSector - Number of last sector to erase.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_EraseSectors(uint32_t u32StartSector, uint32_t u32EndSector)
{
	uint32_t u32Status;
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	if (u32EndSector < u32StartSector)
	{
		u32Status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		au32Command[0] = IAP_CMD_ERASE_SECTORS;
		au32Command[1] = u32StartSector;
		au32Command[2] = u32EndSector;
		au32Command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

		IAP_EXECUTE_CMD(au32Command, au32Result);

		u32Status = au32Result[0];
	}
	return u32Status;
}

/*****************************************************************************
** Function name:	u32IAP_BlankCheckSectors
**
** Description:		Blank check a sector or multiple sectors of on-chip flash
** 					memory.
**
** Parameters:		u32StartSector - Number of first sector to check.
** 					u32EndSector - Number of last sector to check.
** 					pu32Result[0] - Offset of the first non blank word location
**                  if the Status Code is IAP_STA_SECTOR_NOT_BLANK.
** 					pu32Result[1] - Contents of non blank word location.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_BlankCheckSectors(uint32_t u32StartSector, uint32_t u32EndSector, uint32_t *pu32Result)
{
	uint32_t u32Status;
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	if (u32EndSector < u32StartSector)
	{
		u32Status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		au32Command[0] = IAP_CMD_BLANK_CHECK_SECTORS;
		au32Command[1] = u32StartSector;
		au32Command[2] = u32EndSector;

		IAP_EXECUTE_CMD(au32Command, au32Result);

		if (au32Result[0] == IAP_STA_SECTOR_NOT_BLANK)
		{
			*pu32Result       = au32Result[0];
			*(pu32Result + 1) = au32Result[1];
		}
		u32Status = au32Result[0];
	}
	return u32Status;
}

/*****************************************************************************
** Function name:	u32IAP_ReadPartID
**
** Description:		Read the part identification number.
**
** Parameters:		pu32PartID - Pointer to storage for part ID number.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_ReadPartID(uint32_t *pu32PartID)
{
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_READ_PART_ID;
	__disable_irq();
	IAP_EXECUTE_CMD(au32Command, au32Result);
	__enable_irq();
	*pu32PartID = au32Result[1];

	return au32Result[0];
}

/*****************************************************************************
** Function name:	u32IAP_ReadBootVersion
**
** Description:		Read the boot code version number.
**
** Parameters:		pu32Major - Major version number in ASCII format.
** 					pu32Minor - Minor version number in ASCII format.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_ReadBootVersion(uint32_t *pu32Major, uint32_t *pu32Minor)
//uint32_t u32IAP_ReadBootVersion(uint32_t *pu32Major)
{
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_READ_BOOT_ROM_VERSION;

	IAP_EXECUTE_CMD(au32Command, au32Result);


	*pu32Major = (au32Result[1] & 0x0000FF00UL) >> 8;
	*pu32Minor = au32Result[1] & 0x000000FFUL;

	return au32Result[0];
}

/*****************************************************************************
** Function name:	u32IAP_Compare
**
** Description:		Compares the memory contents at two locations.
**
** Parameters:		u32Len - Number of bytes to compare, must be a multiple of 4.
**					pu32Offset - Offset of the first mismatch if the Status Code is COMPARE_ERROR
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_Compare(uint32_t u32DstAddr, uint32_t u32SrcAddr, uint32_t u32Len, uint32_t *pu32Offset)
{
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_COMPARE;
	au32Command[1] = u32DstAddr;
	au32Command[2] = u32SrcAddr;
	au32Command[3] = u32Len;

	IAP_EXECUTE_CMD(au32Command, au32Result);

	if (au32Result[0] == IAP_STA_COMPARE_ERROR)
	{
		if (pu32Offset != 0)
		{
			*pu32Offset = au32Result[1];
		}
	}
	return au32Result[0];
}

/*****************************************************************************
** Function name:	vIAP_ReinvokeISP
**
** Description:		Invoke the bootloader in ISP mode.
**
** Parameters:		None.
*
** Returned value:	None.
**
******************************************************************************/
__attribute__((section(".IAP_Code"))) void vIAP_ReinvokeISP(void)
{
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_REINVOKE_ISP;

	IAP_EXECUTE_CMD(au32Command, au32Result);
}

// read UID
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_ReadUID(uint32_t * pu32UID)
{
	uint32_t au32Result[5];
	uint32_t au32Command[5];

	au32Command[0] = IAP_CMD_READ_UID;

	IAP_EXECUTE_CMD(au32Command, au32Result);
//	*pu32UID++ =  au32Result[1];
//	*pu32UID++ =  au32Result[2];
//	*pu32UID++ =  au32Result[3];
//	*pu32UID =  au32Result[4];

	*pu32UID =  au32Result[1];
	*pu32UID++ =  au32Result[2];
	*pu32UID++ =  au32Result[3];
	*pu32UID++ =  au32Result[4];

	return au32Result[0];

}

//IAP erase	Page  256B	 64K have 0-255 pages, page0-15 in sector 0,	32K have 0-127 pages, 128k have 0-511 pages,
__attribute__((section(".IAP_Code"))) uint32_t u32IAP_ErasePage(uint32_t u32StartPage, uint32_t u32EndPage)
{
	uint32_t u32Status;
	uint32_t au32Result[3];
	uint32_t au32Command[5];

	if (u32EndPage < u32StartPage)
	{
		u32Status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		au32Command[0] = IAP_CMD_ERASE_PAGE;
		au32Command[1] = u32StartPage;
		au32Command[2] = u32EndPage;
		au32Command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

		IAP_EXECUTE_CMD(au32Command, au32Result);

		u32Status = au32Result[0];
	}
	return u32Status;
}


/*****************************************************************************
 **                            End Of File
 *****************************************************************************/

