/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "ffconf.h"
#include "fileff.h"
#include "./minilpc.h"
#include "file_diskio.h"

namespace YAPFS
{

DWORD get_fattime(void) {
    return ((volatile uint32_t*)0x40024000)[2];
}

/** Represents the different SD/MMC card types
 */
enum CardType {
    CARD_NONE,      /**< No card is present */
    CARD_MMC,       /**< MMC card */
    CARD_SD,        /**< Standard capacity SD card */
    CARD_SDHC,      /**< High capacity SD card */
    CARD_UNKNOWN    /**< Unknown or unsupported card */
} m_CardType;

enum Command {
    CMD0 = (0x40 | 0),      /**< GO_IDLE_STATE */
    CMD1 = (0x40 | 1),      /**< SEND_OP_COND */
    CMD8 = (0x40 | 8),      /**< SEND_IF_COND */
    CMD9 = (0x40 | 9),      /**< SEND_CSD */
    CMD12 = (0x40 | 12),    /**< STOP_TRANSMISSION */
    CMD13 = (0x40 | 13),    /**< SEND_STATUS */
    CMD16 = (0x40 | 16),    /**< SET_BLOCKLEN */
    CMD17 = (0x40 | 17),    /**< READ_SINGLE_BLOCK */
    CMD18 = (0x40 | 18),    /**< READ_MULTIPLE_BLOCK */
    ACMD22 = (0x40 | 22),   /**< SEND_NUM_WR_BLOCKS */
    ACMD23 = (0x40 | 23),   /**< SET_WR_BLK_ERASE_COUNT */
    CMD24 = (0x40 | 24),    /**< WRITE_BLOCK */
    CMD25 = (0x40 | 25),    /**< WRITE_MULTIPLE_BLOCK */
    ACMD41 = (0x40 | 41),   /**< SD_SEND_OP_COND */
    ACMD42 = (0x40 | 42),   /**< SET_CLR_CARD_DETECT */
    CMD55 = (0x40 | 55),    /**< APP_CMD */
    CMD58 = (0x40 | 58),    /**< READ_OCR */
    CMD59 = (0x40 | 59)     /**< CRC_ON_OFF */
};


static int m_Status = STA_NOINIT;
static int m_CdAssert;
static YAPFS::FATFS _fs;

static volatile uint8_t * const pSPI = (uint8_t *) 0x40040000;
static volatile uint16_t * const pSPI16 = (uint16_t *) 0x40040000;

static void initSPI(){
    LPC_SYSCON->SYSAHBCLKCTRL |= 1 << 11;
    LPC_SYSCON->SSP0CLKDIV = 0x01;
    LPC_SYSCON->PRESETCTRL |= 1 << 0;

    LPC_IOCON->PIO0_9 = (LPC_IOCON->PIO0_9 & ~(0x7)) | 0x1;   
    LPC_IOCON->PIO0_8  = (LPC_IOCON->PIO0_8 & ~(0x7)) | 0x1;
    LPC_IOCON->PIO0_6 = (LPC_IOCON->PIO0_6 & ~(0x7)) | 0x2; 

    LPC_SSP0->CR0 |= 0x07; 
    LPC_SSP0->CPSR =2; // 25MHz
    LPC_SSP0->CR1 |= (1 << 1); //enable SPI0
}

static inline int m_Spiread16( ){
  while( !(pSPI16[6] & (1<<1)) ); // wait until writeable
  pSPI16[4] = 0xFFFF; // write
  while( !(pSPI16[6] & (1<<2)) ); // wait until readable
  return pSPI16[4]; // read
}

static inline int m_Spiread( ){
  while( !(pSPI[12] & (1<<1)) ); // wait until writeable
  pSPI[8] = 0xFF; // write
  while( !(pSPI[12] & (1<<2)) ); // wait until readable
  return pSPI[8]; // read
}

static inline int m_Spiwrite( int value ){
  while( !(pSPI[12] & (1<<1)) ); // wait until writeable
  pSPI[8] = value; // write
  while( !(pSPI[12] & (1<<2)) ); // wait until readable
  return pSPI[8]; // read
}

inline bool waitReady(int timeout)
{
    char resp;

    //Keep sending dummy clocks with DI held high until the card releases the DO line
    do {
        resp = m_Spiwrite(0xFF);
    } while (resp == 0x00);

    //Return success/failure
    return (resp > 0x00);
}

inline void deselect()
{
    //Deassert /CS
    LPC_GPIO_PORT->SET[0] = 1 << 7; //m_Cs = 1;

    //Send 8 dummy clocks with DI held high to disable DO
    m_Spiwrite(0xFF);
}

inline bool select()
{
    //Assert /CS
    LPC_GPIO_PORT->CLR[0] = 1 << 7; //m_Cs = 0;

    //Send 8 dummy clocks with DI held high to enable DO
    m_Spiwrite(0xFF);

    //Wait for up to 500ms for the card to become ready
    if (waitReady(500)) {
        return true;
    } else {
        //We timed out, deselect and return false
        deselect();
        return false;
    }
}

char writeCommand(char cmd, unsigned int arg, unsigned int* resp = nullptr)
{
    char token;

    //Try to send the command up to 3 times
    for (int f = 0; f < 3; f++) {
        //Send CMD55(0x00000000) prior to an application specific command
        if (cmd == ACMD22 || cmd == ACMD23 || cmd == ACMD41 || cmd == ACMD42) {
            token = writeCommand(CMD55, 0x00000000);
            if (token > 0x01)
                return token;

            //Deselect and reselect the card between CMD55 and an ACMD
            deselect();
            if(!select())
                return 0xFF;
        }

        //Prepare the command packet
        char cmdPacket[6];
        cmdPacket[0] = cmd;
        cmdPacket[1] = arg >> 24;
        cmdPacket[2] = arg >> 16;
        cmdPacket[3] = arg >> 8;
        cmdPacket[4] = arg;
        if ( cmd == CMD0 )
	    cmdPacket[5] = 0x95;
	else if ( cmd == CMD8 )
            cmdPacket[5] = 0x87;
        else
            cmdPacket[5] = 0x01;

        //Send the command packet
        for (int i = 0; i < 6; i++)
            m_Spiwrite(cmdPacket[i]);

        //Discard the stuff byte immediately following CMD12
        if (cmd == CMD12)
            m_Spiwrite(0xFF);

        //Allow up to 8 bytes of delay for the R1 response token
        for (int i = 0; i < 9; i++) {
            token = m_Spiwrite(0xFF);
            if (!(token & 0x80))
                break;
        }

        //Verify the R1 response token
        if (token == 0xFF) {
            //No data was received, get out early
            break;
        } else if (token & (1 << 3)) {
            //There was a CRC error, try again
            continue;
        } else if (token > 0x01) {
            //An error occured, get out early
            break;
        }

        //Handle R2 and R3/R7 response tokens
        if (cmd == CMD13 && resp != 0) {
            //Read the R2 response value
            *resp = m_Spiwrite(0xFF);//m_Spi.write(0xFF);
        } else if ((cmd == CMD8 || cmd == CMD58) && resp != 0) {
            //Read the R3/R7 response value
            /* 
            *resp = (m_Spi.write(0xFF) << 24);
            *resp |= (m_Spi.write(0xFF) << 16);
            *resp |= (m_Spi.write(0xFF) << 8);
            *resp |= m_Spi.write(0xFF);
            */
        
            *resp = (m_Spiwrite(0xFF) << 24);
            *resp |= (m_Spiwrite(0xFF) << 16);
            *resp |= (m_Spiwrite(0xFF) << 8);
            *resp |= m_Spiwrite(0xFF);
        }

        //The command was successful
        break;
    }

    //Return the R1 response token
    return token;
}


inline char commandTransaction(char cmd, unsigned int arg, unsigned int* resp = nullptr)
{
    //Select the card, and wait for ready
    if(!select())
        return 0xFF;

    //Perform the command transaction
    char token = writeCommand(cmd, arg, resp);

    //Deselect the card, and return the R1 response token
    deselect();
    return token;
}

bool readData(char* buffer, int length)
{
    char token;
    unsigned short crc;

    //Wait for up to 500ms for a token to arrive
    do {
        token = m_Spiwrite(0xFF);//m_Spi.write(0xFF);
    } while (token == 0xFF );

    //Check if a valid start block token was received
    if (token != 0xFE)
        return false;
/*
    for (int i = 0; i < length; i++)
        buffer[i] = m_Spiwrite(0xFF);
*/
    //Check if large frames are enabled or not
    //if (m_LargeFrames) {
/* 
    //Switch to 16-bit frames for better performance
        m_Spi.format(16, 0);

	
        //Read the data block into the buffer
        unsigned short dataWord;
        for (int i = 0; i < length; i += 2) {
	    dataWord = m_Spiread16(); // read
            buffer[i] = dataWord >> 8;
            buffer[i + 1] = dataWord;
        }
    */

    while( !(pSPI[12] & (1<<1)) ); // wait until writeable
    volatile void *SPI = pSPI;
    int tmp=0;
    asm volatile(
        ".syntax unified" "\n"
        "next%=:" "\n"

        "strb %[clock], [ %[SPI], 8 ]" "\n"

        "readable%=:" "\n"
        "ldrh %[tmp], [ %[SPI], 12 ]" "\n"
        "lsls %[tmp], 29" "\n"
        "bpl readable%=" "\n"

        "ldrb %[tmp], [ %[SPI], 8 ]" "\n"

        "strb %[tmp], [ %[buffer], 0 ]" "\n"
        //"lsrs %[tmp], 8" "\n"
        //"strb %[tmp], [ %[buffer], 0 ]" "\n"

        "adds %[buffer], 1" "\n"

        "subs %[remain], 1" "\n"
        "bne next%=" "\n"
        : // outputs
        [tmp]"+l"(tmp),
        [remain]"+l"(length),
        [buffer]"+l"(buffer)
        : // inputs
        [SPI]"l"(SPI),
        [clock]"l"(0xFF)
        : // clobbers
        "cc"
        );

        //Read the CRC16 checksum for the data block
        crc = (m_Spiwrite(0xFF) << 8);
        crc |= m_Spiwrite(0xFF);
    //}

    //Return the validity of the CRC16 checksum (if enabled)
    return true;
   
}

char writeData(const char* buffer, char token)
{
    //Calculate the CRC16 checksum for the data block (if enabled)
    unsigned short crc = 0xFFFF;

    //Wait for up to 500ms for the card to become ready
    if (!waitReady(500))
        return false;

    //Send the start block token
    m_Spiwrite(token);

    //Check if large frames are enabled or not
    /*
    if (m_LargeFrames) {
        //Switch to 16-bit frames for better performance
        m_Spi.format(16, 0);

        //Write the data block from the buffer
        for (int i = 0; i < 512; i += 2)
            m_Spiwrite((buffer[i] << 8) | buffer[i + 1]);

        //Send the CRC16 checksum for the data block
        m_Spiwrite(crc);

        //Switch back to 8-bit frames
        m_Spi.format(8, 0);
    /*/
    //Write the data block from the buffer
    for (int i = 0; i < 512; i++)
        m_Spiwrite(buffer[i]);

    //Send the CRC16 checksum for the data block
    m_Spiwrite(crc >> 8);
    m_Spiwrite(crc);
    /**/

    //Return the data response token
    return (m_Spiwrite(0xFF) & 0x1F);
}

inline bool readBlock(char* buffer, unsigned int lba)
{
    //Try to read the block up to 3 times
    for (int f = 0; f < 3; f++) {
        //Select the card, and wait for ready
        if(!select())
            break;

        //Send CMD17(block) to read a single block
        if (writeCommand(CMD17, (m_CardType == CARD_SDHC) ? lba : lba << 9) == 0x00) {
            //Try to read the block, and deselect the card
            bool success = readData(buffer, 512);
            deselect();

            //Return if successful
            if (success)
                return true;
        } else {
            //The command failed, get out
            break;
        }
    }

    //The single block read failed
    deselect();
    return false;
}

inline bool readBlocks(char* buffer, unsigned int lba, unsigned int count)
{
    //Try to read each block up to 3 times
    for (int f = 0; f < 3;) {
        //Select the card, and wait for ready
        if(!select())
            break;

        //Send CMD18(block) to read multiple blocks
        if (writeCommand(CMD18, (m_CardType == CARD_SDHC) ? lba : lba << 9) == 0x00) {
            //Try to read all of the data blocks
            do {
                //Read the next block, and break on errors
                if (!readData(buffer, 512)) {
                    f++;
                    break;
                }

                //Update the variables
                lba++;
                buffer += 512;
                f = 0;
            } while (--count);

            //Send CMD12(0x00000000) to stop the transmission
            if (writeCommand(CMD12, 0x00000000) != 0x00) {
                //The command failed, get out
                break;
            }

            //Deselect the card, and return if successful
            deselect();
            if (count == 0)
                return true;
        } else {
            //The command failed, get out
            break;
        }
    }

    //The multiple block read failed
    deselect();
    return false;
}

inline bool writeBlock(const char* buffer, unsigned int lba)
{
    //Try to write the block up to 3 times
    for (int f = 0; f < 3; f++) {
        //Select the card, and wait for ready
        if(!select())
            break;

        //Send CMD24(block) to write a single block
        if (writeCommand(CMD24, (m_CardType == CARD_SDHC) ? lba : lba << 9) == 0x00) {
            //Try to write the block, and deselect the card
            char token = writeData(buffer, 0xFE);
            deselect();

            //Check the data response token
            if (token == 0x0A) {
                //A CRC error occured, try again
                continue;
            } else if (token == 0x0C) {
                //A write error occured, get out
                break;
            }

            //The data was written successfully
            return true;
        } else {
            //The command failed, get out
            break;
        }
    }

    //The single block write failed
    deselect();
    return false;
}

inline bool writeBlocks(const char* buffer, unsigned int lba, unsigned int count)
{
    char token;
    const char* currentBuffer = buffer;
    unsigned int currentLba = lba;
    int currentCount = count;

    //Try to write each block up to 3 times
    for (int f = 0; f < 3;) {
        //If this is an SD card, send ACMD23(count) to set the number of blocks to pre-erase
        if (m_CardType != CARD_MMC) {
            if (commandTransaction(ACMD23, currentCount) != 0x00) {
                //The command failed, get out
                break;
            }
        }

        //Select the card, and wait for ready
        if(!select())
            break;

        //Send CMD25(block) to write multiple blocks
        if (writeCommand(CMD25, (m_CardType == CARD_SDHC) ? currentLba : currentLba << 9) == 0x00) {
            //Try to write all of the data blocks
            do {
                //Write the next block and break on errors
                token = writeData(currentBuffer, 0xFC);
                if (token != 0x05) {
                    f++;
                    break;
                }

                //Update the variables
                currentBuffer += 512;
                f = 0;
            } while (--currentCount);

            //Wait for up to 500ms for the card to finish processing the last block
            if (!waitReady(500))
                break;

            //Finalize the transmission
            if (currentCount == 0) {
                //Send the stop tran token, and deselect the card
                m_Spiwrite(0xFD);
                deselect();

                //The data was written successfully
                return true;
            } else {
                //Send CMD12(0x00000000) to abort the transmission
                if (writeCommand(CMD12, 0x00000000) != 0x00) {
                    //The command failed, get out
                    break;
                }

                //Deselect the card
                deselect();

                //Check the error token
                if (token == 0x0A) {
                    //Determine the number of well written blocks if possible
                    unsigned int writtenBlocks = 0;
                    if (m_CardType != CARD_MMC && select()) {
                        //Send ACMD22(0x00000000) to get the number of well written blocks
                        if (writeCommand(ACMD22, 0x00000000) == 0x00) {
                            //Read the data
                            char acmdData[4];
                            if (readData(acmdData, 4)) {
                                //Extract the number of well written blocks
                                writtenBlocks = acmdData[0] << 24;
                                writtenBlocks |= acmdData[1] << 16;
                                writtenBlocks |= acmdData[2] << 8;
                                writtenBlocks |= acmdData[3];
                            }
                        }
                        deselect();
                    }

                    //Roll back the variables based on the number of well written blocks
                    currentBuffer = buffer + (writtenBlocks << 9);
                    currentLba = lba + writtenBlocks;
                    currentCount = count - writtenBlocks;

                    //Try again
                    continue;
                } else {
                    //A write error occured, get out
                    break;
                }
            }
        } else {
            //The command failed, get out
            break;
        }
    }

    //The multiple block write failed
    deselect();
    return false;
}


static inline void checkSocket(){
    m_Status &= ~STA_NODISK;
}

static bool disk_sync(){
    if (select()) {
        deselect();
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}

static uint64_t disk_sectors()
{
    //Make sure the card is initialized before proceeding
    if (m_Status & STA_NOINIT)
        return 0;

    //Try to read the CSD register up to 3 times
    for (int f = 0; f < 3; f++) {
        //Select the card, and wait for ready
        if(!select())
            break;

        //Send CMD9(0x00000000) to read the CSD register
        if (writeCommand(CMD9, 0x00000000) == 0x00) {
            //Read the 16B CSD data block
            char csd[16];
            bool success = readData(csd, 16);
            deselect();
            if (success) {
                //Calculate the sector count based on the card type
                if ((csd[0] >> 6) == 0x01) {
                    //Calculate the sector count for a high capacity card
                    unsigned int size = (((csd[7] & 0x3F) << 16) | (csd[8] << 8) | csd[9]) + 1;
                    return size << 10;
                } else {
                    //Calculate the sector count for a standard capacity card
                    unsigned int size = (((csd[6] & 0x03) << 10) | (csd[7] << 2) | ((csd[8] & 0xC0) >> 6)) + 1;
                    size <<= ((((csd[9] & 0x03) << 1) | ((csd[10] & 0x80) >> 7)) + 2);
                    size <<= (csd[5] & 0x0F);
                    return size >> 9;
                }
            }
        } else {
            //The command failed, get out
            break;
        }
    }

    //The read operation failed 3 times
    deselect();
    return 0;
}

DSTATUS disk_initialize (
    BYTE drv                /* Physical drive nmuber (0..) */
){
    char token;
    unsigned int resp;

    //Make sure there's a card in the socket before proceeding
    checkSocket();
    if (m_Status & STA_NODISK)
        return m_Status;

    //Make sure we're not already initialized before proceeding
    if (!(m_Status & STA_NOINIT))
        return m_Status;

    //Send 80 dummy clocks with /CS deasserted and DI held high
    //m_Cs = 1;
    LPC_GPIO_PORT->SET[0] = 1 << 7; 

    for (int i = 0; i < 10; i++)
        m_Spiwrite(0xFF);

    //Send CMD0(0x00000000) to reset the card
    if (commandTransaction(CMD0, 0x00000000) != 0x01) {
        //Initialization failed
        m_CardType = CARD_UNKNOWN;
        return m_Status;
    }

    //Send CMD8(0x000001AA) to see if this is an SDCv2 card
    if (commandTransaction(CMD8, 0x000001AA, &resp) == 0x01) {
        //This is an SDCv2 card, get the 32-bit return value and verify the voltage range/check pattern
        if ((resp & 0xFFF) != 0x1AA) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }

        //Send CMD58(0x00000000) to read the OCR, and verify that the card supports 3.2-3.3V
        if (commandTransaction(CMD58, 0x00000000, &resp) != 0x01 || !(resp & (1 << 20))) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }

        //Send ACMD41(0x40100000) repeatedly for up to 1 second to initialize the card
        //m_Timer.start();
        do {
            token = commandTransaction(ACMD41, 0x40100000);
        } while (token == 0x01);
        //m_Timer.stop();
        //m_Timer.reset();

        //Check if the card initialized
        if (token != 0x00) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }

