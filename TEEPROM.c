#include "TEEPROM.h"

#define LOG_SIZE (TIMESTAMP_SIZE - 1)
#define MAX_LOGS 15

#define EEPROM_IDLE 0
#define EEPROM_WRITING 1
#define EEPROM_READING 2

static BYTE mem_section = 0;
static BYTE amount_of_stored_logs = 0;
static BYTE pos = 0;
static BYTE eeprom_state = EEPROM_IDLE;

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE read_byte(BYTE address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}

static void prepare_write_info(BYTE address, BYTE data)
{
    EECON1bits.WREN = 1;
    EEADR = address;
    EEDATA = data;
}

static void write_prepared_info(void)
{
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    EECON1bits.WREN = 0;
}

static void write_byte(BYTE address, BYTE data)
{
    prepare_write_info(address, data);
    di();
    write_prepared_info();
    ei();
}

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void EEPROM_CleanMemory(void)
{
    // Set variables to initial value
    mem_section = 0;
    amount_of_stored_logs = 0;
    pos = 0;
    eeprom_state = EEPROM_IDLE;

    // Write 0 to all positions from the EEPROM
    for (BYTE i = 0; i < (MAX_LOGS * LOG_SIZE) + 1; i++) // Will do 14 * 15 + 1 iterations = 211
    {
        write_byte(i, (BYTE)'0');
    }
}

BOOL EEPROM_StoreLog(const BYTE *log_data)
{
    if (eeprom_state != EEPROM_IDLE || EECON1bits.WR)
        return FALSE;

    eeprom_state = EEPROM_WRITING;

    if (pos < LOG_SIZE)
    {
        write_byte(pos + (mem_section * LOG_SIZE) + 1, log_data[pos]);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        pos = 0;
        mem_section++;
        amount_of_stored_logs++;

        if (mem_section == MAX_LOGS)
        {
            mem_section = 0;
            amount_of_stored_logs = MAX_LOGS;
        }

        write_byte(0, amount_of_stored_logs);
        eeprom_state = EEPROM_IDLE;
        return TRUE;
    }

    return FALSE;
}

BOOL EEPROM_ReadLog(BYTE section, BYTE *log_data)
{
    if (eeprom_state != EEPROM_IDLE)
        return FALSE;

    eeprom_state = EEPROM_READING;

    if (pos < LOG_SIZE)
    {
        log_data[pos] = read_byte(pos + (section * LOG_SIZE) + 1);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        log_data[pos] = '\0';
        pos++;
    }

    if (pos > LOG_SIZE)
    {
        pos = 0;
        eeprom_state = EEPROM_IDLE;
        return TRUE;
    }

    return FALSE;
}

BYTE EEPROM_GetAmountOfStoredLogs(void)
{
    return amount_of_stored_logs;
}

BYTE EEPROM_CanBeUsed(void)
{
    return eeprom_state == EEPROM_IDLE ? TRUE : FALSE;
}
