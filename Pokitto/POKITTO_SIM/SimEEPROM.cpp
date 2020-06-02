
#include "SimEEPROM.h"
#include <stdio.h>

uint8_t eeprom_read_byte(uint16_t* index) {
    FILE *eeprom_file;
    eeprom_file=fopen("EEP.ROM","rb");

    if (!eeprom_file)
		{
			printf("Creating new EEP.ROM file\n");
			eeprom_file=fopen("EEP.ROM","wb");
			if (!eeprom_file) {
                printf("Could not create EEP.ROM file. Perhaps directory is write protected?\n");
                return 1;
			}
            for (int i=0; i < 0x1000; i++) fputc(0,eeprom_file); //fill with zeroes
            fclose(eeprom_file);
            eeprom_file=fopen("EEP.ROM","rb");
			if (!eeprom_file) {
                printf("Could not open EEP.ROM file.");
                return 1;
			}
		}

    char val;
    #ifndef POK_SIM
    fseek(eeprom_file, (long int)index, SEEK_SET);
    #else
    fseek(eeprom_file, *((long int*) &index), SEEK_SET);
    #endif
    val=fgetc(eeprom_file);
    fclose(eeprom_file);
    return val;
}

void eeprom_write_byte(uint16_t* index , uint8_t val) {
    FILE *eeprom_file;
    eeprom_file=fopen("EEP.ROM","rb+");
    if (!eeprom_file)
    {
        printf("Could not open/create EEP.ROM file.");
        return;
    }
    #ifndef POK_SIM
    fseek(eeprom_file, (long int) index, SEEK_SET);
    #else
    fseek(eeprom_file, *((long int*) &index), SEEK_SET);
    #endif
    fputc(val,eeprom_file);
    fseek(eeprom_file, (long int) 0xFFF, SEEK_SET);
    fputc(0xFF,eeprom_file); //prevent truncation of 4kB file
    fclose(eeprom_file);
}
