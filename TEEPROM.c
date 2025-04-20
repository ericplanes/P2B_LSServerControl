#include "TEEPROM.h"

#define LOG_SIZE 14
#define MAX_LOGS 15

static BYTE mem_section = 0;
static BYTE amount_of_stored_logs = 0;
static BYTE pos = 0;

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static BYTE read_byte(BYTE address);
static void prepare_write_info(BYTE address, BYTE data);
static void write_prepared_info(void);
static void write_byte(BYTE address, BYTE data);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

BOOL EEPROM_StoreLog(const BYTE *log_data)
{
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
        return TRUE;
    }

    return FALSE;
}

BOOL EEPROM_ReadLog(BYTE section, BYTE *log_data)
{
    if (pos < LOG_SIZE)
    {
        log_data[pos] = read_byte(pos + (section * LOG_SIZE) + 1);
        pos++;
    }

    if (pos >= LOG_SIZE)
    {
        pos = 0;
        return TRUE;
    }

    return FALSE;
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE read_byte(BYTE address)
{
    EEADR = address;
    EECON1 = 0x01;
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