        //Send CMD58(0x00000000) to read the OCR
        if (commandTransaction(CMD58, 0x00000000, &resp) == 0x00) {
            //Check the CCS bit to determine if this is a high capacity card
            if (resp & (1 << 30))
                m_CardType = CARD_SDHC;
            else
                m_CardType = CARD_SD;

            //Increase the SPI frequency to full speed (up to 25MHz for SDCv2)
            //if (m_FREQ > 25000000)
                //m_Spi.frequency(25000000);
            //    my_spi_frequency(400000);
            //else
                //m_Spi.frequency(m_FREQ);
            //    my_spi_frequency(m_FREQ);
        } else {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }
    } else {
        //Didn't respond or illegal command, this is either an SDCv1 or MMC card
        //Send CMD58(0x00000000) to read the OCR, and verify that the card supports 3.2-3.3V
        if (commandTransaction(CMD58, 0x00000000, &resp) != 0x01 || !(resp & (1 << 20))) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }

        //Try to initialize the card using ACMD41(0x00100000) for 1 second
        //m_Timer.start();
        do {
            token = commandTransaction(ACMD41, 0x00100000);
        } while (token == 0x01);
        //m_Timer.stop();
        //m_Timer.reset();

        //Check if the card initialized
        if (token == 0x00) {
            //This is an SDCv1 standard capacity card
            m_CardType = CARD_SD;
        } else {
            //Try to initialize the card using CMD1(0x00100000) for 1 second
            do {
                token = commandTransaction(CMD1, 0x00100000);
            } while (token == 0x01);

            //Check if the card initialized
            if (token == 0x00) {
                //This is an MMCv3 card
                m_CardType = CARD_MMC;
            } else {
                //Initialization failed
                m_CardType = CARD_UNKNOWN;
                return m_Status;
            }
        }
    }

    //Send ACMD42(0x00000000) to disconnect the internal pull-up resistor on pin 1 if necessary
    if (m_CardType != CARD_MMC) {
        if (commandTransaction(ACMD42, 0x00000000) != 0x00) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }
    }

    //Send CMD16(0x00000200) to force the block size to 512B if necessary
    if (m_CardType != CARD_SDHC) {
        if (commandTransaction(CMD16, 0x00000200) != 0x00) {
            //Initialization failed
            m_CardType = CARD_UNKNOWN;
            return m_Status;
        }
    }

    //The card is now initialized
    m_Status &= ~STA_NOINIT;

    //Return the disk status
    return m_Status;
}

