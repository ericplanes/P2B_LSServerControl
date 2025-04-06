#include "EEPROM.h"

// private variables
unsigned char seccio_eeprom = 0;
unsigned char max_seccions = 0;
unsigned char i = 0;
char auxReturn[7];

// private methods
void writeByte(unsigned char position, unsigned char data);
unsigned char readByte(unsigned char position);

void EEPROM_SetMaxSeccions(unsigned char max) {
    max_seccions = max;
}

/* ---------------------------
   ----- PUBLIC METHODS ------
   --------------------------- */

unsigned char EEPROM_SaveTrack(char *hhmmssDDMMYYYY) {
    char buffer = hhmmssDDMMYYYY[i];
    if (i < 7) {
        Memory_Save(i + (seccio_eeprom * 7) + 1, buffer);
        i++;
    } else if (i >= 7) {
        i = 0;
        seccio_eeprom++;
        max_seccions++;
        if (seccio_eeprom == 7) {
            seccio_eeprom = 0;
            max_seccions = 7;
        }
        Memory_Save(0, max_seccions);
        return 1;
    }
    return 0;
}

unsigned char EEPROM_ReadTrack(unsigned char seccio, char * nnmmxss) {
    if (i < 7) {
        nnmmxss[i] = Memory_Read(i + (seccio * 7) + 1);
        i++;
    }
    if (i >= 7) {
        i = 0;
        return 1;
    }
    return 0;
}

/* ---------------------------
   ----- PRIVATE METHODS -----
   --------------------------- */
unsigned char readByte(unsigned char address) {
    EEADR = address;
    EECON1 = 0x01;
    return EEDATA;
}


// void writeByte(unsigned char address, unsigned char data)
void prepareWriteInfo(unsigned char address, unsigned char data) {
    EECON1bits.WREN = 1; // Enable write
    EEADR = address; // Set address
    EEDATA = data; // Set byte to write
}

void writePreparedInfo() {
    EECON2 = 0x55; // Part 1/2 of write sequence
    EECON2 = 0x0AA; // Part 2/2 of write sequence
    EECON1bits.WR = 1; // Write
    EECON1bits.WREN = 0; // Disable write
}

void writeByte(unsigned char address, unsigned char data) {
    prepareWriteInfo(address, data);
    di(); // Disable GIE (Global Interruptions)
    writePreparedInfo();
    ei(); // Enable Global Interruptions
}