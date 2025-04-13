#include "TEEPROM.h"

#define LOG_SIZE 14
#define MAX_LOGS 15

unsigned char mem_section = 0;
unsigned char amouont_of_stored_logs = 0;
unsigned char pos = 0;

void writeByte(unsigned char position, unsigned char data);
unsigned char readByte(unsigned char position);

/* ---------------------------
   ----- PUBLIC METHODS ------
   --------------------------- */
unsigned char EEPROM_StoreLog(char *hhmmssDDMMYYYY)
{
    char buffer = hhmmssDDMMYYYY[pos];
    if (pos < LOG_SIZE)
    {
        writeByte(pos + (mem_section * LOG_SIZE) + 1, buffer);
        pos++;
    }

    if (pos >= LOG_SIZE)
    {
        pos = 0;
        mem_section++;
        amouont_of_stored_logs++;
        if (mem_section == MAX_LOGS)
        {
            mem_section = 0;
            amouont_of_stored_logs = MAX_LOGS;
        }
        writeByte(0, amouont_of_stored_logs); // Increase amount of stored logs
        return EEPROM_FINISHED;
    }

    return EEPROM_ONGOING;
}

unsigned char EEPROM_ReadLog(unsigned char section, char *hhmmssDDMMYYYY)
{
    if (pos < LOG_SIZE)
    {
        hhmmssDDMMYYYY[pos] = Memory_Read(pos + (section * LOG_SIZE) + 1);
        pos++;
    }
    if (pos >= LOG_SIZE)
    {
        pos = 0;
        return EEPROM_FINISHED;
    }
    return EEPROM_ONGOING;
}

/* ---------------------------
   ----- PRIVATE METHODS -----
   --------------------------- */
unsigned char readByte(unsigned char address)
{
    EEADR = address;
    EECON1 = 0x01;
    return EEDATA;
}

void prepareWriteInfo(unsigned char address, unsigned char data)
{
    EECON1bits.WREN = 1; // Enable write
    EEADR = address;     // Set address
    EEDATA = data;       // Set byte to write
}

void writePreparedInfo()
{
    EECON2 = 0x55;       // Part 1/2 of write sequence
    EECON2 = 0x0AA;      // Part 2/2 of write sequence
    EECON1bits.WR = 1;   // Write
    EECON1bits.WREN = 0; // Disable write
}

void writeByte(unsigned char address, unsigned char data)
{
    prepareWriteInfo(address, data);
    di(); // Disable GIE (Global Interruptions)
    writePreparedInfo();
    ei(); // Enable Global Interruptions
}