DSTATUS disk_status (
    BYTE drv        /* Physical drive nmuber (0..) */
)
{
    //Check if there's a card in the socket
    checkSocket();

    //Return the disk status
    return m_Status;
}

DRESULT disk_read (
    BYTE drv,        /* Physical drive nmuber (0..) */
    BYTE *buffer,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    BYTE count        /* Number of sectors to read (1..255) */
){
    //Make sure the card is initialized before proceeding
    if (m_Status & STA_NOINIT)
        return RES_PARERR;

    //Read a single block, or multiple blocks
    if (count > 1) {
        return readBlocks((char*)buffer, sector, count) ? RES_OK : RES_PARERR;
    } else {
        return readBlock((char*)buffer, sector) ? RES_OK : RES_PARERR;
    }
}

#if _READONLY == 0
DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0..) */
    const BYTE *buffer,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    BYTE count            /* Number of sectors to write (1..255) */
)
{
    //Make sure the card is initialized before proceeding
    if (m_Status & STA_NOINIT)
        return RES_PARERR;

    //Make sure the card isn't write protected before proceeding
    if (m_Status & STA_PROTECT)
        return RES_PARERR;

    //Write a single block, or multiple blocks
    if (count > 1) {
        return writeBlocks((const char*)buffer, sector, count) ? RES_OK : RES_PARERR;
    } else {
        return writeBlock((const char*)buffer, sector) ? RES_OK : RES_PARERR;
    }
}
#endif /* _READONLY */

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    switch(ctrl) {
        case CTRL_SYNC:
            return disk_sync() ? RES_ERROR : RES_OK;
        case GET_SECTOR_COUNT:
        {
            DWORD res = disk_sectors();
            if(res > 0) {
                *((DWORD*)buff) = res; // minimum allowed
                return RES_OK;
            } else {
                return RES_ERROR;
            }
        }
        case GET_BLOCK_SIZE:
            *((DWORD*)buff) = 1; // default when not known
            return RES_OK;
    }
    return RES_PARERR;
}

} // namespace YAPFS

void initYAPFS(){
    if(YAPFS::m_Status != STA_NOINIT)
        return;
        
    //Initialize the member variables
    YAPFS::m_CardType = YAPFS::CARD_NONE;
    YAPFS::m_Status = STA_NOINIT;
    
    LPC_GPIO_PORT->DIR[0] |= (1  << 7 );//cs pin out
    YAPFS::initSPI();
 
    YAPFS::f_mount(0, &YAPFS::_fs);
    YAPFS::m_CdAssert = -1;
}
